
CC = gcc
CFLAGS = -Wall -Wextra -pthread -pedantic
DEPS = auxiliary.h
OBJ = auxiliary.o
TARGETS = U2 Q2

all: U2 Q2

%.o: %.c $(DEPS)
	@$(CC) $(CFLAGS) -c -o $@ $<
	@echo $@

U2: $(OBJ)
	@$(CC) $(CFLAGS) -o $@ $@.c $(OBJ) -lm
	@echo $@

Q2: $(OBJ)
	@$(CC) $(CFLAGS) -o $@ $@.c $(OBJ) -lm
	@echo $@

clean:
	@rm *.o $(TARGETS) -f
