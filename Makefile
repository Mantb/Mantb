CXX      := g++
CXXFLAGS := -std=c++17 -O2 -Wall -Wextra -Iinclude -Iinclude/image_search $(shell pkg-config --cflags opencv4)
LDLIBS   := $(shell pkg-config --libs opencv4)

TARGET := main
SRCS   := $(wildcard src/*.cpp)
OBJS   := $(SRCS:src/%.cpp=build/%.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $^ -o $@ $(LDLIBS)

# -MMD -MP: track header dependencies so edits to .hpp files trigger rebuilds
build/%.o: src/%.cpp | build
	$(CXX) $(CXXFLAGS) -MMD -MP -c $< -o $@

build:
	mkdir -p build

clean:
	rm -rf build $(TARGET)

-include $(OBJS:.o=.d)

.PHONY: all clean
