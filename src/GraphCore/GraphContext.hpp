#ifndef GRAPH_CONTEXT_HPP
#define GRAPH_CONTEXT_HPP

#include"ConfigUI.hpp"

#include"Windows.h"

#include"Vec2d.hpp"
#include"RelativeVec2d.hpp"
#include"TransformCoordinates.hpp"

#include"ConfigUI.hpp"

class TransformCoordinates;

class GraphContext {
private:
    // from GraphState
    Size2d visible_area_;
    Position2d current_coord_;

    // from WindowState
    RECT          plot_area;
    Position2d    plot_reference_offset;
    RelativeVec2d plot_size;
    RelativeVec2d window_size;
    Position2d    mouse_position;

    // from TugboatState
    Position2d reference_position { 
        ConfigUI::GeneralGraph::reference_position_x, 
        ConfigUI::GeneralGraph::reference_position_y
    };

	// from AutoScaler
	double track_visible_area_x = ConfigUI::AutoScaler::default_track_visible_area_x;

public:
    const Size2d& GetVisibleArea() const;
    const Position2d& GetPlotReferenceOffset() const;
    const Position2d& GetReferencePosition() const;
    const Position2d& GetCurrentCoordinates() const;
    const Position2d& GetMousePosition() const;
    const RECT& GetPlotArea() const;

    Size2d GetPlotSize() const;
    Size2d GetWindowSize() const;
    RelativeVec2d& GetWindowParam();
    RelativeVec2d& GetPlotParam();

    void SetPlotArea(RECT rt);
    void SetPlotReferenceOffset(Position2d position);
    void SetVisibleArea(Size2d new_visible_area);
    void SetVisibleArea(double width, double height);
    void UpdateCoordinates(const TransformCoordinates& coreEngine);
    void UpdateMousePosition(const LPARAM& lParam);
    void UpdateMousePosition(double x, double y);
    void SetReferencePosition(Position2d position);
    void SetOffsetReferencePosition(Offset2d offset);
	void SetTrackVisibleAreaX(double new_visible_area_x);

    double GetTrackVisibleAreaX() const;
};

#endif