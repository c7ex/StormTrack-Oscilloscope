#include "RenderCache.hpp"

void RenderCache::Reset() {
	caches.clear();
}

void RenderCache::extractPixelsX(GraphContext& context) {
	pixelsX = context.GetPlotParam().GetCurrentValues().x;
}

void RenderCache::calculateVisibleRangeX(GraphContext& context) {
	Size2d area = context.GetVisibleArea();
	Position2d pos = context.GetReferencePosition();
	double min_x = static_cast<double>(pos.x);
	double max_x = static_cast<double>(pos.x) + static_cast<double>(area.x);
	double range_x = max_x - min_x;
	vrX = { min_x, max_x, range_x };
}

void RenderCache::calculateRangeXInPixelX() {
	rangeXInPixelX = vrX.range_x / static_cast<double>(pixelsX);
}

double RenderCache::calculateCompressedScale(const LinearData& ldata) {
	double step = (ldata.getParameters()).step;
	double count_points_data_in_pixelX = rangeXInPixelX / step;
	return count_points_data_in_pixelX;
}

void RenderCache::calculateEffectiveIndices(const LinearData& ldata, const Position2d ref, const Size2d area)
{
	// extract
	LinearDataParameters data_parameters = ldata.getParameters();

	// boards
	int64_t left_index_board = 0;
	int64_t right_index_board = ldata.size() - 1;

	if (right_index_board < 0) {
		// no valid case -> exit cache further
		ei.min_index = 0;
		ei.max_index = 0;
		return;
	}

	// calculated effective indexes with +1 correct
	int64_t left_index = static_cast<int64_t>((vrX.min_value_x - data_parameters.offset) / data_parameters.step);
	int64_t right_index = static_cast<int64_t>((vrX.max_value_x - data_parameters.offset) / data_parameters.step);

	left_index--;
	right_index++;

	// index protect
	if (left_index < left_index_board) left_index = left_index_board;
	if (right_index > right_index_board) right_index = right_index_board;

	// plot protect
	double xl = ldata[left_index].x;
	double xr = ldata[right_index].x;
	double left_plot_limit = ref.x;
	double right_plot_limit = ref.x + area.x;

	if (xl > right_plot_limit) {
		ei.min_index = 0;
		ei.max_index = 0;
		return;
	}

	if (xr < left_plot_limit) {
		ei.min_index = 0;
		ei.max_index = 0;
		return;
	}

	// update
	// alwayse > 0
	ei.min_index = left_index;
	ei.max_index = right_index;
}

void RenderCache::CreateScenarioCacheDirect(const TransformCoordinates& coreEngine, const LinearData& data) {
	TraceCache trace;
	trace.trace_id = trace_id;
	trace.is_active = true;
	trace.min_x = (std::numeric_limits<double>::max)();
	trace.max_x = -(std::numeric_limits<double>::max)();
	trace.min_y = (std::numeric_limits<double>::max)();
	trace.max_y = -(std::numeric_limits<double>::max)();
	trace.is_compressed = false;

	size_t start = static_cast<size_t>(ei.min_index);
	size_t end = static_cast<size_t>(ei.max_index);
	size_t count = end - start + 1;

	trace.points.reserve(count);

	for (size_t i = start; i <= end; ++i) {
		Vec2d p = coreEngine.ConvertToPixelCoords(data[i]);
		trace.points.push_back(p);

		trace.min_x = (std::min)(trace.min_x, p.x);
		trace.max_x = (std::max)(trace.max_x, p.x);
		trace.min_y = (std::min)(trace.min_y, p.y);
		trace.max_y = (std::max)(trace.max_y, p.y);
	}

	caches.push_back(std::move(trace));
}

void RenderCache::CreateScenarioCacheCompressed(const TransformCoordinates& coreEngine, const LinearData& data, double compressedScale) {	
	TraceCache trace;
	trace.trace_id = trace_id;
	trace.is_active = true;
	trace.min_x = (std::numeric_limits<double>::max)();
	trace.max_x = -(std::numeric_limits<double>::max)();
	trace.min_y = (std::numeric_limits<double>::max)();
	trace.max_y = -(std::numeric_limits<double>::max)();
	trace.is_compressed = true;

	double float_index = ei.min_index;
	size_t l_idx = static_cast<size_t>(float_index);

	while (float_index < ei.max_index) {
		float_index += compressedScale;
		size_t r_idx = static_cast<size_t>(float_index);

		// protect end
		if (r_idx > ei.max_index) r_idx = static_cast<size_t>(ei.max_index);

		double min_y = data[l_idx].y;
		double max_y = min_y;
		for (int i = l_idx + 1; i <= r_idx; ++i) {
			double y = data[i].y;
			if (y < min_y) min_y = y;
			else if (y > max_y) max_y = y;
		}

		int m_idx = (l_idx + r_idx) >> 1;
		double x = data[m_idx].x;

		Vec2d p1 = coreEngine.ConvertToPixelCoords(x, min_y);
		Vec2d p2 = coreEngine.ConvertToPixelCoords(x, max_y);

		trace.points.push_back(p1);
		trace.points.push_back(p2);

		trace.min_x = (std::min)(trace.min_x, (std::min)(p1.x, p2.x));
		trace.max_x = (std::max)(trace.max_x, (std::max)(p1.x, p2.x));
		trace.min_y = (std::min)(trace.min_y, (std::min)(p1.y, p2.y));
		trace.max_y = (std::max)(trace.max_y, (std::max)(p1.y, p2.y));

		l_idx = r_idx + 1;

		// case end
		if (l_idx > ei.max_index) break;
	}

	caches.push_back(std::move(trace));
}

