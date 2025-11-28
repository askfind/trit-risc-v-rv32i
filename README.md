## Functional Simulator RISC-V, TRIT-RISC-V


# RISC-V RV32E/RV32I
# TRIT-RISC-V TRV32E/TRV32I


# How to use

## Build
```
make
```

## Clean
```
make clean
```


## Test RISC-V Instruction RV32E/RV32I
```
make test
```

## Test TRIT-RISC-V Instruction TRV32E/TRV32I
```
make test_trit
```


## Use
```
Usage: ./emu NCYCLES [-a] [-q] [-m] [-t ROM] [-d RAM] [FILENAME]
Options:
  -q       : No log print
  -m       : Dump memory
  -t ROM   : Initial ROM data
  -d RAM   : Initial RAM data
  -a       : Set cpu trits
  FILENAME : ELF Binary
```


Пример для 32-тритного слова
----------------------------

Little-endian (стандарт для TRIT-RISC-V):
text

Кодирование трита битами
( val1.[3..0]  )  if( (val1.0>0) && (val0.0==0)  then trit0 = +1
( val0.[3..0]  )  if( (val1.0==0) && (val0.0==0) then trit0 =  0
( val0.[3..0]  )  if( (val1.0==0) && (val0.0<0)  then trit0 = -1

TRIT32 = t000000000000000000000000000000++

Адрес ROM:  Содержимое 32-тритов = 64 бита
(Hex)       (Hex)
0x0000:     0x03    (младший трайт)
0x0001:     0x00
0x0002:     0x00
0x0003:     0x00
0x0004:     0x00
0x0005:     0x00
0x0006:     0x00
0x0007:     0x00    (старший трайт)


## Test instruction 'tLB'
```
./emu 1 -a -t "03 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00"
```



Что такое знаковое расширение?

Знаковое расширение - это операция копирования старшего бита (знакового бита) во все старшие разряды более широкого регистра.
Конкретно для SignExt([Address]7:0):

    Вход: 8-битное значение [Address]7:0

    Выход: 32-битное значение (в RISC-V)

Как работает:
python

# Псевдокод
def SignExt_8_to_32(value_8bit):
    if (value_8bit & 0x80):  # Если установлен 7-й бит (знаковый)
        return 0xFFFFFF00 | value_8bit  # Расширяем единицами
    else:
        return value_8bit  # Расширяем нулями

Примеры:
Исходное 8-бит	Знаковый бит	Результат 32-бит
0x7F (127)	0 (положит)	0x0000007F
0x80 (-128)	1 (отрицат)	0xFFFFFF80
0xFF (-1)	1 (отрицат)	0xFFFFFFFF
0x0A (10)	0 (положит)	0x0000000A
Где используется в RISC-V:

    Обработка смещений в инструкциях ветвления

    Загрузка байтов с знаком (lb инструкция)

    Загрузка полуслов с знаком (lh инструкция)

    Расширение непосредственных значений в инструкциях

В контексте эмулятора:
c

// Пример реализации на C
int32_t SignExt_8_to_32(uint8_t value) {
    return (int32_t)(int8_t)value;  // Простое приведение типов
}

Эта функция обеспечивает корректную работу с знаковыми числами при переходе от 8-битных к 32-битным значениям в RISC-V архитектуре.


Пример реализации на C:
c

int32_t LB(int32_t rs1_value, int32_t imm, uint8_t* memory) {
    // Вычисление адреса
    int32_t address = rs1_value + imm;

    // Чтение байта из памяти
    uint8_t byte = memory[address];

    // Знаковое расширение
    return (int32_t)(int8_t)byte;
}

Визуализация процесса:
text

Память:      [A0][A1][A2][A3]... где каждый элемент - байт
             ↑
             Address = rs1 + imm

Читаем:      Byte = Memory[Address] = 0x8F (10001111 в двоичном)

Знаковое расширение:
Byte:        10001111 (8 бит, отрицательное)
Расширяем:   11111111 11111111 11111111 10001111 (32 бита, -113 в десятичном)

Записываем в rd: 0xFFFFFF8F

Особенности:

    Знаковое расширение - отличает LB от LBU (беззнаковой загрузки)

    Выравнивание - адрес может быть любым (не требует выравнивания)

    Трап - может вызывать исключение при невалидном адресе


Стандартная карта памяти RISC-V:
text

0xFFFFFFFF ┌─────────────────┐
           │     I/O Space    │
0x40000000 ├─────────────────┤
           │   Reserved/MMIO  │
0x20000000 ├─────────────────┤
           │       RAM        │ ← Динамическая память (ОЗУ)
0x80000000 ├─────────────────┤
           │       ROM        │ ← Постоянная память (ПЗУ)
0x00000000 └─────────────────┘

1. ROM (Read-Only Memory) - ПЗУ
Характеристики:

    Только для чтения

    Хранит начальный код

    Обычно в нижней части адресного пространства

Типичное расположение:
text

0x00000000 - 0x00000FFF: Boot ROM
0x00001000 - 0x00001FFF: Firmware
0x00002000 - 0x00003FFF: Interrupt Vector Table

Содержимое ROM:
asm

# Пример содержимого ROM
_reset_vector:
    jal x0, _start        # Прыжок к началу программы
    .word 0               # Выравнивание
    .word 0

_start:
    # Инициализация процессора
    la sp, _stack_end     # Установка указателя стека
    call main             # Вызов основной программы

2. RAM (Random Access Memory) - ОЗУ
Характеристики:

    Чтение и запись

    Волантильная память

    Для данных и стека

Типичное расположение:
text

0x80000000 - 0x80007FFF: Data Section
0x80008000 - 0x8000BFFF: Heap (куча)
0x8000C000 - 0x8000FFFF: Stack (стек)

Пример полной карты памяти:
c

// Типичная карта памяти для embedded RISC-V
struct memory_map {
    /* ROM Section */
    uint8_t  boot_rom[0x1000];      // 0x00000000-0x00000FFF
    uint8_t  firmware[0x1000];      // 0x00001000-0x00001FFF
    uint8_t  ivt[0x1000];           // 0x00002000-0x00002FFF

    /* Reserved */
    uint8_t  reserved[0x7FFD000];   // 0x00003000-0x7FFFFFFF

    /* RAM Section */
    uint8_t  data_section[0x8000];  // 0x80000000-0x80007FFF
    uint8_t  heap[0x4000];          // 0x80008000-0x8000BFFF
    uint8_t  stack[0x4000];         // 0x8000C000-0x8000FFFF

    /* Memory Mapped I/O */
    uint32_t gpio_ctrl;             // 0x40000000
    uint32_t uart_tx;               // 0x40000004
    uint32_t uart_rx;               // 0x40000008
};

Реализация в эмуляторе:
c

#define ROM_BASE   0x00000000
#define ROM_SIZE   0x00003000  // 12KB ROM

#define RAM_BASE   0x80000000
#define RAM_SIZE   0x00010000  // 64KB RAM

#define MMIO_BASE  0x40000000
#define MMIO_SIZE  0x00001000

struct riscv_memory {
    uint8_t rom[ROM_SIZE];
    uint8_t ram[RAM_SIZE];
    uint8_t mmio[MMIO_SIZE];
};

uint32_t memory_read(struct riscv_memory *mem, uint32_t addr) {
    if (addr >= ROM_BASE && addr < ROM_BASE + ROM_SIZE) {
        return mem->rom[addr - ROM_BASE];  // Чтение из ROM
    }
    else if (addr >= RAM_BASE && addr < RAM_BASE + RAM_SIZE) {
        return mem->ram[addr - RAM_BASE];  // Чтение из RAM
    }
    else if (addr >= MMIO_BASE && addr < MMIO_BASE + MMIO_SIZE) {
        return mmio_read(addr);  // Чтение из MMIO
    }
    return 0;  // Invalid address
}

Особенности RISC-V памяти:

    Little-Endian - младшие байты по младшим адресам

    Алигмент - рекомендация выравнивания для производительности

    MMU - опциональная поддержка виртуальной памяти

    PMAs - атрибуты физической памяти (кэшируемость, порядок)

Такая организация позволяет эффективно разделить код (ROM) и данные (RAM) в RISC-V системе.
