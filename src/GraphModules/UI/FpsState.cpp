#include "FpsState.hpp"

void FpsState::SwitchActive(const WindowState& window) {
    bool hotkey = window.GetKeysState(ActionHotKey::visible_fps) ^ ConfigUI::Fps::default_active;
    if (holder != hotkey) {
        if (!holder && hotkey) {
            active = !active;
        }
        holder = hotkey;
    }
}

void FpsState::Draw(GraphContext& context, HDC hdc, int fps) {
    if (!active) return;

    double alpha = 1. / static_cast<double>(window_smoov_fps);
    double betta = 1. - alpha;
    average_fps = static_cast<double>(fps) * alpha + betta * average_fps;

    std::wstring text = L"FPS: " + std::to_wstring(static_cast<int>(average_fps));

    rwa::FONT font(hdc, 14, 0, 0, 0, FW_BOLD, false, false, false,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Arial");

    SetTextColor(hdc, RGB(0, 255, 0));

    auto ref_plot = context.GetPlotReferenceOffset();
    auto size_plot = context.GetPlotSize();
    constexpr double offset_scaler_fps_x = 0.01;
    constexpr double offset_scaler_fps_y = 0.01;
    int x = ref_plot.x + size_plot.x * offset_scaler_fps_x;
    int y = ref_plot.y - size_plot.y + size_plot.y * offset_scaler_fps_y;

    TextOutW(hdc, x, y, text.c_str(), text.length());
}