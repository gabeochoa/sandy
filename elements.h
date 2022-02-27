
#pragma once

#include <cmath>
#include <iostream>

#include "materials.h"
#include "utils.h"

const float LIFETIME = 1000.f;

struct Element {
    bool updated;    // has been updated this fram
    float density;   // used for falling
    float friction;  // used for falling
    float lifetime;  // tracks lifetime
    int heading;     // used for things that spread

    Element() {
        heading = 0;
        lifetime = LIFETIME;
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
    Liquid() : Element() {
        friction = 0.f;
        density = 1.f;
    }
    virtual void update(int x, int y, float dt) override;
};
struct Gas : public Element {
    void update(int, int, float) override;
};

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

struct StillSolid : public Solid {
    StillSolid() : Solid() {}
    virtual void update(int, int, float) override {}
};

struct Wood : public StillSolid {
    Wood() : StillSolid() { friction = 1.0f; }
    int color() override { return rgb(55, 25, 0); }
    Material material() const override { return Material::Wood; }
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

struct Fire : public Solid {
    Fire() : Solid() { this->friction = 1; }
    void update(int x, int y, float dt) override;
    int color() override {
        int alpha = std::floor((this->lifetime / LIFETIME) * 255);
        return rgb(255, 0, 0, alpha);
    }
    Material material() const override { return Material::Fire; }
};

struct Smoke : public Gas {
    Smoke() : Gas() { this->friction = 1; }
    int color() override {
        if (rand() % 100 < 33) {
            return rgb(20, 20, 20, 128);
        } else if (rand() % 100 < 66) {
            return rgb(40, 40, 40, 128);
        } else {
            return rgb(60, 60, 60, 128);
        }
    }
    Material material() const override { return Material::Smoke; }
};
