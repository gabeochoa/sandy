
#include "elements.h"

#include <iostream>

#include "grid.h"
#include "materials.h"

bool Element::fall(int x, int y, float) {
    auto grid = Grid::get();
    if (grid->empty(x, y + 1)) {
        grid->swap(x, y, x, y + 1);
    } else if (this->friction < 0.5 && grid->empty(x - 1, y + 1)) {
        grid->swap(x, y, x - 1, y + 1);
    } else if (this->friction < 0.5 && grid->empty(x + 1, y + 1)) {
        grid->swap(x, y, x + 1, y + 1);
    } else if (grid->lessdense(x, y + 1, this->density)) {
        grid->swap(x, y, x, y + 1);
    } else if (this->friction < 0.5 &&
               grid->lessdense(x - 1, y + 1, this->density)) {
        grid->swap(x, y, x - 1, y + 1);
    } else if (this->friction < 0.5 &&
               grid->lessdense(x + 1, y + 1, this->density)) {
        grid->swap(x, y, x + 1, y + 1);
    } else {
        return false;
    }
    return true;
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
    auto grid = Grid::get();

    if (grid->empty(x, y - 1)) {
        grid->swap(x, y, x, y - 1);
    } else if (this->friction < 0.5 && grid->empty(x - 1, y - 1)) {
        grid->swap(x, y, x - 1, y - 1);
    } else if (this->friction < 0.5 && grid->empty(x + 1, y - 1)) {
        grid->swap(x, y, x + 1, y - 1);
    } else if (grid->lessdense(x, y - 1, this->density)) {
        grid->swap(x, y, x, y - 1);
    } else if (this->friction < 0.5 &&
               grid->lessdense(x - 1, y - 1, this->density)) {
        grid->swap(x, y, x - 1, y - 1);
    } else if (this->friction < 0.5 &&
               grid->lessdense(x + 1, y - 1, this->density)) {
        grid->swap(x, y, x + 1, y - 1);
    } else {
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

void Fire::update(int x, int y, float dt) {
    auto grid = Grid::get();
    this->lifetime -= dt;
    // TODO should there only be smoke when something is burning
    for (int i = 0; i < 8; i++) {
        if ((rand() % 100) > 95) continue;
        if (grid->place_if_empty(x + dx[i], y + dy[i], Material::Smoke)) {
            break;
        }
    }

    // for (int i = 0; i < 8; i++) {
    // if (grid.is_flammable(x + dx[i], y + dy[i])) {
    // if (Math.random() > 0.9) {
    // grid.at(x + dx[i], y + dy[i]).onfire = true;
    // }
    // }
    // }
}
