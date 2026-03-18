CXX = g++
DEBUG ?= 0

SRC_DIR = src
INCLUDE_DIR = include
BUILD_DIR = build

CXX_FLAGS = --std=c++11
CPP_FLAGS = -I${INCLUDE_DIR}

PARALLEL_VERSION = main.cpp bitmap.cpp mandelbrot.cpp


PARALLEL_OBJS = $(addprefix $(BUILD_DIR)/, $(PARALLEL_VERSION:.cpp=.o))

ifeq (${DEBUG}, 1)
	CXX_FLAGS += -g -O0
	BUILD_SUFFIX = _debug
else
	CXX_FLAGS += -O3
	BUILD_SUFFIX = 
endif

all: create parallel

parallel: $(PARALLEL_OBJS)
	$(CXX) $(CXX_FLAGS) $^ -o $(BUILD_DIR)/mandelbrot_parallel$(BUILD_SUFFIX)


# Compile .cpp → build/.o
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp | create
	$(CXX) $(CPP_FLAGS) $(CXX_FLAGS) -c $< -o $@

create:
	mkdir -p $(BUILD_DIR)

clean:
	rm -rf $(BUILD_DIR)

.PHONY: clean create all