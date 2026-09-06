#ifndef AUTO_SCALER_H
#define AUTO_SCALER_H

#include <utility>

#include"ConfigUI.hpp"
#include"DataState.hpp"
#include"WindowState.hpp"
#include"GraphContext.hpp"
#include"TransformCoordinates.hpp"

enum UserKeys;
class DataState;
class WindowState;
class GraphContext;
class TransformCoordinates;

struct Range {
	double min;
	double max;
};

class AutoScaler {
	bool active_x = ConfigUI::AutoScaler::default_autoscaler_x_active;
	bool holder_x = false;
	bool active_y = ConfigUI::AutoScaler::default_autoscaler_y_active;
	bool holder_y = false;
	bool active_track = ConfigUI::AutoScaler::default_autoscaler_track_active;
	bool holder_track = false;

private:
	std::pair<bool, Range> GetTotalRangeX(const DataState& data, WindowState& window);

public:
	void SwitchActiveRangeX(const WindowState& window);
	void SwitchActiveRangeY(const WindowState& window);
	void SwitchActiveRangeTrack(const WindowState& window);

	void CorrectAreaX(GraphContext& context, const TransformCoordinates& coreEngine, const DataState& data, WindowState& window);
	void CorrectAreaY(GraphContext& context, const TransformCoordinates& coreEngine, RenderCache& render_cache);

	bool GetStateAutoX() const;
	bool GetStateAutoY() const;
	bool GetStateAutoTrack() const;
};

#endif