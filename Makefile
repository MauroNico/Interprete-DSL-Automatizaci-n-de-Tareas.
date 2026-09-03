CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Iinclude

SRC_DIR = src
OBJ_DIR = obj

# busca los .cpp en
SOURCES = $(wildcard $(SRC_DIR)/*.cpp)
# transforma en .o
OBJECTS = $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SOURCES))

EXECUTABLE = interprete.exe

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $^

# control de directorios cmd
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@if not exist $(OBJ_DIR) mkdir $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# limpieza para windows cmd
clean:
	@if exist $(OBJ_DIR) rmdir /s /q $(OBJ_DIR)
	@if exist $(EXECUTABLE) del /q $(EXECUTABLE)

run: $(EXECUTABLE)
	./$(EXECUTABLE) ejemplos/script_completo.txt

.PHONY: all clean run