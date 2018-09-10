#ifndef POINT_H_
#define POINT_H_

class point {
   public:
    float x, y;
    point();
    point(float x, float y);
    ~point();

    point operator+(point p);
    point operator-(point p);
    point operator*(point p);
    point operator/(point p);

    bool operator==(point p);
    bool operator!=(point p);

    void move(point p);

    float distance(point p);
    float distance(float x, float y);
    float magnitude();

    void print();
};

#endif
