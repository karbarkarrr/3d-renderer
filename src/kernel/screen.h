#pragma once

#include <memory>
#include <vector>
#include <tbb/spin_mutex.h>

#include "../linalg.h"

namespace renderer::kernel {
    class Screen {
        static constexpr int kTileSize = 16;

    public:
        Screen(int width, int height);

        void clear();
        void setPixel(int x, int y, Vector3 color, float depth);
        int getWidth() const;
        int getHeight() const;
        const std::vector<uint8_t> &getPixels() const;

    private:
        tbb::spin_mutex &tileMutex(int x, int y);

        int width_;
        int height_;
        std::vector<float> zbuffer_;
        std::vector<uint8_t> pixels_;

        int tileColumns_;
        std::unique_ptr<tbb::spin_mutex[]> tileMutexes_;
    };
}
