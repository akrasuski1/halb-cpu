#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define err(...) do { printf(__VA_ARGS__); exit(1); } while (0)

uint8_t rom[100000];

typedef enum opcode_type {
	OP_RESERVED, OP_ALU2, OP_ALU1, OP_LOAD, OP_STORE, OP_JUMP, OP_CALL, OP_RETX, OP_FLAG,
} opcode_type;

typedef enum alu1_type {
	ALU1_INC, ALU1_DEC, ALU1_RCR, ALU1_ASR,
} alu1_type;

typedef enum alu2_type {
	ALU2_ADD, ALU2_MOV, ALU2_SUB, ALU2_CMP, ALU2_OR, ALU2_XOR, ALU2_AND, ALU2_BIT,
} alu2_type;

typedef enum operand_type {
	OPER_NONE, OPER_REG, OPER_IMM, OPER_MEM_IMM, OPER_MEM_HL, OPER_MEM_IMM_L, 
	OPER_MEM_L, OPER_PCH_IMM, OPER_HL, OPER_FLAG,
} operand_type;

typedef enum reg_type {
	REG_NONE, REG_H, REG_L, REG_A, REG_B,
} reg_type;

typedef enum condition_type {
	COND_NE, COND_E, COND_NC, COND_C, COND_N, COND_GE, COND_L, COND_1,
} condition_type;

typedef struct operand_t {
	operand_type type;
	reg_type reg;
	uint8_t imm;
} operand_t;

typedef struct opcode_t {
	opcode_type type;
	union {
		alu1_type alu1;
		alu2_type alu2;
		condition_type cond;
	};
	operand_t dst;
	operand_t src;
} opcode_t;

int decode(uint8_t* rom, opcode_t* opcode) {
	uint8_t op = *rom;
	uint8_t imm = rom[1];
	uint8_t dd = (op >> 4) & 3;
	uint8_t last3 = op & 7;
	uint8_t last4 = op & 0xf;
	uint8_t aa = ((op >> 6) & 3);
	memset(opcode, 0, sizeof(opcode_t));
	if (last3 <= 4) { 
		opcode->type = OP_ALU2;
		opcode->alu2 = ((op >> 3) & 1) | (((op >> 6) & 3) << 1);
		opcode->dst.type = OPER_REG;
		opcode->dst.reg = REG_H + dd;
		if (last3 <= 2) {
			opcode->src.type = OPER_REG;
			opcode->src.reg = (last3 == 0) ? REG_A : (last3 == 1) ? REG_B : REG_L;
			return 1;
		}
		else if (last3 == 3) {
			opcode->src.type = OPER_IMM;
			opcode->src.imm = imm;
			return 2;
		}
		else {
			opcode->src.type = OPER_MEM_IMM;
			opcode->src.imm = imm;
			return 2;
		}
	}
	else if (last4 == 5) {
		opcode->type = OP_ALU1;
		opcode->alu1 = aa;
		opcode->dst.type = OPER_REG;
		opcode->dst.reg = REG_H + dd;
		opcode->src = opcode->dst;
		return 1;
	}
	else if (last4 == 6 || last4 == 7) {
		opcode->src.type = OPER_REG;
		opcode->src.reg = REG_H + dd;
		if (aa == 0) {
			opcode->dst.type = OPER_MEM_HL;
		}
		else if (aa == 1) {
			opcode->dst.type = OPER_MEM_IMM;
			opcode->dst.imm = imm;
		}
		else if (aa == 2) {
			opcode->dst.type = OPER_MEM_IMM_L;
			opcode->dst.imm = imm;
		}
		else if (aa == 3) {
			opcode->dst.type = OPER_MEM_L;
		}

		if (last4 == 6) {
			opcode->type = OP_STORE;
		}
		else {
			opcode->type = OP_LOAD;
			operand_t opt = opcode->src;
			opcode->src = opcode->dst;
			opcode->dst = opt;
		}
		if (aa == 1 || aa == 2) {
			return 2;
		}
		else {
			return 1;
		}
	}
	else if (last4 == 0xd) {
		opcode->type = OP_JUMP;
		opcode->cond = (op >> 4) & 7;
		if (op & 0x80) {
			opcode->dst.type = OPER_PCH_IMM;
			opcode->dst.imm = imm;
			return 2;
		}
		else {
			opcode->dst.type = OPER_HL;
			return 1;
		}
	}
	else if (last4 == 0xf) {
		uint8_t first4 = op >> 4;
		if (first4 & 8) {
			opcode->type = OP_FLAG;
			opcode->dst.type = OPER_FLAG;
			opcode->dst.reg = first4 & 3;
			opcode->dst.imm = (first4 & 4) >> 2;
			return 1;
		}
		else if (first4 == 0) {
			opcode->type = OP_CALL;
			return 1;
		}
		else if (first4 <= 3) {
			opcode->type = OP_RETX;
			opcode->dst.type = OPER_REG;
			opcode->dst.reg = REG_H + dd;
			opcode->src.type = OPER_IMM;
			opcode->src.imm = imm;
			return 2;
		}
	}
	opcode->type = OP_RESERVED;
	return 1;
}

