#include "TransparentColor.hpp"

COLORREF TransparentColor::Mix(COLORREF bgColor, COLORREF textColor, double alpha)
{
    alpha = max(ConfigUI::Ruler::transparent_color_min, min(ConfigUI::Ruler::transparent_color_max, alpha));

    int bgR = GetRValue(bgColor);
    int bgG = GetGValue(bgColor);
    int bgB = GetBValue(bgColor);

    int textR = GetRValue(textColor);
    int textG = GetGValue(textColor);
    int textB = GetBValue(textColor);

    int blendedR = static_cast<int>(bgR * (ConfigUI::Ruler::transparent_color_max - alpha) + textR * alpha);
    int blendedG = static_cast<int>(bgG * (ConfigUI::Ruler::transparent_color_max - alpha) + textG * alpha);
    int blendedB = static_cast<int>(bgB * (ConfigUI::Ruler::transparent_color_max - alpha) + textB * alpha);

    return RGB(blendedR, blendedG, blendedB);
}