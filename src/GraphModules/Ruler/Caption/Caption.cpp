#include "Caption.hpp"

Area2d CreateTextExpandAreaCorrection(const SIZE& textSize)
{
    return Area2d{
        -textSize.cx / 2,
        -textSize.cy / 2,
        textSize.cx / 2,
        textSize.cy / 2
    };
}

Area2d CreateReferenceArea(int x, int y)
{
    return Area2d{ x, y, x, y };
}

void Caption::DrawCaptionFilter(std::vector<GridPassContent>& passes) {
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

void Caption::LoadParameters(const CoreContent& cc) {
    const double x_min = cc.ref.x;
    const double x_max = cc.ref.x + cc.area.x;
    const double y_min = cc.ref.y;
    const double y_max = cc.ref.y + cc.area.y;
    min = { x_min, y_min };
    max = { x_max, y_max };
    plot = cc.plot;
}

void Caption::DrawCaptionVertical(HDC hdc, const TransformCoordinates& coreEngine, const Size2d& step) {
    const int offset = ConfigUI::Ruler::caption_offset_x;

    for (double x = std::floor(min.x / step.x) * step.x; x <= max.x; x += step.x) {
        const Position2d p = coreEngine.ConvertToPixelCoords(x, min.y);
        if (p.x >= plot.left && p.x <= plot.right)
        {
            std::wstring label = TextFormatter::Format(x);

            SIZE textSize;
            GetTextExtentPoint32(hdc, label.c_str(), (int)label.length(), &textSize);

            Area2d referenceArea = CreateReferenceArea(p.x, plot.bottom + offset + textSize.cy / 2);
            Area2d centralCorrection = CreateTextExpandAreaCorrection(textSize);
            referenceArea += centralCorrection;
            RECT textRect = referenceArea.ToRect();

            DrawText(hdc, label.c_str(), -1, &textRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
        }
    }
}

void Caption::DrawCaptionHorizontal(HDC hdc, const TransformCoordinates& coreEngine, const Size2d& step) {
    const int offset = ConfigUI::Ruler::caption_offset_y;

    for (double y = std::floor(min.y / step.y) * step.y; y <= max.y; y += step.y) {
        const Position2d p = coreEngine.ConvertToPixelCoords(min.x, y);
        if (p.y >= plot.top && p.y <= plot.bottom)
        {
            std::wstring label = TextFormatter::Format(y);

            SIZE textSize;
            GetTextExtentPoint32(hdc, label.c_str(), (int)label.length(), &textSize);

            Area2d referenceArea = CreateReferenceArea(plot.left - offset, p.y);
            Area2d centralCorrection = CreateTextExpandAreaCorrection(textSize);
            referenceArea += centralCorrection;
            RECT textRect = referenceArea.ToRect();

            DrawText(hdc, label.c_str(), -1, &textRect, DT_RIGHT | DT_VCENTER | DT_SINGLELINE);
        }
    }
}

void Caption::DrawCaptions(HDC& hdc, const TransformCoordinates& coreEngine, const GridPassContent& gpc, const CoreContent& cc)
{
    COLORREF transparentColor = TransparentColor::Mix(ConfigUI::GeneralGraph::boundary, gpc.color, gpc.alpha);
    rwa::PEN penCreatePen(hdc, PS_SOLID, 1, transparentColor);
    rwa::BRUSH brush(hdc, transparentColor);
    rwa::FONT font(hdc, DEFAULT_GUI_FONT);
    SetTextColor(hdc, transparentColor);

    LoadParameters(cc);

    const Size2d step = gpc.step;
    GridDrawSelection gridDrawSelection = gpc.selection;

    if (gridDrawSelection == GridDrawSelection::Vertical) {
        DrawCaptionVertical(hdc, coreEngine, step);
    }

    if (gridDrawSelection == GridDrawSelection::Horizontal) {
        DrawCaptionHorizontal(hdc, coreEngine, step);
    }
};