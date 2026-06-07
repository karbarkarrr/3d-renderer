#include "screen.h"

#include <algorithm>
#include <limits>

namespace renderer::kernel {
    static constexpr int kMaxColorValue = 255;
    static constexpr int kColorCount = 3;

    Screen::Screen(int width, int height)
        : width_(width),
          height_(height),
          zbuffer_(width * height),
          pixels_(width * height * kColorCount) {
        tileColumns_ = (width + kTileSize - 1) / kTileSize;
        int tileRows = (height + kTileSize - 1) / kTileSize;
        tileMutexes_ = std::make_unique<tbb::spin_mutex[]>(tileColumns_ * tileRows);
    }

    tbb::spin_mutex &Screen::tileMutex(int x, int y) {
        return tileMutexes_[(x / kTileSize) + (y / kTileSize) * tileColumns_];
    }

    void Screen::clear() {
        std::ranges::fill(pixels_, static_cast<uint8_t>(0.5f * kMaxColorValue));
        std::ranges::fill(zbuffer_, std::numeric_limits<float>::infinity());
    }

    void Screen::setPixel(int x, int y, Vector3 color, float depth) {
        if (x < 0 || x >= width_ || y < 0 || y >= height_) {
            return;
        }
        tbb::spin_mutex::scoped_lock lock(tileMutex(x, y));
        int idx = y * width_ + x;
        if (depth >= zbuffer_[idx]) {
            return;
        }
        zbuffer_[idx] = depth;
        pixels_[idx * kColorCount] = static_cast<uint8_t>(color.r * kMaxColorValue);
        pixels_[idx * kColorCount + 1] = static_cast<uint8_t>(color.g * kMaxColorValue);
        pixels_[idx * kColorCount + 2] = static_cast<uint8_t>(color.b * kMaxColorValue);
    }

    int Screen::getWidth() const {
        return width_;
    }

    int Screen::getHeight() const {
        return height_;
    }

    const std::vector<uint8_t> &Screen::getPixels() const {
        return pixels_;
    }
}
