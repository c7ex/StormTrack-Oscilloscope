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

std::pair<bool, Range> AutoScaler::GetTotalRangeX(const DataState& data, WindowState& window)
{
	size_t count_active_valid_traces = 0;
	double init_board = 0;
	for (const LinearData& linear_data : data.GetData()) {
		if (linear_data.GetStatus() && linear_data.size() > 0) {
			count_active_valid_traces++;
			init_board = linear_data[0].x;
			break;
		}
	}

	if (count_active_valid_traces == 0) {
		// output & not apply autoscale
		window.UpdateDataException(DataException::_NAN_DATA);
		return std::make_pair(false, Range{ 0, 0 });
	}

	double minX = init_board;
	double maxX = init_board;

	for (const LinearData& linear_data : data.GetData()) {
		if (!linear_data.GetStatus() || linear_data.size() == 0) continue;
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

	// reject singularity
	// set "DataTracker" to 'Invalid X-Range'
	if (minX == maxX) {
		window.UpdateDataException(DataException::_INVALID_DATA_X_RANGE);
		minX += ConfigUI::AutoScaler::default_singularity_case_x_range_min;
		maxX += ConfigUI::AutoScaler::default_singularity_case_x_range_max;
	}

	return std::make_pair(true, Range{ minX, maxX });
}

void AutoScaler::CorrectAreaX(GraphContext& context, const TransformCoordinates& coreEngine, const DataState& data, WindowState& window)
{
	// Free-fly mode by defaut without autoscale
	window.UpdateDataException(DataException::_VALID_DATA_RANGE);

	if (!active) return;

	std::pair<bool, Range> status_with_rangeX = GetTotalRangeX(data, window);

	if (!status_with_rangeX.first) { return; }

	Range rangeX = status_with_rangeX.second;

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