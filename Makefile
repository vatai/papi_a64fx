CC=fcc
CFLAGS=-Kopenmp -Kfast
# CC=clang
# CFLAGS=-O2 -fopenmp
LDLIBS=-lpapi -fopenmp
main: handle_error.o main.o
events1: events1.o handle_error.o
