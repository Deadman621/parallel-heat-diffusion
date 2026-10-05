# ============================================================================
# Parallel Heat Diffusion
# ============================================================================

CC        := gcc
TARGET    := build/heat_diffusion

SRC_DIR   := src
INC_DIR   := include
BUILD_DIR := build

# ----------------------------------------------------------------------------
# Compiler flags
# ----------------------------------------------------------------------------

CPPFLAGS := -I$(INC_DIR)

CFLAGS := \
	-std=c17 \
	-Wall \
	-Wextra \
	-Wpedantic \
	-Wshadow \
	-Wconversion

OPTFLAGS := -O3 -march=native
OMPFLAGS := -fopenmp

# ----------------------------------------------------------------------------
# Sources
# ----------------------------------------------------------------------------

SOURCES := \
	$(SRC_DIR)/main.c \
	$(SRC_DIR)/grid.c \
	$(SRC_DIR)/solver_serial.c \
	$(SRC_DIR)/solver_openmp.c \
	$(SRC_DIR)/validation.c \
	$(SRC_DIR)/timer.c

OBJECTS := $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SOURCES))

# ----------------------------------------------------------------------------
# Default
# ----------------------------------------------------------------------------

.PHONY: all
all: release

# ----------------------------------------------------------------------------
# Release
# ----------------------------------------------------------------------------

.PHONY: release
release: CFLAGS += $(OPTFLAGS)
release: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) $(OMPFLAGS) -lm -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) $(OMPFLAGS) -c $< -o $@

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# ----------------------------------------------------------------------------
# Debug
# ----------------------------------------------------------------------------

.PHONY: debug
debug: clean
	$(MAKE) CFLAGS="$(CFLAGS) -O0 -g3" $(TARGET)

# ----------------------------------------------------------------------------
# Sanitizers
# ----------------------------------------------------------------------------

.PHONY: sanitize
sanitize: clean
	$(MAKE) \
		CFLAGS="$(CFLAGS) -O1 -g -fsanitize=address,undefined" \
		OMPFLAGS="-fopenmp -fsanitize=address,undefined" \
		$(TARGET)

# ----------------------------------------------------------------------------
# Run
# ----------------------------------------------------------------------------

.PHONY: run
run: release
	$(TARGET) --size 1024 --steps 1000 --threads 4 --mode parallel

# ----------------------------------------------------------------------------
# Correctness
# ----------------------------------------------------------------------------

.PHONY: check
check: release
	$(TARGET) --size 64 --steps 100 --threads 1 --mode parallel --validate
	$(TARGET) --size 64 --steps 100 --threads 2 --mode parallel --validate
	$(TARGET) --size 64 --steps 100 --threads 4 --mode parallel --validate

# ----------------------------------------------------------------------------
# Experiments
# ----------------------------------------------------------------------------

.PHONY: benchmark
benchmark: release
	bash scripts/benchmark.sh

.PHONY: strong
strong: release
	bash scripts/strong_scaling.sh

.PHONY: weak
weak: release
	bash scripts/weak_scaling.sh

# ----------------------------------------------------------------------------
# Analysis
# ----------------------------------------------------------------------------

.PHONY: analyze
analyze:
	python3 analysis/analyze.py

.PHONY: plots
plots:
	python3 analysis/plot_results.py

# ----------------------------------------------------------------------------
# Cleanup
# ----------------------------------------------------------------------------

.PHONY: clean
clean:
	rm -rf $(BUILD_DIR)

.PHONY: clean-results
clean-results:
	rm -rf results/raw/*
	rm -rf results/processed/*
	rm -rf results/plots/*

# ----------------------------------------------------------------------------
# Help
# ----------------------------------------------------------------------------

.PHONY: help
help:
	@echo "Parallel Heat Diffusion"
	@echo ""
	@echo "Build:"
	@echo "  make              Build optimized release"
	@echo "  make debug        Build debug version"
	@echo "  make sanitize     Build with ASan/UBSan"
	@echo ""
	@echo "Run:"
	@echo "  make run          Run default configuration"
	@echo "  make check        Run correctness checks"
	@echo ""
	@echo "Experiments:"
	@echo "  make benchmark    Run complete benchmark suite"
	@echo "  make strong       Run strong-scaling experiment"
	@echo "  make weak         Run weak-scaling experiment"
	@echo ""
	@echo "Analysis:"
	@echo "  make analyze      Process benchmark results"
	@echo "  make plots        Generate plots"
	@echo ""
	@echo "Cleanup:"
	@echo "  make clean"
	@echo "  make clean-results"