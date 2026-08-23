#ifndef _DATA_TRACKER_HPP
#define _DATA_TRACKER_HPP

#include"RaiiWinApi.hpp"
#include"LinearData.hpp"
#include"WindowState.hpp"
#include"GraphContext.hpp"
#include"TransformCoordinates.hpp"

#include"ConfigUI.hpp"

class LinearData;
class WindowState;
class GraphContext;
class TransformCoordinates;

struct DataSearchResult {
private:
    inline void Move(const DataSearchResult& other) {
        data_trace_id = other.data_trace_id;
        data_index = other.data_index;
        nearest_range = other.nearest_range;
    }

public:
    size_t data_trace_id;
    size_t data_index;
    double nearest_range;

    void Update(const DataSearchResult& other) {
        // protect invalid input
        if (other.nearest_range < 0) { return; }

        if (nearest_range >= 0) {
            if ((nearest_range > other.nearest_range)) { Move(other); }
        }
        else { Move(other); }
    }

    void Reset() {
        data_trace_id = 0;
        data_index = 0;
        nearest_range = -1;
    }
};

class DataTracker {
private:
    DataSearchResult search_data_result;
    bool track_state = false;

private:
    void SearchPoint(
        const LinearData& data,
        const TransformCoordinates& coreEngine,
        const Position2d& mouse_position,
        size_t index_point,
        size_t index_trace);

private:
    void DrawDot(HDC hdc, int x, int y, int radius, COLORREF color);
    void DrawLabel(HDC hdc, const Position2d& mouse_position, const std::wstring& text, COLORREF bgColor);
    double VecModule(const Vec2d& p1, const Vec2d& p2);
    void SearchNearestLinearData(GraphContext& context, const TransformCoordinates& coreEngine, const LinearData& data);
    Position2d TryHoldNearestData(HDC hdc, GraphContext& context, const TransformCoordinates& coreEngine, DataState& data, const WindowState& ws);
    
public:
    void ShowCoordinates(HDC hdc, GraphContext& context, const TransformCoordinates& coreEngine, DataState& data, const WindowState& ws);
    bool GetTrackState() const;
};

#endif