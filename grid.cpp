
#include "grid.h"

#include <array>
#include <memory>

#include "elements.h"
#include "materials.h"
#include "utils.h"

std::shared_ptr<Grid> _grid;

Grid::Grid() {
    for (int i = 0; i < width * height; i++) {
        grid[i] = std::shared_ptr<Element>(new struct Empty());
    }
}

std::shared_ptr<Grid> Grid::get() {
    if (!_grid) _grid = std::make_shared<Grid>();
    return _grid;
}

std::shared_ptr<Element> Grid::at(int x, int y) const { return grid[xy(x, y)]; }

bool Grid::valid(int x, int y) const {
    if (!in(x, y)) return false;
    auto e = grid[xy(x, y)];
    return (bool)e;
}

bool Grid::valid(int i) const {
    return i >= 0 && i < width * height && grid[i];
}

void Grid::_place(int x, int y, const std::shared_ptr<Element>& e) {
    grid[xy(x, y)] = e;
}

void Grid::place(int x, int y, const std::shared_ptr<Element>& e) {
    if (!valid(x, y)) return;
    grid[xy(x, y)] = e;
}

void Grid::place(int x, int y, Material type) {
    if (!this->valid(x, y)) return;
    std::shared_ptr<Element> e;

    switch (type) {
        case Empty:
            e = std::shared_ptr<Element>(new struct Empty());
            break;
        case Sand:
            e = std::shared_ptr<Element>(new struct Sand());
            break;
        case Water:
            e = std::shared_ptr<Element>(new struct Water());
            break;
        case Wood:
        case Smoke:
        case Fire:
        case Cloud:
        case Steam:
        case Ground:
            break;
    }
    if (e) this->_place(x, y, e);
    return;
}

bool Grid::empty(int x, int y) const {
    if (!valid(x, y)) return false;
    return this->matching<struct Empty>(x, y);
}

template <typename T>
bool Grid::matching(int x, int y) const {
    if (!valid(x, y)) return false;
    std::shared_ptr<T> me = std::dynamic_pointer_cast<T>(at(x, y));
    return (bool)me;
}

bool Grid::swap(int x, int y, int a, int b) {
    if (!valid(x, y)) return false;
    if (!valid(a, b)) return false;
    std::shared_ptr<Element> me = at(x, y);
    std::shared_ptr<Element> them = at(a, b);
    this->place(x, y, them);
    this->place(a, b, me);
    return true;
}

bool Grid::lessdense(int x, int y, float density) const {
    if (!this->valid(x, y)) return false;
    return grid[xy(x, y)]->density < density;
}

int Grid::colorat(int i) const {
    if (!valid(i)) return rgb(255, 0, 255);
    return grid[i]->color();
}

void Grid::update(int x, int y, float dt) const {
    if (!valid(x, y)) return;
    return grid[xy(x, y)]->update(x, y, dt);
}
