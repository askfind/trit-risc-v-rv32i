/*
* Проект 'TRIT-RISC-V : T-RV32I' на языке C
*
* Автор: 		 Vladimir V.
* E-mail: 	  	 askfind@ya.ru   
* 
*/

#include <assert.h>
#include <stdio.h>
#include <stdint.h>

#include "bit_trit_parser.h"
#include "ternary.h"
#include "cpu_trit.h"
#include "inst.h"
#include "inst_trit.h"
#include "log.h"

int flag_inst_trits = 1;

/** ------------------------------------------------------
 * 
 *  Реализация инструкции для троичного режима TRIT-RISC-V
 * 
 */

/*
 *  pc = pc + offset
 */
void trs_pc_update(struct cpu_trit *c, tr32 offset) {	
    c->pc = add_trs(c->pc, offset);
	uint8_t nine_str[16] = {'0'};
	tr32_to_nine_string(nine_str,c->pc);
	log_printf("PC => t%s\n\r", nine_str);
}

/* 
 * pc = addr
 */	
void trs_pc_write(struct cpu_trit *c, tr32 addr) {
	for(int i=0;i<TRIT32_SIZE;i++) {
		c->pc.t[i] = addr.t[i];
	}
}

/* 
 * data = pc_read(pc)
 */	
tr32 trs_pc_read(struct cpu_trit *c) {
	tr32 r;	
	for(int i=0;i<TRIT32_SIZE;i++) {
		r.t[i] = c->pc.t[i];
	}
	return r;
}

/* 
 * reg[reg_idx] = data
 */	
void trs_reg_write(struct cpu_trit *c, uint8_t reg_idx, tr32 data){
    uint8_t nine_str[16] = {'0'};    
    if( reg_idx > REGISTER_SIZE-1 ) {
		log_printf("Error: Reg t%d : Error reg_idx\n\r",reg_idx);
		assert(reg_idx > REGISTER_SIZE-1 && "Reg write from invalid index!");    		
		return; /* Error index. */
	}
    for(int i=0;i<TRIT32_SIZE;i++) {
		c->reg[reg_idx].t[i] = data.t[i];
	}
	tr32_to_nine_string(nine_str,c->reg[reg_idx]);
    log_printf("Reg t%d => t%s\n\r",reg_idx,nine_str);
    return; /* Ok */
}

/* 
 * data = reg[reg_idx] 
 */	
void trs_reg_read(struct cpu_trit *c, uint8_t reg_idx, tr32 * tr ){	
	/* tr = reg_read(reg[reg_idx]) */	
    uint8_t nine_str[16] = {'0'};
    if( reg_idx > REGISTER_SIZE-1 ) {
		log_printf("Error: Reg n%02i : Error reg_idx\n\r",reg_idx);
		assert(reg_idx > REGISTER_SIZE-1 && "Error: Reg read from invalid index!"); 		
		return;
	}
    for(int i=0;i<TRIT32_SIZE;i++) {
		tr->t[i] = c->reg[reg_idx].t[i];
	}    
	tr32_to_nine_string(nine_str,*tr);
    log_printf("Reg x%02i => n%s\n\r",reg_idx,nine_str);
    return; /*Ok */
}

/* 
 * word = rom[pc] 
 */	
void trs_rom_read_w(struct cpu_trit *c, tr32 *tr){
    
    int64_t pc;        
    pc = tr32_to_int64(c->pc);    	
	
    if( abs_int64(pc) > (INST_ROM_SIZE/2 - 1) ) {
		log_printf("Error: ROM read from invalid address!\n\r");
		assert(abs_int64(pc) > INST_ROM_SIZE/2 - 1 && "ROM read from invalid address!");    
		return; /* Error index. */
	}

    for(int i=0;i<TRIT16_SIZE;i++) {		
		tr->t[i] = c->inst_rom[pc].t[i]; // low
	}    
    for(int i=0;i<TRIT16_SIZE;i++) {		
		tr->t[i+TRIT8_SIZE] = c->inst_rom[pc+1].t[i]; // high;
	}     
	
    return; /* Ok */
}

/* 
 * mem[pc] = byte   
 */	
