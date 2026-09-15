#include "PlotResizer.hpp"

bool PlotResizer::GetStatus() const {
	return active;
}

RECT PlotResizer::GetReferenceMargins() const {
	return hold_margins;
};

void PlotResizer::StartResize(Vec2d ref_position, RECT margins) {
	if (!active && !hold_reference) {
		active = true;
		hold_reference = true;
		reference_position = ref_position;
		hold_margins = margins;
	}
}

Vec2d PlotResizer::UpdateResize(Vec2d curr_position) {
	if (active) {
		return (reference_position - curr_position);
	}
	return { 0,0 };
}

void PlotResizer::StopResize() {
	active = false;
	hold_reference = false;
}

PlotResizer::PlotResizer() {
	active = false;
	hold_reference = false;
	hold_margins = { 0,0,0,0 };
}