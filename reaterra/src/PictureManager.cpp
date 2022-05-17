#include "PictureManager.h"

#include "Context.h"

PictureManager::PictureManager()
:	m_id_counter {0}
{
	LD("Ctor");
}

PictureManager::~PictureManager() {
	LD("Dtor");
}


int PictureManager::loadPicture(std::string file) {
//	LD("Start load pictures");
	int ret = 0;

	if (file.empty()) {
//		LD("Finish load pictures empty");
		return ret;
	}

	for (const auto &p : m_pic_storage) {
		if (p.second.file == file) {
			ret = p.second.id;
			break;
		}
	}

	if (ret == 0) {
		std::string f = std::string("reaterra_pics/") + file;
		Picture p;
//		LD("Load picture - " + file);
		p.file = file;
		p.image = std::shared_ptr<cairo_surface_t>(cairo_image_surface_create_from_png(f.c_str()), [=](cairo_surface_t *surface) {
			LD("Dtor image surface");
			cairo_surface_destroy(surface);
		});

		int st = cairo_surface_status(p.image.get());
		if (st == CAIRO_STATUS_SUCCESS) {
			p.id = ++m_id_counter;
			m_pic_storage[m_id_counter] = p;
			ret = p.id;
		} else if (st == CAIRO_STATUS_NO_MEMORY) {
			LE("Error CAIRO_STATUS_NO_MEMORY during loading picture file " + f);
		} else if (st == CAIRO_STATUS_FILE_NOT_FOUND) {
			LE("Error CAIRO_STATUS_FILE_NOT_FOUND during loading picture file " + f);
		} else if (st == CAIRO_STATUS_READ_ERROR) {
			LE("Error CAIRO_STATUS_READ_ERROR during loading picture file " + f);
		}
	} else {
//		LD("Reuse picture - " + file);
	}

//	LD("Finish load pictures");
	return ret;
}

cairo_surface_t* PictureManager::getPicture(int id) {
	if (!id) {
		return nullptr;
	}

	const auto p = m_pic_storage.find(id);
	if (p != m_pic_storage.end()) {
		return p->second.image.get();
	} else {
		return nullptr;
	}
}

