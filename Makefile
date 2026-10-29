CXX      ?= g++
CXXFLAGS ?= -std=c++17 -O2 -Wall -Wextra -Isrc

LIB_SRC  = src/avl_tree.cpp src/rb_tree.cpp src/tree_factory.cpp
LIB_OBJ  = $(LIB_SRC:src/%.cpp=build/%.o)
BIN      = bin

.PHONY: all test experiments demo evidencias run clean

all: $(BIN)/arvores $(BIN)/tests $(BIN)/experiments $(BIN)/demo

build/%.o: src/%.cpp src/tree.hpp src/avl_tree.hpp src/rb_tree.hpp
	@mkdir -p build
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BIN)/arvores: src/main.cpp $(LIB_OBJ)
	@mkdir -p $(BIN)
	$(CXX) $(CXXFLAGS) $^ -o $@

$(BIN)/tests: tests/tests.cpp $(LIB_OBJ)
	@mkdir -p $(BIN)
	$(CXX) $(CXXFLAGS) $^ -o $@

$(BIN)/experiments: tools/experiments.cpp $(LIB_OBJ)
	@mkdir -p $(BIN)
	$(CXX) $(CXXFLAGS) $^ -o $@

$(BIN)/demo: tools/demo.cpp $(LIB_OBJ)
	@mkdir -p $(BIN)
	$(CXX) $(CXXFLAGS) $^ -o $@

run: $(BIN)/arvores
	./$(BIN)/arvores

test: $(BIN)/tests
	./$(BIN)/tests

experiments: $(BIN)/experiments
	./$(BIN)/experiments

demo: $(BIN)/demo
	./$(BIN)/demo

# Regenera os arquivos de evidência usados na apresentação.
evidencias: $(BIN)/tests $(BIN)/experiments $(BIN)/demo
	./$(BIN)/tests       > docs/evidencias/testes.txt
	./$(BIN)/experiments > docs/evidencias/experimentos.md
	./$(BIN)/demo        > docs/evidencias/demo.txt

clean:
	rm -rf build $(BIN)
