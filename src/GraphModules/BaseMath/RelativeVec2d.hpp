#ifndef RELATIVE_VEC2D_HPP
#define RELATIVE_VEC2D_HPP

#include "Vec2d.hpp"

class RelativeVec2d {
private:
	Vec2d start_values;
	Vec2d current_values;
	Scale2d scales;

public:
	RelativeVec2d();
	RelativeVec2d(Vec2d values);

	void SetStartValues(Vec2d values);
	void SetStartValues(double x, double y);
	void UpdateCurrentValues(Vec2d values);
	void UpdateCurrentValues(double x, double y);
	void UpdateScales(Vec2d scales);

	Vec2d GetCurrentValues() const;
	Vec2d GetScales() const;
};

#endif // RELATIVEVEC2D_H