# part of glycerine
# by throwaway96

# Licensed under GNU AGPL version 3 or later.

# https://github.com/throwaway96/glycerine

CC = $(CROSS_COMPILE)gcc
LD = $(CROSS_COMPILE)gcc
CFLAGS = -pipe -Wall -Wextra -fPIC -std=gnu17 -D_GNU_SOURCE
LDFLAGS = -shared -ldl

OBJ = glycerine.o
OUT = libglycerine.so

.PHONY: all
all: $(OUT)

$(OUT): $(OBJ)
	$(LD) -o '$@' $(CFLAGS) $(LDFLAGS) $^

.PHONY: clean
clean:
	rm -f -- $(OBJ) $(OUT)
