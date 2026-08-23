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
	bool active = ConfigUI::AutoScaler::default_autoscaler_x_active;
	bool holder = false;

private:
	std::pair<bool, Range> GetTotalRangeX(const DataState& data, WindowState& window);

public:
	void SwitchActive(const WindowState& window);

	void CorrectAreaX(GraphContext& context, const TransformCoordinates& coreEngine, const DataState& data, WindowState& window);

	void Disactivate();

	bool GetState() const;
};

#endif