void trs_mem_write_b(struct cpu_trit *c, tr32 addr, tr8 data) {
    int64_t pc;        
    pc = tr32_to_int64(c->pc);    	
	
    if( abs_int64(pc) > (DATA_RAM_SIZE/2 - 1) ) {
		log_printf("Error: RAM write tr8 from invalid address!\n\r");
		assert(abs_int64(pc) < DATA_RAM_SIZE/2 - 1 && "RAM write tr8 from invalid address!");    
		return;
	}

    for(int i=0;i<TRIT8_SIZE;i++) {		
		c->data_ram[pc].t[i] = data.t[i];
	}    
	
    return; /* Ok */

}
/* 
 * word = mem[pc]   
 */	
void trs_mem_write_w(struct cpu_trit *c, tr32 addr, tr32 data) {
    
    int64_t pc;        
    pc = tr32_to_int64(c->pc);    	
	
    if( abs_int64(pc) > (DATA_RAM_SIZE/2 - 1) ) {
		log_printf("Error: RAM read tr16 from invalid address!\n\r");
		assert(abs_int64(pc) > DATA_RAM_SIZE/2 - 1 && "RAM read tr16 from invalid address!");    
		return;
	}

    for(int i=0;i<TRIT16_SIZE;i++) {		
		c->data_ram[pc].t[i] = data.t[i]; // low
	}    
    for(int i=0;i<TRIT16_SIZE;i++) {		
		c->data_ram[pc+1].t[i] = data.t[i+TRIT16_SIZE]; // high;
	}     
	
    return; /* Ok */
}

/* 
 * mem[pc] = byte
 */	
void trs_mem_read_b(struct cpu_trit *c, tr32 addr, tr8 *tr) {
    int64_t pc;        
    pc = tr32_to_int64(c->pc);    	
	
    if( abs_int64(pc) > (DATA_RAM_SIZE/2 - 1) ) {
		log_printf("Error: RAM read from invalid address!\n\r");
		assert(abs_int64(pc) > DATA_RAM_SIZE/2 - 1 && "RAM read from invalid address!");    
		return;
	}

    for(int i=0;i<TRIT8_SIZE;i++) {		
		tr->t[i] = c->data_ram[pc].t[i];
	}    
	
    return; /* Ok */
}

/* 
 *  word = mem[pc]
 */	
void trs_mem_read_w(struct cpu_trit *c, tr32 addr, tr32 *tr) {
    
    int64_t pc;        
    pc = tr32_to_int64(c->pc);    	
	
    if( abs_int64(pc) > (DATA_RAM_SIZE/2 - 1) ) {
		log_printf("Error: RAM read tr16 from invalid address!\n\r");
		assert(abs_int64(pc) < DATA_RAM_SIZE/2 - 1 && "RAM read tr16 from invalid address!");    
		return;
	}

    for(int i=0;i<TRIT16_SIZE;i++) {		
		tr->t[i] = c->data_ram[pc].t[i]; // low
	}    
    for(int i=0;i<TRIT16_SIZE;i++) {		
		tr->t[i+TRIT16_SIZE] = c->data_ram[pc+1].t[i]; // high;
	}     
	
    return; /* Ok */
}

//TODO Добавить реалищацию Функции режима работы троичного TRIT-RISC-V 
// 27.10.2025  
// 13.11.2025

/* 
 *  int64 = get_trits(tr32, position start, position stop)
 */	
int64_t trs_get_trits(tr32 tr, int s, int e) {
	
    trit t;
    int64_t r = 0;    	

    if( s<0 || s>31 || e<0 || e>31 || s>e ) {
		log_printf("Error: Get trits s or e from invalid position!\n\r");
		assert( s<0 || s>31 || e<0 || e>31 || s>e  && "Get trits s or e from invalid position!");    
		return r;
	}	
	
	for (int i=s; i<=e; i++)
	{	
		t = get_trit32(tr, i);	
		r += 3^i * (int)t;
	}    

    return r;		
}

void trs_sign_ext(tr32 tr, uint8_t sign_bit, tr32 *ret) {
	
    trit sign = get_trit32(tr, sign_bit);
    
    if( sign_bit<0 || sign_bit>31 ) {
		log_printf("Error: Get trits s from invalid position!\n\r");
		assert( sign_bit<0 || sign_bit>15 && "Get trits s from invalid position!");    
		return;
	}	
	    
    for(int i=sign_bit;i<31;i++){        
        ret->t[i] = sign;
    }
    return; /* Ok */    
}

