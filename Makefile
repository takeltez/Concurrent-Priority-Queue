TARGET := queue

CC := gcc
CFLAGS := -O0 -MD -Wall -Wextra -Werror -fopenmp -g

LIB_DIR := lib
BIN_DIR := bin
BUILD_DIR := build
SRC_DIR := src
INCLUDE_DIR := include
QUEUE_DIR := queue
BENCH_DIR := bench
DATA_DIR := data
PLOT_DIR := plots
REPORT_DIR := report
BENCHMARK_DIR := benchmark

PLOT_NAME := plot
REPORT_NAME := report
BENCHMARK_NAME := benchmark
ARCH_NAME = Aleksei_Mariana_amp_project8

RM := rm
INSTALL := install
MKDIR := mkdir

SRUN_PART := q_student
SRUN_THREAD_NUM := 64

QUEUE_SOURCES := $(wildcard $(SRC_DIR)/$(QUEUE_DIR)/*.c)
BENCH_SOURCES := $(wildcard $(SRC_DIR)/$(BENCH_DIR)/*.c)

QUEUE_OBJECTS := $(patsubst $(SRC_DIR)/$(QUEUE_DIR)/%.c, $(BUILD_DIR)/%.o, $(QUEUE_SOURCES))
BENCH_OBJECTS := $(patsubst $(SRC_DIR)/$(BENCH_DIR)/%.c, $(BUILD_DIR)/%.o, $(BENCH_SOURCES))

OBJECTS := $(BUILD_DIR)/main.o $(QUEUE_OBJECTS) $(BENCH_OBJECTS)

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
	@echo "Compiling src/main.c"
	@$(CC) $(CFLAGS) -fPIC -I$(SRC_DIR)/$(INCLUDE_DIR) -c -o $@ $<

$(BUILD_DIR)/%.o: $(SRC_DIR)/$(QUEUE_DIR)/%.c
	@echo "============================================"
	@echo "Compiling $(QUEUE_SOURCES)"
	@$(CC) $(CFLAGS) -fPIC -I$(SRC_DIR)/$(INCLUDE_DIR) -c -o $@ $<

$(BUILD_DIR)/%.o: $(SRC_DIR)/$(BENCH_DIR)/%.c
	@echo "============================================"
	@echo "Compiling $(BENCH_SOURCES)"
	@$(CC) $(CFLAGS) -fPIC -I$(SRC_DIR)/$(INCLUDE_DIR) -c -o $@ $<

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
	@echo "Installing $(TARGET) and $(TARGET).so into $(BIN_DIR)/ and $(LIB_DIR)/ directories respectively"
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

small-bench: $(DATA_DIR)
	@echo "============================================"
	@echo "Running small-bench"
	@srun -p $(SRUN_PART) -c $(SRUN_THREAD_NUM) ./$(BIN_DIR)/$(TARGET)

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
	@zip -r $(ARCH_NAME).zip $(BENCHMARK_DIR)/$(BENCHMARK_NAME).py $(SRC_DIR)/* $(PLOT_DIR)/$(PLOT_NAME).tex $(PLOT_DIR)/$(PLOT_NAME).pdf $(REPORT_DIR)/$(REPORT_NAME).tex $(REPORT_DIR)/$(REPORT_NAME).pdf Makefile README.md

.PHONY: all clean small-bench small-plot report zip

-include $(OBJECTS:.o=.d)
