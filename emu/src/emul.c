#include <stdio.h>
#include <stdlib.h>

#include "common.h"

uint8_t rom[100000];
uint8_t ram[100000];

int main(int argc, char** argv) {
	if (argc < 2) {
		err("Usage: %s file.bin\n", argv[0]);
	}
	FILE* f = fopen(argv[1], "rb");
	if (!f) {
		err("Could not open %s.\n", argv[1]);
	}
	int sz = fread(rom, 1, 1<<16, f);
	emulator_t emul;
	emulator_init(&emul, rom, ram, 0);
	while (1) {
		emulator_dump(&emul);
		emulator_step(&emul);
	}
}
