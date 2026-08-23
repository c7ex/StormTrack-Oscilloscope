#ifndef AXESSTATE_H
#define AXESSTATE_H

#include <sstream>
#include <iomanip>

#include "Grid.hpp"
#include "Caption.hpp"
#include "WindowState.hpp"
#include "GraphContext.hpp"
#include "TransformCoordinates.hpp"

class WindowState;
class GraphContext;
class TransformCoordinates;

class AxesState {
private:
    CoreContent core_content;

private:
    void ExtractCoreContent(GraphContext& context);

public:
    void LaunchDrawGrids(HDC hdc, GraphContext& context, const TransformCoordinates coreEngine);
    void LaunchDrawCaptions(HDC hdc, GraphContext& context, const TransformCoordinates coreEngine);
};

#endif