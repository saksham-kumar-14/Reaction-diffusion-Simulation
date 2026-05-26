#pragma once
#include "Grid.h"
#include <algorithm>
#include <utility>

template<typename T>
class Simulation {
private:
    Grid gridA;
    Grid gridB;
    Grid* readGrid;
    Grid* writeGrid;
    T solver;

public:
    Simulation(size_t width, size_t height)
        : gridA(width, height), gridB(width, height)
    {
        readGrid = &gridA;
        writeGrid = &gridB;
    }

    void Update(float deltaTime) {
        solver.Compute(*readGrid, *writeGrid, deltaTime);
        std::swap(readGrid, writeGrid);
    }

    const Grid& GetCurrentGrid() const {
        return *readGrid;
    }
};
