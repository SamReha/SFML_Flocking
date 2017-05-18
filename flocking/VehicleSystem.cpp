#include "VehicleSystem.h"

/* Private */
void VehicleSystem::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    // apply the transform
    states.transform *= getTransform();

    // our vehicles don't use a texture
    states.texture = NULL;

    // draw the vertex array
    target.draw(m_vertices, states);
}

void VehicleSystem::seekPoint(int vehicleIndex, sf::Vector2f target, float deltaTime) {
    sf::Vector2f desired = target - m_vertices[vehicleIndex * 3].position;
    desired = normalize(desired);
    desired *= m_maxSpeed;

    sf::Vector2f steer = desired - vehicles[vehicleIndex].velocity;
    steer = limit(steer, m_maxForce);
    vehicles[vehicleIndex].acceleration += desired*deltaTime;
}

void VehicleSystem::setVehiclePosition(int vehicleIndex, sf::Vector2f newPosition) {
    // Do screen wrap
    newPosition.x = fmod(newPosition.x, SCREENWIDTH);
    newPosition.y = fmod(newPosition.y, SCREENHEIGHT);

    // Compute delta
    int triIndex = vehicleIndex * 3;
    sf::Vector2f delta = newPosition - m_vertices[triIndex].position;

    // Appy to appropriate vertices
    m_vertices[triIndex].position += delta;
    m_vertices[triIndex + 1].position += delta;
    m_vertices[triIndex + 2].position += delta;
}

void VehicleSystem::applyRotation(int vehicleIndex) {
    // Compute delta
    int triIndex = vehicleIndex * 3;

    // Rotate the vehicle
    float rotation = atan2f(vehicles[vehicleIndex].velocity.y, vehicles[vehicleIndex].velocity.x);
    m_vertices[triIndex + 1].position = rotatePointAbs(m_vertices[triIndex + 1].position, m_vertices[triIndex].position, rotation + 0.25 + PI);
    m_vertices[triIndex + 2].position = rotatePointAbs(m_vertices[triIndex + 2].position, m_vertices[triIndex].position, rotation - 0.25 + PI);
}

void VehicleSystem::updateFlockForces(int vehicleIndex, float deltaTime) {
    sf::Vector2f sepSum;
    sf::Vector2f alignSum;
    sf::Vector2f cohesionSum;

    int sepCount = 0;
    int alignCount = 0;
    int cohesionCount = 0;

    float sepWeight = 10.0;
    float alignWeight = 10.0;
    float cohesionWeight = 1.0;

    for (unsigned int i = 0; i < vehicles.size(); i++) {
        // If we're not looking at ourselves
        if (i != vehicleIndex) {
            Vehicle neighbor = vehicles[i];

            // Compute distance
            sf::Vertex myVert = m_vertices[vehicleIndex * 3];
            sf::Vertex neighborVert = m_vertices[i * 3];
            float dist = distance(myVert.position, neighborVert.position);
            
            // Apply separation
            if (dist < separationRad) {
                sf::Vector2f diff = myVert.position - neighborVert.position;
                sepSum += normalize(diff);
                sepCount++;
            }
            
            // Apply alignment + cohesion
            if (dist < flockingRad) {
                alignSum += neighbor.velocity;
                alignCount++;

                cohesionSum += neighborVert.position;
                cohesionCount++;
            }
        }
    }

    // If there are any forces, compute the average force and apply it to our vehicle
    if (sepCount > 0 && separationEnabled) {
        sepSum /= (float)sepCount;

        // Apply weight
        //sepSum *= sepWeight;

        normalize(sepSum);    // Scale average to maxSpeed
        sepSum *= m_maxSpeed;

        // Compute steering force (desired speed - current speed) and apply it to the vehicle
        sf::Vector2f steer = sepSum - vehicles[vehicleIndex].velocity;
        steer = limit(steer, m_maxForce);
        vehicles[vehicleIndex].acceleration += steer*deltaTime*sepWeight;
    }

    if (alignCount > 0 && alignmentEnabled) {
        alignSum /= (float)alignCount;

        // Apply weight
        //alignSum *= alignWeight;

        normalize(alignSum);    // Scale average to maxSpeed
        alignSum *= m_maxSpeed;

        // Compute steering force (desired speed - current speed) and apply it to the vehicle
        sf::Vector2f steer = alignSum - vehicles[vehicleIndex].velocity;
        steer = limit(steer, m_maxForce);
        vehicles[vehicleIndex].acceleration += steer*deltaTime*alignWeight;
    }

    if (cohesionCount > 0 && cohesionEnabled) {
        // Compute average neighbor position
        cohesionSum /= (float)cohesionCount;

        // Apply weight
        //cohesionSum *= cohesionWeight;

        // Seek that point
        seekPoint(vehicleIndex, cohesionSum, deltaTime);
    }
}

