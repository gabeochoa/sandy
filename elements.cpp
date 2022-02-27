
#include "elements.h"

#include <iostream>

#include "grid.h"
#include "materials.h"

bool Element::fall(int x, int y, float, int dir) {
    auto grid = Grid::get();
    if (grid->empty(x, y + dir)) {
        grid->swap(x, y, x, y + dir);
    } else if (this->friction < 0.5 && grid->empty(x - 1, y + dir)) {
        grid->swap(x, y, x - 1, y + dir);
    } else if (this->friction < 0.5 && grid->empty(x + 1, y + dir)) {
        grid->swap(x, y, x + 1, y + dir);
    } else if (grid->lessdense(x, y + dir, this->density)) {
        grid->swap(x, y, x, y + dir);
    } else if (this->friction < 0.5 &&
               grid->lessdense(x - 1, y + dir, this->density)) {
        grid->swap(x, y, x - 1, y + dir);
    } else if (this->friction < 0.5 &&
               grid->lessdense(x + 1, y + dir, this->density)) {
        grid->swap(x, y, x + 1, y + dir);
    } else {
        return false;
    }
    return true;
}

void Element::update_fire(int x, int y, float dt) {
    if (!this->onfire) {
        return;
    }

    auto grid = Grid::get();
    this->lifetime -= dt;
    // TODO replace smoke when water is on fire
    if (rand() % 100 < 50) {
        for (int i = 0; i < 8; i++) {
            if (grid->place_if_empty(x + dx[i], y + dy[i], Material::Smoke)) {
                break;
            }
        }
    }

    if (this->spreads_fire()) {
        // only burn if not our first update
        if (this->lifetime > 0.9 * LIFETIME) {
            return;
        }
        for (int i = 0; i < 8; i++) {
            if (grid->flammable(x + dx[i], y + dy[i])) {
                if (rand() % 100 > 90) {
                    grid->at(x + dx[i], y + dy[i])->onfire = true;
                }
            }
        }
    }
}

void Liquid::update(int x, int y, float dt) {
    bool fell = this->fall(x, y, dt);
    if (fell) return;

    if (this->heading == 0) {
        int spread = width;
        this->heading = rand() % 2 == 0 ? spread : -spread;
    }
    this->heading--;

    // Check the tile in the direction we are heading
    int step = this->heading < 0 ? -1 : 1;
    // We hit a non empty tile, stop moving
    if (!Grid::get()->lessdense(x + step, y, this->density)) {
        this->heading = 0;
        return;
    }
    // its empty move there
    Grid::get()->swap(x, y, x + step, y);
}

void Gas::update(int x, int y, float dt) {
    this->lifetime -= dt;
    if (Element::fall(x, y, dt, -1)) {
        return;
    } else {
        auto grid = Grid::get();
        if (this->heading == 0) {
            int spread = width;
            this->heading = rand() % 2 == 0 ? spread : -spread;
        }
        this->heading--;
        // Check the tile in the direction we are heading
        int step = this->heading < 0 ? -1 : 1;
        // We hit a non empty tile, stop moving
        if (!grid->lessdense(x + step, y, this->density)) {
            this->heading = 0;
            return;
        }
        // its empty move there
        grid->swap(x, y, x + step, y);
    }
}

void Fire::update(int x, int y, float dt) { this->update_fire(x, y, dt); }

void Cloud::update(int x, int y, float) {
    if (rand() % 100 < 99) {
        return;
    }

    auto grid = Grid::get();
    int waterPlacementYIndex = y + 1;
    // Find the next non-cloud tile vertically below
    while (waterPlacementYIndex < height &&
           grid->matching<struct Cloud>(x, waterPlacementYIndex)) {
        waterPlacementYIndex++;
    }
    if (grid->matching<struct Empty>(x, waterPlacementYIndex) ||
        grid->matching<struct Smoke>(x, waterPlacementYIndex)) {
        grid->place(x, waterPlacementYIndex, Water);
    }
}

void Torch::update(int x, int y, float) {
    if (rand() % 100 < 99) {
        return;
    }
    for (int i = 0; i < 8; i++) {
        if (Grid::get()->place_if_empty(x + dx[i], y + dy[i], Material::Fire)) {
            break;
        }
    }
}

void Blackhole::update(int x, int y, float) {
    auto grid = Grid::get();
    int pullRange = 10;
    int pullRadius = pullRange / 2;
    for (int row = x - pullRadius; row <= x + pullRadius; row++) {
        for (int col = y + pullRadius; col >= y - pullRadius; col--) {
            if (!in(row, col)) continue;
            auto tile = grid->at(row, col);
            if (grid->matching<struct Empty>(row, col)) continue;
            if (grid->matching<struct Blackhole>(row, col)) continue;
            // Translate the tile in question towards the x,y
            // A positive xd will imply the tile moves right.
            int xd = 0;
            // A negative yd will imply the tile moves up.
            int yd = 0;

            // 1. Determine horizontal translation.
            // left of black hole
            if (row < x) {
                xd = 1;
            }
            // right of black hole
            else if (row > x) {
                xd = -1;
            }

            // 2. Determine vertical translation.
            // below black hole
            if (col > y) {
                yd = -1;
            }
            // above black hole
            else if (col < y) {
                yd = 1;
            }
            if (row + xd != x || col + yd != y) {
                if (grid->matching<Blackhole>(row + xd, col + yd)) continue;
                if (rand() % 100 < 60) continue;
                grid->swap(row, col, row + xd, col + yd);
            }
        }
    }
    for (int i = 0; i < 8; i++) {
        // clear all the ones around the black hole
        grid->clear(x + dx[i], y + dy[i]);
    }
}
