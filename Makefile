CXX = g++
CXXFLAGS = -std=c++17 -O3 -Wall -Wextra
TARGET = reaction
SRCS = main.cpp
HEADERS = Grid.h Simulation.h GrayScott.h

all: $(TARGET)

$(TARGET): $(SRCS) $(HEADERS)
	$(CXX) $(CXXFLAGS) $(SRCS) -o $(TARGET)
.PHONY: all run clean clean-images

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET)

clean-images:
	rm -rf assets/*.ppm
