#ifndef TRANSFORM_COORDINATES_HPP
#define TRANSFORM_COORDINATES_HPP

#include"Vec2d.hpp"
#include"GraphContext.hpp"

class GraphContext;

class CacheTransform {
private:
	Size2d visible_area;
	Size2d plot_size;
	Scale2d world_to_pixel;
	Offset2d reference_position_coord;
	Offset2d plot_reference_offset;

public:
	void update(const GraphContext& ctx);
	Position2d ConvertToPixelCoords(double world_x, double world_y) const;
	Position2d ConvertToPixelCoords(const Position2d& world_pos) const;
};

class TransformCoordinates {
private:
	const GraphContext& context;
	CacheTransform cache_device;
	
public:
	TransformCoordinates(const GraphContext& ctx);

	Position2d ConvertToPixelCoords(double world_x, double world_y) const;
	Position2d ConvertToPixelCoords(Position2d world_pos) const;
	Position2d ConvertToWorldCoords(int pixel_x, int pixel_y) const;

public:
	Position2d FastConvertToPixelCoords(double world_x, double world_y) const;
	Position2d FastConvertToPixelCoords(const Position2d& world_pos) const;
	void InitCacheTransform();
};

#endif