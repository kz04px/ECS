CC         = g++
CFLAGS     = -std=c++14 -pthread -Wall -Wextra

LINKER     = g++ -o
LFLAGS     = -lglfw -lGLEW -lGL -lSDL2

TARGET     = main
SRCDIR     = src
OBJDIR     = obj
BINDIR     = bin

SOURCES  := $(wildcard $(SRCDIR)/*.cpp)
INCLUDES := $(wildcard $(SRCDIR)/*.hpp)
OBJECTS  := $(SOURCES:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)

$(BINDIR)/$(TARGET): $(OBJECTS)
	@$(LINKER) $@ $(OBJECTS) $(LFLAGS)
	@echo "Linking complete!"

$(OBJECTS): $(OBJDIR)/%.o : $(SRCDIR)/%.cpp
	@$(CC) $(CFLAGS) -I./src/ecs/ -I/usr/include/SDL2/ -c $< -o $@
	@echo "Compiled "$<" successfully!"
