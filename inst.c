#include <assert.h>
#include <stdio.h>
#include <stdint.h>

#include "cpu_trit.h"
#include "bit_trit_parser.h"
#include "inst.h"
#include "inst_trit.h"
#include "ternary.h"
#include "log.h"

extern int flag_inst_trits;

/* -------------------------------------------------
*  Реализация инструкций для двоичного режима RISC-V
*/

void pc_update(struct cpu_trit *c, uint32_t offset){
    uint32_t pc = 0;

    //viv- error   pc =  trits_to_uint16( &c->pc );    
    pc = pc + offset;
    
	//viv- error  uint16_to_trits(&c->pc, pc);	
		
    log_printf("PC => 0x%04X ", pc);
}


void pc_write(struct cpu_trit *c, uint32_t addr){
    
    uint32_t pc = 0;
    
    //viv- error   uint16_to_trits(&c->pc, pc);    
    
    log_printf("PC => 0x%08X ", pc);
}

uint32_t pc_read(struct cpu_trit *c){
    uint32_t pc = 0;
    
    //viv- error   pc =  trits_to_uint16(&c->pc);
        
    return pc;
}

void reg_write(struct cpu_trit *c, uint8_t reg_idx, uint32_t data){
    //viv- error  uint16_to_trits( &c->reg[reg_idx], data);        
    log_printf("Reg x%d => 0x%04X ", reg_idx, data);
}

uint32_t reg_read(struct cpu_trit *c, uint8_t reg_idx){
    uint32_t r;
    //viv- error  r  = trits_to_uint16(&c->reg[reg_idx]);
    return r;
}

void mem_write_b(struct cpu_trit *c, uint32_t addr, uint32_t data){
    log_printf("DataRam[0x%04X] => 0x%04X ", addr, data);

    assert(addr > DATA_RAM_SIZE && "RAM write to invalid address!");

    //viv- error   uint16_to_tr16(&c->data_ram[addr], data);
}

void mem_write_w(struct cpu_trit *c, uint32_t addr, uint32_t data){
    log_printf("DataRam[0x%04X] => 0x%04X ", addr, data&0xFF);
    log_printf("DataRam[0x%04X] => 0x%04X ", addr+1, data>>8);

    assert(addr > DATA_RAM_SIZE && "RAM write to invalid address!");
    
    uint8_to_tr8(&c->data_ram[addr], data&0xFF);
    uint8_to_tr8(&c->data_ram[addr+1], data>>8);
}

uint8_t mem_read_b(struct cpu_trit *c, uint32_t addr){
    assert(addr > DATA_RAM_SIZE - 1 && "RAM read from invalid address!");
    return trits_to_uint8(&c->data_ram[addr]);
}

uint16_t mem_read_w(struct cpu_trit *c, uint32_t addr){
    assert(addr > DATA_RAM_SIZE - 1 && "RAM read from invalid address!");
    return trits_to_uint8(&c->data_ram[addr]) + (trits_to_uint8(&c->data_ram[addr+1])<<8);
}

uint32_t rom_read_w(struct cpu_trit *c){
    uint32_t r;
    uint32_t pc;
    
    //viv- error   pc = trits_to_uint16(&c->pc);
    
    assert(pc > INST_ROM_SIZE - 1 && "ROM read from invalid address!");    
    r = trits_to_uint8(&c->inst_rom[pc]) + (trits_to_uint8(&c->inst_rom[pc+1])<<8);
    return r;
}

/*
 * Выделить биты [s...e]
 */ 
uint16_t get_bits(uint32_t t, int s, int e){
    int bit_len = e-s;
    uint32_t bit_mask = 1;
    for(int i=0;i<bit_len;i++){
        bit_mask = (bit_mask<<1)+1;
    }
    return (t>>s)&bit_mask;
}

