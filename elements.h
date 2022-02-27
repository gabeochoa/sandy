
#pragma once

#include <cmath>
#include <iostream>

#include "materials.h"
#include "utils.h"

struct Element {
    bool updated;
    float density;
    float friction;
    Element() {
        updated = false;
        density = 0;
        friction = 0;
    }
    virtual ~Element() {}
    virtual int color() = 0;
    virtual void update(int x, int y, float dt) = 0;
    bool fall(int x, int y, float);
    virtual Material material() const = 0;
};

struct Solid : public Element {
    Solid() : Element() { density = 2.f; }
};
struct Liquid : public Element {
    int heading;
    Liquid() : Element() {
        heading = 0;
        friction = 0.f;
        density = 1.f;
    }
    virtual void update(int x, int y, float dt) override;
};
struct Gas : public Element {};

struct Empty : public Element {
    int color() override { return rgb(0, 0, 0); }
    void update(int, int, float) override {}
    Material material() const override { return Material::Empty; }
};

struct FallingSolid : public Solid {
    FallingSolid() : Solid() { friction = 1.f; }
    virtual void update(int x, int y, float dt) override {
        this->fall(x, y, dt);
    }
};

struct Sand : public FallingSolid {
    Sand() : FallingSolid() { friction = 0.4f; }
    int color() override { return rgb(189, 183, 107); }
    Material material() const override { return Material::Sand; }
};

struct Water : public Liquid {
    Water() : Liquid() {}
    int color() override { return rgb(0, 0, 250); }
    Material material() const override { return Material::Water; }
};

