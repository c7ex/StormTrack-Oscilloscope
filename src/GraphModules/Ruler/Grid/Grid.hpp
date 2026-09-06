#ifndef GRID_HPP
#define GRID_HPP

#include "ConfigUI.hpp"

#include "RaiiWinApi.hpp"
#include "AxesDescription.hpp"
#include "TransformCoordinates.hpp"

class TransformCoordinates;

class Grid {
private:
    Vec2d min;
    Vec2d max;
    RECT plot;

private:
    void LoadParameters(const CoreContent& cc);
    void DrawGridVertical(HDC& hdc, const TransformCoordinates& coreEngine, const Size2d& step);
    void DrawGridHorizontal(HDC& hdc, const TransformCoordinates& coreEngine, const Size2d& step);

public:
    void DrawGrid(HDC& hdc, const TransformCoordinates& coreEngine, const GridPassContent& gpc, const CoreContent& cc);
    void DrawGridFilter(std::vector<GridPassContent>& passes);
};

#endif