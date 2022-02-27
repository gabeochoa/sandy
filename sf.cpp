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

/*
const int LIFESPAN = 1000;


enum MaterialFlags {
    None = 0,
    Solid = 1,
    Floats = 2,
    Falls = 4,
    Ages = 8,
    Flammable = 16,
    Emits = 32,
};

struct Tile {
    Material material;
    bool updated;
    float lifetime;
};

Material selectedMaterial = Water;
std::array<Tile, height * width> grid;

int getColorForMat(Material mat) {
    switch (mat) {
        case Sand:
            return rgb(189, 183, 107);
        case Empty:
            return rgb(0, 0, 0);
        case Ground:
            return rgb(255, 255, 255);
        case Water:
            return rgb(0, 0, 255);
        case Wood:
            return rgb(75, 42, 42);
        case Smoke:
            return rgb(5, 12, 12);
        case Fire:
            return rgb(255, 42, 42);
        case Cloud:
            return rgb(255, 255, 255, 150);
        case Steam:
            return rgb(200, 200, 200, 100);
    }
    return 0;
}

inline int xy(int x, int y) { return y * width + x; }
inline bool empty(int x, int y) { return grid[xy(x, y)].material == Empty; }
inline bool inbounds(int x, int y) {
    return !(x < 0 || y < 0 || x > width - 1 || y > height - 1);
}

inline int flags(Material m) {
    switch (m) {
        case Empty:
            return None;
        case Sand:
            return Solid | Falls;
        case Water:
            return None;
        case Wood:
            return Solid | Flammable;
        case Smoke:
            return Floats | Ages;
        case Fire:
            return None | Ages;
        case Cloud:
            return Solid | Emits;
        case Steam:
            return Floats | Ages;
        case Ground:
            return Solid;
    }
}

inline void age(Tile &t, int elapsed) {
    if (flags(t.material) & Ages) {
        t.lifetime -= elapsed;
        if (t.lifetime <= 0) t = Tile();
    }
}

inline bool is_flammable(int x, int y) {
    return flags(grid[xy(x, y)].material) & Flammable;
}

inline bool sink(int x, int y, Material m) {
    // Things sink if they are Falls and the material they are falling through
    // is not Solid
    if (flags(m) & Falls) {
        Material fallingThrough = grid[xy(x, y)].material;
        if (!(flags(fallingThrough) & Solid)) {
            return true;
        }
    }
    return false;
}

inline void clear(int x, int y) { grid[xy(x, y)] = Tile(); }

void _place(int x, int y, Material m) {
    if (!inbounds(x, y)) return;
    Tile &t = grid[xy(x, y)];
    t.material = m;
    t.lifetime = LIFESPAN;
}

void place(int x, int y, Material m) {
    if (scale < 100) {
        for (int i = 0; i < 8; i++) {
            _place(x + dx[i], y + dy[i], m);
        }
        _place(x, y, m);
    } else {
        _place(x, y, m);
    }
}

void place_if_empty(int x, int y, Material m) {
    if (empty(x, y)) return;
    place(x, y, m);
}

inline int pos_if_empty(int x, int y) {
    if (!inbounds(x, y)) return 0;
    return empty(x, y) ? xy(x, y) : 0;
}

void emit(int x, int y, Tile &t, Material p) {
    if (t.lifetime > 0) {
        place_if_empty(x, y, p);
    }
    t.lifetime -= 10;
}

bool fall_if_can(int x, int y, int dir = 1) {
    Tile &t = grid[xy(x, y)];
    int pos = 0;
    pos = pos_if_empty(x - 1, y + dir);  // down left
    pos = pos_if_empty(x + 1, y + dir);  // down right
    pos = pos_if_empty(x, y + dir);      // down
    if (pos == 0) return false;
    grid[pos] = Tile(t);
    t.updated = true;
    clear(x, y);
    return true;
}

// TODO because sand sinks
// its displacing the water UP

bool sink_if_can(int x, int y, Material m, int dir = 1) {
    Tile &t = grid[xy(x, y)];
    int pos = 0;
    if (sink(x, y + dir, m)) {
        pos = xy(x, y + dir);
    } else if (sink(x - 1, y + dir, m)) {
        pos = xy(x - 1, y + dir);
    } else if (sink(x + 1, y + dir, m)) {
        pos = xy(x + 1, y + dir);
    } else {
        return false;
    }

    // no sinking
    if (pos == 0) return false;

    t.updated = true;
    Tile displace = grid[pos];
    displace.updated = true;
    grid[pos] = Tile(t);
    grid[xy(x, y)] = displace;
    return true;
}

void spread_if_can(int x, int y) {
    bool left = empty(x - 1, y);
    bool right = empty(x + 1, y);
    if (!left && !right) return;

    Tile &t = grid[xy(x, y)];
    if (t.updated) return;
    int pos = 0;
    if (left && right) {
        if (rand() % 2 == 0)
            right = false;
        else
            left = false;
    }

    if (left) pos = xy(x - 1, y);
    if (right) pos = xy(x + 1, y);

    grid[pos] = Tile(t);
    clear(x, y);
    t.updated = true;
}

void drawColorForTile(unsigned char *pic, int i, const Tile &t) {
    int color = getColorForMat(t.material);
    pic[i * 4] = (color & 0xFF000000) >> 24;
    pic[i * 4 + 1] = (color & 0x00FF0000) >> 16;
    pic[i * 4 + 2] = (color & 0x0000FF00) >> 8;
    pic[i * 4 + 3] = (int)((1.f * t.lifetime / LIFESPAN) * 255);
}

void update_for_material(Material material, int x, int y) {
    Tile &t = grid[xy(x, y)];
    switch (material) {
        case Empty:
        case Ground:
            break;
        case Water: {
            bool fell = fall_if_can(x, y);
            // if (fell) return;
            spread_if_can(x, y);
        } break;
        case Sand: {
            sink_if_can(x, y, Sand);
            fall_if_can(x, y);
        } break;
        case Wood:
            break;
        case Steam:
        case Smoke: {
            sink_if_can(x, y, material, -1);
            fall_if_can(x, y, -1);
            spread_if_can(x, y);
        } break;
        case Fire: {
            if (rand() % 2 == 0 && empty(x, y - 1)) {
                place(x, y - 1, Smoke);
            }
            for (int i = 0; i < 8; i++) {
                int xp = x + dx[i];
                int yp = y + dy[i];
                if (is_flammable(xp, yp)) {
                    place(xp, yp, Fire);
                    // reset lifetime since we were able to spread
                    t.lifetime = LIFESPAN / 2.f;
                }
                if (grid[xy(xp, yp)].material == Water) {
                    place(xp, yp, Steam);
                    continue;
                }
            }
        } break;
        case Cloud: {
            emit(x, y + 2, t, Water);
        } break;
    }
}

void draw(unsigned char *col) {
    for (int i = 0; i < width * height; i++) {
        drawColorForTile(col, i, grid[i]);
    }
}

void update(int elapsed) {
    msSinceLastUpdate += elapsed;

    // Run updates  100/s
    if (msSinceLastUpdate > 10) {
        for (int i = 0; i < width * height; i++) {
            age(grid[i], msSinceLastUpdate);
            grid[i].updated = false;
        }

        msSinceLastUpdate = 0;
        for (int i = 0; i < width; i++) {
            for (int j = 0; j < height; j++) {
                Tile &t = grid[xy(i, j)];
                if (t.lifetime <= 0) {
                    clear(i, j);
                    continue;
                }
                if (t.updated) continue;
                update_for_material(t.material, i, j);
            }
        }
    }
}

void place_starting() {
    place(10, 10, Wood);

    for (int i = 0; i < width; i++) {
        place(i, 0, Ground);
        place(i, height - 2, Ground);
    }
    for (int i = 0; i < height; i++) {
        place(0, i, Ground);
        place(width - 1, i, Ground);
    }
}

*/

#include "elements.h"
#include "grid.h"
#include "materials.h"

Material selectedMaterial = Material::Sand;
void handle_keycode(int code) {
    switch (code) {
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
        default:
            break;
    }
    std::cout << "selected material is now: " << selectedMaterial << std::endl;
}

inline void draw(unsigned char* pic) {
    auto grid = Grid::get();
    for (int i = 0; i < width * height; i++) {
        int color = grid->colorat(i);
        pic[i * 4] = (color & 0xFF000000) >> 24;
        pic[i * 4 + 1] = (color & 0x00FF0000) >> 16;
        pic[i * 4 + 2] = (color & 0x0000FF00) >> 8;
        pic[i * 4 + 3] = 255;
    }
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
            auto pos = sf::Mouse::getPosition(window);
            if (pos.x < 0 || pos.y < 0 || (uint)pos.x > window_width ||
                (uint)pos.y > window_height) {
            } else {
                Grid::get()->place(pos.x / scale, pos.y / scale,
                                   selectedMaterial);
            }
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
