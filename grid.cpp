
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
    fluid = std::make_shared<FS>(0.02f, 100.1, 0.01f);
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

void Grid::clear(int x, int y) {
    if (!valid(x, y)) return;
    return this->_place(x, y, materialToElement(Material::Empty));
}

bool Grid::place_if_empty(int x, int y, Material type) {
    if (!valid(x, y)) return false;
    if (!empty(x, y)) return false;
    this->place(x, y, type);
    return true;
}
std::shared_ptr<Element> Grid::materialToElement(Material type) {
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
            e = std::shared_ptr<Element>(new struct Wood());
            break;
        case Smoke:
            e = std::shared_ptr<Element>(new struct Smoke());
            break;
        case Fire:
            e = std::shared_ptr<Element>(new struct Fire());
            break;
        case Cloud:
            e = std::shared_ptr<Element>(new struct Cloud());
            break;
        case Steam:
            break;
        case Blackhole:
            e = std::shared_ptr<Element>(new struct Blackhole());
            break;
        case Dirt:
            e = std::shared_ptr<Element>(new struct Dirt());
            break;
        case Torch:
            e = std::shared_ptr<Element>(new struct Torch());
            break;
        case Oil:
            e = std::shared_ptr<Element>(new struct Oil());
            break;
    }
    return e;
}

void Grid::place(int x, int y, Material type) {
    if (!this->valid(x, y)) return;
    auto e = this->materialToElement(type);
    if (e) this->_place(x, y, e);
    return;
}

void Grid::place_in_circle(int x, int y, int r, Material type) {
    circle(x, y, r, [type](int i, int j) { Grid::get()->place(i, j, type); });
    return;
}

bool Grid::empty(int x, int y) const {
    if (!valid(x, y)) return false;
    return this->matching<struct Empty>(x, y);
}

bool Grid::flammable(int x, int y) const {
    if (!valid(x, y)) return false;
    return this->at(x, y)->flammable();
};

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
