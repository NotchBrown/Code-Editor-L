# Makefile test file for tree-sitter symbol extraction
# Should extract: variable assignments, rule targets

# ---- Variables ----
CC = gcc
CXX = g++
CFLAGS = -Wall -Wextra -O2
CXXFLAGS = -Wall -Wextra -O2
LDFLAGS = -lm
TARGET = myapp
SRCDIR = src
BUILDDIR = build
SOURCES = $(wildcard $(SRCDIR)/*.c)
OBJECTS = $(patsubst $(SRCDIR)/%.c,$(BUILDDIR)/%.o,$(SOURCES))
HEADERS = $(wildcard inc/*.h)
INSTALL_DIR = /usr/local/bin

# ---- Phony targets ----
.PHONY: all clean install test

# ---- Default target ----
all: $(TARGET)

# ---- Link target ----
$(TARGET): $(OBJECTS)
	$(CC) $^ -o $@ $(LDFLAGS)

# ---- Compile target ----
$(BUILDDIR)/%.o: $(SRCDIR)/%.c $(HEADERS) | $(BUILDDIR)
	$(CC) $(CFLAGS) -Iinc -c $< -o $@

# ---- Directory creation ----
$(BUILDDIR):
	mkdir -p $@

# ---- Test target ----
test: $(TARGET)
	./$(TARGET) --test

# ---- Install target ----
install: $(TARGET)
	cp $(TARGET) $(INSTALL_DIR)

# ---- Clean target ----
clean:
	rm -rf $(BUILDDIR) $(TARGET)

# ---- Debug build ----
debug: CFLAGS += -g -DDEBUG
debug: all

# ---- Release build ----
release: CFLAGS += -DNDEBUG
release: all

# ---- Help ----
help:
	@echo "Available targets:"
	@echo "  all     - Build the project (default)"
	@echo "  clean   - Remove build artifacts"
	@echo "  test    - Run tests"
	@echo "  install - Install to system"
	@echo "  debug   - Build with debug symbols"
	@echo "  release - Build with optimizations"
