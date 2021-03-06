CC         = g++
CFLAGS     = -std=c++14 -Wall -Wextra

LINKER     = g++ -o
LFLAGS     = -lGL -lSDL2 -lSDL2_image

TARGET     = main
SRCDIR     = src
OBJDIR     = obj
BINDIR     = bin

SOURCES  := $(wildcard $(SRCDIR)/*.cpp)
INCLUDES := $(wildcard $(SRCDIR)/*.hpp)
OBJECTS  := $(SOURCES:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)

$(BINDIR)/$(TARGET): $(BINDIR) $(OBJDIR) $(OBJECTS)
	@$(LINKER) $@ $(OBJECTS) $(LFLAGS)
	@echo "Linking complete!"

$(OBJECTS): $(OBJDIR)/%.o : $(SRCDIR)/%.cpp
	@$(CC) $(CFLAGS) -I./src/ecs/ -I/usr/include/SDL2/ -c $< -o $@
	@echo "Compiled "$<" successfully!"

bin:
	mkdir -p $(BINDIR)
obj:
	mkdir -p $(OBJDIR)

clean:
	rm -r $(OBJDIR)

.PHONY: clean
