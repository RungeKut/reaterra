#include "GraphicEngine.h"

#include <linux/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <stropts.h>

#include <algorithm>

#include "Context.h"
#include "Locale.h"
#include "Utils.h"

namespace {
	void _cairo_linuxfb_surface_destroy(void *) {
		ctx.m_graphic.cairo_linuxfb_surface_destroy();
	}

	cairo_status_t _cairo_png_writer(void *closure, const unsigned char *data, unsigned int length) {
		ctx.m_graphic.cairo_png_writer(data, length);
		return CAIRO_STATUS_SUCCESS;
	}
}

GraphicEngine::GraphicEngine()
:	m_good {false},
	bufid {1},
	m_remote_screen {false}
{
	LD("Ctor");

	buff.reserve(300000);
	fbsurface = cairo_linuxfb_surface_create("/dev/fb0");
	fbcr = cairo_create(fbsurface);
	cr = fbcr;

	memcr = cairo_create(memsurf);

	m_good = true;
}

GraphicEngine::~GraphicEngine() {
	flip_buffer(false, true);
	cairo_destroy(fbcr);
	LD("Dtor");
}


int GraphicEngine::flip_buffer(bool vsync, bool to_first_page)
{
	int dummy = 0;

	bufid = to_first_page ? 0 : bufid;

	/* Pan the framebuffer */
	device.fb_vinfo.yoffset = device.fb_vinfo.yres * bufid;
	if (ioctl(device.fb_fd, FBIOPAN_DISPLAY, &device.fb_vinfo)) {
		LE("Error panning display");
		return -1;
	}

	if (vsync) {
		if (ioctl(device.fb_fd, FBIO_WAITFORVSYNC, &dummy)) {
			LE("Error waiting for VSYNC");
			return -1;
		}
	}

	bufid = !bufid;

	return 0;
}


/* Destroy a cairo surface */
void GraphicEngine::cairo_linuxfb_surface_destroy()
{
	munmap(device.fb_data, device.fb_screensize);
	close(device.fb_fd);
}


void GraphicEngine::show() {
	static std::chrono::steady_clock::time_point send_delay = std::chrono::steady_clock::now();

	bool remote = ctx.m_remote.isConnected();
//	LD(" ---- Flip buffer ----- " + (remote ? "remote" : ""));

	if (m_remote_screen != remote) {
		if (remote) {
			showRemoteScreen();
			memset(fbuff, 0, device.fb_finfo.smem_len);
			cr = memcr;
		} else {
			cr = fbcr;
		}
		m_remote_screen = remote;

		ctx.m_queue.putEvent(std::make_unique<Event>(Event::EventType::Refresh));
		return;
	}

	flip_buffer(!remote);
	if (remote) {
//		LD("tr01");
		int xmin = SCREEN_WIDE;
		int ymin = SCREEN_HEIGHT;
		int xmax = 0;
		int ymax = 0;

		register const uint32_t *ub1 = fbuff;
		register const uint32_t *ub2 = ub1 + device.fb_finfo.smem_len / 8;

		int num = PIXELS_IN_SCREEN + 1;
		bool diff = false;
		while (--num) {
			if (*ub1++ != *ub2++) {
				diff = true;
				break;
			}
		}

		if (diff) {
			int cnt = 0;
			ub1 = fbuff;
			ub2 = ub1 + device.fb_finfo.smem_len / 8;
			num = PIXELS_IN_SCREEN - num;
			xmin = num % SCREEN_WIDE;
			ymin = num / SCREEN_WIDE;

			for (int j = ymin; j < SCREEN_HEIGHT && xmin; ++j) {
				cnt = j * SCREEN_WIDE;
				for (int i = 0; i < xmin; ++i) {
					if (ub1[cnt] != ub2[cnt]) {
						xmin = i;
					}
					++cnt;
				}
			}


			bool first = true;
			xmax = xmin;
			for (int j = SCREEN_HEIGHT - 1; j >= 0 && xmax < SCREEN_WIDE - 1; --j) {
				cnt = j * SCREEN_WIDE + SCREEN_WIDE - 1;
				for (int i = SCREEN_WIDE - 1; i > xmax; --i) {
					if (ub1[cnt] != ub2[cnt]) {
						xmax = i;
						if (first) {
							ymax = j;
							first = false;
						}
					}
					--cnt;
				}
			}
		}


		if (xmin != SCREEN_WIDE && ymin != SCREEN_HEIGHT) {
//			LD("rect 2: x:" + std::to_string(xmin) + "   y:" + std::to_string(ymin) + "   w:" + std::to_string(xmax - xmin + 1) + "   h:" + std::to_string(ymax - ymin + 1));
			writeScreen(xmin, ymin, xmax - xmin + 1, ymax - ymin + 1);
			send_delay = std::chrono::steady_clock::now();
		} else if (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - send_delay).count() > 2000) {
			writeScreen(0, 0, 1, 1);
			LD("Send HB to remote");
			send_delay = std::chrono::steady_clock::now();
		}
//		LD("tr02");
	}
}

