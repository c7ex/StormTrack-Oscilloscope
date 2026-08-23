#include"GraphState.hpp"

void DataTracker::DrawDot(HDC hdc, int x, int y, int radius, COLORREF color) {
    rwa::BRUSH brush(hdc, color);
    Ellipse(hdc, x - radius, y - radius, x + radius, y + radius);
}

void DataTracker::DrawLabel(HDC hdc, const Position2d& mouse_position,
    const std::wstring& text, COLORREF bgColor) {
    SIZE textSize;
    GetTextExtentPoint32W(hdc, text.c_str(), text.length(), &textSize);
    int left = mouse_position.x + ConfigUI::DataTracker::offsetX;
    int top = mouse_position.y + ConfigUI::DataTracker::offsetY;
    int right = left + textSize.cx + 2 * ConfigUI::DataTracker::padding;
    int bottom = top + textSize.cy + 2 * ConfigUI::DataTracker::padding;

    rwa::PEN pen(hdc, PS_SOLID, 1, ConfigUI::DataTracker::borderColor);
    rwa::BRUSH brush(hdc, bgColor);
    Rectangle(hdc, left, top, right, bottom);

    SetTextColor(hdc, ConfigUI::DataTracker::textColor);
    TextOutW(hdc,
        left + ConfigUI::DataTracker::padding,
        top + ConfigUI::DataTracker::padding,
        text.c_str(), text.length());
}

void DataTracker::SearchPoint(
    const LinearData& data, 
    const TransformCoordinates& coreEngine, 
    const Position2d& mouse_position,
    size_t index_point,
    size_t index_trace) 
{
    Position2d data_pos = data[index_point];
    Position2d data_mouse_pos = coreEngine.ConvertToPixelCoords(data_pos.x, data_pos.y);
    double range = VecModule(mouse_position, data_mouse_pos);
    if (range < ConfigUI::DataTracker::radiusTrackDataInPixels) {
        DataSearchResult currentResult = DataSearchResult{ index_trace, index_point, range };
        search_data_result.Update(currentResult);
    }
}

double DataTracker::VecModule(const Vec2d& p1, const Vec2d& p2) {
    Size2d distance_point = p1 - p2;
    double distance_squa_x = distance_point.x * distance_point.x;
    double distance_squa_y = distance_point.y * distance_point.y;
    double distance = sqrt(distance_squa_x + distance_squa_y);
    return distance;
}

void DataTracker::SearchNearestLinearData(GraphContext& context, const TransformCoordinates& coreEngine, const LinearData& data)
{
    LinearDataParameters ldp = data.getParameters();
    if (!ldp.active) return;

    Position2d mouse_position = context.GetMousePosition();
    Position2d coordinates = coreEngine.ConvertToWorldCoords(mouse_position.x, mouse_position.y);

    Position2d reference = context.GetReferencePosition();
    Size2d area = context.GetVisibleArea();
    Size2d plot = context.GetPlotSize();

    // pixel = {X} coordinates range
    Scale2d pixelStepInCoordRange = area / plot;

    // range area in coordinates
    Position2d max_area = reference + area;
    Position2d min_area = reference;

    DataSearchResult currentResult;

    // step != 0
    if (ldp.step != 0){
        size_t index_trace = data.GetIndexTrace();

        double halfWindowTrackX = static_cast<double>(ConfigUI::DataTracker::radiusTrackDataInPixels) * pixelStepInCoordRange.x;
        double min_track_x = coordinates.x - halfWindowTrackX;
        double max_track_x = coordinates.x + halfWindowTrackX;

        // protect_area
        if (min_track_x < min_area.x) { min_track_x = min_area.x; }
        if (max_track_x > max_area.x) { max_track_x = max_area.x; }

        // check min
        int64_t min_track_index = data.ReturnIndex(min_track_x);
        if (min_track_index == -1) { min_track_index = 0; }

        // check max
        int64_t max_track_index = data.ReturnIndex(max_track_x);
        if (max_track_index == -1) { max_track_index = data.size() - 1; }

        // here need found nearest range form [min_track_index: max_track_index]
        // 1. if (max_track_index - min_track_index > 16) it's very long, do compression search
        // 2. if (max_track_index - min_track_index > 1024) - off search

        int64_t count_indexes = (max_track_index - min_track_index) + 1;
        if (count_indexes <= 0 || count_indexes > ConfigUI::DataTracker::maximalSearchCountPounts) { /*no valid solution*/ return; }

        /*
           -------------------------------
           calculate in pixel coordination
           -------------------------------
                                           */

        // single point case
        if (count_indexes == 1) {
            SearchPoint(data, coreEngine, mouse_position, min_track_index, index_trace);
            return;
        }

        // if radius = 6, MaxCheckDataPoints = 16
        // if radius = 8, MaxCheckDataPoints = 32
        // ...
        const int countMaxCheckDataPoints = pow(2, std::floor(std::log2(ConfigUI::DataTracker::radiusTrackDataInPixels * 2)) + 1);

        if (count_indexes > countMaxCheckDataPoints) {
            double float_index_step = static_cast<double>(count_indexes) / static_cast<double>(countMaxCheckDataPoints);
            double current_index = static_cast<double>(min_track_index);

            // not include last point!
            for (size_t j = 0; j < (static_cast<unsigned long long>(countMaxCheckDataPoints) - 1); ++j) {
                SearchPoint(data, coreEngine, mouse_position, static_cast<size_t>(current_index), index_trace);
                current_index += float_index_step;
            }

            // last point
            SearchPoint(data, coreEngine, mouse_position, max_track_index, index_trace);
        }
        else {
            for (size_t j = min_track_index; j <= max_track_index; ++j) {
                SearchPoint(data, coreEngine, mouse_position, j, index_trace);
            }
        }
    }
}

