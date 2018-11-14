#ifndef COMMON_H_
#define COMMON_H_

#include <stdint.h>

#define err(...) do { printf(__VA_ARGS__); exit(1); } while (0)

uint8_t rom[100000];

typedef enum opcode_type {
	OP_RESERVED, OP_ALU2, OP_ALU1, OP_LOAD, OP_STORE, OP_JUMP, OP_CALL, OP_LPM, OP_FLAG,
} opcode_type;

typedef enum alu1_type {
	ALU1_INC, ALU1_DEC, ALU1_RCR, ALU1_ASR,
} alu1_type;

typedef enum alu2_type {
	ALU2_ADD, ALU2_MOV, ALU2_SUB, ALU2_CMP, ALU2_OR, ALU2_XOR, ALU2_AND, ALU2_BIT,
} alu2_type;

typedef enum operand_type {
	OPER_NONE, OPER_REG, OPER_IMM, OPER_MEM_IMM, OPER_MEM_HL, OPER_MEM_IMM_L, 
	OPER_MEM_L, OPER_PCH_IMM, OPER_HL, OPER_FLAG, OPER_MEM_HL_PLUS,
	OPER_MEM_IMM_L_PLUS, OPER_MEM_L_PLUS,
} operand_type;

typedef enum reg_type {
	REG_A, REG_B, REG_L, REG_H,
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

typedef struct emulator_t {
	uint8_t regs[4];
	uint16_t pc;
	uint8_t flags;
	uint8_t* ram;
	uint8_t* rom;
} emulator_t;

int decode(uint8_t* rom, opcode_t* opcode);
int print_operand(operand_t* operand, char* str);
void disassemble(opcode_t* opcode, char* str);
void emulator_init(emulator_t* emul, uint8_t* rom, uint8_t* ram, uint16_t pc);
void emulator_dump(emulator_t* emul);
void emulator_step(emulator_t* emul);

#endif
