#ifndef AUTO_SCALER_H
#define AUTO_SCALER_H

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
	Range GetTotalRangeX(const DataState& data);

public:
	void SwitchActive(const WindowState& window);

	void CorrectAreaX(GraphContext& context, const TransformCoordinates& coreEngine, const DataState& data);

	void Disactivate();

	bool GetState() const;
};

#endif