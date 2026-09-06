#ifndef GRIDCALCULATOR_H
#define GRIDCALCULATOR_H

#include "ConfigUI.hpp"

#include "Vec2d.hpp"

#include <string>
#include <sstream>
#include <iomanip>
#include <cmath>
#include <algorithm>

class GridCalculator {
public:
    static Size2d CalculateCurrentStepGrid(Size2d span);
    static Size2d CalculateLowStepGrid(Size2d current_step);
    static Size2d CalculateAlphaColorGrid(Size2d span, Size2d step);
};

#endif