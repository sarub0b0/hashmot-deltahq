#include <cstdio>
#include <cmath>

#include <point.hh>

point::point() : x(0.0), y(0.0) {
}
point::point(float x_, float y_) : x(x_), y(y_) {
}
point::~point() {
}

// point &point::operator=(point &p) {
//     return p;
// }
// point point::operator=(point p) {
//     return p;
// }
point point::operator+(point p) {
    return point(x + p.x, y + p.y);
}

point point::operator-(point p) {
    return point(x - p.x, y - p.y);
}

point point::operator*(point p) {
    return point(x * p.x, y * p.y);
}

point point::operator/(point p) {
    return point(x / p.x, y / p.y);
}

bool point::operator==(point p) {
    if (fabs(x - p.x) <= __FLT_EPSILON__ && fabs(y - p.y) <= __FLT_EPSILON__)
        return true;
    else
        return false;
}
bool point::operator!=(point p) {
    if (fabs(x - p.x) >= __FLT_EPSILON__ || fabs(y - p.y) >= __FLT_EPSILON__)
        return true;
    else
        return false;
}

void point::move(point p) {
    x += p.x;
    y += p.y;
}

float point::distance(point p) {
    return sqrt(pow(x - p.x, 2) + pow(y - p.y, 2));
}

float point::distance(float x, float y) {
    return sqrt(pow(x + x, 2) + pow(y - y, 2));
}

float point::magnitude() {
    return sqrt(pow(x, 2) + pow(y, 2));
}

void point::print() {
    printf("x=%.2f y=%.2f\n", x, y);
}