uint16_t sign_ext(uint32_t t, uint8_t sign_bit){
    uint32_t sign_v = 0;
    uint32_t sign = get_bits(t, sign_bit, sign_bit);
    for(int i=sign_bit;i<16;i++){
        sign_v |= (sign<<i);
    }
    return t|sign_v;
}

uint8_t flag_zero(uint32_t res){
    return res == 0;
}

uint8_t flag_sign(uint32_t res){
    return get_bits(res, 15, 15);
}

uint8_t flag_overflow(uint32_t s1, uint32_t s2, uint32_t res){
    uint8_t s1_sign = get_bits(s1, 15, 15);
    uint8_t s2_sign = get_bits(s2, 15, 15);
    uint8_t res_sign = get_bits(res, 15, 15);
    return ((s1_sign^s2_sign) == 0)&((s2_sign^res_sign) == 1);
}

//------------------ INSTs ---------------------

void inst_lb(struct cpu_trit *c, uint32_t inst){
    log_printf("Inst: LB\t");

    uint8_t rs = get_bits(inst, 4, 7);
    uint8_t rd = get_bits(inst, 0, 3);
    pc_update(c, 2);

    uint32_t imm = rom_read_w(c);
    uint32_t s_data = reg_read(c, rs);
    uint32_t res = imm+s_data;
    if(res > 0xFFFF){
        //viv- error   c->flag_carry = getbool(0);
    }else{
        //viv- error   c->flag_carry = getbool(1);
    }
    //viv- error  c->flag_sign = flag_sign(res&0xFFFF);
    //viv- error  c->flag_overflow = flag_overflow(imm, s_data, res&0xFFFF);
    //viv- error  c->flag_zero = flag_zero(res&0xFFFF);
    reg_write(c, rd, sign_ext(mem_read_b(c, res&0xFFFF), 7));
    pc_update(c, 2);
}

void inst_lh(struct cpu_trit *c, uint32_t inst){
    log_printf("Inst: LH\t"); 
} 

void inst_lw(struct cpu_trit *c, uint32_t inst){
    log_printf("Inst: LW\t");

    uint8_t rs = get_bits(inst, 4, 7);
    uint8_t rd = get_bits(inst, 0, 3);
    pc_update(c, 2);

    uint32_t imm = rom_read_w(c);
    uint32_t s_data = reg_read(c, rs);
    uint32_t res = imm+s_data;
    if(res > 0xFFFF){
        //viv- error  c->flag_carry = getbool(0);
    }else{
        //viv- error  c->flag_carry = getbool(1);
    }
    //viv- error  c->flag_sign = getbool(flag_sign(res&0xFFFF));
    //viv- error  c->flag_overflow = getbool(flag_overflow(imm, s_data, res&0xFFFF));
    //viv- error  c->flag_zero = getbool(flag_zero(res&0xFFFF));
    reg_write(c, rd, mem_read_w(c, res&0xFFFF));
    pc_update(c, 2);
}

void inst_lbu(struct cpu_trit *c, uint32_t inst){
    log_printf("Inst: LBU\t");

    uint8_t rs = get_bits(inst, 4, 7);
    uint8_t rd = get_bits(inst, 0, 3);
    pc_update(c, 2);

    uint32_t imm = rom_read_w(c);
    uint32_t s_data = reg_read(c, rs);
    uint32_t res = imm+s_data;
    if(res > 0xFFFF){
        //viv- error  c->flag_carry = getbool(0);
    }else{
        //viv- error  c->flag_carry = getbool(1);
    }
    //viv- error  c->flag_sign = flag_sign(res&0xFFFF);
    //viv- error  c->flag_overflow = flag_overflow(imm, s_data, res&0xFFFF);
    //viv- error  c->flag_zero = flag_zero(res&0xFFFF);
    reg_write(c, rd, mem_read_b(c, res&0xFFFF));
    pc_update(c, 2);
}

void inst_lhu(struct cpu_trit *c, uint32_t inst){
    log_printf("Inst: LHU\t"); 
} 

