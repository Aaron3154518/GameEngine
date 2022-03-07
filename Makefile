CXX := g++
CXXFLAGS := 

INC := include
BIN := bin
SRC := src
OBJ := obj

SDL_INC := i686-w64-mingw32/include/SDL2
SDL_LIB := i686-w64-mingw32/lib

INCLUDE_PATHS := -I$(INC)/SDL2-2.0.12/$(SDL_INC) -I$(INC)/SDL2_image-2.0.5/$(SDL_INC) -I$(INC)/SDL2_ttf-2.0.15/$(SDL_INC)
LIBRARY_PATHS := -L$(INC)/SDL2-2.0.12/$(SDL_LIB) -L$(INC)/SDL2_image-2.0.5/$(SDL_LIB) -L$(INC)/SDL2_ttf-2.0.15/$(SDL_LIB)
LINKER_FLAGS := -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf

define TARGET
$1: $(patsubst %.cpp, $(OBJ)/%.o, $2) $(patsubst %.h, $(OBJ)/%.o, $(filter %.h, $(shell $(CXX) -MM $2)));\
	$(CXX) $(CXXFLAGS) $$^ -o $(BIN)/$$@ $(INCLUDE_PATHS) $(LIBRARY_PATHS) $(LINKER_FLAGS)
endef

$(call TARGET,RenderTest,src/RenderSystem/TestRenderSystem.cpp)

$(OBJ)/%.o: %.cpp
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -MMD -MP -c $< -o $@ $(INCLUDE_PATHS) $(LIBRARY_PATHS) $(LINKER_FLAGS)