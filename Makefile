CXX := g++ -Wall
CXXFLAGS :=  

AR := ar
ARFLAGS := -rv

INC := include
BIN := bin
SRC := src
OBJ := obj
LIB := lib

SDL_INC := x86_64-w64-mingw32/include/SDL2
SDL_LIB := x86_64-w64-mingw32/lib

INCLUDE_PATHS := -I$(SRC) -I$(INC)/SDL2-2.0.12/$(SDL_INC) -I$(INC)/SDL2_image-2.0.5/$(SDL_INC) -I$(INC)/SDL2_ttf-2.20.1/$(SDL_INC)
LIBRARY_PATHS := -L$(INC)/SDL2-2.0.12/$(SDL_LIB) -L$(INC)/SDL2_image-2.0.5/$(SDL_LIB) -L$(INC)/SDL2_ttf-2.20.1/$(SDL_LIB)
LINKER_FLAGS := -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf
GAME_LIBRARY := -L$(LIB) -lGameEngine

DEPFLAGS := -MM $(INCLUDE_PATHS)

# Get all header file dependencies relative to ./ and convert to .cpp
SOURCES = $(shell realpath --relative-to ./ $(patsubst %.h,%.cpp,$(filter $(SRC)/%.h, $(shell $(CXX) $(DEPFLAGS) $1))))
# Filter out missing .cpp files
EXIST = $(filter $(foreach file,$(call SOURCES,$1),$(wildcard $(file))),$(call SOURCES,$1))
# Get all .d and .o files
DEPS = $(patsubst $(SRC)/%.cpp,$(OBJ)/%.d,$(call EXIST,$1))
OBJS = $(patsubst $(SRC)/%.cpp,$(OBJ)/%.o,$(call EXIST,$1))
# Get all .d/.o plus the input file
DEPS_ALL = $(patsubst $(SRC)/%.cpp,$(OBJ)/%.d,$1) $(call DEPS,$1)
OBJS_ALL = $(patsubst $(SRC)/%.cpp,$(OBJ)/%.o,$1) $(call OBJS,$1)

.PHONY: all clean

all: GameLibrary

clean:
	@find $(OBJ) -type f \( -name "*.o" -o -name "*.d" \) -delete

test: RenderTest EventTest ServiceTest GameTest

ifeq ($(O),)
O := $(basename $(notdir $(T)))
endif

ifeq ($(T),)
compile:
	@echo "Please specify a compile target using 'T=path-to-file'"
else
compile: $(call OBJS_ALL,$(T))
	$(CXX) $(CXXFLAGS) $^ -o $(BIN)/$(O).exe $(INCLUDE_PATHS) $(LIBRARY_PATHS) $(LINKER_FLAGS)
-include $(call DEPS_ALL,$(T))
endif

RenderTest: $(call OBJS_ALL,$(SRC)/RenderSystem/TestRenderSystem.cpp)
	$(CXX) $(CXXFLAGS) $^ -o $(BIN)/$@ $(INCLUDE_PATHS) $(LIBRARY_PATHS) $(LINKER_FLAGS)
	@$(BIN)/RenderTest
-include $(call DEPS_ALL,$(SRC)/RenderSystem/TestRenderSystem.cpp)

EventTest: $(call OBJS_ALL,$(SRC)/EventSystem/TestEventSystem.cpp)
	$(CXX) $(CXXFLAGS) $^ -o $(BIN)/$@ $(INCLUDE_PATHS) $(LIBRARY_PATHS) $(LINKER_FLAGS)
	@$(BIN)/EventTest
-include $(call DEPS_ALL,$(SRC)/EventSystem/TestEventSystem.cpp)

ServiceTest: $(call OBJS_ALL,$(SRC)/ServiceSystem/TestServiceSystem.cpp)
	$(CXX) $(CXXFLAGS) $^ -o $(BIN)/$@ $(INCLUDE_PATHS) $(LIBRARY_PATHS) $(LINKER_FLAGS)
	@$(BIN)/ServiceTest
-include $(call DEPS_ALL,$(SRC)/ServiceSystem/TestServiceSystem.cpp)

GameTest: $(SRC)/TestGameSystem.cpp $(LIB)/libGameEngine.a
	$(CXX) $(CXXFLAGS) $< -o $(BIN)/$@ $(INCLUDE_PATHS) $(GAME_LIBRARY) $(LIBRARY_PATHS) $(LINKER_FLAGS)
	@$(BIN)/GameTest
-include $(call DEPS_ALL,$(SRC)/TestGameSystem.cpp)

$(OBJ)/%.o: $(SRC)/%.cpp
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -MMD -MP -c $< -o $@ $(INCLUDE_PATHS) $(LIBRARY_PATHS) $(LINKER_FLAGS)

.PHONY: GameLibrary
GameLibrary: $(LIB)/libGameEngine.a

$(LIB)/libGameEngine.a: $(call OBJS,$(SRC)/Includes.h)
	@mkdir -p $(@D)
	$(AR) $(ARFLAGS) $@ $^
-include $(call DEPS,$(SRC)/Includes.h)
