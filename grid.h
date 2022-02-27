
#pragma once

#include <array>
#include <iostream>
#include <memory>

#include "materials.h"
#include "utils.h"
#include "vec.h"

struct Element;

const int iterations = 4;

struct FS {
    int size;
    float dt;
    float diff;
    float visc;
    float* s;
    float* density;

    float* vx;
    float* vy;

    float* vx0;
    float* vy0;

    float mx;

    FS(float dt, float diffusion, float viscocity) {
        this->size = fmax(width, height);
        this->dt = dt;
        this->diff = diffusion;
        this->visc = viscocity;

        this->s = new float[width * height];
        this->density = new float[width * height];

        this->vx = new float[width * height];
        this->vy = new float[width * height];

        this->vx0 = new float[width * height];
        this->vy0 = new float[width * height];

        for (int i = 0; i < width * height; i++) {
            this->s[i] = 0;
            this->density[i] = 0;
            this->vx[i] = 0;
            this->vy[i] = 0;
            this->vx0[i] = 0;
            this->vy0[i] = 0;
        }

        this->mx = 0.f;
    }

    ~FS() {
        delete[] s;
        delete[] density;
        delete[] vx;
        delete[] vy;
        delete[] vx0;
        delete[] vy0;
    }

    void addDensity(int x, int y, float amt) { this->density[xy(x, y)] += amt; }

    void addVelocity(int x, int y, float amtx, float amty) {
        this->vx[xy(x, y)] += amtx;
        this->vy[xy(x, y)] += amty;
    }

    void render(unsigned char* pic) {
        for (int i = 0; i < width * height; i++) {
            float d = this->density[i];
            unsigned char alpha = int_ceil(d * 255);
            int color = rgb(255, 255, 255, alpha);
            pic[i * 4] = (color & 0xFF000000) >> 24;
            pic[i * 4 + 1] = (color & 0x00FF0000) >> 16;
            pic[i * 4 + 2] = (color & 0x0000FF00) >> 8;
            pic[i * 4 + 3] = (color & 0x000000FF);
        }
    }
};

inline void set_bnd(int b, float* x) {
    int w = width;
    int h = height;
    for (int i = 1; i < w - 1; i++) {
        x[xy(i, 0)] = b == 2 ? -x[xy(i, 1)] : x[xy(i, 1)];
        x[xy(i, h - 1)] = b == 2 ? -x[xy(i, h - 2)] : x[xy(i, h - 2)];
    }
    for (int j = 1; j < h - 1; j++) {
        x[xy(0, j)] = b == 1 ? -x[xy(1, j)] : x[xy(1, j)];
        x[xy(w - 1, j)] = b == 1 ? -x[xy(w - 2, j)] : x[xy(w - 2, j)];
    }
    x[xy(0, 0)] = 0.5f * (x[xy(1, 0)] + x[xy(0, 1)]);
    x[xy(0, h - 1)] = 0.5f * (x[xy(1, h - 1)] + x[xy(0, h - 2)]);
    x[xy(w - 1, 0)] = 0.5f * (x[xy(w - 2, 0)] + x[xy(w - 1, 1)]);
    x[xy(w - 1, h - 1)] = 0.5f * (x[xy(w - 2, h - 1)] + x[xy(w - 1, h - 2)]);
}

inline void lin_solve(int b, float* x, float* x0, float a, float c) {
    float cRecip = 1.0 / c;
    for (int k = 0; k < iterations; k++) {
        for (int j = 1; j < height - 1; j++) {
            for (int i = 1; i < width - 1; i++) {
                x[xy(i, j)] =
                    (x0[xy(i, j)] +
                     a * (x[xy(i + 1, j)] + x[xy(i - 1, j)] + x[xy(i, j + 1)] +
                          x[xy(i, j - 1)] + x[xy(i, j)] + x[xy(i, j)])) *
                    cRecip;
            }
        }
        set_bnd(b, x);
    }
}

inline void diffuse(int b, float* x, float* x0, float diff, float dt) {
    float a = dt * diff * width - 2 * height - 2;
    lin_solve(b, x, x0, a, 1 + 6 * a);
}