void inst_addi(struct cpu_trit *c, uint32_t inst){
    log_printf("Inst: ADDI\t");

    uint8_t rs = get_bits(inst, 4, 7);
    uint8_t rd = get_bits(inst, 0, 3);

    uint32_t s_data = sign_ext(rs, 3);
    uint32_t d_data = reg_read(c, rd);
    uint32_t res = s_data+d_data;
    if(res > 0xFFFF){
        //viv- error  c->flag_carry = getbool(0);
    }else{
        //viv- error  c->flag_carry = getbool(1);
    }
    //viv- error  c->flag_sign = getbool(flag_sign(res&0xFFFF));
    //viv- error  c->flag_overflow = getbool(flag_overflow(s_data, d_data, res&0xFFFF));
    //viv- error  c->flag_zero = getbool(flag_zero(res&0xFFFF));
    reg_write(c, rd, res&0xFFFF);
    pc_update(c, 2);
}

void inst_slli(struct cpu_trit *c, uint32_t inst){
    log_printf("Inst: SLLI\t"); 
} 

void inst_slti(struct cpu_trit *c, uint32_t inst){
    log_printf("Inst: SLTI\t"); 
} 

void inst_sltiu(struct cpu_trit *c, uint32_t inst){
    log_printf("Inst: SLTIU\t"); 
} 

void inst_xori(struct cpu_trit *c, uint32_t inst){
    log_printf("Inst: XORI\t"); 
} 

void inst_srli(struct cpu_trit *c, uint32_t inst){
    log_printf("Inst: SRLI\t"); 
} 

void inst_srai(struct cpu_trit *c, uint32_t inst){
    log_printf("Inst: SRAI\t"); 
} 

void inst_ori(struct cpu_trit *c, uint32_t inst){
    log_printf("Inst: ORI\t"); 
} 

void inst_andi(struct cpu_trit *c, uint32_t inst){
    log_printf("Inst: ANDI\t"); 
} 

void inst_auipc(struct cpu_trit *c, uint32_t inst){
    log_printf("Inst: AUIPC\t"); 
} 

void inst_sb(struct cpu_trit *c, uint32_t inst){
    log_printf("Inst: SB\t");

    uint8_t rs = get_bits(inst, 4, 7);
    uint8_t rd = get_bits(inst, 0, 3);
    pc_update(c, 2);

    uint32_t imm = rom_read_w(c);
    uint32_t d_data = reg_read(c, rd);
    uint32_t res = imm+d_data;
    if(res > 0xFFFF){
        //viv- error  c->flag_carry = getbool(0);
    }else{
        //viv- error  c->flag_carry = getbool(1);
    }
    //viv- error  c->flag_sign = getbool(flag_sign(res&0xFFFF));
    //viv- error  c->flag_overflow = getbool(flag_overflow(imm, d_data, res&0xFFFF));
    //viv- error  c->flag_zero = getbool(flag_zero(res&0xFFFF));
    mem_write_b(c, res&0xFFFF, reg_read(c, rs)&0xFF);
    pc_update(c, 2);
}

void inst_sh(struct cpu_trit *c, uint32_t inst){
    log_printf("Inst: SH\t"); 
} 

void inst_sw(struct cpu_trit *c, uint32_t inst){
    log_printf("Inst: SW\t");

    uint8_t rs = get_bits(inst, 4, 7);
    uint8_t rd = get_bits(inst, 0, 3);
    pc_update(c, 2);

    uint32_t imm = rom_read_w(c);
    uint32_t d_data = reg_read(c, rd);
    uint32_t res = imm+d_data;
    if(res > 0xFFFF){
        //viv- error  c->flag_carry = getbool(0);
    }else{
        //viv- error  c->flag_carry = getbool(1);
    }
    //viv- error  c->flag_sign = getbool(flag_sign(res&0xFFFF));
    //viv- error  c->flag_overflow = getbool(flag_overflow(imm, d_data, res&0xFFFF));
    //viv- error  c->flag_zero = getbool(flag_zero(res&0xFFFF));
    mem_write_w(c, res&0xFFFF, reg_read(c, rs));
    pc_update(c, 2);
}

