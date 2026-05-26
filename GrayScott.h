#pragma once
#include "Grid.h"

class GrayScott {
private:
    static constexpr float diffusionA = 1.0f;
    static constexpr float diffusionB = 0.5f;
    static constexpr float feed = 0.055f;
    static constexpr float kill = 0.062f;

public:
    void Compute(const Grid& readGrid, Grid& writeGrid, float deltaTime) {
        size_t width = readGrid.GetWidth();
        size_t height = readGrid.GetHeight();

        for(size_t y = 1; y < height - 1; ++y){
            for(size_t x = 1; x < width - 1; ++x){

                float a = readGrid.Get(x, y).a;
                float b = readGrid.Get(x, y).b;

                float lapA = (readGrid.Get(x, y-1).a + readGrid.Get(x, y+1).a + readGrid.Get(x-1, y).a + readGrid.Get(x+1, y).a) * 0.2f + (readGrid.Get(x-1, y-1).a + readGrid.Get(x+1, y-1).a + readGrid.Get(x-1, y+1).a + readGrid.Get(x+1, y+1).a) * 0.05f - (a * 1.0f);

                float lapB = (readGrid.Get(x, y-1).b + readGrid.Get(x, y+1).b + readGrid.Get(x-1, y).b + readGrid.Get(x+1, y).b) * 0.2f + (readGrid.Get(x-1, y-1).b + readGrid.Get(x+1, y-1).b + readGrid.Get(x-1, y+1).b + readGrid.Get(x+1, y+1).b) * 0.05f - (b * 1.0f);

                float abb = a * b * b;
                float nextA = a + (diffusionA * lapA - abb + feed * (1.0f - a)) * deltaTime;
                float nextB = b + (diffusionB * lapB + abb - (kill + feed) * b) * deltaTime;

                if (nextA < 0.0f) nextA = 0.0f; else if (nextA > 1.0f) nextA = 1.0f;
                if (nextB < 0.0f) nextB = 0.0f; else if (nextB > 1.0f) nextB = 1.0f;
                writeGrid.Get(x, y).a = nextA;
                writeGrid.Get(x, y).b = nextB;
            }
        }
    }
};
