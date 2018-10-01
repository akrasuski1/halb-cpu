#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define err(...) do { printf(__VA_ARGS__); exit(1); } while (0)

uint8_t rom[100000];

int disassemble(uint8_t* rom, char* str) {
	uint8_t op = *rom;
	uint8_t imm = rom[1];

	const char* dest[] = {"H", "L", "A", "B"};
	uint8_t dd = (op >> 4) & 3;
	uint8_t last3 = op & 7;
	uint8_t last4 = op & 0xf;
	uint8_t aa = ((op >> 6) & 3);
	if (last3 <= 4) { 
		uint8_t aaa = ((op >> 3) & 1) | (((op >> 6) & 3) << 1);
		const char* alu2[] = {"ADD", "MOV", "SUB", "CMP", "OR", "XOR", "AND", "BIT"};
		str += sprintf(str, "%s %s, ", alu2[aaa], dest[dd]);
		if (last3 <= 2) {
			const char* src[] = {"A", "B", "L"};
			sprintf(str, "%s", src[last3]);
			return 1;
		}
		else {
			sprintf(str, "%s0x%02x%s", 
					last3 == 3 ? "" : "[", 
					imm,
					last3 == 3 ? "" : "]");
			return 2;
		}
	}
	else if (last4 == 5) {
		const char* alu1[] = {"INC", "DEC", "RCR", "ASR"};
		sprintf(str, "%s %s", alu1[aa], dest[dd]);
		return 1;
	}
	else if (last4 == 6 || last4 == 7) {
		str += sprintf(str, "MOV ");

		if (last4 == 7) {
			str += sprintf(str, "%s, ", dest[dd]);
		}

		if (aa == 0) str += sprintf(str, "[H:L]");
		if (aa == 1) str += sprintf(str, "[00:%02x]", imm);
		if (aa == 2) str += sprintf(str, "[%02x:L]", imm);
		if (aa == 3) str += sprintf(str, "[00:L]");

		if (last4 == 6) {
			sprintf(str, ", %s", dest[dd]);
		}

		if (aa == 1 || aa == 2) {
			return 2;
		}
		else {
			return 1;
		}
	}
	else if (last4 == 0xd) {
		uint8_t ccc = (op >> 4) & 7;
		const char* cond[] = {"JNE", "JE", "JNC", "JC", "JN", "JGE", "JL", "JMP"};
		str += sprintf(str, "%s ", cond[ccc]);
		if (op & 0x80) {
			sprintf(str, "H:L");
			return 1;
		}
		else {
			sprintf(str, "PCH:%02x", imm);
			return 2;
		}
	}
	else if (last4 == 0xf) {
		uint8_t first4 = op >> 4;
		if (first4 & 8) {
			sprintf(str, "%s%c", first4 & 4 ? "SE" : "CL", "ZCNV"[first4 & 3]);
			return 1;
		}
		else if (first4 == 0) {
			sprintf(str, "CALL");
			return 1;
		}
		else if (first4 <= 3) {
			sprintf(str, "RET%s, 0x%02x", dest[dd], imm);
			return 2;
		}
	}
	sprintf(str, "RESERVED");
	return 1;
}

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
		int cnt = disassemble(&rom[pc], str);
		if (cnt == 1) {
			printf("%04x: [%02x]    %s\n", pc, rom[pc], str);
		}
		else if (cnt == 2) {
			printf("%04x: [%02x %02x] %s\n", pc, rom[pc], rom[pc+1], str);
		}
		pc += cnt;
	}
}
