#ifndef CPU_TRIT_H
#define CPU_TRIT_H

#include "ternary.h"


/*  
 * Стандартная карта памяти RISC-V
 * 
 * 0x00000000 - 0x00000FFF: Boot ROM                  [t0000000000 ... t0+-0-0--00] 
 * 0x00001000 - 0x00001FFF: Firmware                  [t0+-0-0--0+ ... t0++-+-0+0+]   
 * 0x00002000 - 0x00003FFF: Interrupt Vector Table    [t0++-+-0++- ... t+-+++++-+0]
 *
 *
 * 0xFFFFFFFF ┌────────────────┐					   t++-0+-+00-+-00-----+0	
 *            │    I/O Space   │
 * 0x40000000 ├────────────────┤                       t0+0-+0-++++0-+-0-0-0+  
 *            │  Reserved/MMIO │
 * 0x20000000 ├────────────────┤                       t00++0+++-+-00--++00--
 *            │      RAM       │ ← Динамическая память (ОЗУ)
 * 0x80000000 ├────────────────┤                       t+-0--0-000--+++-+-++-
 *            │      ROM       │ ← Постоянная память (ПЗУ)
 * 0x00000000 └────────────────┘
 *
 * ROM (Read-Only Memory) - ПЗУ
 *   Характеристики:
 *   Только для чтения
 *   Хранит начальный код
 *   Обычно в нижней части адресного пространства
 */

/*
 * Реализация в эмуляторе: 
 */
#define ROM_BASE   0x00000000
//viv dbg #define ROM_SIZE   0x00003000  // ROM  : 12KB, 12288 bytes,   t0+-0-0--0+0

#define RAM_BASE   0x80000000
//viv dbg #define RAM_SIZE   0x00010000  // RAM  : 64KB , 65536 bytes,  t+0+000-0+-+

#define MMIO_BASE  0x40000000
#define MMIO_SIZE  0x00001000  // MMIO :      , 4096 bytes,   t00+-0-0--0+       

#define REGISTER_SIZE 16 	 /* количество троичных регистров процессора */

#define INST_ROM_SIZE 243  /* для троичного числа адреса ROM */
#define DATA_RAM_SIZE 243  /* для троичного числа адреса RAM */

//viv dbg
#define ROM_SIZE INST_ROM_SIZE
#define RAM_SIZE DATA_RAM_SIZE

struct cpu_trit {
    /* 
     * Модель процессора TRIT RISC-V
     */
    tr32 pc;							/* Program Counter (PC) */
    tr32 reg[REGISTER_SIZE];    		/* User general-purpose (base) registers, namely, x1 to x31 */
    tr8  inst_rom_arr[INST_ROM_SIZE];   /*  */
    tr8  data_ram_arr[DATA_RAM_SIZE];
    tr32 csr; 							/* Control and Status Registers (CSRs) */
    /*
     * Указатели на память с возвожностью отрицательных индексов
     */
    tr8 * inst_rom;
    tr8 * data_ram;
};

#endif  /* CPU_TRIT_H */
