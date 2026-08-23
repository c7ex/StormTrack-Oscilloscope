#ifndef GRAPHSTATE_H
#define GRAPHSTATE_H

#include <mutex>

#include "RaiiWinApi.hpp"
#include "ZoomState.hpp"
#include "AxesState.hpp"
#include "RenderCache.hpp"
#include "DataState.hpp"
#include "LegendItem.hpp"
#include "TugboatState.hpp"
#include "PlotResizer.hpp"
#include "AutoScaler.hpp"
#include "WindowState.hpp"
#include "DataTracker.hpp"
#include "FpsState.hpp"
#include "GraphContext.hpp"
#include "TransformCoordinates.hpp"

class GraphState {
private:
    std::mutex mtx;

private:
    TransformCoordinates transform_coords_;
    GraphContext graph_context_;

private:
    ZoomState zoom_state_;
    AxesState axes_state_;
    RenderCache render_cache_;
    DataState data_state_;
    LegendItem legend_item_;
    TugboatState tugboat_state_;
    PlotResizer plot_resizer_;
    AutoScaler auto_scaler_;
    WindowState window_state_;
    DataTracker data_tracker_;
    FpsState fps_state_;

public:
    GraphState();

public:
    void HandlerChangeSizeWindow(LPARAM lParam);
    void HandlerMouseMove(LPARAM lParam);
    void HandlerLButtonDown(LPARAM lParam, HWND hwnd);
    void HandlerLButtonUp(LPARAM lParam);
    void HandlerMouseWheel(LPARAM lParam, WPARAM wParam, HWND hwnd);
    void HandlerMouseLeave(LPARAM lParam);
    void HandlerKeyDown(WPARAM wParam);
    void HandlerKeyUp(WPARAM wParam);

    void InitializeWindowSize(int width, int height);
    void InitializeVisibleArea(double x, double y);
    void InitializeReferencePosition(double x, double y);

    Vec2d ExtractWindowSize();
    CursorType ExtractCursorType();
    
    bool CursorMonitor(CursorType new_cursor_type);
    bool PlotAreaActiveStatus() const;
    bool ExtractFlagOfPlotResize() const;
    bool ExtractFlagOfDataTrack() const;

    size_t CreateTrace(std::wstring caption, COLORREF color, double step = 1.0, double offset = 0.0);

    void AddData(std::vector<double>& load_data, std::wstring caption, COLORREF color, double step = 1, double offset = 0);
    bool StreamUpdate(std::vector<double>& load_data, size_t trace_index);
    bool StreamAppend(std::vector<double>& load_data, size_t trace_index);
    bool StreamAppend(const double load_value, size_t trace_index);

    void DrawCentralPoint(HDC hdc);
    void Render(HDC hdc);
};

#endif