void inst_add(struct cpu_trit *c, uint32_t inst){
    log_printf("Inst: ADD\t");

    uint8_t rs = get_bits(inst, 4, 7);
    uint8_t rd = get_bits(inst, 0, 3);

    uint32_t s_data = reg_read(c, rs);
    uint32_t d_data = reg_read(c, rd);
    uint32_t res = s_data+d_data;
    if(res > 0xFFFF){
        //viv- error  c->flag_carry = getbool(0);
    }else{
        //viv- error  c->flag_carry = getbool(1);
    }
    //viv- error  c->flag_sign = getbool(flag_sign(res&0xFFFF));
    //viv- error  c->flag_overflow = getbool(flag_overflow(s_data, d_data, res&0xFFFF));
    //viv- error  c->flag_zero = getbool(flag_zero(res&0xFFFF));
    reg_write(c, rd, res&0xFFFF);
    pc_update(c, 2);
}

void inst_sub(struct cpu_trit *c, uint32_t inst){
    log_printf("Inst: SUB\t");

    uint8_t rs = get_bits(inst, 4, 7);
    uint8_t rd = get_bits(inst, 0, 3);

    uint32_t s_data = (~reg_read(c, rs))+1;
    uint32_t d_data = reg_read(c, rd);
    uint32_t res = s_data+d_data;
    if(res > 0xFFFF || s_data == 0){
        //viv- error  c->flag_carry = getbool(0);
    }else{
        //viv- error  c->flag_carry = getbool(1);
    }
    //viv- error  c->flag_sign = getbool(flag_sign(res&0xFFFF));
    //viv- error  c->flag_overflow = getbool(flag_overflow(s_data, d_data, res&0xFFFF));
    //viv- error  c->flag_zero = getbool(flag_zero(res&0xFFFF));
    reg_write(c, rd, res&0xFFFF);
    pc_update(c, 2);
}

void inst_sll(struct cpu_trit *c, uint32_t inst){
    log_printf("Inst: SLL\t"); 
} 

void inst_slt(struct cpu_trit *c, uint32_t inst){
    log_printf("Inst: SLT\t"); 
} 

void inst_sltu(struct cpu_trit *c, uint32_t inst){
    log_printf("Inst: SLTU\t"); 
} 

void inst_xor(struct cpu_trit *c, uint32_t inst){
    log_printf("Inst: XOR\t");

    uint8_t rs = get_bits(inst, 4, 7);
    uint8_t rd = get_bits(inst, 0, 3);

    uint32_t s_data = reg_read(c, rs);
    uint32_t d_data = reg_read(c, rd);
    uint32_t res_w = s_data^d_data;
    reg_write(c, rd, res_w);
    //viv- error  c->flag_carry = getbool(0);
    //viv- error  c->flag_sign = getbool(flag_sign(res_w));
    //viv- error  c->flag_overflow = getbool(flag_overflow(s_data, d_data, res_w));
    //viv- error  c->flag_zero = getbool(flag_zero(res_w));
    pc_update(c, 2);
}

void inst_srl(struct cpu_trit *c, uint32_t inst){
    log_printf("Inst: SRL\t"); 
} 

void inst_sra(struct cpu_trit *c, uint32_t inst){
    log_printf("Inst: SRA\t"); 
} 

void inst_or(struct cpu_trit *c, uint32_t inst){
    log_printf("Inst: OR\t");

    uint8_t rs = get_bits(inst, 4, 7);
    uint8_t rd = get_bits(inst, 0, 3);

    uint32_t s_data = reg_read(c, rs);
    uint32_t d_data = reg_read(c, rd);
    uint32_t res_w = s_data|d_data;
    reg_write(c, rd, res_w);
    //viv- error  c->flag_carry = getbool(0);
    //viv- error  c->flag_sign = getbool(flag_sign(res_w));
    //viv- error  c->flag_overflow = getbool(flag_overflow(s_data, d_data, res_w));
    //viv- error  c->flag_zero = getbool(flag_zero(res_w));
    pc_update(c, 2);
}

