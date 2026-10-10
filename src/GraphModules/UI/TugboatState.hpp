#ifndef TUGBOAT_STATE_HPP
#define TUGBOAT_STATE_HPP

#include "windows.h"

#include "Vec2d.hpp"
#include "GraphContext.hpp"

class GraphContext;

class TugboatState {
private:
    Position2d last_reference_position;
    Position2d hold_position;
    Position2d current_position;
    bool is_active = false;

public:
    void StartDragging(GraphContext& context, const LPARAM lParam);
    void UpdateDrag(GraphContext& context, const LPARAM lParam);
    void StopDragging(GraphContext& context, const LPARAM lParam);
    bool IsActive() const;
};

#endif