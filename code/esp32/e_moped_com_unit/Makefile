#Make file intended for running tests on pc instead of ESP32 hw.
#Change MODULE to the folder that you want to run, test must be named test.c

CC := gcc
CFLAGS := -Wall -Werror -D DEVENV

MODULE = ma_filter

LIB_DIR := lib
TEST_DIR := test
TEST_EXE := test
BUILD_DIR := build_test

INCLUDES := $(LIB_DIR)/$(MODULE) $(TEST_DIR)/$(MODULE)
INCLUDES := $(addprefix -I./, $(INCLUDES:%/=%))

OBJECTS := $(notdir $(wildcard $(LIB_DIR)/$(MODULE)/*.c) $(wildcard $(TEST_DIR)/$(MODULE)/*.c))
OBJECTS := $(addprefix $(BUILD_DIR)/, $(OBJECTS:.c=.o))

all: .mkbuild $(TEST_EXE)
	@echo ""
	@echo "************ The Targets ************"
	@echo "** clean: to clean"
	@echo "** check: to run the test"
	@echo "*************************************"
	@echo ""

$(TEST_EXE): $(OBJECTS)
	$(CC) $^ -lunity -lgcov -o $(BUILD_DIR)/$@

$(BUILD_DIR)/%.o: $(LIB_DIR)/$(MODULE)/%.c
	$(CC) -MMD $(CFLAGS) --coverage -o $@ $(INCLUDES) -c $<

$(BUILD_DIR)/%.o : $(TEST_DIR)/$(MODULE)/%.c
	$(CC) -MMD $(CFLAGS) -o $@ $(INCLUDES) -c $<


check: .mkbuild $(TEST_EXE)
	@echo ""
	@echo "**************************************"
	@echo "********** Run The Test **************"
	@echo "**************************************"
	@echo ""
	@./$(BUILD_DIR)/$(TEST_EXE); \
	gcovr -r . --html-details -o $(BUILD_DIR)/index.html

# Include automatically generated dependencies
-include $(OBJECTS:.o=.d)

.PHONY: clean .mkbuild check all

clean:
	@rm -rf $(BUILD_DIR)

.mkbuild:
	@mkdir -p $(BUILD_DIR)
