#ifndef ROCKET_H
#define ROCKET_H
#include <SFML/Graphics.hpp>

#define ROCKET_SCALE 0.1f

using namespace sf;

class Rocket : public Sprite {
public:
    float vx;
    float vy;
    float x;
    float y;
    float angle;
    int speed;
    Rocket();
};

#endif // ROCKET_H
