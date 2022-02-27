
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
    bool empty(int x, int y) const;
    bool swap(int x, int y, int a, int b);
    int colorat(int i) const;
    void update(int x, int y, float dt) const;
    bool lessdense(int x, int y, float density) const;

    template <typename T>
    bool matching(int x, int y) const;

    static std::shared_ptr<Grid> get();
};