void VehicleSystem::addVertexTri() {
    // Get a random point in screenspace
    //srand(time(NULL));
    sf::Vector2f pos(rand() % SCREENWIDTH, rand() % SCREENHEIGHT);

    // Note to self you'll need to re-add tex coords if you want to use those again
    m_vertices.append(sf::Vertex(pos, m_color));
    m_vertices.append(sf::Vertex(sf::Vector2f(pos.x - QUAD_WIDTH/3, pos.y + QUAD_WIDTH), m_color));
    m_vertices.append(sf::Vertex(sf::Vector2f(pos.x + QUAD_WIDTH/3, pos.y + QUAD_WIDTH), m_color));
}

void VehicleSystem::removeVertexTri() {
    m_vertices.resize(m_vertices.getVertexCount() - 3);
}

/* Public */
VehicleSystem::VehicleSystem(unsigned int count, float minSpeed, float maxSpeed) :
    vehicles(count),
    m_vertices(sf::Triangles),
    m_gravity(0),
    m_maxForce(0.5),
    separationRad(100),
    flockingRad(300) {
    m_color = sf::Color::White;
    m_minSpeed = minSpeed;
    m_maxSpeed = maxSpeed;

    for (unsigned int i = 0; i < count; i++) {
        addVertexTri();
    }
}

void VehicleSystem::setGravity(float newGravity) {
    m_gravity = newGravity;
}

void VehicleSystem::setColor(sf::Color newColor) {
    m_color = newColor;
}

void VehicleSystem::setMinSpeed(float newSpeed) {
    m_minSpeed = abs(newSpeed);
}

void VehicleSystem::setMaxSpeed(float newSpeed) {
    m_maxSpeed = abs(newSpeed);
}

int VehicleSystem::count() {
    return vehicles.size();
}

void VehicleSystem::addVehicle() {
    Vehicle newVehicle;

    vehicles.push_back(newVehicle);
    addVertexTri();
}

void VehicleSystem::removeVehicle() {
    if (!vehicles.empty()) {
        vehicles.pop_back();
        removeVertexTri();
    }
}

void VehicleSystem::addRepulsor(float x, float y, float strength) {
    ForceEmitter force;

    force.position.x = x;
    force.position.y = y;
    force.strength = -strength;

    m_forceEmitters.push_back(force);
}

void VehicleSystem::addAttractor(float x, float y, float strength) {
    ForceEmitter force;

    force.position.x = x;
    force.position.y = y;
    force.strength = strength;

    m_forceEmitters.push_back(force);
}

void VehicleSystem::update(sf::Time elapsed) {
    for (std::size_t i = 0; i < vehicles.size(); i++) {
        int triIndex = i * 3;
        Vehicle& p = vehicles[i];

        // Update velocity w/ respect to time
        p.acceleration.y += m_gravity * elapsed.asSeconds();

        // Update velocity w/ respect to attractors / repulsors
        for (auto forceEmitter : m_forceEmitters) {
            sf::Vector2f particlePos = m_vertices[triIndex].position;
            float deltaX = forceEmitter.position.x - particlePos.x;
            float deltaY = forceEmitter.position.y - particlePos.y;
            float angleInRadians = atan2(deltaY, deltaX);
            float distance = sin(angleInRadians) * deltaY;

            float maximumXForce = forceEmitter.strength * cos(angleInRadians);
            float maximumYForce = forceEmitter.strength * sin(angleInRadians);

            float finalXForce = maximumXForce;// / (distance * distance); WHY DON'T YOU WORK
            float finalYForce = maximumYForce;// / (distance * distance);

            p.acceleration.x += finalXForce * elapsed.asSeconds();
            p.acceleration.y += finalYForce * elapsed.asSeconds();
        }

        updateFlockForces(i, elapsed.asSeconds());

        // Update velocity
        p.velocity += p.acceleration;

        // update the position of the corresponding tri
        sf::Vector2f newPosition = m_vertices[triIndex].position + p.velocity * elapsed.asSeconds();
        setVehiclePosition(i, newPosition);
        applyRotation(i);

        // Reset acceleration
        p.acceleration.x = 0; p.acceleration.y = 0;

        // update the color of the corresponding tri
        m_vertices[triIndex].color = m_color;
        m_vertices[triIndex + 1].color = m_color;
        m_vertices[triIndex + 2].color = m_color;
    }
}
