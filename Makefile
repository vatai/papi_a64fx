# CC=fcc
CC=clang
CFLAGS=-O2 -Rpass-analysis=vector
LDLIBS=-lpapi
main: handle_error.o main.o
native1: native1.o handle_error.o
native2: native2.o handle_error.o
events1: events1.o handle_error.o
