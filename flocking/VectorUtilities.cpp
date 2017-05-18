#include "VectorUtilities.h"

float  distance(sf::Vector2f a, sf::Vector2f b) {
    return sqrt(pow(b.x - a.x, 2) + pow(b.y - a.y, 2));
}

float magnitude(sf::Vector2f vec) {
    return sqrt((vec.x * vec.x) + (vec.y * vec.y));
}

sf::Vector2f limit(sf::Vector2f vec, float max) {
    float magSquard = (vec.x * vec.x) + (vec.y * vec.y);

    if ((magSquard > max * max) && (magSquard > 0)) {
        float ratio = max / sqrt(magSquard);
        vec.x *= ratio;
        vec.y *= ratio;
    }

    return vec;
}

sf::Vector2f normalize(sf::Vector2f vec) {
    return vec /= magnitude(vec);
}

sf::Vector2f rotatePoint(sf::Vector2f point, sf::Vector2f center, float rad) {
    // Precompute sin and cos for MAXIMUM EFFICIENCY
    float sine = sin(rad);
    float cosine = cos(rad);

    point -= center; // move to origin

    // Rotate
    sf::Vector2f newPoint;
    newPoint.x = point.x*cosine - point.y*sine;
    newPoint.y = point.y*cosine + point.x*sine;

    point = newPoint + center; // move back
    return point;
}

sf::Vector2f rotatePointAbs(sf::Vector2f point, sf::Vector2f center, float rad) {
    float currentAngle = atan2f(point.y - center.y, point.x - center.x);
    float angleDelta = rad - currentAngle;

    return rotatePoint(point, center, angleDelta);
}
