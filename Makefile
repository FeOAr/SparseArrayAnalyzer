# 编译器
CXX := g++
CXXFLAGS := -std=c++17 -Wall -Icore/inc

# 目录
CORE_SRC_DIR := core/src
CORE_OBJ_DIR := build/obj/core
TEST_DIR := test
BIN_DIR := build/bin

# 所有源文件和目标文件
CORE_SRCS := $(wildcard $(CORE_SRC_DIR)/*.cpp)
CORE_OBJS := $(patsubst $(CORE_SRC_DIR)/%.cpp, $(CORE_OBJ_DIR)/%.o, $(CORE_SRCS))

TEST_SRCS := $(wildcard $(TEST_DIR)/*.cpp)
TEST_OBJS := $(patsubst $(TEST_DIR)/%.cpp, $(CORE_OBJ_DIR)/%.o, $(TEST_SRCS))

TARGET := $(BIN_DIR)/sparse_array_analyzer

# 默认目标
all: $(TARGET)

# 生成最终可执行文件
$(TARGET): $(CORE_OBJS) $(TEST_OBJS)
	@mkdir -p $(BIN_DIR)
	$(CXX) $^ -o $@

# 编译 core 源码
$(CORE_OBJ_DIR)/%.o: $(CORE_SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# 编译 test 源码
$(CORE_OBJ_DIR)/%.o: $(TEST_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# 清理
clean:
	rm -rf build

.PHONY: all clean
