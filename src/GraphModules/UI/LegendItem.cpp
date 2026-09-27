#include"LegendItem.hpp"

void LegendItem::Draw(HDC hdc, GraphContext& context, const DataState& data) {
    RECT plot_area = context.GetPlotArea();
    Size2d window_size = context.GetWindowSize();

    int right_margin = window_size.x * ConfigUI::LegendItem::background_right_padding;
    int left_margin = window_size.x * ConfigUI::LegendItem::background_left_padding;

    int legend_width = window_size.x - plot_area.right - left_margin - right_margin;
    if (legend_width < 1) return;

    RECT rect = {
        plot_area.right + left_margin,
        plot_area.top,
        window_size.x - right_margin,
        plot_area.bottom
    };

    DrawBackground(hdc, rect);
    DrawFrame(hdc, rect);

    auto count_traces = data.GetDataSize();
    if (count_traces == 0) return;

    const int start_x = rect.left + ConfigUI::LegendItem::padding_start_x;
    const int start_y = rect.top + ConfigUI::LegendItem::padding_start_y;

    int available_width = rect.right - start_x - ConfigUI::LegendItem::right_offset;

    bool show_color_box = (available_width >= ConfigUI::LegendItem::item_size);
    bool show_text = (available_width >= ConfigUI::LegendItem::item_size + ConfigUI::LegendItem::text_freespace);

    if (!show_color_box) return;

    for (size_t i = 0; i < count_traces; ++i) {
        int y = start_y + i * ConfigUI::LegendItem::step_checkbox_y;
        DrawColorBox(hdc, start_x, y, ConfigUI::LegendItem::item_size, data.GetData()[i]);

        if (show_text) {
            DrawCaptions(
                hdc, start_x + ConfigUI::LegendItem::item_size + ConfigUI::LegendItem::spacing, y, data.GetData()[i].getParameters().prop.caption);
        }
    }
}

void LegendItem::DrawBackground(HDC hdc, const RECT& rect) {
    rwa::BRUSH brush(hdc, ConfigUI::LegendItem::background);
    FillRect(hdc, &rect, brush.get());
}

void LegendItem::DrawFrame(HDC hdc, const RECT& rect) {
    FrameRect(hdc, &rect, (HBRUSH)GetStockObject(NULL_BRUSH));
}

void LegendItem::DrawColorBox(HDC hdc, int x, int y, int size, const LinearData& data) {
    RECT color_rect = { x, y, x + size, y + size };
    rwa::PEN pen(hdc, PS_SOLID, 2, data.getParameters().prop.color);
    if (data.getParameters().prop.active) {
        rwa::BRUSH color_brush(hdc, data.getParameters().prop.color);
        Rectangle(hdc, color_rect.left, color_rect.top, color_rect.right, color_rect.bottom);
    } else {
        rwa::BRUSH null_brush(hdc, ConfigUI::LegendItem::unactive_checkbox);
        Rectangle(hdc, color_rect.left, color_rect.top, color_rect.right, color_rect.bottom);
    }
}

void LegendItem::DrawCaptions(HDC hdc, int x, int y, const std::wstring& text) {
    SetTextColor(hdc, ConfigUI::LegendItem::captions_color);
    SetBkMode(hdc, TRANSPARENT);
    TextOut(hdc, x, y, text.c_str(), (int)text.length());
}

void LegendItem::HitCheckAndToggle(GraphContext& context, HWND hwnd, DataState& data) {
    RECT plot_area = context.GetPlotArea();
    Size2d window_size = context.GetWindowSize();
    Position2d mouse = context.GetMousePosition();

    int right_margin = window_size.x * ConfigUI::Margins::clip_scale_x;
    int left_margin = window_size.x * ConfigUI::Margins::clip_scale_x;

    int legend_width = window_size.x - plot_area.right - left_margin - right_margin;
    if (legend_width < 1) return;

    RECT rect = {
        plot_area.right + left_margin,
        plot_area.top,
        window_size.x - right_margin,
        plot_area.bottom
    };

    if (mouse.x < rect.left || mouse.x > rect.right) return;
    if (mouse.y < rect.top || mouse.y > rect.bottom) return;

    const int start_x = rect.left + ConfigUI::LegendItem::padding_start_x;
    const int start_y = rect.top + ConfigUI::LegendItem::padding_start_y;

    int available_width = rect.right - start_x - ConfigUI::LegendItem::right_offset;
    bool show_color_box = (available_width >= ConfigUI::LegendItem::item_size);

    if (!show_color_box) return;

    int index = (mouse.y - start_y) / ConfigUI::LegendItem::step_checkbox_y;

    if (index < 0 || index >= (int)data.GetDataSize()) return;

    int item_x = start_x;
    int item_y = start_y + index * ConfigUI::LegendItem::step_checkbox_y;

    RECT color_rect = { item_x, item_y, item_x + ConfigUI::LegendItem::item_size, item_y + ConfigUI::LegendItem::item_size };

    if (mouse.x >= color_rect.left && mouse.x <= color_rect.right &&
        mouse.y >= color_rect.top && mouse.y <= color_rect.bottom) {

        if (GetAsyncKeyState(VK_CONTROL) & 0x8000) {
            for (size_t i = 0; i < data.GetDataSize(); ++i) {
                bool state = data.GetActiveState(i);
                data.SetActiveState(i, !state);
            }
        }

        bool state = data.GetActiveState(index);
        data.SetActiveState(index, !state);

        InvalidateRect(hwnd, NULL, TRUE);
    }
}