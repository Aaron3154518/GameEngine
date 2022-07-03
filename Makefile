CXX := g++
CXXFLAGS :=  

AR := ar
ARFLAGS := -rv

INC := include
BIN := bin
SRC := src
OBJ := obj
LIB := lib

SDL_INC := i686-w64-mingw32/include/SDL2
SDL_LIB := i686-w64-mingw32/lib

INCLUDE_PATHS := -I$(SRC) -I$(INC)/SDL2-2.0.12/$(SDL_INC) -I$(INC)/SDL2_image-2.0.5/$(SDL_INC) -I$(INC)/SDL2_ttf-2.0.15/$(SDL_INC)
LIBRARY_PATHS := -L$(INC)/SDL2-2.0.12/$(SDL_LIB) -L$(INC)/SDL2_image-2.0.5/$(SDL_LIB) -L$(INC)/SDL2_ttf-2.0.15/$(SDL_LIB)
LINKER_FLAGS := -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf
GAME_LIBRARY := -L$(LIB) -lGameEngine

DEPFLAGS := -MM $(INCLUDE_PATHS)

# Get all header file dependencies relative to ./ and convert to .cpp
SOURCES = $(shell realpath --relative-to ./ $(patsubst %.h,%.cpp,$(filter $(SRC)/%.h, $(shell $(CXX) $(DEPFLAGS) $1))))
# Filter out missing .cpp files
EXIST = $(filter $(foreach file,$(call SOURCES,$1),$(wildcard $(file))),$(call SOURCES,$1))
# Compute .d and .h dependencies
DEPS = $(patsubst $(SRC)/%.cpp,$(OBJ)/%.d,$1) $(patsubst $(SRC)/%.cpp,$(OBJ)/%.d,$(call EXIST,$1))
OBJS = $(patsubst $(SRC)/%.cpp,$(OBJ)/%.o,$1) $(patsubst $(SRC)/%.cpp,$(OBJ)/%.o,$(call EXIST,$1))

.PHONY: all clean

all: GameLibrary

clean:
	@find $(OBJ) -type f \( -name "*.o" -o -name "*.d" \) -delete

test: RenderTest EventTest ServiceTest GameTest
	@$(BIN)/RenderTest
	@$(BIN)/EventTest
	@$(BIN)/ServiceTest
	@$(BIN)/GameTest

RenderTest: $(call OBJS,$(SRC)/RenderSystem/TestRenderSystem.cpp)
	$(CXX) $(CXXFLAGS) $^ -o $(BIN)/$@ $(INCLUDE_PATHS) $(LIBRARY_PATHS) $(LINKER_FLAGS)
-include $(call DEPS,$(SRC)/RenderSystem/TestRenderSystem.cpp)

EventTest: $(call OBJS,$(SRC)/EventSystem/TestEventSystem.cpp)
	$(CXX) $(CXXFLAGS) $^ -o $(BIN)/$@ $(INCLUDE_PATHS) $(LIBRARY_PATHS) $(LINKER_FLAGS)
-include $(call DEPS,$(SRC)/EventSystem/TestEventSystem.cpp)

ServiceTest: $(call OBJS,$(SRC)/ServiceSystem/TestServiceSystem.cpp)
	$(CXX) $(CXXFLAGS) $^ -o $(BIN)/$@ $(INCLUDE_PATHS) $(LIBRARY_PATHS) $(LINKER_FLAGS)
-include $(call DEPS,$(SRC)/ServiceSystem/TestServiceSystem.cpp)

GameTest: $(SRC)/TestGameSystem.cpp $(LIB)/libGameEngine.a
	$(CXX) $(CXXFLAGS) $< -o $(BIN)/$@ $(INCLUDE_PATHS) $(GAME_LIBRARY) $(LIBRARY_PATHS) $(LINKER_FLAGS)
-include $(call DEPS,$(SRC)/TestGameSystem.cpp)

$(OBJ)/%.o: $(SRC)/%.cpp
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -MMD -MP -c $< -o $@ $(INCLUDE_PATHS) $(LIBRARY_PATHS) $(LINKER_FLAGS)

.PHONY: GameLibrary
GameLibrary: $(LIB)/libGameEngine.a

$(LIB)/libGameEngine.a: $(call OBJS,$(SRC)/GameSystem.cpp)
	@mkdir -p $(@D)
	$(AR) $(ARFLAGS) $@ $^

testing: testing.cpp
	$(CXX) $(CXXFLAGS) $< -o $@ $(INCLUDE_PATHS) $(LIBRARY_PATHS) $(LINKER_FLAGS)
