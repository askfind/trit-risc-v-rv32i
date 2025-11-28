#ifndef INST_TRIT_H
#define INST_TRIT_H

#include <stdint.h>

#include "ternary.h"
#include "inst_trit.h"

typedef void (*inst_trit_func)(struct cpu_trit *c, tr32 inst_trit);

struct inst_trit_data {
    char *bit_pattern;
    inst_trit_func func;
};

extern const struct inst_trit_data trs_inst_list[];


/* Троичные */
void trs_pc_update(struct cpu_trit *c, tr32 offset);
void trs_pc_write(struct cpu_trit *c, tr32 addr);
tr32 trs_pc_read(struct cpu_trit *c);

//viv+ dbg
void trs_reg_write(struct cpu_trit *c, uint8_t reg_idx, tr32 data);
void trs_reg_read(struct cpu_trit *c, uint8_t reg_idx, tr32 * tr );

void trs_rom_read_w(struct cpu_trit *c, tr32 *tr);

void trs_mem_read_b(struct cpu_trit *c, tr32 addr, tr8 *tr);
void trs_mem_read_w(struct cpu_trit *c, tr32 addr, tr32 *tr);


/* Отладка команд троичного процессра */
void trs_dbg_oper(struct cpu_trit *c);

#endif /* INST_TRIT_H */
