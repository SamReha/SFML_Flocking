#pragma once
#include <SFML/Graphics.hpp>

#include "config.h"

/* Gets the distance between two vectors */
float distance(sf::Vector2f a, sf::Vector2f b);

/* Computes the magnitude of the vector */
float magnitude(sf::Vector2f vec);

/* Limits the magnitude of the given vector to the given max magnitude */
sf::Vector2f limit(sf::Vector2f vec, float max);

/* Normalizes the passed-in vector */
sf::Vector2f normalize(sf::Vector2f vec);

/* Rotates the point about the given center BY the given amount (in radians) */
sf::Vector2f rotatePoint(sf::Vector2f point, sf::Vector2f center, float rad);

/* Rotates the point about the given center TO the given amount (in radians) */
sf::Vector2f rotatePointAbs(sf::Vector2f point, sf::Vector2f center, float rad);