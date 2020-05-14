
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
<<<<<<< HEAD
	@rm *.o $(TARGETS) -f
=======
	@rm *.o $(TARGETS) -f
>>>>>>> 7d1c8d7306b48f64894e4e61f249fcf64abf9ac6
