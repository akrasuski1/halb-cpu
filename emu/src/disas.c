#include <stdio.h>
#include <stdlib.h>

#include "common.h"

uint8_t rom[100000];

int main(int argc, char** argv) {
	if (argc < 2) {
		err("Usage: %s file.bin\n", argv[0]);
	}
	FILE* f = fopen(argv[1], "rb");
	if (!f) {
		err("Could not open %s.\n", argv[1]);
	}
	int sz = fread(rom, 1, 1<<16, f);
	int pc = 0;
	while (pc < sz) {
		char str[256];
		opcode_t opcode;
		int cnt = decode(rom, &opcode);
		disassemble(&opcode, str);
		if (cnt == 1) {
			printf("%04x: [%02x]    %s\n", pc, rom[pc], str);
		}
		else if (cnt == 2) {
			printf("%04x: [%02x %02x] %s\n", pc, rom[pc], rom[pc+1], str);
		}
		pc += cnt;
	}
}
