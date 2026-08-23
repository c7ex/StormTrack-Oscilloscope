#include"AutoScaler.hpp"

void AutoScaler::SwitchActive(const WindowState& window) {
	bool hotkey = window.GetKeysState(UserKeys::_A) ^ ConfigUI::AutoScaler::default_autoscaler_x_active;
	if (holder != hotkey) {
		if (!holder && hotkey) {
			active = !active;
		}
		holder = hotkey;
	}
}

Range AutoScaler::GetTotalRangeX(const DataState& data) {
	double init_board = data.GetData()[0][0].x;
	double minX = init_board;
	double maxX = init_board;

	for (const LinearData& linear_data : data.GetData()) {
		if (!linear_data.GetStatus()) continue;
		size_t first_index = 0;
		double current_minX = linear_data[first_index].x;
		if (current_minX < minX) minX = current_minX;
		double current_maxX = current_minX;
		if (current_maxX > maxX) maxX = current_maxX;
		size_t last_index = linear_data.size() - 1;
		if ((int64_t)last_index < 0) continue;
		current_maxX = linear_data[last_index].x;
		if (current_maxX > maxX) maxX = current_maxX;
	}

	return Range{ minX, maxX };
}

void AutoScaler::CorrectAreaX(GraphContext& context, const TransformCoordinates& coreEngine, const DataState& data, WindowState& window)
{
	// Free-fly mode by defaut without autoscale
	window.UpdateDataException(DataException::_VALID_DATA_RANGE);

	if (!active) return;
	
	size_t count_active_traces = 0;
	for (const LinearData& linear_data : data.GetData()) {
		if (linear_data.GetStatus()) {
			count_active_traces++;
		}
	}

	if (count_active_traces == 0) {
		window.UpdateDataException(DataException::_NAN_DATA);
		return;
	}

	Range rangeX = GetTotalRangeX(data);

	// reject singularity
	// set "DataTracker" to 'Invalid X-Range'
	if (rangeX.min == rangeX.max) {
		window.UpdateDataException(DataException::_INVALID_DATA_X_RANGE);
		rangeX.min += ConfigUI::AutoScaler::default_singularity_case_x_range_min;
		rangeX.max += ConfigUI::AutoScaler::default_singularity_case_x_range_max;
	}

	auto current_area = context.GetVisibleArea();
	auto current_ref = context.GetReferencePosition();

	double deltaX = rangeX.max - rangeX.min;
	Position2d new_ref = Position2d{ rangeX.min, current_ref.y };
	Position2d new_area = Position2d{ deltaX, current_area.y };

	context.SetReferencePosition(new_ref);
	context.SetVisibleArea(new_area);
}

void AutoScaler::Disactivate() {
	active = false;
	holder = false;
}

bool AutoScaler::GetState() const {
	return active;
}