void GraphicEngine::drawText(std::string text, int x, int y, int size, float r, float g, float b,  Align align, FontStyle style, float angle) {
	cairo_set_source_rgb(cr, r, g, b);

	cairo_select_font_face(cr, "Sans", CAIRO_FONT_SLANT_NORMAL, style == FontStyle::Bold ? CAIRO_FONT_WEIGHT_BOLD : CAIRO_FONT_WEIGHT_NORMAL);
	cairo_set_font_size(cr, size);

	cairo_text_extents_t extents;
	cairo_text_extents(cr, text.c_str(), &extents);
	if (align == Align::Center) {
		x = x - (extents.width/2 + extents.x_bearing);
	} else if (align == Align::Right) {
		x -= extents.width + extents.x_bearing;
	}
//	y = y - (extents.height/2 + extents.y_bearing);

	cairo_move_to(cr, x, y + device.fb_vinfo.yres * bufid);
	if (angle != 0) {
		cairo_rotate(cr, angle);
	}
	cairo_show_text(cr, text.c_str());
	if (angle != 0) {
		cairo_rotate(cr, -angle);
	}
}

void GraphicEngine::drawRectangle(int x, int y, int width, int height,
		uint32_t fill_color, int line_width, uint32_t line_color,
		int xt, int yt, uint32_t high_color) {

	cairo_rectangle(cr, x, y + device.fb_vinfo.yres * bufid, width, height);
	uint32_t fc = fill_color;
	if (xt && yt && cairo_in_fill(cr, xt, yt + device.fb_vinfo.yres * bufid)) {
		fc = high_color;
	}
	cairo_set_source_rgb(cr, RGB2R(fc), RGB2G(fc), RGB2B(fc));
	if (line_width) {
		cairo_fill_preserve(cr);
		cairo_set_source_rgb(cr, RGB2R(line_color), RGB2G(line_color), RGB2B(line_color));
		cairo_set_line_width(cr, line_width);
		cairo_stroke(cr);
	} else {
		cairo_fill(cr);
	}
}

