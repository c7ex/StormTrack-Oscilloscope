#ifndef TEXTFORMATTER_H
#define TEXTFORMATTER_H

#include "ConfigUI.hpp"

#include <string>
#include <sstream>
#include <iomanip>
#include <cmath>

class TextFormatter {
public:
    static std::wstring Format(double value);

private:
    static bool IsZero(double value);
    static std::wstring FormatZero(double value);
    static std::wstring FormatScientific(double value);
    static std::wstring FormatFixed(double value, int precision);
    static void CleanTrailingZeros(std::wstring& s);
    static void CleanScientificNotation(std::wstring& s);
};

#endif