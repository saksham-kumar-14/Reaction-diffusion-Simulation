#include <algorithm>
#include <cstddef>
#include <memory>

enum class SimulationState{
    Uninitialised,
    Running,
    Paused,
    Error
};

class Grid{
private:
    size_t height;
    size_t width;
    std::unique_ptr<float[]> data;  // all grid data in 1D array

public:

    Grid(size_t IWidth, size_t IHeight): height(IHeight), width(IWidth), data(std::make_unique<float[]>(IWidth*IHeight)) {
        std::fill_n(data.get(), width*height, 0.0f);
    }

    Grid(const Grid&) = delete;                 // disable copies
    Grid& operator=(const Grid&) = delete;      // disable copies

    Grid(Grid&&) = default;
    Grid& operator=(Grid&&) = default;

    inline float& Get(size_t x, size_t y){
        #ifdef DEBUG
        if(x >= width or y >= height ){
            throw std::out_of_range("Grid coordiantes out of bound!\n");
        }
        #endif

        return data[y*width + x];
    }

    inline const float& Get(size_t x, size_t y) const {
        return data[y*width + x];
    }

    size_t GetHeight(){
        return height;
    }

    size_t GetWidth(){
        return width;
    }
};




int main(){

    return 0;
}