trit trs_flag_zero(tr32 res) {
	
	if ( tr32_to_int64(res) == 0 ) {
		return +1;
	}
	else {
		return -1;
	}
}

trit trs_flag_sign(tr32 res) {
    trit sng = 0;
    for(int i=0;i<TRIT32_SIZE;i++){        
         trit sng = get_trit32(res,i);
         if( sng != 0 ) {
			return sng;
		 }
    }			
	return sng;
}

trit trs_flag_overflow(tr32 s1, tr32 s2, tr32 res) {
	
	//viv- old code
	trit s1_sign = get_trit32(s1, 31);
    trit s2_sign = get_trit32(s2, 31);
    trit res_sign = get_trit32(res, 31);
    
    // Какое условие переполнения для тритов? 
    //return ((s1_sign^s2_sign) == 0)&((s2_sign^res_sign) == 1);    
	return 0;
}

//TODO Добавить реалищацию Функции режима работы троичного TRIT-RISC-V 
// 27.10.2025  
// 13.11.2025
// 14.11.2025

/*
 * Троичные команды TRIT-RISC-V
 */  

void inst_trs_lb(struct cpu_trit *c, tr32 inst) {
	/* rd = SignExt([Address]7:0) */
	
	tr8 trb = {0};
	tr32 data = {0};   	
	uint8_t reg_idx  = (uint8_t)trs_get_trits(inst, 7, 11); 
	trs_mem_read_b(c, c->pc, &trb);
	
	vlog8("reg_idx=",reg_idx);
	tlog32("c->pc=",c->pc);
	tlog8("trb=",trb);
	
    for(int i=0;i<TRIT32_SIZE;i++){
         
         if(i < TRIT8_SIZE) {
			trit sng = get_trit8(trb,i);         
			data.t[i] = sng;
		 }
		 else {
			 data.t[i] = 0;
		 }
		 		
    }		
	
	trs_reg_write(c, reg_idx, data);
	log_printf("Inst: tLB\t");
	tlog32("   c->reg=", c->reg[reg_idx]); //viv+ test
}
void inst_trs_lh(struct cpu_trit *c, tr32 inst) {
	/* rd = SignExt([Address]15:0) */
	log_printf("Inst: tLH\t");	
}
void inst_trs_lw(struct cpu_trit *c, tr32 inst) {
	/* rd = ([Address]31:0) */
	log_printf("Inst: tLW\t");	
}
void inst_trs_lbu(struct cpu_trit *c, tr32 inst) {
	/* rd = ZeroExt([Address]7:0) */
	    log_printf("Inst: tLBU\t");	
}
void inst_trs_lhu(struct cpu_trit *c, tr32 inst) {
	/* rd = ZeroExt([Address]7:0) */
	    log_printf("Inst: tLHU\t");	
}
void inst_trs_addi(struct cpu_trit *c, tr32 inst) {
    /* rd = rs1 + SignExt(imm) */
    log_printf("Inst: tADDI\t");	
}
void inst_trs_slli(struct cpu_trit *c, tr32 inst) {
    /* rd = rs1 << uimm */
    log_printf("Inst: tSLLI\t");	
}
void inst_trs_slti(struct cpu_trit *c, tr32 inst) {
    /* rd = rs1 < SignExt(imm) */
    log_printf("Inst: tSLTI\t");	
}
void inst_trs_sltiu(struct cpu_trit *c, tr32 inst) {
    /* rd = rs1 < SignExt(imm) */
    log_printf("Inst: tSLTIU\t");	
}
void inst_trs_xori(struct cpu_trit *c, tr32 inst) {
	/* rd = rs1 xor SignExt(imm) */
	log_printf("Inst: tXORI\t");	
}
void inst_trs_srli(struct cpu_trit *c, tr32 inst) {
	/* rd = rs1 >> uimm  Shift right logical immediate */
	log_printf("Inst: tSRLI\t");	
}
void inst_trs_srai(struct cpu_trit *c, tr32 inst) {
	/* rd = rs1 >> uimm  Shift right arithmetic immediate */
	log_printf("Inst: tSRAI\t");	
}
void inst_trs_ori(struct cpu_trit *c, tr32 inst) {
	/* rd = rs1 | SignExt(imm) */
	log_printf("Inst: tORI\t");	
}
void inst_trs_andi(struct cpu_trit *c, tr32 inst) {
	/* rd = rs1 & SignExt(imm) */
	log_printf("Inst: tANDI\t");	
}
void inst_trs_auipc(struct cpu_trit *c, tr32 inst) {
	/* rd = (upimm, 12’b0) + PC */
	log_printf("Inst: tAUIPC\t");	
}
void inst_trs_sb(struct cpu_trit *c, tr32 inst) {
	/* [Address]7:0 = rs 27:0 */
	log_printf("Inst: tSB\t");	
}
void inst_trs_sh(struct cpu_trit *c, tr32 inst) {
	/* [Address]15:0 = rs2 15:0 */
	log_printf("Inst: tSH\t");	
}
void inst_trs_sw(struct cpu_trit *c, tr32 inst) {
	/* [Address]31:0 = rs2 */
	log_printf("Inst: tSW\t");	
}
void inst_trs_add(struct cpu_trit *c, tr32 inst) {
	/* rd = rs1 + rs2 */
	log_printf("Inst: tADD\t");	
}
void inst_trs_sub(struct cpu_trit *c, tr32 inst) {
	/* rd = rs1 - rs2 */
	log_printf("Inst: tSUB\t");	
}
void inst_trs_sll(struct cpu_trit *c, tr32 inst) {
	/* rd = rs1 << rs24:0 */
	log_printf("Inst: tSLL\t");	
}
void inst_trs_slt(struct cpu_trit *c, tr32 inst) {
	/* rd = rs1 < rs2 */
	log_printf("Inst: tSLT\t");	
}
void inst_trs_sltu(struct cpu_trit *c, tr32 inst) {
	/* rd = rs1 < rs2 */
	log_printf("Inst: tSLTU\t");	
}
void inst_trs_xor(struct cpu_trit *c, tr32 inst) {
	/* rd = rs1 ^ rs2 */
	log_printf("Inst: tXOR\t");	
}
void inst_trs_srl(struct cpu_trit *c, tr32 inst) {
	/* rd = rs1 >> rs24:0 */
	log_printf("Inst: tSRL\t");	
}
void inst_trs_sra(struct cpu_trit *c, tr32 inst) {
	/* rd = rs1 >>>rs24:0 */
	log_printf("Inst: tSRA\t");	
}
void inst_trs_or(struct cpu_trit *c, tr32 inst) {
	/* rd = rs1 | rs2 */
	log_printf("Inst: tOR\t");	
}
void inst_trs_and(struct cpu_trit *c, tr32 inst) {
	/* rd = rs1 & rs2 */
	log_printf("Inst: tAND\t");	
}
void inst_trs_lui(struct cpu_trit *c, tr32 inst) {
	/* rd = {upimm, 12’b0} */
	log_printf("Inst: tLUI\t");	
}
void inst_trs_beq(struct cpu_trit *c, tr32 inst) {
	/* if (rs1 == rs2) PC = BTA */
	log_printf("Inst: tBEQ\t");	
}
void inst_trs_bne(struct cpu_trit *c, tr32 inst) {
	/* if (rs1 != rs2) PC = BTA */
	log_printf("Inst: tBNE\t");	
}
void inst_trs_blt(struct cpu_trit *c, tr32 inst) {
	/* if (rs1 < rs2) PC = BTA */
	log_printf("Inst: tBLT\t");	
}
void inst_trs_bge(struct cpu_trit *c, tr32 inst) {
	/* if (rs1 ≥ rs2) PC = BTA */
	log_printf("Inst: tBGE\t");	
}
void inst_trs_bltu(struct cpu_trit *c, tr32 inst) {
	/* if (rs1 < rs2) PC = BTA */
	log_printf("Inst: tBLTU\t");	
}
void inst_trs_bgeu(struct cpu_trit *c, tr32 inst) {
	/* if (rs1 ≥ rs2) PC = BTA */
	log_printf("Inst: tBGEU\t");	
}
void inst_trs_jalr(struct cpu_trit *c, tr32 inst) {
	/* PC = rs1 + SignExt(imm) rd = PC + 4 */
	log_printf("Inst: tJALR\t");	
}
void inst_trs_jal(struct cpu_trit *c, tr32 inst) {
	/* PC = JTA rd = PC + 4 */
	log_printf("Inst: tJAL\t");	
}

