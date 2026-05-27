#ifndef RENDER_CACHE_HPP
#define RENDER_CACHE_HPP

#include"Vec2d.hpp"
#include"LegendItem.hpp"
#include"LinearData.hpp"
#include"GraphContext.hpp"
#include"TransformCoordinates.hpp"

class LegendItem;
class LinearData;
class GraphContext;
class TransformCoordinates;

struct VisibleRangeX {
	double min_value_x;
	double max_value_x;
	double range_x;
};

struct EffectiveIndices {
	size_t min_index;
	size_t max_index;
};

struct ThresholdsYInPixel {
	double min;
	double max;
};

struct ThresholdCacheYAnalyzeData { // for optimization in DirectMode Y-caches
	bool begin_in_area;
	double begin_state_y;
	bool end_in_area;
	double end_state_y;
	ThresholdsYInPixel thresholds;
};

struct TraceCache {
	std::vector<Vec2d> points;
	size_t trace_id;
	double min_x;
	double max_x;
	double min_y;
	double max_y;
	bool is_active;
	bool is_compressed;
};

class RenderCache {
private:
	std::vector<TraceCache> caches;

private:
	EffectiveIndices ei;
	VisibleRangeX vrX;
	size_t pixelsX;
	double rangeXInPixelX;
	int trace_id;

private:
	void extractPixelsX(GraphContext& context);
	void calculateVisibleRangeX(GraphContext& context);
	void calculateRangeXInPixelX();
	double calculateCompressedScale(const LinearData& ldata);
	void calculateEffectiveIndices(const LinearData& ldata, const Position2d ref, const Size2d area);

private:
	void CreateScenarioCacheDirect(const TransformCoordinates& coreEngine, const LinearData& data);
	void CreateScenarioCacheCompressed(const TransformCoordinates& coreEngine, const LinearData& data, double compressedScale);
	void CachesManager(GraphContext& context, const TransformCoordinates& coreEngine, const LinearData& data);

private:
	void ThresholdCacheYAnalyzeEntry(const TraceCache& cache, std::vector<Vec2d>& visible_cache, ThresholdCacheYAnalyzeData& data);
	void ThresholdCacheYAnalyzeContinues(const TraceCache& cache, std::vector<Vec2d>& visible_cache, ThresholdCacheYAnalyzeData& data);
	void ThresholdCacheYDirectMode(TraceCache& cache, const ThresholdsYInPixel& thresholds);

public:
	void ThresholdCacheY(GraphContext& context);

private:
	void Reset();

public:
	const std::vector<TraceCache>& GetCaches() const;

public:
	void GenerateRenderCacheData(
		GraphContext& context, 
		const TransformCoordinates& coreEngine, 
		const LegendItem& li, 
		const std::vector<LinearData>& data_pull);
};

#endif