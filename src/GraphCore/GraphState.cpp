#include "GraphState.hpp"
#include <ctime>
#include <iomanip>
#include <algorithm>
#include <chrono>
#include <windowsx.h>

//#define CURSORZOOM

GraphState::GraphState() : transform_coords_(graph_context_) {}

void GraphState::HandlerChangeSizeWindow(LPARAM lParam) {
    window_state_.UpdateWindowSize(graph_context_, lParam);
}

void GraphState::HandlerMouseMove(LPARAM lParam) {
    graph_context_.UpdateMousePosition(lParam);

    if (plot_resizer_.GetStatus()) {
        Vec2d delta = plot_resizer_.UpdateResize(graph_context_.GetMousePosition());
        window_state_.UpdatePlotSize(graph_context_, delta, plot_resizer_.GetReferenceMargins());
        return;
    }

    bool active_plot = window_state_.GetIsPlotArea();
    if (active_plot) {
        tugboat_state_.UpdateDrag(graph_context_, lParam);
        graph_context_.UpdateCoordinates(transform_coords_);
    }
    else {
        tugboat_state_.StopDragging(graph_context_, lParam);
    }
}

void GraphState::HandlerLButtonDown(LPARAM lParam, HWND hwnd) {
    graph_context_.UpdateMousePosition(lParam);

    if (window_state_.GetIsPlotBound()) {
        plot_resizer_.StartResize(graph_context_.GetMousePosition(), window_state_.GetMargins());
    }

    if (plot_resizer_.GetStatus() == false) {
        bool active_plot = window_state_.GetIsPlotArea();
        if (active_plot) {
            tugboat_state_.StartDragging(graph_context_, lParam);
        }

        std::lock_guard<std::mutex> lock(mtx);
        legend_item_.HitCheckAndToggle(graph_context_, hwnd);
    }
}

void GraphState::HandlerLButtonUp(LPARAM lParam) {
    if (plot_resizer_.GetStatus() == true) {
        plot_resizer_.StopResize();
        window_state_.UpdatePlotStates(graph_context_);
    }

    tugboat_state_.StopDragging(graph_context_, lParam);
}

void GraphState::HandlerMouseWheel(LPARAM lParam, WPARAM wParam, HWND hwnd) {
    bool active_plot = window_state_.GetIsPlotArea();
    if (active_plot) {
        int delta = GET_WHEEL_DELTA_WPARAM(wParam);
        ScaleDirection direction = (delta < 0) ?
            ScaleDirection::ZoomOut : ScaleDirection::ZoomIn;

        POINT pt;
        pt.x = GET_X_LPARAM(lParam);
        pt.y = GET_Y_LPARAM(lParam);
        ScreenToClient(hwnd, &pt);

        graph_context_.UpdateMousePosition(pt.x, pt.y);

        zoom_state_.ApplyZoom(
            graph_context_, 
            transform_coords_, 
            tugboat_state_,
            direction,
            window_state_.GetKeysState(UserKeys::_CTRL), 
            window_state_.GetKeysState(UserKeys::_SHIFT));
    }
}

void GraphState::HandlerMouseLeave(LPARAM lParam) {
    tugboat_state_.StopDragging(graph_context_, 0);
    graph_context_.UpdateMousePosition(lParam);
    window_state_.UpdatePlotStates(graph_context_);
    plot_resizer_.StopResize();
}

void GraphState::HandlerKeyDown(WPARAM wParam) {
    switch (wParam) {
    case VK_SHIFT:
        window_state_.ActivateKey(UserKeys::_SHIFT);
        break;
    case VK_CONTROL:
        window_state_.ActivateKey(UserKeys::_CTRL);
        break;
    case 'A':
        window_state_.ActivateKey(UserKeys::_A);
        break;
    }
}

void GraphState::HandlerKeyUp(WPARAM wParam) {
    switch (wParam) {
    case VK_SHIFT:
        window_state_.DisactivateKey(UserKeys::_SHIFT);
        break;
    case VK_CONTROL:
        window_state_.DisactivateKey(UserKeys::_CTRL);
        break;
    case 'A':
        window_state_.DisactivateKey(UserKeys::_A);
        break;
    }
}

void GraphState::InitializeWindowSize(int width, int height) {
    graph_context_.GetWindowParam().SetStartValues(width, height);

    auto size = graph_context_.GetWindowParam().GetCurrentValues();

    RECT margins_area = window_state_.GetMargins();
    RECT new_area = RECT{
        margins_area.left,
        margins_area.top,
        static_cast<long>(size.x - margins_area.right),
        static_cast<long>(size.y - margins_area.bottom)
    };

    graph_context_.SetPlotArea(new_area);

    graph_context_.SetPlotReferenceOffset(Position2d(
        new_area.left,
        new_area.bottom));

    graph_context_.GetPlotParam().SetStartValues(
        size.x - (margins_area.left + margins_area.right),
        size.y - (margins_area.bottom + margins_area.top));
}

void GraphState::InitializeVisibleArea(double x, double y) {
    graph_context_.SetVisibleArea(x, y);
}

void GraphState::InitializeReferencePosition(double x, double y) {
    graph_context_.SetReferencePosition(Vec2d(x, y));
}