int print_operand(operand_t* operand, char* str) {
	switch (operand->type) {
	case OPER_REG:       return sprintf(str, "%c", "HLAB"[operand->reg - REG_H]);
	case OPER_IMM:       return sprintf(str, "0x%02x", operand->imm);
	case OPER_MEM_IMM:   return sprintf(str, "[00:%02x]", operand->imm);
	case OPER_MEM_HL:    return sprintf(str, "[H:L]");
	case OPER_HL:        return sprintf(str, "H:L");
	case OPER_PCH_IMM:   return sprintf(str, "PCH:%02x", operand->imm);
	case OPER_MEM_IMM_L: return sprintf(str, "[%02x:L]", operand->imm);
	case OPER_MEM_L:     return sprintf(str, "[00:L]");
	case OPER_FLAG:      return sprintf(str, "%c", "ZCNV"[operand->reg]);
	default:             return sprintf(str, "???");
	}
}
		
void disassemble(opcode_t* opcode, char* str) {
	if (opcode->type == OP_ALU2) {
		const char* alu2[] = {"ADD", "MOV", "SUB", "CMP", "OR", "XOR", "AND", "BIT"};
		str += sprintf(str, "%s ", alu2[opcode->alu2]);
		str += print_operand(&opcode->dst, str);
		str += sprintf(str, ", ");
		str += print_operand(&opcode->src, str);
	}
	else if (opcode->type == OP_ALU1) {
		const char* alu1[] = {"INC", "DEC", "RCR", "ASR"};
		str += sprintf(str, "%s ", alu1[opcode->alu1]);
		str += print_operand(&opcode->dst, str);
	}
	else if (opcode->type == OP_LOAD || opcode->type == OP_STORE) {
		str += sprintf(str, "MOV ");
		str += print_operand(&opcode->dst, str);
		str += sprintf(str, ", ");
		str += print_operand(&opcode->src, str);
	}
	else if (opcode->type == OP_JUMP) {
		const char* cond[] = {"JNE", "JE", "JNC", "JC", "JN", "JGE", "JL", "JMP"};
		str += sprintf(str, "%s ", cond[opcode->cond]);
		str += print_operand(&opcode->dst, str);
	}
	else if (opcode->type == OP_CALL) {
		str += sprintf(str, "CALL");
	}
	else if (opcode->type == OP_RESERVED) {
		str += sprintf(str, "RESERVED");
	}
	else if (opcode->type == OP_RETX) {
		str += sprintf(str, "RET ");
		str += print_operand(&opcode->dst, str);
		str += sprintf(str, ", ");
		str += print_operand(&opcode->src, str);
	}
	else if (opcode->type == OP_FLAG) {
		str += sprintf(str, "%s", opcode->dst.imm ? "SE" : "CL");
		str += print_operand(&opcode->dst, str);
	}
	else {
		str += sprintf(str, "???");
	}
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
