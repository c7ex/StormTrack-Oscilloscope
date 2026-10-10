#ifndef AREA_2D_HPP
#define AREA_2D_HPP

#include <windows.h>

class Area2d {
private:
    int left;
    int top;
    int right;
    int bottom;

public:
    Area2d operator + (const Area2d& other);
    Area2d operator - (const Area2d& other);
    void operator += (const Area2d& other);
    void operator -= (const Area2d& other);

public:
    RECT ToRect();

    Area2d(int left_init, int top_init, int right_init, int bottom_init);
    Area2d(const RECT& rect);
    Area2d();
};

#endif