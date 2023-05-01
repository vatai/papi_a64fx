CC=fcc
CFLAGS=-Kfast -Kopenmp 
#-Koptmsg=2

# CC=clang
# CFLAGS=-O2 -fopenmp

LDLIBS=-lpapi -fopenmp

all: triad triad.multi
triad: triad.o handle_error.o
triad.multi: triad.multi.o handle_error.o
