#ifndef PLOT_RESIZER_HPP
#define PLOT_RESIZER_HPP

#include <windows.h>
#include "Vec2d.hpp"

class PlotResizer {
private:
	bool active;
	bool hold_reference;
	Vec2d reference_position;
	RECT hold_margins;

public:
	bool GetStatus() const;
	RECT GetReferenceMargins() const;
	void StartResize(Vec2d ref_position, RECT margins);
	Vec2d UpdateResize(Vec2d curr_position);
	void StopResize();
	PlotResizer();
};

#endif