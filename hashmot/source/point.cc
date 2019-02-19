// MIT License

// Copyright (c) 2019 kosay

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.


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