// return or mouse position in world coordination
// or nearest data position
Position2d DataTracker::TryHoldNearestData(HDC hdc, GraphContext& context, const TransformCoordinates& coreEngine, DataState& data, const WindowState& ws) {
    Position2d mouse_position = context.GetMousePosition();
    Position2d coordinates = coreEngine.ConvertToWorldCoords(mouse_position.x, mouse_position.y);

    auto linear_data = data.GetData();
    auto size_linear_data = data.GetData().size();

    search_data_result.Reset();

    for (size_t i = 0; i < size_linear_data; ++i) {
        const LinearData& current_linear_data = linear_data[i];
        SearchNearestLinearData(context, coreEngine, current_linear_data);
    }

    if (search_data_result.nearest_range >= 0) {
        // data is tracking
        coordinates = linear_data[search_data_result.data_trace_id][search_data_result.data_index];
        auto track_point = coreEngine.ConvertToPixelCoords(coordinates);
        COLORREF colorData = linear_data[search_data_result.data_trace_id].GetColor();
        DrawDot(hdc, track_point.x, track_point.y, ConfigUI::DataTracker::radiusTrackDataInPixels, colorData);
    }

    return coordinates;
}

void DataTracker::ShowCoordinates(HDC hdc, GraphContext& context, const TransformCoordinates& coreEngine, DataState& data, const WindowState& ws)
{
    if (ws.GetIsPlotArea())
    {
        Position2d mouse_position = context.GetMousePosition();

        switch (ws.GetDataException())
        {
            case DataException::_VALID_DATA_RANGE: {
                Position2d coordinates = TryHoldNearestData(hdc, context, coreEngine, data, ws);

                COLORREF bgColor = search_data_result.nearest_range >= 0
                    ? (data.GetData()[search_data_result.data_trace_id].GetColor())
                    : ConfigUI::DataTracker::freefly_backgroundColor;

                std::wstring coord_text = L"" + std::to_wstring(coordinates.x) + L", " +
                    std::to_wstring(coordinates.y) + L"";
                int oldBkMode = SetBkMode(hdc, TRANSPARENT);
                COLORREF oldTextColor = SetTextColor(hdc, ConfigUI::DataTracker::textColor);

                LOGFONT lf = {};
                GetObject(GetStockObject(DEFAULT_GUI_FONT), sizeof(LOGFONT), &lf);
                lf.lfWeight = (search_data_result.nearest_range >= 0) ? FW_BOLD : FW_NORMAL;
                rwa::FONT font(hdc, lf);

                DrawLabel(hdc, mouse_position, coord_text, bgColor);

                SetBkMode(hdc, oldBkMode);
                SetTextColor(hdc, oldTextColor);
                break;
            }
        
			case DataException::_INVALID_DATA_X_RANGE: {
				std::wstring warning_text = L"Invalid X-Range, trying auto scale";
				int oldBkMode = SetBkMode(hdc, TRANSPARENT);
				COLORREF oldTextColor = SetTextColor(hdc, ConfigUI::DataTracker::textColor);
				LOGFONT lf = {};
				GetObject(GetStockObject(DEFAULT_GUI_FONT), sizeof(LOGFONT), &lf);
				lf.lfWeight = FW_BOLD;
				rwa::FONT font(hdc, lf);

                DrawLabel(hdc, mouse_position, warning_text, ConfigUI::DataTracker::warning_backgroundColor);

				SetBkMode(hdc, oldBkMode);
				SetTextColor(hdc, oldTextColor);
				break;
			}

			case DataException::_NAN_DATA: {
				std::wstring warning_text = L"No data or all traces hidden, trying auto scale";
				int oldBkMode = SetBkMode(hdc, TRANSPARENT);
				COLORREF oldTextColor = SetTextColor(hdc, ConfigUI::DataTracker::textColor);
				LOGFONT lf = {};
				GetObject(GetStockObject(DEFAULT_GUI_FONT), sizeof(LOGFONT), &lf);
				lf.lfWeight = FW_BOLD;
				rwa::FONT font(hdc, lf);

                DrawLabel(hdc, mouse_position, warning_text, ConfigUI::DataTracker::warning_backgroundColor);

				SetBkMode(hdc, oldBkMode);
				SetTextColor(hdc, oldTextColor);
				break;
			}

			default:
				break;
        }
    }
}