void inst_and(struct cpu_trit *c, uint32_t inst){
    log_printf("Inst: AND\t");

    uint8_t rs = get_bits(inst, 4, 7);
    uint8_t rd = get_bits(inst, 0, 3);

    uint32_t s_data = reg_read(c, rs);
    uint32_t d_data = reg_read(c, rd);
    uint32_t res_w = s_data&d_data;
    reg_write(c, rd, res_w);
    //viv- error  c->flag_carry = getbool(0);
    //viv- error  c->flag_sign = getbool(flag_sign(res_w));
    //viv- error  c->flag_overflow = getbool(flag_overflow(s_data, d_data, res_w));
    //viv- error  c->flag_zero = getbool(flag_zero(res_w));
    pc_update(c, 2);
}

void inst_lui(struct cpu_trit *c, uint32_t inst){
    log_printf("Inst: LUI\t"); 
} 

void inst_beq(struct cpu_trit *c, uint32_t inst){
    log_printf("Inst: BEQ\t"); 
} 

void inst_bne(struct cpu_trit *c, uint32_t inst){
    log_printf("Inst: BNE\t"); 
} 

void inst_blt(struct cpu_trit *c, uint32_t inst){
    log_printf("Inst: BLT\t"); 
} 

void inst_bge(struct cpu_trit *c, uint32_t inst){
    log_printf("Inst: BGE\t"); 
} 

void inst_bltu(struct cpu_trit *c, uint32_t inst){
    log_printf("Inst: BLTU\t"); 
} 

void inst_bgeu(struct cpu_trit *c, uint32_t inst){
    log_printf("Inst: BGEU\t"); 
} 

void inst_jalr(struct cpu_trit *c, uint32_t inst){
    log_printf("Inst: JALR\t");

    uint8_t rs = get_bits(inst, 4, 7);

    //viv- error  c->flag_carry = getbool(0);
    //viv- error  c->flag_sign = getbool(0);
    //viv- error  c->flag_overflow = getbool(0);
    //viv- error  c->flag_zero = getbool(0);
    reg_write(c, 0, pc_read(c)+2);
    pc_write(c, reg_read(c, rs));
}

void inst_jal(struct cpu_trit *c, uint32_t inst){
    log_printf("Inst: JAL\t");

    pc_update(c, 2);

    uint32_t imm = rom_read_w(c);
    //viv- error  c->flag_carry = getbool(0);
    //viv- error  c->flag_sign = getbool(0);
    //viv- error  c->flag_overflow = getbool(0);
    //viv- error  c->flag_zero = getbool(0);
    reg_write(c, 0, pc_read(c)+2);
    pc_update(c, imm);
}

