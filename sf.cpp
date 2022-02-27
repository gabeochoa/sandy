#define SFML_STATIC
#include <SFML/Graphics.hpp>
#include <array>
#include <chrono>
#include <cmath>
#include <iostream>
#include <memory>
#include <thread>

#include "utils.h"

// run the program as long as the window is open
int frame = 0;
int msSinceLastUpdate = 0;

#include "elements.h"
#include "grid.h"
#include "materials.h"

Material selectedMaterial = Material::Sand;
int penRadius = 1;

void handle_keycode(int code) {
    switch (code) {
        case sf::Keyboard::Num1:
            penRadius += 1;
            break;
        case sf::Keyboard::Num2:
            penRadius -= 1;
            break;
        case sf::Keyboard::E:
            selectedMaterial = Material::Empty;
            break;
        case sf::Keyboard::S:
            selectedMaterial = Material::Sand;
            break;
        case sf::Keyboard::W:
            selectedMaterial = Material::Water;
            break;
        case sf::Keyboard::T:
            selectedMaterial = Material::Wood;
            break;
        case sf::Keyboard::Y:
            selectedMaterial = Material::Smoke;
            break;
        case sf::Keyboard::F:
            selectedMaterial = Material::Fire;
            break;
        case sf::Keyboard::C:
            selectedMaterial = Material::Cloud;
            break;
        case sf::Keyboard::B:
            selectedMaterial = Material::Blackhole;
            break;
        case sf::Keyboard::D:
            selectedMaterial = Material::Dirt;
            break;
        case sf::Keyboard::H:
            selectedMaterial = Material::Torch;
            break;
        case sf::Keyboard::O:
            selectedMaterial = Material::Oil;
            break;
        default:
            break;
    }

    penRadius = fmin(height, fmax(0, penRadius));

    std::cout << "now selected " << selectedMaterial << ", pen " << penRadius
              << std::endl;
}

inline void draw(unsigned char* pic) {
    auto grid = Grid::get();

    // for (int i = 0; i < width * height; i++) {
    // int color = grid->colorat(i);
    // pic[i * 4] = (color & 0xFF000000) >> 24;
    // pic[i * 4 + 1] = (color & 0x00FF0000) >> 16;
    // pic[i * 4 + 2] = (color & 0x0000FF00) >> 8;
    // pic[i * 4 + 3] = (color & 0x000000FF);
    // }

    grid->fluid->render(pic);
}

inline void update(int elapsed) {
    msSinceLastUpdate += elapsed;

    // Run updates  100/s
    if (msSinceLastUpdate > 10) {
        auto grid = Grid::get();
        for (int i = 0; i < width; i++) {
            for (int j = 0; j < height; j++) {
                if (grid->at(i, j)->updated) continue;
                grid->at(i, j)->updated = true;
                grid->update(i, j, msSinceLastUpdate);
            }
        }
        for (int i = 0; i < width; i++) {
            for (int j = 0; j < height; j++) {
                grid->at(i, j)->updated = false;
            }
        }
        for (int i = 0; i < width; i++) {
            for (int j = 0; j < height; j++) {
                if (grid->at(i, j)->lifetime <= 0) {
                    grid->clear(i, j);
                }
            }
        }
        fluid_step(grid->fluid);
        msSinceLastUpdate = 0;
    }
}

int main() {
    auto grid = Grid::get();
    // create the window
    sf::RenderWindow window(sf::VideoMode(window_width, window_height),
                            "I'm just one grain of sand on this beach");

    // create a texture
    sf::Texture texture;
    texture.create(width, height);

    // Create a pixel buffer to fill with RGBA data
    unsigned char* pixbuff = new unsigned char[width * height * 4];

    std::chrono::time_point<std::chrono::steady_clock> endTime =
        std::chrono::steady_clock::now();

    float counting = 0;
    bool mouseDown = false;
    sf::Vector2i lastMouse;
    while (window.isOpen()) {
        std::chrono::time_point<std::chrono::steady_clock> startTime =
            std::chrono::steady_clock::now();
        std::chrono::milliseconds elapsed(
            std::chrono::duration_cast<std::chrono::milliseconds>(startTime -
                                                                  endTime));
        endTime = startTime;
        counting += elapsed.count();

        // check all the window's events that were triggered since the last
        // iteration of the loop
        sf::Event event;
        while (window.pollEvent(event)) {
            // "close requested" event: we close the window
            if (event.type == sf::Event::Closed) window.close();
            if (event.type == sf::Event::KeyPressed) {
                handle_keycode(event.key.code);
                if (event.key.code == sf::Keyboard::Escape) {
                    window.close();
                }
            }
            if (event.type == sf::Event::MouseButtonPressed) {
                mouseDown = true;
            }
            if (event.type == sf::Event::MouseButtonReleased) {
                mouseDown = false;
            }
        }

        // clear the window with black color
        window.clear(sf::Color::Black);

        if (mouseDown) {
            sf::Vector2i pos = sf::Mouse::getPosition(window);
            if (pos.x < 0 || pos.y < 0 || (uint)pos.x > window_width ||
                (uint)pos.y > window_height) {
            } else {
                // Grid::get()->place_in_circle(pos.x / scale, pos.y / scale,
                // penRadius, selectedMaterial);
                Grid::get()->fluid->addDensity(pos.x / scale, pos.y / scale,
                                               1000);
                Grid::get()->fluid->addVelocity(pos.x / scale, pos.y / scale,
                                                10 * (lastMouse.x - pos.x),
                                                10 * (lastMouse.y - pos.y));
            }
            lastMouse = pos;
        }

        update(elapsed.count());
        draw(pixbuff);
        // Update screen
        texture.update(pixbuff);
        sf::Sprite sprite(texture);
        sprite.scale(sf::Vector2f(scale, scale));
        window.draw(sprite);
        // end the current frame
        window.display();

#ifdef SHOW_FPS
        frame++;
        if (counting > 1000) {
            counting = 0;
            std::cout << "fps: " << frame << std::endl;
            frame = 0;
        }
#endif
    }

    return 0;
}
