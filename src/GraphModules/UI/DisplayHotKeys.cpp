#include "DisplayHotKeys.hpp"

void DisplayHotKeys::drawSingleHotKeyState(HDC hdc, GraphContext& context, int key, bool active, int& offset_x, int offset_y)
{
    constexpr int hot_key_x_step = 20;

    if(active)
        SetTextColor(hdc, RGB(138, 43, 226));
    else
        SetTextColor(hdc, RGB(128, 110, 150));

    std::wstring text { static_cast<wchar_t>(key) };

    if (key == VK_CONTROL || key == VK_SHIFT) {
		text = (key == VK_CONTROL) ? L"Ctrl" : L"Shift";
    }

    TextOutW(hdc, offset_x, offset_y, text.c_str(), text.length());

    offset_x += text.length() * hot_key_x_step;
}

void DisplayHotKeys::drawHotKeyStates(HDC hdc, GraphContext& context, const AutoScaler& autoscaler, const FpsState& fps_state) {
	std::wstring text = L"X";
    
    rwa::FONT font(hdc, 16, 0, 0, 0, FW_BOLD, false, false, false,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Arial");

    auto ref_plot = context.GetPlotReferenceOffset();
    auto size_plot = context.GetPlotSize();

    constexpr int hot_key_y_offset = 20;
    int y = ref_plot.y - size_plot.y - hot_key_y_offset;

    constexpr double offset_scaler_fps_x = 0.01;
    int x = ref_plot.x + size_plot.x * offset_scaler_fps_x;

	// variable x - autochange inside
    drawSingleHotKeyState(hdc, context, ConfigUI::BindHotKeys::toggle_autoscale_x, autoscaler.GetStateAutoX(), x, y);
    drawSingleHotKeyState(hdc, context, ConfigUI::BindHotKeys::toggle_autoscale_y, autoscaler.GetStateAutoY(), x, y);
    drawSingleHotKeyState(hdc, context, ConfigUI::BindHotKeys::toggle_autoscale_track, autoscaler.GetStateAutoTrack(), x, y);
    drawSingleHotKeyState(hdc, context, ConfigUI::BindHotKeys::toggle_visible_fps, fps_state.GetState(), x, y);

	// for 'Shift' and 'Ctrl' keys
	// can be used in future
    //drawSingleHotKeyState(hdc, context, ConfigUI::BindHotKeys::toggle_fast_zoom, (GetKeyState(VK_SHIFT) & 0x8000), x, y);
    //drawSingleHotKeyState(hdc, context, ConfigUI::BindHotKeys::toggle_x_mode_zoom, (GetKeyState(VK_CONTROL) & 0x8000), x, y);
}