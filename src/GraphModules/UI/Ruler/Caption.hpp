#ifndef CAPTION_HPP
#define CAPTION_HPP

#include "ConfigUI.hpp"

#include "RaiiWinApi.hpp"
#include "AxesDescription.hpp"
#include "TransformCoordinates.hpp"

class TransformCoordinates;

class Caption {
private:
    Vec2d min;
    Vec2d max;
    RECT plot;

private:
    void LoadParameters(const CoreContent& cc);
    void DrawCaptionVertical(HDC hdc, const TransformCoordinates& coreEngine, const Size2d& step);
    void DrawCaptionHorizontal(HDC hdc, const TransformCoordinates& coreEngine, const Size2d& step);

public:
    void DrawCaptions(HDC& hdc, const TransformCoordinates& coreEngine, const GridPassContent& gpc, const CoreContent& cc);
    void DrawCaptionFilter(std::vector<GridPassContent>& passes);
};

#endif