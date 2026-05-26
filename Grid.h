#pragma once
#include <algorithm>
#include <cstddef>
#include <memory>
#include <utility>
#include <stdexcept>

enum class SimulationState {
    Uninitialised,
    Running,
    Paused,
    Error
};

struct Cell {
    float a = 1.0f;
    float b = 0.0f;
};

class Grid {
private:
    size_t height;
    size_t width;
    std::unique_ptr<Cell[]> data;

public:
    Grid(size_t IWidth, size_t IHeight)
        : height(IHeight), width(IWidth), data(std::make_unique<Cell[]>(IWidth * IHeight))
    {
        std::fill_n(data.get(), width * height, Cell{1.0f, 0.0f});
    }

    Grid(const Grid&) = delete;
    Grid& operator=(const Grid&) = delete;

    Grid(Grid&& other) noexcept
        : height(std::exchange(other.height, 0)),
          width(std::exchange(other.width, 0)),
          data(std::move(other.data))
    {}

    Grid& operator=(Grid&& other) noexcept {
        if(this != &other){
            width = std::exchange(other.width, 0);
            height = std::exchange(other.height, 0);
            data = std::move(other.data);
        }
        return *this;
    }

    inline Cell& Get(size_t x, size_t y) {
        #ifdef DEBUG
        if(x >= width || y >= height) {
            throw std::out_of_range("Grid coordinates out of bound!\n");
        }
        #endif

        return data[y * width + x];
    }

    inline const Cell& Get(size_t x, size_t y) const {
        return data[y * width + x];
    }

    size_t GetHeight() const {
        return height;
    }

    size_t GetWidth() const {
        return width;
    }
};
