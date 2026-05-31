#ifndef _PLOTRESIZER_HPP_
#define _PLOTRESIZER_HPP_

#include"Vec2d.hpp"

class PlotResizer {
private:
	bool active;
	bool hold_reference;
	Vec2d reference_position;
	RECT hold_margins;

public:
	bool GetStatus() const {
		return active;
	}

	RECT GetReferenceMargins() const {
		return hold_margins;
	};

	void StartResize(Vec2d ref_position, RECT margins) {
		if (!active && !hold_reference) {
			active = true;
			hold_reference = true;
			reference_position = ref_position;
			hold_margins = margins;
		}
	}

	Vec2d UpdateResize(Vec2d curr_position) {
		if (active) {
			return (reference_position - curr_position);
		}
		return { 0,0 };
	}

	void StopResize() {
		active = false;
		hold_reference = false;
	}

	PlotResizer() {
		active = false;
		hold_reference = false;
		hold_margins = { 0,0,0,0 };
	}
};

#endif