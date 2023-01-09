CXX := g++
CXXFLAGS := -Wall

AR := ar
ARFLAGS := -rv

INC := include
BIN := bin
SRC := src
OBJ := obj
LIB := lib

# SDL Variables
MINGW := x86_64-w64-mingw32
# MINGW := i686-w64-mingw32
SDL := $(INC)/SDL2-2.0.12/$(MINGW)
SDL_IMG := $(INC)/SDL2_image-2.0.5/$(MINGW)
SDL_TTF := $(INC)/SDL2_ttf-2.20.1/$(MINGW)
SDL_DIRS := $(SDL) $(SDL_IMG) $(SDL_TTF)
SDL_LIBS := SDL2 SDL2_image SDL2_ttf
SDL_BIN := bin
SDL_INC := include/SDL2
SDL_LIB := lib

INCLUDE_PATHS := -I$(SRC) $(foreach dir,$(SDL_DIRS),-I$(dir)/$(SDL_INC))
LIBRARY_PATHS := $(foreach dir,$(SDL_DIRS),-L$(dir)/$(SDL_LIB))
LINKER_FLAGS := -lSDL2main $(foreach lib,$(SDL_LIBS),-l$(lib))
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

$(OBJ)/%.o: $(SRC)/%.cpp
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -MMD -MP -c $< -o $@ $(INCLUDE_PATHS) $(LIBRARY_PATHS) $(LINKER_FLAGS)

ifeq ($(O),)
O := $(basename $(notdir $(T)))
endif

ifeq ($(T),)
compile:
	@echo "Please specify a compile target using 'T=path-to-file'"	
run: compile
else
compile: $(call OBJS_ALL,$(T))
	for dir in $(SDL_DIRS); do \
		cp $$dir/$(SDL_BIN)/*.dll ./; \
	done
	$(CXX) $(CXXFLAGS) $^ -o $(BIN)/$(O).exe $(INCLUDE_PATHS) $(LIBRARY_PATHS) $(LINKER_FLAGS)
-include $(call DEPS_ALL,$(T))
run: compile
	bin/$(O)
endif

RenderTest:
	make run T=$(SRC)/RenderSystem/TestRenderSystem.cpp O=$@
EventTest:
	make run T=$(SRC)/EventSystem/TestEventSystem.cpp O=$@
ServiceTest:
	make run T=$(SRC)/ServiceSystem/TestServiceSystem.cpp O=$@

ifeq ($(SRC),src)
.PHONY: GameLibrary
GameLibrary: $(LIB)/libGameEngine.a

$(LIB)/libGameEngine.a: $(call OBJS,$(SRC)/Includes.h)
	@mkdir -p $(@D)
	$(AR) $(ARFLAGS) $@ $^
-include $(call DEPS,$(SRC)/Includes.h)

GameTest: $(SRC)/TestGameSystem.cpp $(LIB)/libGameEngine.a
	$(CXX) $(CXXFLAGS) $< -o $(BIN)/$@ $(INCLUDE_PATHS) $(GAME_LIBRARY) $(LIBRARY_PATHS) $(LINKER_FLAGS)
	@$(BIN)/GameTest
-include $(call DEPS_ALL,$(SRC)/TestGameSystem.cpp)
endif