/* --------------------------------------------
*  Инструкции для троичного TRIT-RISC-V T-RV32I
*/
const struct inst_trit_data trs_inst_list[] = {
	
    // TRIT-RISC-V 32-trit instruction formats
    // 0t31..0 
    //  31......24....19....14..11..7.6.....0 
    {"0txxxxxxx_xxxxx_xxxxx_000_xxxxx_00000++", inst_trs_lb},		//tLB         	t000000000000000000000000000000++   n0000000000000004 
    {"0txxxxxxx_xxxxx_xxxxx_00+_xxxxx_00000++", inst_trs_lh},		//tLH         	t0000000000000000000+0000000000++   n0000000001000004 
    {"0txxxxxxx_xxxxx_xxxxx_0+0_xxxxx_00000++", inst_trs_lw},		//tLW			t000000000000000000+00000000000++	n0000000003000004
    {"0txxxxxxx_xxxxx_xxxxx_+00_xxxxx_00000++", inst_trs_lbu},		//tLBU			t00000000000000000+000000000000++	n0000000010000004
    {"0txxxxxxx_xxxxx_xxxxx_+0+_xxxxx_00000++", inst_trs_lhu},		//tLHU			t00000000000000000+0+0000000000++	n0000000011000004
    {"0txxxxxxx_xxxxx_xxxxx_000_xxxxx_00+00++", inst_trs_addi},		//tADDI			t000000000000000000000000000+00++	n0000000000000104
    {"0txxxxxxx_xxxx0_00000_00+_xxxxx_00+00++", inst_trs_slli},		//tSLLI			t0000000000000000000+0000000+00++	n0000000001000104
    {"0txxxxxxx_xxxxx_xxxxx_0+0_xxxxx_00+00++", inst_trs_slti},		//tSLTI			t000000000000000000+00000000+00++	n0000000003000104
    {"0txxxxxxx_xxxxx_xxxxx_0++_xxxxx_00+00++", inst_trs_sltiu},	//tSLTIU		t000000000000000000++0000000+00++	n0000000004000104
    {"0txxxxxxx_xxxxx_xxxxx_+00_xxxxx_00+00++", inst_trs_xori},		//tXORI			t00000000000000000+000000000+00++	n0000000010000104
    {"0txxxxxxx_xxxx0_00000_+0+_xxxxx_00+00++", inst_trs_srli},		//tSRLI			t00000000000000000+0+0000000+00++	n0000000011000104
    {"0txxxxxxx_xxxx0_+0000_0+0_xxxxx_00+00++", inst_trs_srai},		//tSRAI			t000000000000000000+00000000+00++	n0000000003000104
    {"0txxxxxxx_xxxxx_xxxxx_++0_xxxxx_00+00++", inst_trs_ori},		//tORI			t00000000000000000++00000000+00++	n0000000013000104
    {"0txxxxxxx_xxxxx_xxxxx_+++_xxxxx_00+00++", inst_trs_andi},		//tANDI			t00000000000000000+++0000000+00++	n0000000014000104
    {"0txxxxxxx_xxxxx_xxxxx_xxx_xxxxx_00+0+++", inst_trs_auipc},	//tAUIPC		t000000000000000000000000000+0+++	n0000000000000114
    {"0txxxxxxx_xxxxx_xxxxx_000_xxxxx_0+000++", inst_trs_sb},		//tSB			t00000000000000000000000000+000++	n0000000000000304
    {"0txxxxxxx_xxxxx_xxxxx_00+_xxxxx_00+00++", inst_trs_sh},		//tSH			t0000000000000000000+0000000+00++	n0000000001000104
    {"0txxxxxxx_xxxxx_xxxxx_0+0_xxxxx_00+00++", inst_trs_sw},		//tSW			t000000000000000000+00000000+00++	n0000000003000104
    {"0txxxxxxx_xxxx0_00000_000_xxxxx_0++00++", inst_trs_add},		//tADD			t00000000000000000000000000++00++	n0000000000000404
    {"0txxxxxxx_xxxx0_+0000_000_xxxxx_0++00++", inst_trs_sub},		//tSUB			t000000000000+0000000000000++00++	n0000003000000404
    {"0txxxxxxx_xxxx0_00000_00+_xxxxx_0++00++", inst_trs_sll},		//tSLL			t0000000000000000000+000000++00++	n0000000001000404
    {"0txxxxxxx_xxxx0_00000_0+0_xxxxx_0++00++", inst_trs_slt},		//tSLT			t000000000000000000+0000000++00++	n0000000003000404
    {"0txxxxxxx_xxxx0_00000_0++_xxxxx_0++00++", inst_trs_sltu},		//tSLTU			t000000000000000000++000000++00++	n0000000004000404
    {"0txxxxxxx_xxxx0_00000_+00_xxxxx_0++00++", inst_trs_xor},		//tXOR			t00000000000000000+00000000++00++	n0000000010000404
    {"0txxxxxxx_xxxx0_00000_+0+_xxxxx_0++00++", inst_trs_srl},		//tSRL			t00000000000000000+0+000000++00++	n0000000011000404
    {"0txxxxxxx_xxxx0_+0000_+0+_xxxxx_0++00++", inst_trs_sra},		//tSRA			t000000000000+0000+0+000000++00++	n0000003011000404
    {"0txxxxxxx_xxxx0_00000_++0_xxxxx_0++00++", inst_trs_or},		//tOR			t00000000000000000++0000000++00++	n0000000013000404
    {"0txxxxxxx_xxxx0_00000_+++_xxxxx_0++00++", inst_trs_and},		//tAND			t00000000000000000+++000000++00++	n0000000014000404
    {"0txxxxxxx_xxxxx_xxxxx_xxx_xxxxx_0++0+++", inst_trs_lui},		//tLUI			t00000000000000000000000000++0+++	n0000000000000414
    {"0txxxxxxx_xxxxx_xxxxx_000_xxxxx_++000++", inst_trs_beq},		//tBEQ			t0000000000000000000000000++000++	n0000000000001304
    {"0txxxxxxx_xxxxx_xxxxx_00+_xxxxx_++000++", inst_trs_bne},		//tBNE			t0000000000000000000+00000++000++	n0000000001001304
    {"0txxxxxxx_xxxxx_xxxxx_+00_xxxxx_++000++", inst_trs_blt},		//tBLT			t00000000000000000+0000000++000++	n0000000010001304
    {"0txxxxxxx_xxxxx_xxxxx_+0+_xxxxx_++000++", inst_trs_bge},		//tBGE			t00000000000000000+0+00000++000++	n0000000011001304
    {"0txxxxxxx_xxxxx_xxxxx_++0_xxxxx_++000++", inst_trs_bltu},		//tBLTU			t00000000000000000++000000++000++	n0000000013001304
    {"0txxxxxxx_xxxxx_xxxxx_+++_xxxxx_++000++", inst_trs_bgeu},		//tBGEU			t00000000000000000+++00000++000++	n0000000014001304
    {"0txxxxxxx_xxxxx_xxxxx_000_xxxxx_++00+++", inst_trs_jalr},		//tJALR			t0000000000000000000000000++00+++	n0000000000001314
    {"0txxxxxxx_xxxxx_xxxxx_xxx_xxxxx_++0++++", inst_trs_jal},		//tJAL        	t0000000000000000000000000++0++++	n0000000000001344
    {NULL, NULL} //Terminator
};

