#ifndef ZOOM_STATE_HPP
#define ZOOM_STATE_HPP

#include<algorithm>

#include"TugboatState.hpp"
#include"GraphContext.hpp"
#include"TransformCoordinates.hpp"

#include"ConfigUI.hpp"

class TugboatState;
class GraphContext;
class TransformCoordinates;

enum class ScaleDirection {
    ZoomIn,
    ZoomOut
};

class ZoomState {
public:
    void ApplyZoom(
        GraphContext& context, 
        const TransformCoordinates& coreEngine,
        const TugboatState& ts,
        ScaleDirection direction, 
        bool fast_zoom, 
        bool mode_axes_X,
        bool mode_track);
};

#endif