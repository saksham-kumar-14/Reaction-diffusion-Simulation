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

        const Cell* readData = readGrid.GetRawData();
        Cell* writeData = writeGrid.GetRawData();

        for (size_t y = 1; y < height - 1; ++y) {
            size_t centerRow = y * width;
            size_t upRow = (y - 1) * width;
            size_t downRow = (y + 1) * width;

            for (size_t x = 1; x < width - 1; ++x) {
                size_t c = centerRow + x;
                float a = readData[c].a;
                float b = readData[c].b;

                float lapA = (readData[upRow + x].a + readData[downRow + x].a +  readData[centerRow + x - 1].a + readData[centerRow + x + 1].a) * 0.2f + (readData[upRow + x - 1].a + readData[upRow + x + 1].a + readData[downRow + x - 1].a + readData[downRow + x + 1].a) * 0.05f - (a * 1.0f);

                float lapB = (readData[upRow + x].b + readData[downRow + x].b + readData[centerRow + x - 1].b + readData[centerRow + x + 1].b) * 0.2f + (readData[upRow + x - 1].b + readData[upRow + x + 1].b + readData[downRow + x - 1].b + readData[downRow + x + 1].b) * 0.05f - (b * 1.0f);

                float abb = a * b * b;
                float nextA = a + (diffusionA * lapA - abb + feed * (1.0f - a)) * deltaTime;
                float nextB = b + (diffusionB * lapB + abb - (kill + feed) * b) * deltaTime;

                if (nextA < 0.0f) nextA = 0.0f; else if (nextA > 1.0f) nextA = 1.0f;
                if (nextB < 0.0f) nextB = 0.0f; else if (nextB > 1.0f) nextB = 1.0f;

                writeData[c].a = nextA;
                writeData[c].b = nextB;
            }
        }

    }
};
