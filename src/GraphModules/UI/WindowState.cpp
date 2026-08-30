#include"WindowState.hpp"

RECT WindowState::GetMargins() const {
    return margins;
}

// change margins -> reload plot form
void WindowState::UpdatePlotForm(GraphContext& context) {
    auto size = context.GetWindowParam().GetCurrentValues();

    RECT new_area = RECT{
        margins.left,
        margins.top,
        static_cast<long>(size.x - margins.right),
        static_cast<long>(size.y - margins.bottom)
    };

    context.SetPlotArea(new_area);

    context.SetPlotReferenceOffset(Position2d(
        new_area.left,
        new_area.bottom));

    context.GetPlotParam().UpdateCurrentValues(
        size.x - (margins.left + margins.right),
        size.y - (margins.bottom + margins.top));
}

void WindowState::UpdateCursor(CursorType new_cursor_type, PlotSideType new_plot_side_type) {
    if (new_cursor_type != cursor_type) {
        cursor_type = new_cursor_type;
        plot_side_type = new_plot_side_type;
    }
}

bool WindowState::UpdateCursor(CursorType new_cursor_type) {
    if (new_cursor_type != cursor_type) {
        cursor_type = new_cursor_type;
        return true;
    }
    return false;
}

void WindowState::UpdateDataException(DataException new_data_exception) {
    data_exception = new_data_exception;
}

void WindowState::UpdatePlotStates(GraphContext& context)
{
    auto mouse_position = context.GetMousePosition();
    auto plot_area = context.GetPlotArea();

    is_plot_bound = false;
    is_plot_area = false;

    if ((mouse_position.x > plot_area.left) && (mouse_position.x < plot_area.right)) {
        if ((mouse_position.y > plot_area.top) && (mouse_position.y < plot_area.bottom)) {
            UpdateCursor(CursorType::_CROSS, PlotSideType::_NAN);
            is_plot_area = true;
            return;
        }
    }

    auto top_board = plot_area.top - ConfigUI::Plot::width_board;
    auto left_board = plot_area.left - ConfigUI::Plot::width_board;
    auto right_board = plot_area.right + ConfigUI::Plot::width_board;
    auto bottom_board = plot_area.bottom + ConfigUI::Plot::width_board;

    bool top_sign = (mouse_position.y <= plot_area.top) && (mouse_position.y > top_board);
    bool left_sign = (mouse_position.x > left_board) && (mouse_position.x <= plot_area.left);
    bool right_sign = (mouse_position.x >= plot_area.right) && (mouse_position.x < right_board);
    bool bottom_sign = (mouse_position.y < bottom_board) && (mouse_position.y >= plot_area.bottom);
    
    if (left_sign || right_sign) {
        if ((mouse_position.y > plot_area.top) && (mouse_position.y < plot_area.bottom)) {
            UpdateCursor(CursorType::_WE, left_sign ? PlotSideType::_LEFT : PlotSideType::_RIGHT);
            is_plot_bound = true;
            return;
        }
    }

    if (top_sign || bottom_sign) {
        if ((mouse_position.x > plot_area.left) && (mouse_position.x < plot_area.right)) {
            UpdateCursor(CursorType::_NS, top_sign ? PlotSideType::_TOP : PlotSideType::_BOT);
            is_plot_bound = true;
            return;
        }
    }

    if ((left_sign && top_sign) || (right_sign && bottom_sign)) {
        UpdateCursor(CursorType::_NWSE, (left_sign && top_sign) ? PlotSideType::_TOP_LEFT : PlotSideType::_BOT_RIGHT);
        is_plot_bound = true;
        return;
    }

    if ((left_sign && bottom_sign) || (right_sign && top_sign)) {
        UpdateCursor(CursorType::_NESW, (left_sign && bottom_sign) ? PlotSideType::_BOT_LEFT : PlotSideType::_TOP_RIGHT);
        is_plot_bound = true;
        return;
    }

    UpdateCursor(CursorType::_ARROW, PlotSideType::_NAN);
}

void WindowState::UpdateWindowSize(GraphContext& context, const LPARAM lParam) {
    Size2d new_size(LOWORD(lParam), HIWORD(lParam));
    context.GetWindowParam().UpdateCurrentValues(new_size);

    Scale2d scale = context.GetWindowParam().GetScales();

    margins.left = base_margins.left * scale.x;
    margins.top = base_margins.top * scale.y;
    margins.right = base_margins.right * scale.x;
    margins.bottom = base_margins.bottom * scale.y;

    RECT plot_area = {
        static_cast<long>(margins.left),
        static_cast<long>(margins.top),
        static_cast<long>(new_size.x - margins.right),
        static_cast<long>(new_size.y - margins.bottom)
    };

    context.SetPlotArea(plot_area);

    Position2d plot_reference_offset = Position2d(plot_area.left, plot_area.bottom);
    context.SetPlotReferenceOffset(plot_reference_offset);

    Size2d new_plot_size(
        plot_area.right - plot_area.left,
        plot_area.bottom - plot_area.top
    );

    context.GetPlotParam().UpdateCurrentValues(new_plot_size);
}

