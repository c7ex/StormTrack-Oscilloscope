#include "TextFormatter.hpp"

bool TextFormatter::IsZero(double value) {
    return (std::fabs(value) < ConfigUI::Ruler::text_format_zero_epsilon);
}

std::wstring TextFormatter::FormatZero(double value) {
    return L"0";
}

std::wstring TextFormatter::FormatScientific(double value) {
    std::wstringstream ss;
    ss << std::scientific << std::setprecision(2) << value;
    return ss.str();
}

std::wstring TextFormatter::FormatFixed(double value, int precision) {
    std::wstringstream ss;
    ss << std::fixed << std::setprecision(precision) << value;
    return ss.str();
}

void TextFormatter::CleanTrailingZeros(std::wstring& s) {
    size_t dot_pos = s.find(L'.');
    if (dot_pos != std::wstring::npos) {
        s.erase(s.find_last_not_of(L'0') + 1, std::wstring::npos);
        if (s.back() == L'.') {
            s.erase(s.end() - 1);
        }
    }
}

void TextFormatter::CleanScientificNotation(std::wstring& s) {
    size_t e_pos = s.find(L'e+');
    if (e_pos != std::wstring::npos) {
        s.replace(e_pos, 2, L"e");
    }
}

std::wstring TextFormatter::Format(double value) {
    std::wstring s;

    if (IsZero(value)) {
        return FormatZero(value);
    }

    double absValue = std::fabs(value);
    if (absValue < ConfigUI::Ruler::text_format_scientific_threshold || absValue >= ConfigUI::Ruler::text_format_large_threshold) {
        s = FormatScientific(value);
    }
    else if (absValue >= 100) {
        s = FormatFixed(value, 0);
    }
    else if (absValue >= 1) {
        s = FormatFixed(value, 2);
    }
    else {
        s = FormatFixed(value, 4);
    }

    CleanTrailingZeros(s);
    CleanScientificNotation(s);

    return s;
}