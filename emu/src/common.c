#include "common.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
		opcode->dst.reg = dd;
		if (last3 <= 2) {
			opcode->src.type = OPER_REG;
			opcode->src.reg = last3;
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
		opcode->dst.reg = dd;
		opcode->src = opcode->dst;
		return 1;
	}
	else if (last3 == 6 || last3 == 7) {
		opcode->src.type = OPER_REG;
		opcode->src.reg = dd;
		uint8_t first4 = op >> 4;
		if (op == 0xaf || op == 0xef) {
			opcode->type = OP_FLAG;
			opcode->dst.type = OPER_FLAG;
			opcode->dst.reg = 0;
			opcode->dst.imm = op == 0xaf;
			return 1;
		}
		else if (op == 0x3e || op == 0x36 || op == 0xa6 || 
				op == 0xae || op == 0x3f) {
			opcode->type = OP_RESERVED;
			return 1;
		}
		else if (op == 0x2f) {
			opcode->type = OP_ALU1;
			opcode->dst.type = OPER_HL;
			return 1;
		}
		else if (aa == 0) {
			opcode->dst.type = last4 <= 7 ? OPER_MEM_HL : OPER_MEM_HL_PLUS;
		}
		else if (aa == 1) {
			if (last4 == 6) {
				opcode->dst.type = OPER_MEM_IMM;
				opcode->dst.imm = imm;
			}
			else if (last4 == 7) {
				if (first4 == 7) {
					opcode->type = OP_CALL;
					return 1;
				}
				else {
					opcode->type = OP_LPM;
					opcode->dst.type = OPER_REG;
					opcode->dst.reg = dd;
					opcode->src.type = OPER_MEM_HL;
					return 2;
				}
			}
			else {
				opcode->type = OP_RESERVED;
				return 1;
			}
		}
		else if (aa == 2) {
			opcode->dst.type = last4 <= 7 ? 
				OPER_MEM_IMM_L : OPER_MEM_IMM_L_PLUS;
			opcode->dst.imm = imm;
		}
		else if (aa == 3) {
			opcode->dst.type = last4 <= 7 ? OPER_MEM_L : OPER_MEM_L_PLUS;
		}

		if (last4 == 6 || last4 == 0xe) {
			opcode->type = OP_STORE;
			if (first4 == 2 || first4 == 0xe) {
				opcode->src.type = OPER_IMM;
				opcode->src.imm = imm;
			}
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
	opcode->type = OP_RESERVED;
	return 1;
}

int print_operand(operand_t* operand, char* str) {
	switch (operand->type) {
	case OPER_REG:       return sprintf(str, "%c", "ABLH"[operand->reg]);
	case OPER_IMM:       return sprintf(str, "0x%02x", operand->imm);
	case OPER_MEM_IMM:   return sprintf(str, "[00:%02x]", operand->imm);
	case OPER_MEM_HL:    return sprintf(str, "[H:L]");
	case OPER_MEM_HL_PLUS: return sprintf(str, "[H:L++]");
	case OPER_HL:        return sprintf(str, "H:L");
	case OPER_PCH_IMM:   return sprintf(str, "PCH:%02x", operand->imm);
	case OPER_MEM_IMM_L: return sprintf(str, "[%02x:L]", operand->imm);
	case OPER_MEM_IMM_L_PLUS: return sprintf(str, "[%02x:L++]", operand->imm);
	case OPER_MEM_L:     return sprintf(str, "[00:L]");
	case OPER_MEM_L_PLUS: return sprintf(str, "[00:L++]");
	case OPER_FLAG:      return sprintf(str, "%c", "CZNV"[operand->reg]);
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
		str += sprintf(str, "CALL H:L");
	}
	else if (opcode->type == OP_RESERVED) {
		str += sprintf(str, "RESERVED");
	}
	else if (opcode->type == OP_LPM) {
		str += sprintf(str, "LPM ");
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

void emulator_init(emulator_t* emul, uint8_t* rom, uint8_t* ram, uint16_t pc) {
	for (int i = 0; i < 4; i++) {
		emul->regs[i] = 0;
	}
	emul->pc = pc;
	emul->flags = 0;
	emul->ram = ram;
	emul->rom = rom;
}

void emulator_dump(emulator_t* emul) {
	opcode_t opcode;
	int cnt = decode(emul->rom + emul->pc, &opcode);
	char str[256];
	disassemble(&opcode, str);
	if (cnt == 1) {
		printf("PC: %04x -> [%02x]    %s\n", emul->pc, 
				emul->rom[emul->pc], str);
	}
	else if (cnt == 2) {
		printf("PC: %04x -> [%02x %02x] %s\n", emul->pc,
				emul->rom[emul->pc], emul->rom[emul->pc+1], str);
	}
	printf("H: %02x L: %02x\n", emul->regs[REG_H], emul->regs[REG_L]);
	printf("A: %02x B: %02x F: %02x\n", emul->regs[REG_A], emul->regs[REG_B], emul->flags);
	printf("====\n");
}

void emulator_step(emulator_t* emul) {
	opcode_t opcode;
	int cnt = decode(emul->rom + emul->pc, &opcode);
	emul->pc += cnt;
}