void RenderCache::CachesManager(GraphContext& context, const TransformCoordinates& coreEngine, const LinearData& ldata)
{
	if (ldata.size() == 0) return;

	Position2d ref = context.GetReferencePosition();
	Size2d area = context.GetVisibleArea();
	calculateEffectiveIndices(ldata, ref, area);

	if (ei.min_index >= ei.max_index) return;

	double compressedScale = calculateCompressedScale(ldata);

	constexpr double visible_points_in_pixel = 2.;
	if (compressedScale < visible_points_in_pixel) {
		CreateScenarioCacheDirect(coreEngine, ldata);
	}
	else {
		CreateScenarioCacheCompressed(coreEngine, ldata, compressedScale);
	}
}

void RenderCache::ThresholdCacheYAnalyzeEntry(const TraceCache& cache, std::vector<Vec2d>& visible_cache, ThresholdCacheYAnalyzeData& data)
{
	const size_t IndexOfPoint = 0;
	
	const double y_min_pixels = data.thresholds.min;
	const double y_max_pixels = data.thresholds.max;

	auto curr_point = cache.points[IndexOfPoint];
	auto next_point = cache.points[IndexOfPoint + 1];

	// coefficient lean "k" of line and constant Y-offset "b"
	auto delta = next_point - curr_point;

	// <delta.x> cannot be equal to zero, delta.x == step of LinearData
	auto k = delta.y / delta.x;
	auto b = curr_point.y - (curr_point.x * k);

	// update flags of first line
	// check begin of line
	if (curr_point.y < y_max_pixels) {
		data.begin_in_area = false;
		data.begin_state_y = y_max_pixels;
	}
	else if (curr_point.y > y_min_pixels) {
		data.begin_in_area = false;
		data.begin_state_y = y_min_pixels;
	}
	else {
		data.begin_in_area = true;
	}

	// check end of line
	if (next_point.y < y_max_pixels) {
		data.end_in_area = false;
		data.end_state_y = y_max_pixels;
	}
	else if (next_point.y > y_min_pixels) {
		data.end_in_area = false;
		data.end_state_y = y_min_pixels;
	}
	else {
		data.end_in_area = true;
	}

	if (data.begin_in_area == true && data.end_in_area == true) {
		// not changes
		visible_cache.push_back(curr_point);
		visible_cache.push_back(next_point);
	}
	else if (data.begin_in_area == true && data.end_in_area == false) {
		// recalculate end
		double end_state_x = (data.end_state_y - b) / k;
		visible_cache.push_back(curr_point);
		visible_cache.push_back({ end_state_x , data.end_state_y });
		visible_cache.push_back({ next_point.x , data.end_state_y });
	}
	else if (data.begin_in_area == false && data.end_in_area == true) {
		// recalculate begin
		double begin_state_x = (data.begin_state_y - b) / k;
		visible_cache.push_back({ curr_point.x , data.begin_state_y });
		visible_cache.push_back({ begin_state_x , data.begin_state_y });
		visible_cache.push_back(next_point);
	}
	else if (data.begin_in_area == false && data.end_in_area == false) {
		// recalculate begin/end
		if (data.begin_state_y == data.end_state_y) {
			// invisible line
			visible_cache.push_back({ curr_point.x , data.begin_state_y });
			visible_cache.push_back({ next_point.x , data.end_state_y });
		}
		else {
			double begin_state_x = (data.begin_state_y - b) / k;
			double end_state_x = (data.end_state_y - b) / k;
			visible_cache.push_back({ curr_point.x , data.begin_state_y });
			visible_cache.push_back({ begin_state_x , data.begin_state_y });
			visible_cache.push_back({ end_state_x , data.end_state_y });
			visible_cache.push_back({ next_point.x , data.end_state_y });
		}
	}

	// state flow
	data.begin_in_area = data.end_in_area;
	data.begin_state_y = data.end_state_y;
}

