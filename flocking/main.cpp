#include <vector>
#include <memory>

#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>

#include "config.h"
#include "VehicleSystem.h"

int main() {
    // create the window
    sf::RenderWindow window(sf::VideoMode(SCREENWIDTH, SCREENHEIGHT), "Waka Flocka Flames");

    // Initialize text elements
    sf::Font font;
    if (!font.loadFromFile("sansation.ttf")) {
        return EXIT_FAILURE;
    }

    // create the flock
    VehicleSystem activeSystem(100, 0.f, 10.f);
    //activeSystem.setGravity(-9.8);
    activeSystem.setColor(sf::Color::Magenta);
    //activeSystem.addAttractor(SCREENWIDTH / 2, SCREENHEIGHT / 2, 800);

    sf::Text particlesCount("particles", font, 25);
    particlesCount.setColor(sf::Color::White);
    particlesCount.setPosition(0, 0);

    sf::Text increaseParticlesText("Use W and S to change the number of particles", font, 25);
    increaseParticlesText.setColor(sf::Color::White);
    increaseParticlesText.setPosition(0, 25);

    sf::Text changeSystemText("Use Q and E to change the active particle system", font, 25);
    changeSystemText.setColor(sf::Color::White);
    changeSystemText.setPosition(0, 50);

    sf::Text fpsText("0.0 fps", font, 25);
    fpsText.setColor(sf::Color::White);
    fpsText.setPosition(0, SCREENHEIGHT - 30);

    sf::Text sepText("Separation Enabled", font, 25);
    sepText.setColor(sf::Color::White);
    sepText.setPosition(0, SCREENHEIGHT - 60);

    sf::Text aliText("Alignment Enabled", font, 25);
    aliText.setColor(sf::Color::White);
    aliText.setPosition(0, SCREENHEIGHT - 90);

    sf::Text cohText("Cohesion Enabled", font, 25);
    cohText.setColor(sf::Color::White);
    cohText.setPosition(0, SCREENHEIGHT - 120);

    // create a clock to track the elapsed time
    sf::Clock clock;

    // run the main loop
    while (window.isOpen()) {
        // handle events
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            // Escape pressed: exit
            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Escape) {
                    window.close();
                } else if (event.key.code == sf::Keyboard::W) {
                    activeSystem.addVehicle();
                } else if (event.key.code == sf::Keyboard::S) {
                    activeSystem.removeVehicle();
                } else if (event.key.code == sf::Keyboard::Z) {
                    activeSystem.toggleSeparation();
                } else if (event.key.code == sf::Keyboard::X) {
                    activeSystem.toggleAlignment();
                } else if (event.key.code == sf::Keyboard::C) {
                    activeSystem.toggleCohesion();
                }
            }
        }

        // update
        sf::Time elapsed = clock.restart();
        activeSystem.update(elapsed);

        // Update some text
        particlesCount.setString(std::to_string(activeSystem.count()) + " particles");
        fpsText.setString(std::to_string((int)(1/elapsed.asSeconds())) + " fps");

        if (activeSystem.separationEnabled) {
            sepText.setString("Separation Enabled");
        } else sepText.setString("Separation Disabled");

        if (activeSystem.alignmentEnabled) {
            aliText.setString("Alignment Enabled");
        } else aliText.setString("Alignment Disabled");

        if (activeSystem.cohesionEnabled) {
            cohText.setString("Cohesion Enabled");
        } else cohText.setString("Cohesion Disabled");

        // draw it
        window.clear();
        window.draw(activeSystem);
        window.draw(particlesCount);
        window.draw(increaseParticlesText);
        window.draw(changeSystemText);
        window.draw(fpsText);
        window.draw(sepText);
        window.draw(aliText);
        window.draw(cohText);
        window.display();
    }

    return 0;
}