void GraphState::DrawCentralPoint(HDC hdc) {
    rwa::PEN pen(hdc, PS_SOLID, 1, RGB(150, 150, 150));
    RECT wspa = graph_context_.GetPlotArea();
    const long center_x = (wspa.left + wspa.right + 1) / 2; // +1 for placement in the center
    const long center_y = (wspa.top + wspa.bottom) / 2;
    Ellipse(hdc, center_x - 1, center_y - 1, center_x + 1, center_y + 1);
}

void GraphState::DrawFPS(HDC hdc, int fps) {
    double alpha = 1. / static_cast<double>(fps_state_.window_smoov_fps);
    double betta = 1. - alpha;
    fps_state_.average_fps = static_cast<double>(fps) * alpha + betta * fps_state_.average_fps;

    std::wstring text = L"FPS: " + std::to_wstring(static_cast<int>(fps_state_.average_fps));

    rwa::FONT font(hdc, 14, 0, 0, 0, FW_BOLD, false, false, false,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Arial");

    SetTextColor(hdc, RGB(0, 255, 0));

    auto ref_plot = graph_context_.GetPlotReferenceOffset();
    auto size_plot = graph_context_.GetPlotSize();
    constexpr double offset_scaler_fps_x = 0.01;
    constexpr double offset_scaler_fps_y = 0.01;
    int x = ref_plot.x + size_plot.x * offset_scaler_fps_x;
    int y = ref_plot.y - size_plot.y + size_plot.y * offset_scaler_fps_y;

    TextOutW(hdc, x, y, text.c_str(), text.length());
}

void GraphState::RenderGraph(HDC hdc)
{
    auto start = std::chrono::high_resolution_clock::now();

    std::lock_guard<std::mutex> lock(mtx);

// >> check & recalculate area if auto scale ON

    auto_scaler_.CorrectAreaX(graph_context_, transform_coords_, data_state_);

    render_cache_.GenerateRenderCacheData(
        graph_context_,
        transform_coords_,
        legend_item_,
        data_state_.GetData()
    );

    render_cache_.ThresholdCacheY(graph_context_);

    auto_scaler_.SwitchActive(window_state_);

// >> begin draw

    RECT rplt = graph_context_.GetPlotArea();
    rwa::RGN rgn(hdc, &rplt);

    axes_state_.LaunchDrawGrids(hdc, graph_context_, transform_coords_);

    // display 1080x1920, Intel i5-7300HQ, 100ê x 3 graphs = 60 fps 
    data_state_.draw(hdc, render_cache_, legend_item_);

    window_state_.DrawPlotBoundary(hdc, graph_context_, transform_coords_);

    rgn.Free();

    axes_state_.LaunchDrawCaptions(hdc, graph_context_, transform_coords_);
    legend_item_.Draw(hdc, graph_context_);

//#ifndef CURSORZOOM
//    DrawCentralPoint(hdc);
//#endif
    constexpr double millisecond = 1e6;
    double duration = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - start).count();
    size_t fps = static_cast<size_t>(millisecond / duration);
    data_tracker_.ShowCoordinates(hdc, graph_context_, transform_coords_, data_state_, window_state_);
    DrawFPS(hdc, fps);

// >> end draw
}

Vec2d GraphState::ExtractWindowSize() {
    return graph_context_.GetWindowSize();
}

CursorType GraphState::ExtractCursorType() {
    window_state_.UpdatePlotStates(graph_context_);
    CursorType ct = window_state_.GetCursorType();
    return ct;
}

bool GraphState::ExtractFlagOfPlotResize() {
    bool plotIsResized = plot_resizer_.GetStatus();
    return plotIsResized;
}

void GraphState::AddData(std::vector<double>& load_data, std::wstring caption, COLORREF color, double step, double offset) {
    std::lock_guard<std::mutex> lock(mtx);
    data_state_.load(load_data, step, offset);
    LegendProperties lp;
    lp.active = true;
    lp.caption = caption;
    lp.color = color;
    legend_item_.Add(lp);
}

size_t GraphState::CreateTrace(std::wstring caption, COLORREF color, double step, double offset) {
    std::lock_guard<std::mutex> lock(mtx);
    std::vector<double> emptyData(0);
    data_state_.load(emptyData, step, offset);
    LegendProperties lp;
    lp.active = true;
    lp.caption = caption;
    lp.color = color;
    legend_item_.Add(lp);

    auto count_traces = legend_item_.GetCountTrace();
    auto new_traces_id = count_traces - 1;
    return new_traces_id;
}

bool GraphState::StreamUpdate(std::vector<double>& load_data, size_t trace_index) {
    std::lock_guard<std::mutex> lock(mtx);
    auto count_traces = legend_item_.GetCountTrace();

    if (trace_index < count_traces) {
        data_state_.stream(load_data, trace_index);
        return true;
    }
    
    return false;
}

bool GraphState::StreamAppend(std::vector<double>& load_data, size_t trace_index) {
    std::lock_guard<std::mutex> lock(mtx);
    auto count_traces = legend_item_.GetCountTrace();

    if (trace_index < count_traces) {
        data_state_.append(load_data, trace_index);
        return true;
    }

    return false;
}