/* Отладка троичных операций */
void trs_dbg_oper(struct cpu_trit *c) {	
	
	static uint8_t buf8[8] = {0};
	static uint8_t buf16[16] = {0};
	
	tr32 tr;
	tr32 tr1;
	tr32 tr2;
	tr32 tr3;	
	tr8  t8_1;
	tr16 t16_1;	
	tr32 t32_1;	
	tr32 offset; 
	tr32 addr;	
	 
	printf("[ Отладка троичных операций TRIT RISC-V ]\n\r");
	
	printf("\n\r t000: tlog32(...)\n\r");
	clear_tr32(&tr);	
	tr.t[1] = 1;
	tr.t[0] = 1;	
	tlog32("++ -> ", tr);
	tr.t[1] = -1;
	tr.t[0] = -1;	
	tlog32("-- -> ", tr);

	printf("\n\r t001: symb_nine_form(...)\n\r");
	t16_1.t[1] = 1;
	t16_1.t[0] = 1;	
	printf("++ -> %c\n\r",symb_nine_form(t16_1));
	t16_1.t[1] = 0;
	t16_1.t[0] = 0;	
	printf("00 -> %c\n\r",symb_nine_form(t16_1));
	t16_1.t[1] = -1;
	t16_1.t[0] = -1;	
	printf("-- -> %c\n\r",symb_nine_form(t16_1));

	printf("\n\r t002: tr16_to_nine_string(...), tr32_to_nine_string(...)\n\r");	
	clear_tr16(&t16_1);	
	t16_1.t[15] = -1;
	t16_1.t[14] =  0;
	t16_1.t[13] = +1;
	t16_1.t[12] = -1;
	t16_1.t[11] =  0;
	t16_1.t[10] = +1;
	t16_1.t[9] = -1;
	t16_1.t[8] =  0;
	t16_1.t[7] = +1;
	t16_1.t[6] = -1;
	t16_1.t[5] =  0;
	t16_1.t[4] = +1;
	t16_1.t[3] = -1;
	t16_1.t[2] =  0;
	t16_1.t[1] = +1;
	t16_1.t[0] =  0;
	tr16_to_nine_string(buf8,t16_1);	 
	printf("t16 -> ");
	buflog8(buf8);
	printf("\n\r");
	
	clear_tr32(&t32_1);	
	t32_1.t[31] = +1;
	t32_1.t[30] =  0;
	t32_1.t[29] = -1;
	t32_1.t[28] = +1;
	t32_1.t[27] =  0;
	t32_1.t[26] = -1;
	t32_1.t[25] = +1;
	t32_1.t[24] =  0;
	t32_1.t[23] = -1;
	t32_1.t[22] = +1;
	t32_1.t[21] =  0;
	t32_1.t[4] = -1;
	t32_1.t[3] = +1;
	t32_1.t[2] =  0;
	t32_1.t[1] = -1;
	t32_1.t[0] =  0;
	tr32_to_nine_string(buf16,t32_1);	 	
	printf("t32 -> ");
	buflog16(buf16);
	printf("\n\r");
	
	printf("\n\r t003: trs_pc_update(...)\n\r");
	clear_tr32(&c->pc);
	clear_tr32(&offset);
	offset.t[0] = 0;
	trs_pc_update(c, offset);
	offset.t[0] = 1;
	
	trs_pc_update(c, offset);
	tr1 = c->pc;
	trs_pc_update(c, offset);	
	trs_pc_update(c, offset);	
	trs_pc_update(c, offset);	
	trs_pc_update(c, offset);	
	trs_pc_update(c, offset);	
	
	printf("\n\r t004: trs_pc_write(...), trs_pc_read(...) \n\r");
	t32_1 = trs_pc_read(c);
	tlog32("t32_1 =",t32_1);	
	clear_tr32(&t32_1);
	tlog32("t32_1 =",t32_1);
	trs_pc_write(c, t32_1);
	t32_1 = trs_pc_read(c);
	tlog32("t32_1 =",t32_1);
	
	printf("\n\r t005: trs_reg_write(...) \n\r");
	clear_tr32(&t32_1);
	t32_1.t[2] = +1;
	t32_1.t[1] =  0;
	t32_1.t[0] = -1;	
	tlog32("t32_1 =",t32_1);
	uint8_t reg_idx = 7;
	trs_reg_write(c,reg_idx,tr1);	
	reg_idx = 21;
	trs_reg_write(c,reg_idx,tr1);	
	
	printf("\n\r t006: trs_reg_read(...) \n\r");
	clear_tr32(&t32_1);
	reg_idx = 7;	
	tlog32("1. t32_1 =",t32_1);
	trs_reg_read(c, reg_idx, &t32_1);
	tlog32("2. t32_1 =",t32_1);
	
	printf("\n\r t007: trs_rom_read_w(...) \n\r");	
	clear_tr32(&c->pc);
	clear_tr32(&t32_1);
	clear_tr32(&offset);

	offset.t[0] = +1;
	trs_pc_update(c, offset);
		
	t32_1.t[2] = +1;
	t32_1.t[1] =  0;
	t32_1.t[0] = -1;	
	tlog32("1. t pc =",c->pc);	
	tlog32("2. t t32_1 =",t32_1);	
	trs_rom_read_w(c, &t32_1);
	tlog32("3. t pc =",c->pc);
	tlog32("4. t tr1=", t32_1);
	
	printf("\n\r t008: trs_mem_write_b(...), trs_mem_read_b(...) \n\r");	
	clear_tr8(&t8_1);
	clear_tr32(&addr);

	addr.t[2] = +1; 
	addr.t[1] = -1;
	addr.t[0] =  0;		

	t8_1.t[7] = -1; 
	t8_1.t[1] = +1;
	t8_1.t[0] =  0;		
	
	tlog8("t8_1 = ",t8_1);
	trs_mem_write_b(c, addr, t8_1);
	clear_tr8(&t8_1);	
	trs_mem_read_b(c, addr, &t8_1); 
	tlog8("t8_1 = ",t8_1);

	printf("\n\r t009: trs_rom_read_w(...) \n\r");	
	clear_tr32(&t32_1);
	clear_tr32(&addr);

	addr.t[2] = +1; 
	addr.t[1] = -1;
	addr.t[0] =  0;		

	t32_1.t[15] = -1; 
	t32_1.t[1] = +1;
	t32_1.t[0] =  0;		
	
	tlog32("t32_1 = ",t32_1);
	trs_mem_write_w(c, addr, t32_1);
	clear_tr32(&t32_1);	
	trs_mem_read_w(c, addr, &t32_1); 
	tlog32("t32_1 = ",t32_1);

	printf("\n\r t010: smtr16(...) \n\r");	
	clear_tr16(&t16_1);
    t16_1 = smtr16("0+-+-+-+-+-+-00+");
	tlog16("t16_1 = ",t16_1);
	tr16_to_nine_string(buf8,t16_1);	 
	printf("t16 -> ");
	buflog8(buf8);
	printf("\n\r");		

	printf("\n\r t011: smtr32(...) \n\r");	
	clear_tr32(&t32_1);
    t32_1 = smtr32("0+-+-+-+-+-+-00+0+-+-+-+-+-+-00+");
	tlog32("t32_1 = ",t32_1);
	tr32_to_nine_string(buf16,t32_1);	 
	printf("t32 -> ");		
	buflog16(buf16);
	printf("\n\r");		

	printf("\n\r t012: search inst into inst_list[] \n\r");	
	extern const struct inst_data inst_list[];
	uint32_t inst = 0x00000003;

	for(int idx=0;inst_list[idx].bit_pattern != NULL;idx++){        
			if(bitpat_match_s(inst, inst_list[idx].bit_pattern)){
				//viv- old code  inst_list[idx].func(&cput, inst);
				vlog32("done inst = ",inst); //viv+ test
				break;
			}			
	}                    	

	printf("\n\r t013: search trit inst into trs_inst_list[] \n\r");	
	extern const struct inst_trit_data trs_inst_list[];
	tr32 trs_inst = smtr32("000000000000000000000000000000++");
        tlog32("trs_inst = ",trs_inst);
                        
	for(int idx=0;trs_inst_list[idx].bit_pattern != NULL;idx++){        
			if(tritpat_match_s(trs_inst, trs_inst_list[idx].bit_pattern)){
				//viv- old code  inst_list[idx].func(&cput, inst);
				tlog32("done trs_inst = ",trs_inst); //viv+ test
				break;
			}			
	}                    	

	printf("\n\r t014: memory_write(...)  memory_read(...) \n\r");	
	tr32 addr_ram = smtr32("00000000000+-0--0-000--+++-+-++-");
	tr8 data8 = smtr8("+-0--0-0");
	tlog32("addr_ram=",addr_ram);
	vlog64( "addr_ram_binary=", tr32_to_int64(addr_ram) ); 
	tlog8("wr data8=",data8);	
	memory_write(c, addr_ram, data8);
	clear_tr8(&data8);		
	memory_read(c, addr_ram, &data8);
	tlog8("rd data8=",data8);
	
	printf("\n\r [ END Tests. ]\n\r");
}

