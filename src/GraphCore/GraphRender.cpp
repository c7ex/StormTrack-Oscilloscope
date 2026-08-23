#include "GraphState.hpp"

#include <ctime>
#include <iomanip>
#include <algorithm>
#include <chrono>

void GraphState::DrawCentralPoint(HDC hdc) {
    rwa::PEN pen(hdc, PS_SOLID, 1, RGB(150, 150, 150));
    RECT wspa = graph_context_.GetPlotArea();
    const long center_x = (wspa.left + wspa.right + 1) / 2; // +1 for placement in the center
    const long center_y = (wspa.top + wspa.bottom) / 2;
    Ellipse(hdc, center_x - 1, center_y - 1, center_x + 1, center_y + 1);
}

void GraphState::Render(HDC hdc)
{
    auto start = std::chrono::high_resolution_clock::now();

    std::lock_guard<std::mutex> lock(mtx);

    auto_scaler_.SwitchActive(window_state_);
    fps_state_.SwitchActive(window_state_);

    // >> check & recalculate area if auto scale ON

    auto_scaler_.CorrectAreaX(graph_context_, transform_coords_, data_state_, window_state_);

    render_cache_.GenerateRenderCacheData(
        graph_context_,
        transform_coords_,
        legend_item_,
        data_state_.GetData()
    );

    render_cache_.ThresholdCacheY(graph_context_);

    // >> begin draw

    RECT rplt = graph_context_.GetPlotArea();
    rwa::RGN rgn(hdc, &rplt);

    rwa::BRUSH bgBrush(hdc, ConfigUI::GeneralGraph::background);
    FillRect(hdc, &rplt, bgBrush.get());

    axes_state_.LaunchDrawGrids(hdc, graph_context_, transform_coords_);

    // display 1080x1920, Intel i5-7300HQ, 100к x 3 graphs = 60 fps 
    data_state_.draw(hdc, render_cache_, legend_item_);

    window_state_.DrawPlotBoundary(hdc, graph_context_, transform_coords_);

    rgn.Free();

    axes_state_.LaunchDrawCaptions(hdc, graph_context_, transform_coords_);
    legend_item_.Draw(hdc, graph_context_);
    data_tracker_.ShowCoordinates(hdc, graph_context_, transform_coords_, data_state_, window_state_);

    //#ifndef CURSORZOOM
    //    DrawCentralPoint(hdc);
    //#endif

    constexpr double millisecond = 1e6;
    double duration = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - start).count();
    size_t fps = static_cast<size_t>(millisecond / duration);
    fps_state_.Draw(graph_context_, hdc, fps);

    // >> end draw
}
