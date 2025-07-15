# 模式控制：debug 或 release（默认是 debug）
BUILD_TYPE ?= debug

# 编译器
CXX := g++
CXXFLAGS := -std=c++17 -Wall -Icore/inc

# 根据模式设置路径和选项
ifeq ($(BUILD_TYPE),debug)
    CXXFLAGS += -g
    BIN_DIR := build/debug/bin
    OBJ_DIR := build/debug/obj
else ifeq ($(BUILD_TYPE),release)
    CXXFLAGS += -O2
    BIN_DIR := build/release/bin
    OBJ_DIR := build/release/obj
else
    $(error Unknown BUILD_TYPE: $(BUILD_TYPE))
endif

# 目录
CORE_SRC_DIR := core/src
TEST_SRC_DIR := test

CORE_OBJ_DIR := $(OBJ_DIR)/core
TEST_OBJ_DIR := $(OBJ_DIR)/test

# 所有源文件和目标文件
CORE_SRCS := $(wildcard $(CORE_SRC_DIR)/*.cpp)
CORE_OBJS := $(patsubst $(CORE_SRC_DIR)/%.cpp, $(CORE_OBJ_DIR)/%.o, $(CORE_SRCS))

TEST_SRCS := $(wildcard $(TEST_SRC_DIR)/*.cpp)
TEST_OBJS := $(patsubst $(TEST_SRC_DIR)/%.cpp, $(TEST_OBJ_DIR)/%.o, $(TEST_SRCS))

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
$(TEST_OBJ_DIR)/%.o: $(TEST_SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# 清理
clean:
	rm -rf build

.PHONY: all clean
