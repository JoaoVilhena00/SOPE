CC = gcc
CFLAGS = -Wall
DEPS = auxiliary.h directory.h 
OBJ = auxiliary.o directory.o 
TARGETS = simpledu

all: simpledu

%.o: %.c $(DEPS)
	@$(CC) $(CFLAGS) -c -o $@ $<
	@echo $@

simpledu: $(OBJ)
	@$(CC) $(CFLAGS) -o $@ $@.c $(OBJ) -lm
	@echo $@

clean:
	@rm *.o $(TARGETS)