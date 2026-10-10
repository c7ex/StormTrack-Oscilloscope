#ifndef VEC_2D_HPP
#define VEC_2D_HPP

#include <cmath>
#include <stdexcept>

class Vec2d {
public:
    double x, y;

    Vec2d();
    Vec2d(double x, double y);

    Vec2d operator+(const Vec2d& other) const;
    Vec2d operator-(const Vec2d& other) const;
    Vec2d operator*(const Vec2d& other) const;
    Vec2d operator/(const Vec2d& other) const;

    Vec2d operator*(double scalar) const;
    Vec2d operator/(double scalar) const;

    Vec2d& operator+=(const Vec2d& other);
    Vec2d& operator-=(const Vec2d& other);
    Vec2d& operator*=(double scalar);
    Vec2d& operator/=(double scalar);

    void invert_y();

    bool operator==(const Vec2d& other) const;
    bool operator!=(const Vec2d& other) const;

    //static double Module(const Vec2d& p1, const Vec2d& p2);
};

typedef Vec2d Size2d;
typedef Vec2d Scale2d;
typedef Vec2d Offset2d;
typedef Vec2d Position2d;

#endif // VEC2D_H