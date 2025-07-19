
BUILD_TYPE ?= release

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
TOOL_SRC_DIR := tool

CORE_OBJ_DIR := $(OBJ_DIR)/core
TEST_OBJ_DIR := $(OBJ_DIR)/test
TOOL_OBJ_DIR := $(OBJ_DIR)/tool

# 所有源文件和目标文件
CORE_SRCS := $(wildcard $(CORE_SRC_DIR)/*.cpp)
CORE_OBJS := $(patsubst $(CORE_SRC_DIR)/%.cpp, $(CORE_OBJ_DIR)/%.o, $(CORE_SRCS))

TEST_SRCS := $(wildcard $(TEST_SRC_DIR)/*.cpp)
TEST_OBJS := $(patsubst $(TEST_SRC_DIR)/%.cpp, $(TEST_OBJ_DIR)/%.o, $(TEST_SRCS))

TOOL_SRCS := $(wildcard $(TOOL_SRC_DIR)/*.cpp)
TOOL_OBJS := $(patsubst $(TOOL_SRC_DIR)/%.cpp, $(TOOL_OBJ_DIR)/%.o, $(TOOL_SRCS))

ANALYZER_BIN := $(BIN_DIR)/sparse_array_analyzer
TOOL_BIN := $(BIN_DIR)/generate_sparse_matrix


all: analyzer tool

# 1. 编译分析器
analyzer: $(ANALYZER_BIN)

$(ANALYZER_BIN): $(CORE_OBJS) $(TEST_OBJS)
	@mkdir -p $(BIN_DIR)
	$(CXX) $^ -o $@

$(CORE_OBJ_DIR)/%.o: $(CORE_SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(TEST_OBJ_DIR)/%.o: $(TEST_SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# 2.编译工具源码
tool: $(TOOL_BIN)

$(TOOL_BIN): $(TOOL_OBJS)
	@mkdir -p $(BIN_DIR)
	$(CXX) $^ -o $@

$(TOOL_OBJ_DIR)/%.o: $(TOOL_SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# 清理
clean:
	rm -rf build

.PHONY: all clean analyzer tool
