#include "GridCalculator.hpp"

Size2d GridCalculator::CalculateCurrentStepGrid(Size2d span) {
    return Size2d(
        pow(10., std::floor(log10(span.x))),
        pow(10., std::floor(log10(span.y)))
    );
}

Size2d GridCalculator::CalculateLowStepGrid(Size2d current_step) {
    return (current_step / ConfigUI::Ruler::grid_calculator_subdivision);
}

Size2d GridCalculator::CalculateAlphaColorGrid(Size2d span, Size2d step) {
    Size2d alpha(0, 0);
    Size2d lines = span / step;

    for (int i = 0; i < 2; i++) {
        double l = (i == 0) ? lines.x : lines.y;
        double& a = (i == 0) ? alpha.x : alpha.y;

        double deviation = std::abs(l - ConfigUI::Ruler::optimal_count_grid_lines) / ConfigUI::Ruler::optimal_count_grid_lines;

        a = std::exp(-deviation * deviation * ConfigUI::Ruler::alpha_gauss_ratio);

        a *= a;

        a = std::clamp(a, 0.0, 1.0);
    }

    return alpha;
}