void RenderCache::ThresholdCacheYAnalyzeContinues(const TraceCache& cache, std::vector<Vec2d>& visible_cache, ThresholdCacheYAnalyzeData& data)
{
	const double y_min_pixels = data.thresholds.min;
	const double y_max_pixels = data.thresholds.max;

	size_t IndexOfPoint = 1;

	for (size_t l = 0; l < cache.points.size() - 2; ++l)
	{
		// here see only next point
		// update only next point(points), without current!!!
		auto curr_point = cache.points[IndexOfPoint];
		auto next_point = cache.points[IndexOfPoint + 1];

		// coefficient lean "k" of line and constant Y-offset "b"
		auto delta = next_point - curr_point;

		// <delta.x> cannot be equal to zero, delta.x == step of LinearData
		auto k = delta.y / delta.x;
		auto b = curr_point.y - (curr_point.x * k);

		// check end of line
		if (next_point.y < y_max_pixels) {
			data.end_in_area = false;
			data.end_state_y = y_max_pixels;
		}
		else if (next_point.y > y_min_pixels) {
			data.end_in_area = false;
			data.end_state_y = y_min_pixels;
		}
		else {
			data.end_in_area = true;
		}

		if (data.begin_in_area == true && data.end_in_area == true) {
			// not changes
			visible_cache.push_back(next_point);
		}
		else if (data.begin_in_area == true && data.end_in_area == false) {
			// recalculate end
			double end_state_x = (data.end_state_y - b) / k;
			visible_cache.push_back({ end_state_x , data.end_state_y });
			visible_cache.push_back({ next_point.x , data.end_state_y });
		}
		else if (data.begin_in_area == false && data.end_in_area == true) {
			// recalculate begin
			double begin_state_x = (data.begin_state_y - b) / k;
			visible_cache.push_back({ begin_state_x , data.begin_state_y });
			visible_cache.push_back(next_point);
		}
		else if (data.begin_in_area == false && data.end_in_area == false) {
			// recalculate begin/end
			if (data.begin_state_y == data.end_state_y) {
				// invisible line
				visible_cache.push_back({ next_point.x , data.end_state_y });
			}
			else {
				double begin_state_x = (data.begin_state_y - b) / k;
				double end_state_x = (data.end_state_y - b) / k;
				visible_cache.push_back({ begin_state_x , data.begin_state_y });
				visible_cache.push_back({ end_state_x , data.end_state_y });
				visible_cache.push_back({ next_point.x , data.end_state_y });
			}
		}

		// next line
		IndexOfPoint++;

		// state flow
		data.begin_in_area = data.end_in_area;
		data.begin_state_y = data.end_state_y;
	}
}

void RenderCache::ThresholdCacheYDirectMode(TraceCache& cache, const ThresholdsYInPixel& thresholds)
{
	std::vector<Vec2d> visible_cache;

	ThresholdCacheYAnalyzeData data;
	data.begin_in_area = true;
	data.begin_state_y = 0;
	data.end_in_area = true;
	data.end_state_y = 0;
	data.thresholds = thresholds;

	ThresholdCacheYAnalyzeEntry(cache, visible_cache, data);
	ThresholdCacheYAnalyzeContinues(cache, visible_cache, data);

	cache.points = visible_cache; // load full content
}

// reject invisible Y-parts of image
void RenderCache::ThresholdCacheY(GraphContext& context)
{
	// reject threshold
	// shouldn't tracking points on boundary plot from "DataTracker"
	constexpr double threshold = 15.;

	// extract world-coordinates Y-range
	// in values: y_min_pixels > y_max_pixels (inverted logic)
	double y_min_pixels = context.GetPlotReferenceOffset().y + threshold;
	double y_max_pixels = context.GetPlotReferenceOffset().y - context.GetPlotSize().y - threshold;

	for (int i = 0; i < caches.size(); ++i) {
		auto& cache = caches[i];

		if (cache.is_active == false) continue;

		// if 1 point -> it's just point
		if (cache.points.size() <= 1) {
			// not valid
			if (cache.points.size() == 0) continue;

			// size == 1
			// logic single point
			// empty line ?
		}

		// here size >= 2
		auto& points = cache.points;

		// scenario ?
		if (cache.is_compressed == true) {
			// just cut Y + threshold ...
			for (size_t i = 0; i < points.size(); ++i) {
				if (points[i].y > y_min_pixels) {
					points[i].y = y_min_pixels;
					continue;
				}
				if (points[i].y < y_max_pixels) {
					points[i].y = y_max_pixels;
				}
			}
		}
		else {
			// linear approximation in the visible region
			// y = k * x + b
			ThresholdsYInPixel thresholds = { y_min_pixels , y_max_pixels };
			ThresholdCacheYDirectMode(caches[i], thresholds);
		}
	}
}

const std::vector<TraceCache>& RenderCache::GetCaches() const { return caches; }

void RenderCache::GenerateRenderCacheData(
	GraphContext& context,
	const TransformCoordinates& coreEngine,
	const LegendItem& li,
	const std::vector<LinearData>& data_pull) {

	// clear caches
	Reset();

	// get X-pixels of current plot
	extractPixelsX(context);

	// get current X-range coordinates in plot (min/max)
	calculateVisibleRangeX(context);

	// get rangeX in PixelX
	calculateRangeXInPixelX();

	for (int i = 0; i < data_pull.size(); ++i) {
		bool active_data = li.IsActive(i);
		trace_id = i;
		if (active_data) {
			CachesManager(context, coreEngine, data_pull[i]);
		}
	}
}