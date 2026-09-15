#include "AxesState.hpp"

void AxesState::ExtractCoreContent(GraphContext& context) {
    Position2d ref = context.GetReferencePosition();
    Size2d area = context.GetVisibleArea();
    RECT plot = context.GetPlotArea();
    CoreContent cc{ ref, plot, area};
    core_content = cc;
}

void AxesState::LaunchDrawCaptions(HDC hdc, GraphContext& context, const TransformCoordinates coreEngine) {

    ExtractCoreContent(context);

    Size2d slp = GridCalculator::CalculateCurrentStepGrid(core_content.area);
    Size2d sll = GridCalculator::CalculateLowStepGrid(slp);

    Size2d test_alpha_p = GridCalculator::CalculateAlphaColorGrid(core_content.area, slp);
    Size2d test_alpha_l = GridCalculator::CalculateAlphaColorGrid(core_content.area, sll);

    COLORREF grid_color_p = ConfigUI::Ruler::caption_main_color;
    COLORREF grid_color_l = ConfigUI::Ruler::caption_sub_color;

    std::vector<GridPassContent> passes = {
        {test_alpha_l.y, sll, grid_color_l, GridDrawSelection::Horizontal},
        {test_alpha_l.x, sll, grid_color_l, GridDrawSelection::Vertical},
        {test_alpha_p.y, slp, grid_color_p, GridDrawSelection::Horizontal},
        {test_alpha_p.x, slp, grid_color_p, GridDrawSelection::Vertical}
    };

    Caption caption;
    caption.DrawCaptionFilter(passes);
    for (const auto& pass : passes) {
        caption.DrawCaptions(hdc, coreEngine, pass, core_content);
    }
}

void AxesState::LaunchDrawGrids(HDC hdc, GraphContext& context, const TransformCoordinates coreEngine) {

    ExtractCoreContent(context);

    Size2d slp = GridCalculator::CalculateCurrentStepGrid(core_content.area);
    Size2d sll = GridCalculator::CalculateLowStepGrid(slp);

    Size2d test_alpha_p = GridCalculator::CalculateAlphaColorGrid(core_content.area, slp);
    Size2d test_alpha_l = GridCalculator::CalculateAlphaColorGrid(core_content.area, sll);

    COLORREF grid_color_p = ConfigUI::Ruler::grid_main_color;
    COLORREF grid_color_l = ConfigUI::Ruler::grid_sub_color;

    std::vector<GridPassContent> passes = {
        {test_alpha_l.y, sll, grid_color_l, GridDrawSelection::Horizontal},
        {test_alpha_l.x, sll, grid_color_l, GridDrawSelection::Vertical},
        {test_alpha_p.y, slp, grid_color_p, GridDrawSelection::Horizontal},
        {test_alpha_p.x, slp, grid_color_p, GridDrawSelection::Vertical}
    };

    Grid grid;
    grid.DrawGridFilter(passes);
    for (const auto& pass : passes) {
        grid.DrawGrid(hdc, coreEngine, pass, core_content);
    }
}