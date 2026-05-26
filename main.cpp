#include <chrono>
#include <fstream>
#include <iostream>
#include <cstddef>
#include <ratio>
#include <string>
#include <algorithm>
#include <filesystem>
#include "Grid.h"
#include "Simulation.h"
#include "GrayScott.h"

void PPMSave(const Grid& grid, const std::string& filename){
    std::ofstream file(filename);
    size_t width = grid.GetWidth();
    size_t height = grid.GetHeight();

    file << "P3\n" << width << " " << height << "\n255\n";      // PPM header: P3 (text RGB), width, height, 255 (max color value)

    for(size_t y = 0; y < height; ++y){
        for(size_t x = 0; x < width; ++x){
            float b = grid.Get(x, y).b;
            int color = std::clamp(static_cast<int>(b*255.0f), 0, 255);
            file << color << " " << color << " " << color << '\n';
        }
    }
}

int main(){
    const size_t WIDTH = 256;
    const size_t HEIGHT = 256;
    Simulation<GrayScott> simulation(WIDTH, HEIGHT);

    Grid& startG = const_cast<Grid&>(simulation.GetCurrentGrid());
    size_t centerX = WIDTH/2;
    size_t centerY = HEIGHT/2;

    // dropping chemical B in center of 20x20
    for(size_t x = centerX - 10; x < centerX + 10; ++x){
        for(size_t y = centerY - 10; y < centerY + 10; ++y){
            startG.Get(x, y).b = 1.0f;
        }
    }

    const int FRAMES = 1000;
    const float DELTA = 1.0f;

    std::cout << "Stating simulation with frames : " << FRAMES << '\n';

    auto stateTime = std::chrono::high_resolution_clock::now();
    std::filesystem::create_directory("assets");
    for(size_t frame = 1; frame <= FRAMES; ++frame){
        simulation.Update(DELTA);

        // saving image to see the pattern grow
        if(frame%100 == 0){
            std::string filename = "assets/output_frame-" + std::to_string(frame) + ".ppm";
            PPMSave(simulation.GetCurrentGrid(), filename);
            std::cout << "Rendered : " << filename << '\n';
        }
    }

    auto endTime = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> duration = endTime - stateTime;
    std::cout << "Simulation finished in : " << duration.count() << '\n';

    return 0;
}
