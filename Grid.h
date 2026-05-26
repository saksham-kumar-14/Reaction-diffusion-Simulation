#pragma once
#include <algorithm>
#include <cstddef>
#include <memory>
#include <utility>
#include <stdexcept>

class Grid {
private:
    size_t width;
    size_t height;

    // Separate contiguous arrays for A and B
    std::unique_ptr<float[]> dataA;
    std::unique_ptr<float[]> dataB;

public:
    Grid(size_t IWidth, size_t IHeight)
        : width(IWidth), height(IHeight),
          dataA(std::make_unique<float[]>(IWidth * IHeight)),
          dataB(std::make_unique<float[]>(IWidth * IHeight))
    {
        std::fill_n(dataA.get(), width * height, 1.0f);
        std::fill_n(dataB.get(), width * height, 0.0f);
    }

    Grid(const Grid&) = delete;
    Grid& operator=(const Grid&) = delete;

    Grid(Grid&& other) noexcept
        : width(std::exchange(other.width, 0)),
          height(std::exchange(other.height, 0)),
          dataA(std::move(other.dataA)),
          dataB(std::move(other.dataB))
    {}

    Grid& operator=(Grid&& other) noexcept {
        if(this != &other){
            width = std::exchange(other.width, 0);
            height = std::exchange(other.height, 0);
            dataA = std::move(other.dataA);
            dataB = std::move(other.dataB);
        }
        return *this;
    }

    inline float& GetA(size_t x, size_t y) { return dataA[y * width + x]; }
    inline float& GetB(size_t x, size_t y) { return dataB[y * width + x]; }
    inline const float& GetB(size_t x, size_t y) const { return dataB[y * width + x]; }

    inline float* GetRawA() { return dataA.get(); }
    inline float* GetRawB() { return dataB.get(); }
    inline const float* GetRawA() const { return dataA.get(); }
    inline const float* GetRawB() const { return dataB.get(); }

    size_t GetWidth() const { return width; }
    size_t GetHeight() const { return height; }
};
