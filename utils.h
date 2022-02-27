
#pragma once

#include <cmath>
#include <functional>

typedef unsigned int uint;
typedef unsigned char uchar;

constexpr int int_ceil(float f) {
    const int i = static_cast<int>(f);
    return f > i ? i + 1 : i;
}

inline int rgb(uchar r, uchar g, uchar b, uchar a = 255) {
    return (r << 24) | (g << 16) | (b << 8) | (a);
}

// #define SHOW_FPS

#if 1
const uint window_width = 1920;
const uint window_height = 1080;
#else
const unsigned int window_width = 3840;
const unsigned int window_height = 2160;
#endif

const uint scale = 10;

constexpr int width = int_ceil(1.f * window_width / scale) - 1;
constexpr int height = int_ceil(1.f * window_height / scale) - 1;

constexpr int dx[] = {-1, 0, 1, -1, 1, -1, 0, 1};
constexpr int dy[] = {-1, -1, -1, 0, 0, 1, 1, 1};

inline int xy(int x, int y) { return y * width + x; }
inline bool in(int x, int y) {
    return x >= 0 && y >= 0 && x < width && y < height;
}

inline void circle(int x, int y, int r,
                   const std::function<void(int, int)>& func) {
    int top = y - r;
    int bottom = y + r;
    for (int j = top; j <= bottom; j++) {
        int yd = j - y;
        int xd = sqrt(r * r - yd * yd);
        int left = ceil(x - xd);
        int right = floor(x + xd);
        for (int i = left; i <= right; i++) {
            func(i, j);
        }
    }
}

