#include "Grid.hpp"

void Grid::LoadParameters(const CoreContent& cc) {
    const double x_min = cc.ref.x;
    const double x_max = cc.ref.x + cc.area.x;
    const double y_min = cc.ref.y;
    const double y_max = cc.ref.y + cc.area.y;
    min = { x_min, y_min };
    max = { x_max, y_max };
    plot = cc.plot;
}

void Grid::DrawGridFilter(std::vector<GridPassContent>& passes) {
    for (int i = passes.size() - 1; i >= 0; i--) {
        if (passes[i].alpha < ConfigUI::Ruler::alpha_threshold_visible) {
            passes.erase(passes.begin() + i);
        }
    }

    for (int i = 0; i < passes.size() - 1; i++) {
        for (int j = 0; j < passes.size() - i - 1; j++) {
            if (passes[j].alpha > passes[j + 1].alpha) {
                std::swap(passes[j], passes[j + 1]);
            }
        }
    }
}

void Grid::DrawGridVertical(HDC& hdc, const TransformCoordinates& coreEngine, const Size2d& step) {
    const double start_x = std::floor(min.x / step.x) * step.x;
    const double epsilon = ConfigUI::Ruler::accuracy_grid_draw * step.x;
    int lines_drawn = 0;

    for (double x = start_x; x <= max.x + epsilon && lines_drawn < ConfigUI::Ruler::max_count_grid_lines; x += step.x, ++lines_drawn) {
        Position2d p1 = coreEngine.ConvertToPixelCoords(x, min.y);
        Position2d p2 = coreEngine.ConvertToPixelCoords(x, max.y);
        if (p1.x >= plot.left && p1.x <= plot.right) {
            MoveToEx(hdc, p1.x, plot.top, NULL);
            LineTo(hdc, p2.x, plot.bottom);
        }
    }
}

void Grid::DrawGridHorizontal(HDC& hdc, const TransformCoordinates& coreEngine, const Size2d& step) {
    const double start_y = std::floor(min.y / step.y) * step.y;
    const double epsilon = ConfigUI::Ruler::accuracy_grid_draw * step.y;
    int lines_drawn = 0;

    for (double y = start_y; y <= max.y + epsilon && lines_drawn < ConfigUI::Ruler::max_count_grid_lines; y += step.y, ++lines_drawn) {
        Position2d p1 = coreEngine.ConvertToPixelCoords(min.x, y);
        Position2d p2 = coreEngine.ConvertToPixelCoords(max.x, y);
        if (p1.y >= plot.top && p1.y <= plot.bottom) {
            MoveToEx(hdc, plot.left, p1.y, NULL);
            LineTo(hdc, plot.right, p2.y);
        }
    }
}

void Grid::DrawGrid(HDC& hdc, const TransformCoordinates& coreEngine, const GridPassContent& gpc, const CoreContent& cc)
{
    LoadParameters(cc);

    const Size2d step = gpc.step;
    GridDrawSelection gridDrawSelection = gpc.selection;

    COLORREF current_color = TransparentColor::Mix(ConfigUI::GeneralGraph::background, gpc.color, gpc.alpha);
    
    rwa::PEN pen(hdc, PS_SOLID, ConfigUI::Ruler::grid_line_width, current_color);
    
    if (gridDrawSelection == GridDrawSelection::Vertical) {
        DrawGridVertical(hdc, coreEngine, step);
    }

    if (gridDrawSelection == GridDrawSelection::Horizontal) {
        DrawGridHorizontal(hdc, coreEngine, step);
    }
};