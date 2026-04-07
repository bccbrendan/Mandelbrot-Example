CXX = g++
DEBUG ?= 0

SRC_DIR = src
INCLUDE_DIR = include
BUILD_DIR = build
IMAGES_DIR = images

CXX_FLAGS = --std=c++11
CPP_FLAGS = -I${INCLUDE_DIR}

PARALLEL_VERSION = main_parallel.cpp bitmap.cpp mandelbrot_parallel.cpp
SINGLE_THREAD_VERSION = main_single_thread.cpp bitmap.cpp mandelbrot_single_thread.cpp


PARALLEL_OBJS = $(addprefix $(BUILD_DIR)/, $(PARALLEL_VERSION:.cpp=.o))
SINGLE_THREAD_OBJS = $(addprefix $(BUILD_DIR)/, $(SINGLE_THREAD_VERSION:.cpp=.o))

ifeq (${DEBUG}, 1)
	CXX_FLAGS += -g -O0
	BUILD_SUFFIX = _debug
else
	CXX_FLAGS += -O3
	BUILD_SUFFIX = 
endif

all: create parallel single_thread

parallel: $(PARALLEL_OBJS)
	$(CXX) $(CXX_FLAGS) $^ -o $(BUILD_DIR)/mandelbrot_parallel$(BUILD_SUFFIX)

single_thread: $(SINGLE_THREAD_OBJS)
	$(CXX) $(CXX_FLAGS) $^ -o $(BUILD_DIR)/mandelbrot_single_thread$(BUILD_SUFFIX)


# Compile .cpp → build/.o
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp | create
	$(CXX) $(CPP_FLAGS) $(CXX_FLAGS) -c $< -o $@

create:
	mkdir -p $(BUILD_DIR) $(IMAGES_DIR)

clean:
	rm -rf $(BUILD_DIR) $(IMAGES_DIR)

.PHONY: clean create all
