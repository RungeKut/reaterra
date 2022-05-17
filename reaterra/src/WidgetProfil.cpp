#include "WidgetProfil.h"

//#include <vector>
//#include <tuple>

#include "Context.h"
#include "GraphicEngine.h"

WidgetProfil::WidgetProfil(int x, int y, std::vector<std::tuple<unsigned, float, float>> points)
: Widget(x, y, State::Disabled),
  m_points {points}
{
}

WidgetProfil::~WidgetProfil() {
}

void WidgetProfil::show() {
	if (ctx.m_session.getDuration() > 0) {
		ctx.m_graphic.drawRectangle(getX(), getY(), (ctx.m_session.getDuration() + 1)  * 1053. / std::get<0>(m_points.back()), 417, 0x3599FE);
	}

	uint32_t c1 = 0x638AFF;
	uint32_t c2 = 0x4576AD;
	ctx.m_graphic.drawLineGradient(getX(), getY(), getX(), getY() + 416, 1, {{0, c1}, {0.49, c2}, {0.50, c1}, {1, c2}});
	for (int i = 1; i < 16; ++i) {
		ctx.m_graphic.drawLine(getX(), getY() + 26 * i, getX() + 1053, getY() + 26 * i, 1, ctx.m_graphic.getGradientColor(c1, c2, (i % 8) / 8.));
	}
	for (int i = 1; i < 9; ++i) {
		ctx.m_graphic.drawLineGradient(getX() + 117 * i, getY(), getX() + 117 * i, getY() + 416, 1, {{0, c1}, {0.49, c2}, {0.50, c1}, {1, c2}});
	}

	ctx.m_graphic.drawLine(getX(), getY() + 208, getX() + 1053, getY() + 208, 3, 0x87CEEA);
	ctx.m_graphic.drawLine(getX() + 1043, getY() + 208 - 5, getX() + 1053, getY() + 208, 2, 0x87CEEA);
	ctx.m_graphic.drawLine(getX() + 1043, getY() + 208 + 5, getX() + 1053, getY() + 208, 2, 0x87CEEA);

	for (size_t i = 1; i < m_points.size(); ++i) {
		const auto &p1 = m_points.at(i - 1);
		int x1 = std::get<0>(p1) * 1053. / std::get<0>(m_points.back());
		int s1 = 416 / 2 - std::get<1>(p1) * 412 / 2 / ctx.m_treadmill.getMaxForwardSpeed() * 10.;
		int a1 = 415 - std::get<2>(p1) * 408 / 2 / ctx.m_treadmill.getMaxAngle() * 100.;

		const auto &p2 = m_points.at(i);
		int x2 = std::get<0>(p2) * 1053. / std::get<0>(m_points.back());
		int s2 = 416 / 2 - std::get<1>(p2) * 412 / 2 / ctx.m_treadmill.getMaxForwardSpeed() * 10.;
		int a2 = 415 - std::get<2>(p2) * 408 / 2 / ctx.m_treadmill.getMaxAngle() * 100.;

		ctx.m_graphic.drawLine(getX() + x1, getY() + s1, getX() + x2, getY() + s2, 3, 0xCC9933);
		ctx.m_graphic.drawLine(getX() + x1, getY() + a1, getX() + x2, getY() + a2, 3, 0x8ACE81);
	}

}
