#ifndef TRANSPARENTCOLOR_H
#define TRANSPARENTCOLOR_H

#include "ConfigUI.hpp"

#include <windows.h>

class TransparentColor {
public:
    static COLORREF Mix(COLORREF bgColor, COLORREF textColor, double alpha);
};

#endif