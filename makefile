CFLAGS     = -g -O2 -Wall -Werror -std=c17 -Wno-unused-function -Wdiscarded-qualifiers -Wincompatible-pointer-types -Wint-conversion -fno-plt
CC         = gcc
LD         = gcc
TARGET     = calculator

all: clean compile run

bin-dir:
	mkdir -p bin

build-dir:
	mkdir -p build

compile: bin-dir build-dir bin/$(TARGET)

build/ast.o: src/ast.c
	$(CC) -c $(CFLAGS) -o $@ $<

build/main.o: src/main.c
	$(CC) -c $(CFLAGS) -o $@ $<

build/tokenizer.o: src/tokenizer.c
	$(CC) -c $(CFLAGS) -o $@ $<

build/parser.o: src/parser.c
	$(CC) -c $(CFLAGS) -o $@ $<

bin/$(TARGET): build/ast.o build/main.o build/tokenizer.o build/parser.o
	$(LD) -o $@ $^

clean: 
	rm -rf bin
	rm -rf build

run: compile
	bin/$(TARGET)

.PHONY: clean all run compile
