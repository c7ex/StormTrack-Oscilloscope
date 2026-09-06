#ifndef WINDOWSTATE_H
#define WINDOWSTATE_H

#include <windows.h>

#include "RaiiWinApi.hpp"
#include "AxesState.hpp"
#include "DataState.hpp"
#include "GraphContext.hpp"
#include "TransformCoordinates.hpp"

#include "ConfigUI.hpp"

class DataState;
class GraphContext;
class TransformCoordinates;

enum CursorType {
    _ARROW,
    _CROSS,
    _WE,
    _NS,
    _NWSE,
    _NESW,
    _HIDDEN
};

enum PlotSideType {
    _NAN,
    _TOP,
    _BOT,
    _LEFT,
    _RIGHT,
    _TOP_LEFT,
    _TOP_RIGHT,
    _BOT_LEFT,
    _BOT_RIGHT
};

enum DataException {
    _VALID_DATA_RANGE,
    _INVALID_DATA_X_RANGE,
    _NAN_DATA
};

enum ActionHotKey {
    fast_zoom,
    x_zoom,
    autoscale_x,
    autoscale_y,
	visible_fps
};

struct ActionHotKeyState {
    bool fast_zoom = false;
    bool x_zoom = false;
    bool autoscale_x = ConfigUI::AutoScaler::default_autoscaler_x_active;
    bool autoscale_y = ConfigUI::AutoScaler::default_autoscaler_y_active;
    bool visible_fps = ConfigUI::Fps::default_active;
};

class WindowState {
private:
    bool is_plot_area;
    bool is_plot_bound;
    CursorType cursor_type;
    PlotSideType plot_side_type;
    DataException data_exception;
    ActionHotKeyState keys_state;

    RECT base_margins = { 
        ConfigUI::GeneralGraph::base_margins_left, 
        ConfigUI::GeneralGraph::base_margins_top, 
        ConfigUI::GeneralGraph::base_margins_right, 
        ConfigUI::GeneralGraph::base_margins_bottom };

    RECT margins = base_margins;

private:
    void UpdateCursor(CursorType new_cursor_type, PlotSideType new_plot_size_type);

public:
    bool UpdateCursor(CursorType new_cursor_type);
    void UpdateDataException(DataException new_data_exception);
    void UpdatePlotStates(GraphContext& context);
    
    bool GetIsPlotArea() const;
    bool GetIsPlotBound() const;
    CursorType GetCursorType() const;
	DataException GetDataException() const;

    void UpdatePlotSize(GraphContext& context, Vec2d delta, RECT ref_margins);

    RECT GetMargins() const;

    void UpdateWindowSize(GraphContext& context, const LPARAM lParam);
    void UpdatePlotForm(GraphContext& context);

    bool GetKeysState(ActionHotKey key) const;
    void ActivateKey(ActionHotKey key);
    void DisactivateKey(ActionHotKey key);

    void DrawPlotBoundary(HDC hdc, GraphContext& context, const TransformCoordinates coreEngine);
};

#endif