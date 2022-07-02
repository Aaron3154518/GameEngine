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

# Get all header file dependencies relative to ./ and convert to .cpp
SOURCES = $(shell realpath --relative-to ./ $(patsubst %.h,%.cpp,$(filter $(dir $1)%.h, $(shell $(CXX) -MM $1))))
# Filter out missing .cpp files
EXIST = $(filter $(foreach file,$(call SOURCES,$1),$(wildcard $(file))),$(call SOURCES,$1))
# Compute .d and .h dependencies
DEPS = $(patsubst $(SRC)/%.cpp,$(OBJ)/%.d,$1) $(patsubst $(SRC)/%.cpp,$(OBJ)/%.d,$(call EXIST,$1))
OBJS = $(patsubst $(SRC)/%.cpp,$(OBJ)/%.o,$1) $(patsubst $(SRC)/%.cpp,$(OBJ)/%.o,$(call EXIST,$1))

.PHONY: all clean

all: test run-test

clean:
	@find $(OBJ) -type f \( -name "*.o" -o -name "*.d" \) -delete

test: RenderTest EventTest ServiceTest GameTest

run-test:
	@$(BIN)/RenderTest
	@$(BIN)/EventTest
	@$(BIN)/ServiceTest
	@$(BIN)/GameTest

RenderTest: $(call OBJS,src/RenderSystem/TestRenderSystem.cpp)
	$(CXX) $(CXXFLAGS) $^ -o $(BIN)/$@ $(INCLUDE_PATHS) $(LIBRARY_PATHS) $(LINKER_FLAGS)
-include $(call DEPS,src/RenderSystem/TestRenderSystem.cpp)

EventTest: $(call OBJS,src/EventSystem/TestEventSystem.cpp)
	$(CXX) $(CXXFLAGS) $^ -o $(BIN)/$@ $(INCLUDE_PATHS) $(LIBRARY_PATHS) $(LINKER_FLAGS)
-include $(call DEPS,src/EventSystem/TestEventSystem.cpp)

ServiceTest: $(call OBJS,src/ServiceSystem/TestServiceSystem.cpp)
	$(CXX) $(CXXFLAGS) $^ -o $(BIN)/$@ $(INCLUDE_PATHS) $(LIBRARY_PATHS) $(LINKER_FLAGS)
-include $(call DEPS,src/ServiceSystem/TestServiceSystem.cpp)

GameTest: $(call OBJS,src/TestGameSystem.cpp)
	$(CXX) $(CXXFLAGS) $^ -o $(BIN)/$@ $(INCLUDE_PATHS) $(LIBRARY_PATHS) $(LINKER_FLAGS)
-include $(call DEPS,src/TestGameSystem.cpp)

$(OBJ)/%.o: $(SRC)/%.cpp
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -MMD -MP -c $< -o $@ $(INCLUDE_PATHS) $(LIBRARY_PATHS) $(LINKER_FLAGS)

testing: testing.cpp
	$(CXX) $(CXXFLAGS) $< -o $@ $(INCLUDE_PATHS) $(LIBRARY_PATHS) $(LINKER_FLAGS)
