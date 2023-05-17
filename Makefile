# Note! Makefile is ready but unusable due to nothing to work with.

TARGET := queue

CC := gcc
RM := rm
INSTALL := install
MKDIR := mkdir

CFLAGS := -O3 -Wall -Wextra -fopenmp

LIB_DIR := lib
BIN_DIR := bin
BUILD_DIR := build
SRC_DIR := src
INCLUDE_DIR := $(SRC_DIR)/include
DATA_DIR := data
PLOT_DIR := plots
REPORT_DIR := report
BENCH_DIR := bench

PLOT_NAME := plot
REPORT_NAME := report
BENCH_NAME := benchmark
ARCH_NAME = Aleksei_Mariana_amp_project8

SOURCES := $(wildcard $(SRC_DIR)/*.c)
OBJECTS := $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(SOURCES))

all: $(LIB_DIR) $(BIN_DIR) $(BUILD_DIR) $(TARGET) $(TARGET).so install

$(LIB_DIR):
	@echo "============================================"
	@echo "Creating $(LIB_DIR)/ directory"
	@$(MKDIR) -p $(LIB_DIR)

$(BIN_DIR):
	@echo "Creating $(BIN_DIR)/ directory"
	@$(MKDIR) -p $(BIN_DIR)

$(BUILD_DIR):
	@echo "Creating $(BUILD_DIR)/ directory"
	@$(MKDIR) -p $(BUILD_DIR)

$(DATA_DIR):
	@echo "============================================"
	@echo "Creating $(DATA_DIR)/ directory"
	@$(MKDIR) -p $(DATA_DIR)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@echo "============================================"
	@echo "Compiling $(SOURCES)"
	@$(CC) $(CFLAGS) -fPIC -I$(INCLUDE_DIR) -c -o $@ $<

$(TARGET): $(OBJECTS)
	@echo "============================================"
	@echo "Linking $(TARGET)"
	@$(CC) $(CFLAGS) -o $@ $^

$(TARGET).so: $(OBJECTS)
	@echo "============================================"
	@echo "Linking $(TARGET).so"
	@$(CC) $(CFLAGS) -fPIC -shared -o $@ $^

install:
	@echo "============================================"
	@echo "Installing $(TARGET) and $(TARGET).so into $(BIN_DIR)/ directory"
	@$(INSTALL) $(TARGET) $(BIN_DIR)
	@$(INSTALL) $(TARGET).so $(LIB_DIR)
	@$(RM) -f $(TARGET)
	@$(RM) -f $(TARGET).so

clean:
	@echo "============================================"
	@echo "Cleaning directories $(LIB_DIR)/, $(BIN_DIR)/ and $(BUILD_DIR)/"
	@$(RM) -rf $(LIB_DIR)
	@$(RM) -rf $(BIN_DIR)
	@$(RM) -rf $(BUILD_DIR)
	@$(RM) -rf $(DATA_DIR)

	@echo "Cleaning directories $(PLOT_DIR)/ and $(REPORT_DIR)/ from extra files"
	@bash -c 'cd $(PLOT_DIR) && find . -type f -not -name "*.tex" -delete'
	@bash -c 'cd $(REPORT_DIR) && find . -type f -not -name "*.tex" -delete'

	@echo "Removing archive $(ARCH_NAME).zip"
	@$(RM) -f $(ARCH_NAME).zip

bench: $(DATA_DIR)
	@echo "============================================"
	@echo "This could run a sophisticated benchmark"

small-bench: $(DATA_DIR)
	@echo "============================================"
	@echo "Running small-bench"
	@python3 $(BENCH_DIR)/$(BENCH_NAME).py

small-plot:
	@echo "============================================"
	@echo "Plotting small-bench results"
	@bash -c 'cd $(PLOT_DIR) && pdflatex "\batchmode\newcommand{\DATAPATH}{../data/$$(ls ../data/ | sort -r | head -n 1)}\input{$(PLOT_NAME).tex}"'
	@echo "Created $(PLOT_DIR)/$(PLOT_NAME).pdf"

report: small-plot
	@echo "============================================"
	@echo "Compiling report"
	@bash -c 'cd $(REPORT_DIR) && pdflatex -interaction=batchmode $(REPORT_NAME).tex'
	@echo "Created $(REPORT_DIR)/$(REPORT_NAME).pdf"

zip:
	@zip $(ARCH_NAME).zip $(BENCH_DIR)/* $(SRC_DIR)/* $(PLOT_DIR)/$(PLOT_NAME).tex $(REPORT_DIR)/$(REPORT_NAME).tex Makefile README.md

.PHONY: all clean bench small-bench small-plot report zip
