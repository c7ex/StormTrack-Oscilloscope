#ifndef TEXTFORMATTER_H
#define TEXTFORMATTER_H

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

    static constexpr double SCIENTIFIC_THRESHOLD = 0.001;
    static constexpr double LARGE_THRESHOLD = 1000.0;
    static constexpr double ZERO_EPSILON = 1e-12;
};

#endif