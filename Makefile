#
# Student makefile for Cache Lab
# Note: requires a 64-bit x86-64 system 
#
CC = gcc
CFLAGS += -g -Wall -Werror -std=c99 -m64 -lm

all: |  csim test-trans tracegen

STUDENTID = 519021911045
NAME = 蒋圩淏

csim: cachelab.o csim.o
	$(CC) $(CFLAGS) $^ -O0 -o $@

test-trans: test-trans.o trans.o cachelab.o
	$(CC) $(CFLAGS) $^ -O0 -o $@

tracegen: tracegen.o trans.o cachelab.o
	$(CC) $(CFLAGS) $^ -O0 -o $@

%.o: %.c
	$(CC) $(CFLAGS) -O0 -c $^

report.pdf:
	+make -C report report.pdf
	cp ./report/report.pdf ./report.pdf

handout: report.pdf
	# Generate a handin tar file each time you compile
	zip ./${STUDENTID}-${NAME}.zip  csim.c trans.c report.pdf
#
# Clean the src directory
#
clean:
	+make -C report clean
	rm -rf *.o
	rm -f *.tar *.tmp ${STUDENTID}-${NAME}.zip
	rm -f csim report.pdf
	rm -f test-trans tracegen
	rm -f trace.all trace.f*
	rm -f .csim_results .marker
