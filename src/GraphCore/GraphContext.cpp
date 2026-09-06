#include"GraphContext.hpp"

Size2d GraphContext::GetVisibleArea() const {
    return visible_area_;
}

void GraphContext::SetVisibleArea(Size2d new_visible_area) {
    visible_area_ = new_visible_area;
}

void GraphContext::SetVisibleArea(double width, double height) {
    visible_area_.x = width;
    visible_area_.y = height;
}

Position2d GraphContext::GetCurrentCoordinates() const {
    return current_coord_;
}

void GraphContext::UpdateCoordinates(const TransformCoordinates& coreEngine) {
    Vec2d mp = GetMousePosition();
    current_coord_ = coreEngine.ConvertToWorldCoords(mp.x, mp.y);
}

/////

RECT GraphContext::GetPlotArea() const {
    return plot_area;
}

Position2d GraphContext::GetMousePosition() const {
    return mouse_position;
}

Size2d GraphContext::GetWindowSize() {
    return window_size.GetCurrentValues();
}

RelativeVec2d& GraphContext::GetWindowParam() {
    return window_size;
}

RelativeVec2d& GraphContext::GetPlotParam() {
    return plot_size;
}

Size2d GraphContext::GetPlotSize() const {
    return plot_size.GetCurrentValues();
}

Position2d GraphContext::GetPlotReferenceOffset() const {
    return plot_reference_offset;
}

void GraphContext::SetPlotArea(RECT rt) {
    plot_area = rt;
}

void GraphContext::SetPlotReferenceOffset(Position2d position) {
    plot_reference_offset = position;
}

void GraphContext::UpdateMousePosition(const LPARAM& lParam) {
    mouse_position.x = LOWORD(lParam);
    mouse_position.y = HIWORD(lParam);
}

void GraphContext::UpdateMousePosition(double x, double y) {
    mouse_position.x = x;
    mouse_position.y = y;
}

Position2d GraphContext::GetReferencePosition() const {
    return reference_position;
}

void GraphContext::SetReferencePosition(Position2d position) {
    reference_position = position;
}

void GraphContext::SetOffsetReferencePosition(Offset2d offset) {
    reference_position += offset;
}

double GraphContext::GetTrackVisibleAreaX() const {
    return track_visible_area_x;
}

void GraphContext::SetTrackVisibleAreaX(double new_visible_area_x) {
    track_visible_area_x = new_visible_area_x;
}