void WindowState::UpdatePlotSize(GraphContext& context, Vec2d delta, RECT ref_margins) {
    Size2d window = context.GetWindowParam().GetCurrentValues();
    Scale2d scale = context.GetWindowParam().GetScales();

    switch (plot_side_type) {
    case PlotSideType::_LEFT:
        margins.left = ref_margins.left - delta.x;
        break;
    case PlotSideType::_RIGHT:
        margins.right = ref_margins.right + delta.x;
        break;
    case PlotSideType::_TOP:
        margins.top = ref_margins.top - delta.y;
        break;
    case PlotSideType::_BOT:
        margins.bottom = ref_margins.bottom + delta.y;
        break;
    case PlotSideType::_TOP_LEFT:
        margins.left = ref_margins.left - delta.x;
        margins.top = ref_margins.top - delta.y;
        break;
    case PlotSideType::_TOP_RIGHT:
        margins.right = ref_margins.right + delta.x;
        margins.top = ref_margins.top - delta.y;
        break;
    case PlotSideType::_BOT_LEFT:
        margins.left = ref_margins.left - delta.x;
        margins.bottom = ref_margins.bottom + delta.y;
        break;
    case PlotSideType::_BOT_RIGHT:
        margins.right = ref_margins.right + delta.x;
        margins.bottom = ref_margins.bottom + delta.y;
        break;
    default:
        return;
    }

    int min_margin_x = window.x * ConfigUI::Margins::clip_scale_x;
    int min_margin_y = window.y * ConfigUI::Margins::clip_scale_y;

    int min_plot_width = window.x * ConfigUI::Plot::minumal_x;
    int min_plot_height = window.y * ConfigUI::Plot::minumal_y;

    if (margins.left < min_margin_x) margins.left = min_margin_x;
    if (margins.top < min_margin_y) margins.top = min_margin_y;
    if (margins.right < min_margin_x) margins.right = min_margin_x;
    if (margins.bottom < min_margin_y) margins.bottom = min_margin_y;

    if (window.x - margins.left - margins.right < min_plot_width) {
        if (plot_side_type == PlotSideType::_LEFT || plot_side_type == PlotSideType::_TOP_LEFT || plot_side_type == PlotSideType::_BOT_LEFT)
            margins.left = window.x - margins.right - min_plot_width;
        else
            margins.right = window.x - margins.left - min_plot_width;
    }

    if (window.y - margins.top - margins.bottom < min_plot_height) {
        if (plot_side_type == PlotSideType::_TOP || plot_side_type == PlotSideType::_TOP_LEFT || plot_side_type == PlotSideType::_TOP_RIGHT)
            margins.top = window.y - margins.bottom - min_plot_height;
        else
            margins.bottom = window.y - margins.top - min_plot_height;
    }

    base_margins.left = margins.left / scale.x;
    base_margins.top = margins.top / scale.y;
    base_margins.right = margins.right / scale.x;
    base_margins.bottom = margins.bottom / scale.y;

    UpdatePlotForm(context);
}

bool WindowState::GetIsPlotArea() const {
    return is_plot_area;
}

bool WindowState::GetIsPlotBound() const {
    return is_plot_bound;
}

CursorType WindowState::GetCursorType() const {
    return cursor_type;
}

DataException WindowState::GetDataException() const {
	return data_exception;
}

bool WindowState::GetKeysState(ActionHotKey key) const {
    switch (key) {
        case ActionHotKey::fast_zoom: { return keys_state.fast_zoom; }
        case ActionHotKey::x_zoom: { return keys_state.x_zoom; }
        case ActionHotKey::autoscale_x: { return keys_state.autoscale_x; }
        case ActionHotKey::autoscale_y: { return keys_state.autoscale_y; }
        case ActionHotKey::visible_fps: { return keys_state.visible_fps; }
    }
}

void WindowState::ActivateKey(ActionHotKey key) {
    switch (key) {
        case ActionHotKey::fast_zoom: { keys_state.fast_zoom = true; break; }
        case ActionHotKey::x_zoom: { keys_state.x_zoom = true; break; }
        case ActionHotKey::autoscale_x: { keys_state.autoscale_x = true; break; }
        case ActionHotKey::autoscale_y: { keys_state.autoscale_y = true; break; }
        case ActionHotKey::visible_fps: { keys_state.visible_fps = true; break; }

    }
}

void WindowState::DisactivateKey(ActionHotKey key) {
    switch (key) {
        case ActionHotKey::fast_zoom: { keys_state.fast_zoom = false; break; }
        case ActionHotKey::x_zoom: { keys_state.x_zoom = false; break; }
        case ActionHotKey::autoscale_x: { keys_state.autoscale_x = false; break; }
        case ActionHotKey::autoscale_y: { keys_state.autoscale_y = false; break; }
        case ActionHotKey::visible_fps: { keys_state.visible_fps = false; break; }
    }
}

void WindowState::DrawPlotBoundary(HDC hdc, GraphContext& context, const TransformCoordinates coreEngine)
{
    RECT plot = context.GetPlotArea();

    int top = plot.top;
    int left = plot.left;
    int right = plot.right;
    int bottom = plot.bottom;

    COLORREF color = is_plot_area ? GridConfig::BORDER_ACTIVE : GridConfig::BORDER_UNACTIVE;
    rwa::PEN pen(hdc, PS_SOLID, ConfigUI::Plot::width_board, color);

    MoveToEx(hdc, left, bottom - 1, NULL);
    LineTo(hdc, right - 1, bottom - 1);
    LineTo(hdc, right - 1, top);
    LineTo(hdc, left, top);
    LineTo(hdc, left, bottom);
}