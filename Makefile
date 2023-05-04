CC=fcc
CFLAGS=-Kfast -Kopenmp -Ideps/papi/opt/include
#-Koptmsg=2

# CC=clang
# CFLAGS=-O2 -fopenmp

LDFLAGS=-Ldeps/papi/opt/lib
LDLIBS=-lpapi -fopenmp

all: triad triad.multi triad.fapp
triad: triad.o handle_error.o
triad.multi: triad.multi.o handle_error.o
triad.fapp: triad.fapp.o handle_error.o
