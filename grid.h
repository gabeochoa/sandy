
#pragma once

#include <array>

#include "materials.h"
#include "utils.h"

struct Element;

struct Grid {
    std::array<std::shared_ptr<Element>, width * height> grid;
    Grid();
    std::shared_ptr<Element> at(int x, int y) const;
    bool valid(int x, int y) const;
    bool valid(int i) const;
    void _place(int x, int y, const std::shared_ptr<Element>& e);
    void place(int x, int y, const std::shared_ptr<Element>& e);
    void place(int x, int y, Material type);
    void place_in_circle(int x, int y, int r, Material type);
    bool place_if_empty(int x, int y, Material type);
    void clear(int x, int y);
    bool empty(int x, int y) const;
    bool swap(int x, int y, int a, int b);
    int colorat(int i) const;
    void update(int x, int y, float dt) const;
    bool lessdense(int x, int y, float density) const;
    std::shared_ptr<Element> materialToElement(Material);

    template <typename T>
    bool matching(int x, int y) const {
        if (!this->valid(x, y)) return false;
        std::shared_ptr<T> me = std::dynamic_pointer_cast<T>(at(x, y));
        return (bool)me;
    }

    static std::shared_ptr<Grid> get();
};