/* ---------------------------------------------
*  Инструкции для двоичного RISC-V RV32E / RV32I
*/
const struct inst_data inst_list[] = {	
    // RISC-V 32-bit instruction formats
    // 0b31..0 
    //  31......24....19....14..11..7.6.....0 
    {"0bxxxxxxx_xxxxx_xxxxx_000_xxxxx_0000011", inst_lb},		//LB
    {"0bxxxxxxx_xxxxx_xxxxx_001_xxxxx_0000011", inst_lh},		//LH
    {"0bxxxxxxx_xxxxx_xxxxx_010_xxxxx_0000011", inst_lw},		//LW
    {"0bxxxxxxx_xxxxx_xxxxx_100_xxxxx_0000011", inst_lbu},		//LBU
    {"0bxxxxxxx_xxxxx_xxxxx_101_xxxxx_0000011", inst_lhu},		//LHU
    {"0bxxxxxxx_xxxxx_xxxxx_000_xxxxx_0010011", inst_addi},		//ADDI
    {"0bxxxxxxx_xxxx0_00000_001_xxxxx_0010011", inst_slli},		//SLLI
    {"0bxxxxxxx_xxxxx_xxxxx_010_xxxxx_0010011", inst_slti},		//SLTI
    {"0bxxxxxxx_xxxxx_xxxxx_011_xxxxx_0010011", inst_sltiu},	//SLTIU
    {"0bxxxxxxx_xxxxx_xxxxx_100_xxxxx_0010011", inst_xori},		//XORI
    {"0bxxxxxxx_xxxx0_00000_101_xxxxx_0010011", inst_srli},		//SRLI
    {"0bxxxxxxx_xxxx0_10000_010_xxxxx_0010011", inst_srai},		//SRAI
    {"0bxxxxxxx_xxxxx_xxxxx_110_xxxxx_0010011", inst_ori},		//ORI
    {"0bxxxxxxx_xxxxx_xxxxx_111_xxxxx_0010011", inst_andi},		//ANDI
    {"0bxxxxxxx_xxxxx_xxxxx_xxx_xxxxx_0010111", inst_auipc},	//AUIPC
    {"0bxxxxxxx_xxxxx_xxxxx_000_xxxxx_0100011", inst_sb},		//SB
    {"0bxxxxxxx_xxxxx_xxxxx_001_xxxxx_0010011", inst_sh},		//SH
    {"0bxxxxxxx_xxxxx_xxxxx_010_xxxxx_0010011", inst_sw},		//SW
    {"0bxxxxxxx_xxxx0_00000_000_xxxxx_0110011", inst_add},		//ADD
    {"0bxxxxxxx_xxxx0_10000_000_xxxxx_0110011", inst_sub},		//SUB
    {"0bxxxxxxx_xxxx0_00000_001_xxxxx_0110011", inst_sll},		//SLL
    {"0bxxxxxxx_xxxx0_00000_010_xxxxx_0110011", inst_slt},		//SLT
    {"0bxxxxxxx_xxxx0_00000_011_xxxxx_0110011", inst_sltu},		//SLTU
    {"0bxxxxxxx_xxxx0_00000_100_xxxxx_0110011", inst_xor},		//XOR
    {"0bxxxxxxx_xxxx0_00000_101_xxxxx_0110011", inst_srl},		//SRL
    {"0bxxxxxxx_xxxx0_10000_101_xxxxx_0110011", inst_sra},		//SRA
    {"0bxxxxxxx_xxxx0_00000_110_xxxxx_0110011", inst_or},		//OR
    {"0bxxxxxxx_xxxx0_00000_111_xxxxx_0110011", inst_and},		//AND
    {"0bxxxxxxx_xxxxx_xxxxx_xxx_xxxxx_0110111", inst_lui},		//LUI
    {"0bxxxxxxx_xxxxx_xxxxx_000_xxxxx_1100011", inst_beq},		//BEQ
    {"0bxxxxxxx_xxxxx_xxxxx_001_xxxxx_1100011", inst_bne},		//BNE
    {"0bxxxxxxx_xxxxx_xxxxx_100_xxxxx_1100011", inst_blt},		//BLT
    {"0bxxxxxxx_xxxxx_xxxxx_101_xxxxx_1100011", inst_bge},		//BGE
    {"0bxxxxxxx_xxxxx_xxxxx_110_xxxxx_1100011", inst_bltu},		//BLTU
    {"0bxxxxxxx_xxxxx_xxxxx_111_xxxxx_1100011", inst_bgeu},		//BGEU
    {"0bxxxxxxx_xxxxx_xxxxx_000_xxxxx_1100111", inst_jalr},		//JALR
    {"0bxxxxxxx_xxxxx_xxxxx_xxx_xxxxx_1101111", inst_jal},		//JAL        
    {NULL, NULL} //Terminator
};
