#pragma once

#include <SFML/Graphics.hpp>

#include <math.h>
#include <iostream>

#include "config.h"
#include "Vehicle.h"
#include "VectorUtilities.h"

#define QUAD_WIDTH 64

class VehicleSystem : public sf::Drawable, public sf::Transformable {
private:
    struct ForceEmitter {
        sf::Vector2f position;
        float strength;
    };

    float m_gravity;       // Acceleration due to gravity (pixels per second). Defaults to 0.
    float m_minSpeed;      // The minimum speed of a vehicle (pixels per second).
    float m_maxSpeed;      // The maximum speed of a vehicle (pixels per second).
    float m_maxForce;      // How 'nimble' is the steering of a vehicle. Defaults to 0.1.
    float separationRad;   // How close two vehicles can be before they start to push away from each other (in pixels). Defaults to 32
    float flockingRad;     // How far apart two vehicles can be before they start ignoring each other (in pixels). Defaults to 150
    sf::Color m_color;     // The fill color of a vehicle. Defaults to white.

    std::vector<Vehicle> vehicles;
    std::vector<ForceEmitter> m_forceEmitters;
    sf::VertexArray m_vertices;

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

    void seekPoint(int vehicleIndex, sf::Vector2f target, float deltaTime);
    void setVehiclePosition(int vehicleIndex, sf::Vector2f newPosition);
    void applyRotation(int vehicleIndex);
    void updateFlockForces(int vehicleIndex, float deltaTime);
    void addVertexTri();
    void removeVertexTri();

public:
    bool separationEnabled = true;
    bool cohesionEnabled = true;
    bool alignmentEnabled = true;

    VehicleSystem(unsigned int count, float minSpeed, float maxSpeed);

    void toggleSeparation() { separationEnabled = !separationEnabled; };
    void toggleCohesion() { cohesionEnabled = !cohesionEnabled; };
    void toggleAlignment() { alignmentEnabled = !alignmentEnabled; };

    void setGravity(float newGravity);
    void setMinSpeed(float newSpeed);
    void setMaxSpeed(float newSpeed);
    void setColor(sf::Color newColor);

    int count();

    void addVehicle();
    void removeVehicle();

    void addRepulsor(float x, float y, float strength);
    void addAttractor(float x, float y, float strength);

    void update(sf::Time elapsed);
};