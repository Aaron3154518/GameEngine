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

OBJS = $(patsubst %.cpp, $(OBJ)/%.o, $1) $(patsubst %.h, $(OBJ)/%.o, $(filter %.h, $(shell $(CXX) -MM $1)))
DEPS = $(patsubst %.cpp, $(OBJ)/%.d, $1) $(patsubst %.h, $(OBJ)/%.d, $(filter %.h, $(shell $(CXX) -MM $1)))

test: RenderTest EventTest
	$(BIN)/RenderTest
	$(BIN)/EventTest

RenderTest: $(call OBJS,src/RenderSystem/TestRenderSystem.cpp)
	$(CXX) $(CXXFLAGS) $^ -o $(BIN)/$@ $(INCLUDE_PATHS) $(LIBRARY_PATHS) $(LINKER_FLAGS)
-include $(call DEPS,src/RenderSystem/TestRenderSystem.cpp)

EventTest: $(call OBJS,src/EventSystem/TestEventSystem.cpp)
	$(CXX) $(CXXFLAGS) $^ -o $(BIN)/$@ $(INCLUDE_PATHS) $(LIBRARY_PATHS) $(LINKER_FLAGS)
-include $(call DEPS,src/EventSystem/TestEventSystem.cpp)

$(OBJ)/%.o: %.cpp
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -MMD -MP -c $< -o $@ $(INCLUDE_PATHS) $(LIBRARY_PATHS) $(LINKER_FLAGS)