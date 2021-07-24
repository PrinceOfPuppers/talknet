CC = gcc

SRC = src
BUILD = build
PROGRAM = main
CFLAGS = -Wall -lssl -lcrypto

INCDIRS = $(addprefix -I, $(dir $(wildcard include/*)))

INC = -Iinclude $(INCDIRS) #-Ilib/clog/include -Ilib/rpi-sense-hat-api/include
LIB = -lpthread #-L ./lib -l:clog/lib/clog.a -l:rpi-sense-hat-api/lib/sense-api.a

INCDIRS = $(addprefix -I, $(dir $(wildcard include/*/)))

FILES = $(wildcard $(SRC)/*/*.c) $(wildcard $(SRC)/*.c) 

# directories with makefiles which must be called
SUBMAKE = #lib/clog lib/rpi-sense-hat-api

# switched to debug if debug recipe is used
SUBMAKE_RECIPE = 

all: compile run

makeLibs:
	for dir in $(SUBMAKE); do \
		$(MAKE) -C $$dir $(SUBMAKE_RECIPE); \
	done

testLibs: 
	for dir in $(SUBMAKE); do \
		$(MAKE) -C $$dir test; \
	done

cleanLibs:
	for dir in $(SUBMAKE); do \
		$(MAKE) -C $$dir clean; \
	done


compile: makeLibs
	@mkdir -p $(BUILD)
	$(CC) $(CFLAGS) $(FILES) $(LIB) $(INC) -o $(BUILD)/$(PROGRAM)

debug: CFLAGS += -g
debug: SUBMAKE_RECIPE = debug
debug: compile

run:
	./$(BUILD)/$(PROGRAM)

clean: cleanLibs
	rm -rf $(BUILD)/*
