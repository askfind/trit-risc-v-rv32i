
#ifndef CLI_H
#define CLI_H

#include <ctype.h>
 
/* 
 * Состояние работы эмулятора
 */
typedef enum
{
	NOREADY_EMU_ST = 0, /* Не готов после сброса питания */
	BEGIN_EMU_ST,		/* Готов к работе */
	LOOP_WORK_EMU_ST,	/* Непрерывная работа */
	STEP_EMU_ST,		/* Пошаговая выполнение операций */
	PAUSE_EMU_ST,		/* Пауза */
	STOP_EMU_ST,		/* Стоп */
	ERROR_EMU_ST,		/* Ошибка */
	ERROR_MB_NUMBER_EMU_ST,
	CLI_WELCOM_EMU_ST,
	WAIT_EMU_ST
} emustatus_e;   

/**
 * Статус выполнения инструкции Emu
 */
enum
{
	OK = 0,					 /* Успешное выполнение операции */
	WORK = 1,				 /* Выполнение операций виртуальной машины */
	STOP = 2,				 /* Успешный останов машины */
	STOP_OVER = 3,			 /* Останов по переполнению результата операции машины */
	STOP_ERROR = 4,			 /* Аварийный останов машины */
	STOP_ERROR_MB_NUMBER = 5 /* Номер зоны DRUM не поддерживается */
};

/* Cтруктура, описывающая ascii-сообщение */
typedef struct _ascii_message
{
	char *name_cmd;						   /* имя команды */
	char (*parser)(char *buf, void *data); /* функция команды */
	void *data;							   /* дополнительный параметры команды */
} ascii_message_t;



#define ARRAY_SIZE(array) (sizeof(array) / sizeof(array[0]))

#define BUF_SIZE 1024	   /* размер буфера */

extern void cli_ascii(void);
extern void print_version(void);
extern void Process_ascii_string(char c);
extern int Process_Work_Emulation(void);

#endif /* EOF CLI_H */