inline void project(float* velocX, float* velocY, float* p, float* div) {
    int N = fmax(width, height);

    for (int j = 1; j < height - 1; j++) {
        for (int i = 1; i < width - 1; i++) {
            float vx = (velocX[xy(i + 1, j)] - velocX[xy(i - 1, j)]) / width;
            float vy = (velocY[xy(i, j + 1)] - velocY[xy(i, j - 1)]) / height;
            div[xy(i, j)] = -0.5f * vx + vy;
            p[xy(i, j)] = 0;
        }
    }
    set_bnd(0, div);
    set_bnd(0, p);
    lin_solve(0, p, div, 1, 6);

    for (int j = 1; j < N - 1; j++) {
        for (int i = 1; i < N - 1; i++) {
            velocX[xy(i, j)] -=
                0.5f * (p[xy(i + 1, j)] - p[xy(i - 1, j)]) * width;
            velocY[xy(i, j)] -=
                0.5f * (p[xy(i, j + 1)] - p[xy(i, j - 1)]) * height;
        }
    }
    set_bnd(1, velocX);
    set_bnd(2, velocY);
}

inline void advect(int b, float* d, float* d0, float* velocX, float* velocY,
                   float dt) {
    float i0, i1, j0, j1;

    float dtx = dt * (width - 2);
    float dty = dt * (height - 2);

    float s0, s1, t0, t1;
    float tmp1, tmp2, x, y;

    float ifloat, jfloat;
    int i, j;

    for (j = 1, jfloat = 1; j < height - 1; j++, jfloat++) {
        for (i = 1, ifloat = 1; i < width - 1; i++, ifloat++) {
            tmp1 = dtx * velocX[xy(i, j)];
            tmp2 = dty * velocY[xy(i, j)];
            x = ifloat - tmp1;
            y = jfloat - tmp2;

            if (x < 0.5f) x = 0.5f;
            if (x > width + 0.5f) x = width + 0.5f;
            i0 = floor(x);
            i1 = i0 + 1.0f;
            if (y < 0.5f) y = 0.5f;
            if (y > height + 0.5f) y = height + 0.5f;
            j0 = floor(y);
            j1 = j0 + 1.0f;

            s1 = x - i0;
            s0 = 1.0f - s1;
            t1 = y - j0;
            t0 = 1.0f - t1;

            int i0i = i0;
            int i1i = i1;
            int j0i = j0;
            int j1i = j1;

            d[xy(i, j)] =                       //
                s0 * (t0 * d0[xy(i0i, j0i)])    //
                + (t1 * d0[xy(i0i, j1i)])       //
                + s1 * (t0 * d0[xy(i1i, j0i)])  //
                + (t1 * d0[xy(i1i, j1i)]);
        }
    }
    set_bnd(b, d);
}

inline void fluid_step(std::shared_ptr<FS> fluid) {
    float visc = fluid->visc;
    float diff = fluid->diff;
    float dt = fluid->dt;
    float* vx = fluid->vx;
    float* vy = fluid->vy;
    float* vx0 = fluid->vx0;
    float* vy0 = fluid->vy0;
    float* s = fluid->s;
    float* density = fluid->density;

    diffuse(1, vx0, vx, visc, dt);
    diffuse(2, vy0, vy, visc, dt);

    project(vx0, vy0, vx, vy);

    advect(1, vx, vx0, vx0, vy0, dt);
    advect(2, vy, vy0, vx0, vy0, dt);

    project(vx, vy, vx0, vy0);

    diffuse(0, s, density, diff, dt);
    advect(0, density, s, vx, vy, dt);
}

struct Grid {
    std::shared_ptr<FS> fluid;
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
    bool flammable(int x, int y) const;

    std::shared_ptr<Element> materialToElement(Material);

    template <typename T>
    bool matching(int x, int y) const {
        if (!this->valid(x, y)) return false;
        std::shared_ptr<T> me = std::dynamic_pointer_cast<T>(at(x, y));
        return (bool)me;
    }

    static std::shared_ptr<Grid> get();
};
