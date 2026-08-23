#ifndef AXESCONFIG_H
#define AXESCONFIG_H

#include <windows.h>

#include "ConfigUI.hpp"

struct GridConfig {
    static constexpr int OFFSET_CAPTION_X = 5;
    static constexpr int OFFSET_CAPTION_Y = 25;

    static constexpr double ALPHA_GAUSS_VALUE = 0.5;
    static constexpr double ALPHA_THRESHOLD_VISIBLE = 0.05;

    static constexpr double ACCURACY_FACTOR = 1e-10;
    static constexpr int MAX_DRAW_LINES = 70;

    static constexpr int GRID_LINE_WIDTH = 1;

    static constexpr COLORREF GRID_MAIN = RGB(200, 200, 220);
    static constexpr COLORREF GRID_SUB = RGB(120, 120, 140);
    
    static constexpr COLORREF BORDER_ACTIVE = RGB(25, 25, 55);
    static constexpr COLORREF BORDER_UNACTIVE = RGB(150, 150, 150);
};

#endif