void GraphicEngine::drawKbdButton(const KeyboardButton& button) {

	cairo_rectangle(cr, button.x, button.y + device.fb_vinfo.yres * bufid, button.width, button.height);
	uint32_t fc = button.fill_color;

	if (button.active) {
		fc = button.high_fill_color;
	}
	cairo_set_source_rgb(cr, RGB2R(fc), RGB2G(fc), RGB2B(fc));
	if (button.line_width) {
		cairo_fill_preserve(cr);
		cairo_set_source_rgb(cr, RGB2R(button.line_color), RGB2G(button.line_color), RGB2B(button.line_color));
		cairo_set_line_width(cr, button.line_width);
		cairo_stroke(cr);
	} else {
		cairo_fill(cr);
	}


	fc = button.text_color;
	if (button.active) {
		fc = button.high_text_color;
	}
	if (!button.name.empty()) {
		cairo_set_source_rgb(cr, RGB2R(fc), RGB2G(fc), RGB2B(fc));

		cairo_select_font_face(cr, "Sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
		cairo_set_font_size(cr, button.text_size);

		cairo_text_extents_t extents;
		cairo_text_extents(cr, button.name.c_str(), &extents);
		int x = button.width / 2 + button.x - (extents.width/2 + extents.x_bearing);
		int y = button.height / 2 + button.y + button.text_size / 3;

		cairo_move_to(cr, x, y + device.fb_vinfo.yres * bufid);
		cairo_show_text(cr, button.name.c_str());
	}
}

//TODO: Сделать разбивку с учетом символов переноса
std::vector<std::string> GraphicEngine::splitString(std::string str, int width, bool use_font, int font_size, FontStyle font_style) {
	std::vector<std::string> ret;

	if (use_font) {
		cairo_select_font_face(cr, "Sans", CAIRO_FONT_SLANT_NORMAL, font_style == FontStyle::Bold ? CAIRO_FONT_WEIGHT_BOLD : CAIRO_FONT_WEIGHT_NORMAL);
		cairo_set_font_size(cr, font_size);
	}

	cairo_status_t status;

	cairo_glyph_t *glyphs = NULL;
	int num_glyphs;
	auto scaled_font = cairo_get_scaled_font(cr);

	status = cairo_scaled_font_text_to_glyphs (scaled_font, 0, 0, str.c_str(), str.size(), &glyphs, &num_glyphs, NULL, NULL, NULL);

	if (status == CAIRO_STATUS_SUCCESS) {
		int size = 0;
		int prev_size = 0;
		std::wstring ws = Utils::utf8_to_wstring(str);
		int start = 0;
		bool new_line_found {false};
		bool line_not_fit_in_width {false};

		for (int i = 0; i < num_glyphs; ++i) {

			new_line_found = (ws.at(i) == Utils::utf8_to_wstring("\\").at(0));
			line_not_fit_in_width = (glyphs[i].x - size > width - glyphs[i].x + prev_size);

			if (new_line_found || line_not_fit_in_width) {
				ret.push_back(Utils::wstring_to_utf8(ws.substr(start, i - start)));
				start = i + (new_line_found ? 1 : 0);
				size = prev_size;
			} else if (ws.at(i) == Utils::utf8_to_wstring("¦").at(0)) {
				ws.at(i) = Utils::utf8_to_wstring("\\").at(0);
			}
			prev_size = glyphs[i].x;
//			LD("Glyph " + std::to_string(i) + ":    index " + std::to_string(glyphs[i].index) + "    x " + std::to_string(glyphs[i].x) + "    y " + std::to_string(glyphs[i].y));
		}

		ret.push_back(Utils::wstring_to_utf8(ws.substr(start)));

	    cairo_glyph_free (glyphs);
	}
	return ret;
}

void GraphicEngine::drawMultiText(std::string text, int x, int y, int width, int lines, int dy,
		int font_size, Align h_align, Align v_align, FontStyle style, uint32_t color, float angle)
{
	std::vector<std::string> strs = splitString(text, width, true, font_size, style);
	int lines_actual = static_cast<int>(strs.size());
	lines = lines ? std::min(lines, lines_actual) : lines_actual;

	int half = lines / 2;
	int mid = lines % 2;
	int mid_y;
	int i0 = -half;

	if (mid != 0) {
		mid_y = y + 3/8. * font_size;
	} else {
		mid_y = y + (dy - 3/4. * font_size) / 2. + 3/4. * font_size;
	}

	if (v_align == Align::Top) {
		mid_y = y + 3/4. * font_size;
		i0 = 0;
		half = 0;
		mid = lines;
	}

	int j = 0;
	for (int i = i0; i < mid + half; ++i) {
		if (!strs.at(j++).empty()) {
			drawText(strs.at(j - 1), x, mid_y + i * dy, font_size, RGB2R(color), RGB2G(color), RGB2B(color), h_align, style, angle);
		}
	}
}

void GraphicEngine::writeScreen(int x, int y, int w, int h) {
	static int packet_id = 0;

	cairo_surface_t *sf = nullptr;
	if (w > 1 && h > 1) {
		sf = cairo_surface_create_for_rectangle(memsurf, x, y + 800 * (!bufid), w, h);
	}
	packet_id++;


//	LD("Start writing PNG");
	if (!buff.size()) {
		buff.insert(buff.end(), 8, 0xFF);
		//TODO: Убрать лишний байт синхронно с Андроид приложением.
		buff.insert(buff.end(), 6 * 4 + 1, 0);
		LD("before");
		if (sf) {
			cairo_surface_write_to_png_stream(sf, _cairo_png_writer, nullptr);
		}
		LD("after");
		unsigned size = buff.size();
		for (int i = 0; i < 4; ++i) {
			buff.at(i + 8) = size & 0xFF;
			size >>= 8;
		}

		for (int i = 0; i < 4; ++i) {
			buff.at(i + 12) = x & 0xFF;
			x >>= 8;
		}
		for (int i = 0; i < 4; ++i) {
			buff.at(i + 16) = y & 0xFF;
			y >>= 8;
		}
		for (int i = 0; i < 4; ++i) {
			buff.at(i + 20) = w & 0xFF;
			w >>= 8;
		}
		for (int i = 0; i < 4; ++i) {
			buff.at(i + 24) = h & 0xFF;
			h >>= 8;
		}
		int id = packet_id;
		for (int i = 0; i < 4; ++i) {
			buff.at(i + 28) = id & 0xFF;
			id >>= 8;
		}


//		LD("Begin of array:[" + std::to_string(buff.size()) + "] " + Utils::bytes2Hex(buff.data(), 20));
//		LD("End writing PNG");
		ctx.m_remote.send(buff, packet_id);
		buff.clear();
	}
}

void GraphicEngine::showRemoteScreen() {
	for (int i = 0; i < 2; ++i) {
		drawSurface(ctx.m_pic_mgr.getPicture(ctx.m_pic_mgr.loadPicture("remote_screen.png")));
		drawMultiText(M("БЕГОВАЯ ДОРОЖКА С БИОЛОГИЧЕСКОЙ ОБРАТНОЙ СВЯЗЬЮ"), 1280 / 2, 440, 1280, 3, 30 * 1.2,
				30, Align::Center, Align::Center, FontStyle::Normal, 0x306396);
		drawMultiText(M("ДОРОЖКА УПРАВЛЯЕТСЯ С ПЛАНШЕТА"), 1280 / 2, 600, 1280, 3, 30 * 1.2,
				30, Align::Center, Align::Center, FontStyle::Bold, 0xED7028);
		flip_buffer(true);
	}
}


/* Create a cairo surface using the specified framebuffer */
cairo_surface_t *GraphicEngine::cairo_linuxfb_surface_create(const char *fb_name)
{
	cairo_surface_t *surface;

	// Open the file for reading and writing
	device.fb_fd = open(fb_name, O_RDWR);
	if (device.fb_fd == -1) {
		LE("Error: cannot open framebuffer device");
		throw std::runtime_error("Error: cannot open framebuffer device");
	}

	// Get variable screen information
	if (ioctl(device.fb_fd, FBIOGET_VSCREENINFO, &device.fb_vinfo) == -1) {
		LE("Error: reading variable information");
		throw std::runtime_error("Error: reading variable information");
	}

	/* Set virtual display size double the width for double buffering */
	device.fb_vinfo.yoffset = 0;
	device.fb_vinfo.yres_virtual = device.fb_vinfo.yres * 2;
	if (ioctl(device.fb_fd, FBIOPUT_VSCREENINFO, &device.fb_vinfo)) {
		LE("Error setting variable screen info from fb");
		throw std::runtime_error("Error setting variable screen info from fb");
	}

	// Get fixed screen information
	if (ioctl(device.fb_fd, FBIOGET_FSCREENINFO, &device.fb_finfo) == -1) {
		LE("Error reading fixed information");
		throw std::runtime_error("Error reading fixed information");
	}

	// Map the device to memory
	device.fb_data = (unsigned char *)mmap(0, device.fb_finfo.smem_len,
			PROT_READ | PROT_WRITE, MAP_SHARED,
			device.fb_fd, 0);
	if ((int)device.fb_data == -1) {
		LE("Error: failed to map framebuffer device to memory");
		throw std::runtime_error("Error: failed to map framebuffer device to memory");;
	}


	/* Create the cairo surface which will be used to draw to */
	surface = cairo_image_surface_create_for_data(device.fb_data,
			CAIRO_FORMAT_ARGB32,
			device.fb_vinfo.xres,
			device.fb_vinfo.yres_virtual,
			cairo_format_stride_for_width(CAIRO_FORMAT_ARGB32,
				device.fb_vinfo.xres));
	cairo_surface_set_user_data(surface, NULL, &device,
			&_cairo_linuxfb_surface_destroy);

	fbsurface = surface;

	surface = cairo_image_surface_create_for_data(reinterpret_cast<unsigned char*>(fbuff),
			CAIRO_FORMAT_ARGB32,
			device.fb_vinfo.xres,
			device.fb_vinfo.yres_virtual,
			cairo_format_stride_for_width(CAIRO_FORMAT_ARGB32,
				device.fb_vinfo.xres));

	memsurf = surface;

	return fbsurface;
}


void GraphicEngine::drawSurface(cairo_surface_t* surface, int x, int y) {
	if (!surface) {
		return;
	}

	cairo_set_source_surface(cr, surface, x, y + device.fb_vinfo.yres * bufid);
	cairo_paint(cr);
}


SizeGraph GraphicEngine::getTextSize(std::string text, int font_size, FontStyle style) {
	cairo_select_font_face(cr, "Sans", CAIRO_FONT_SLANT_NORMAL, style == FontStyle::Bold ? CAIRO_FONT_WEIGHT_BOLD : CAIRO_FONT_WEIGHT_NORMAL);
	cairo_set_font_size(cr, font_size);

	cairo_text_extents_t extents;
	cairo_text_extents(cr, text.c_str(), &extents);
	return SizeGraph {static_cast<int>(extents.width + extents.x_bearing), static_cast<int>(extents.height + extents.y_bearing)};
}

void GraphicEngine::drawLine(int x1, int y1, int x2, int y2, int line_width, uint32_t line_color) {
	cairo_set_line_cap(cr, CAIRO_LINE_CAP_ROUND);
	if (line_color != 0xFFFFFFFF) {
		cairo_set_source_rgb(cr, RGB2R(line_color), RGB2G(line_color), RGB2B(line_color));
	}
	cairo_set_line_width(cr, line_width);
	cairo_move_to(cr, x1 + 0.5, y1 + device.fb_vinfo.yres * bufid + 0.5);
	cairo_line_to(cr, x2 + 0.5, y2 + device.fb_vinfo.yres * bufid + 0.5);
	cairo_stroke(cr);
}

void GraphicEngine::drawLineGradient(int x1, int y1, int x2, int y2, int line_width, std::vector<std::tuple<float, uint32_t> > color) {
	cairo_pattern_t *linpat = cairo_pattern_create_linear(x1, y1 + device.fb_vinfo.yres * bufid, x2, y2 + device.fb_vinfo.yres * bufid);

	for (const auto &c : color) {
		cairo_pattern_add_color_stop_rgb(linpat, std::get<0>(c),  RGB2R(std::get<1>(c)), RGB2G(std::get<1>(c)), RGB2B(std::get<1>(c)));
	}
	cairo_set_source(cr, linpat);

	drawLine(x1, y1, x2, y2, line_width, 0xFFFFFFFF);
}

uint32_t GraphicEngine::getGradientColor(uint32_t beg, uint32_t end, float index) {
	uint32_t r1 = RGB2Rd(beg);
	uint32_t g1 = RGB2Gd(beg);
	uint32_t b1 = RGB2Bd(beg);
	uint32_t r2 = RGB2Rd(end);
	uint32_t g2 = RGB2Gd(end);
	uint32_t b2 = RGB2Bd(end);

	uint32_t r, g, b;
	if (r2 > r1) {
		r = Utils::clamp(static_cast<uint32_t>(r1 + (r2 - r1) * index), 0u, 255u);
	} else {
		r = Utils::clamp(static_cast<uint32_t>(r1 - (r1 - r2) * index), 0u, 255u);
	}
	if (g2 > g1) {
		g = Utils::clamp(static_cast<uint32_t>(g1 + (g2 - g1) * index), 0u, 255u);
	} else {
		g = Utils::clamp(static_cast<uint32_t>(g1 - (g1 - g2) * index), 0u, 255u);
	}
	if (b2 > b1) {
		b = Utils::clamp(static_cast<uint32_t>(b1 + (b2 - b1) * index), 0u, 255u);
	} else {
		b = Utils::clamp(static_cast<uint32_t>(b1 - (b1 - b2) * index), 0u, 255u);
	}

	return DEC2RGB(r, g, b);
}

void GraphicEngine::cairo_png_writer(const unsigned char *data, unsigned int length) {
	buff.insert(buff.end(), data, data + length);
}
