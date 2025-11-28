#include <assert.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <unistd.h>
#include <dirent.h>
#include <stdarg.h>
#include <fcntl.h>
#include <sys/select.h>
#include <errno.h>  

#include "cli.h"  


/** --------------------------------------------------------
 *  CLI командный интерпретатор
 */
/* Разбор параметров команды с параметрами */
char par1[1024];
char par2[1024];
char par3[1024];
char par4[1024];


/* Cтруктура описывающая информацию в сообщении. */
typedef struct _cmd_data
{
	int count; /* счетчик парметров */
	char *par2;
	char *par3;
	char *par4;
} cmd_data_t;

static char buf[BUF_SIZE]; /* буфер для приема сообщений */

cmd_data_t cmd_data;  

static emustatus_e emu_stat = NOREADY_EMU_ST;  

/**
 * Вывод отладочной информации памяти машины "Сетунь-1958"
 */
static uint8_t LOGGING = 0;	  /* флаг логирование выполнение операций */
static uint8_t STEP_FLAG = 0; /* флаг выполнить количество шагов */
static uint32_t STEP = 0;	  /* счетчик количества операций */
static uint32_t counter_step = 0;
static int32_t BREAKPOINT = 0;	  /* режима останова по значению программного счетчика */
//viv- static trs_t BREAKPOINT_TRS;	  /* режима останова по значению программного счетчика */
static int32_t BREAKPOINT_MB = 0; /* режима останова по значению адреса магнитного барабана */
//viv-  static trs_t BREAKPOINT_MB_TRS;	  /* режима останова по значению адреса магнитного барабана */ 
/* 
 * Регист переключения Русский/Латинский 
 * RUS == 0
 * LAT == 1   
 */
uint8_t russian_latin_sw = 0;
/* Регист переключения Буквенный/Цифровой */
uint8_t letter_number_sw = 0;
/* Регист переключения цвета печатающей ленты */
uint8_t color_sw = 0;   

#define Version "0.04" 

void print_version(void)
{
	printf(" Emulator computer architecture 'TRIT-RISC-V':\r\n");
	printf(" Version: %s\r\n", Version);
	printf(" Author:  Vladimir V.I.\r\n");
	printf(" E-mail:  askfind@ya.ru\r\n");
	printf("\r\n");
}  

/* Вывод списка команд виртуальной панели управления */
void help_print(void)
{
	printf(" Commands control for Emulator TRIT-RISC-V:\r\n");
	printf(" dump     [arglist]\r\n");
	printf(" load     [l]    [arglist]\r\n");
	printf(" prn      [p]    [arglist]\r\n");
	printf(" debug    [d]    [arglist]\r\n");
	printf(" view     [v]\r\n");
	printf(" begin    [b]\r\n");
	printf(" pause    [p]\r\n");
	printf(" run      [r]\r\n");
	printf(" step     [s]    [arglist] \r\n");
	printf(" break    [br]   [arglist]\r\n");
	printf(" breakmb  [brm]  [arglist]\r\n");
	printf(" reg      [rg]   [arglist]\r\n");
	printf(" fram     [fr]   [arglist]\r\n");
	printf(" drum     [dr]   [arglist]\r\n");
	printf(" help     [h]\r\n");
	printf(" quit     [q]\r\n");
	printf(" calc     [arglist]\r\n");

}

static char prn_cmd(char *buf, void *data);
static char dump_cmd(char *buf, void *data);
static char load_cmd(char *buf, void *data);
static char debug_cmd(char *buf, void *data);
static char begin_cmd(char *buf, void *data);
static char pause_cmd(char *buf, void *data);
static char run_cmd(char *buf, void *data);
static char step_cmd(char *buf, void *data);
static char break_cmd(char *buf, void *data);
static char break_drum_cmd(char *buf, void *data);
static char reg_cmd(char *buf, void *data);
static char view_cmd(char *buf, void *data);
static char fram_cmd(char *buf, void *data);
static char drum_cmd(char *buf, void *data);
static char help_cmd(char *buf, void *data);
static char quit_cmd(char *buf, void *data);
static char calc_cmd(char *buf, void *data);
                                                                                            
/*
 * Массив команд CLI
 */
ascii_message_t command[] =
	{
		 {.name_cmd = "dump",
		 .parser = dump_cmd,
		 .data = &cmd_data},

		{.name_cmd = "load",
		 .parser = load_cmd,
		 .data = &cmd_data},
		{.name_cmd = "l",
		 .parser = load_cmd,
		 .data = &cmd_data},

		{.name_cmd = "debug",
		 .parser = debug_cmd,
		 .data = &cmd_data},
		{.name_cmd = "d",
		 .parser = debug_cmd,
		 .data = &cmd_data},

		{.name_cmd = "begin",
		 .parser = begin_cmd,
		 .data = &cmd_data},
		{.name_cmd = "b",
		 .parser = begin_cmd,
		 .data = &cmd_data},

		{.name_cmd = "pause",
		 .parser = pause_cmd,
		 .data = &cmd_data},
		
		 {.name_cmd = "p",
		 .parser = pause_cmd,
		 .data = &cmd_data},

		{.name_cmd = "prn",
		 .parser = prn_cmd,
		 .data = &cmd_data},

		{.name_cmd = "run",
		 .parser = run_cmd,
		 .data = &cmd_data},
		{.name_cmd = "r",
		 .parser = run_cmd,
		 .data = &cmd_data},

		{.name_cmd = "step",
		 .parser = step_cmd,
		 .data = &cmd_data},
		{.name_cmd = "s",
		 .parser = step_cmd,
		 .data = &cmd_data},

		{.name_cmd = "break",
		 .parser = break_cmd,
		 .data = &cmd_data},
		{.name_cmd = "br",
		 .parser = break_cmd,
		 .data = &cmd_data},

		{.name_cmd = "breakmb",
		 .parser = break_drum_cmd,
		 .data = &cmd_data},
		{.name_cmd = "brm",
		 .parser = break_drum_cmd,
		 .data = &cmd_data},

		{.name_cmd = "reg",
		 .parser = reg_cmd,
		 .data = &cmd_data},
		{.name_cmd = "rg",
		 .parser = reg_cmd,
		 .data = &cmd_data},

		{.name_cmd = "view",
		 .parser = view_cmd,
		 .data = &cmd_data},
		{.name_cmd = "v",
		 .parser = view_cmd,
		 .data = &cmd_data},

		{.name_cmd = "fram",
		 .parser = fram_cmd,
		 .data = &cmd_data},
		{.name_cmd = "fr",
		 .parser = fram_cmd,
		 .data = &cmd_data},

		{.name_cmd = "drum",
		 .parser = drum_cmd,
		 .data = &cmd_data},
		{.name_cmd = "dr",
		 .parser = drum_cmd,
		 .data = &cmd_data},

		{.name_cmd = "help",
		 .parser = help_cmd,
		 .data = &cmd_data},
		{.name_cmd = "h",
		 .parser = help_cmd,
		 .data = &cmd_data},

		{.name_cmd = "quit",
		 .parser = quit_cmd,
		 .data = &cmd_data},
		{.name_cmd = "q",
		 .parser = quit_cmd,
		 .data = &cmd_data},
		{.name_cmd = "calc",
		 .parser = calc_cmd,
		 .data = &cmd_data}
};


/* Func 'cli_ascii' */
void cli_ascii(void)
{
	printf("\r\n");
	printf("Emu trit-risc-v:\r\n");
} 

/** -------------------------------
 *   Реализация команд
 */

/* Func 'prn_cmd' установка языка для печати */
char prn_cmd(char *buf, void *data)
{
	cmd_data_t *pars = (cmd_data_t *)data;

	if ((pars->count < 1) || (pars->count > 1))
	{
		/* Error */
		printf("prn: ERR#1\r\n");
		return 1; /* ERR#1 */
	}

	int par2_numb = 0;
	sscanf(pars->par2, "%i", &par2_numb);

	if (par2_numb > 0)
	{
		//viv- russian_latin_sw = 1; /* Печать LAT */
		printf("switch print 'lat'\r\n");
	}
	else
	{
		//viv- russian_latin_sw = 0; /* Печать RUS */
		printf("switch print 'rus'\r\n");
	}
	
	return 0; /* OK' */
}

/* Func 'dump_cmd' */
char dump_cmd(char *buf, void *data)
{
	cmd_data_t *pars = (cmd_data_t *)data;

	if (pars->count > 1)
	{
		/* Error */
		printf("dbg: ERR#1\r\n");
		return 1; /* ERR#1 */
	}

	/* Проверить путь к каталогам,  файл списка lst */
	
	//viv- 
	//if (DumpFileTxs(pars->par2) != 0)
	//{
	//	printf("no path file\r\n");
	//}

	return 0; /* OK' */
}

/* Func 'load_cmd' */
char load_cmd(char *buf, void *data)
{
	cmd_data_t *pars = (cmd_data_t *)data;

	if (pars->count > 2)
	{
		/* Error */
		printf("ERROR load_cmd!\r\n");
		return 1; /* ERR#1 */
	}

	/* Проверить путь к каталогам,  файл списка lst */
	//viv- 
	//if (ConvertSWtoPaper(pars->par2, pars->par3) != 0)
	//{
	//	printf("no path file\r\n");
	//}

	return 0; /* OK' */
}

/* Func 'debug_cmd' */
char debug_cmd(char *buf, void *data)
{
	cmd_data_t *pars = (cmd_data_t *)data;

	if ((pars->count < 1) || (pars->count > 1))
	{
		/* Error */
		printf("dbg: ERR#1\r\n");
		return 1; /* ERR#1 */
	}

	int par2_numb = 0;
	sscanf(pars->par2, "%i", &par2_numb);

	if (par2_numb > 0)
	{
	//viv- 
		//LOGGING = 1; /* Вывод отладочной информации */
		printf("switch debug on\r\n");
	}
	else
	{
	//viv- 
		//LOGGING = 0; /* Вывод отладочной информации */
		printf("switch debug off\r\n");
	}

	return 0; /* OK' */
}

/* Func 'begin_cmd' */
char begin_cmd(char *buf, void *data)
{
	cmd_data_t *pars = (cmd_data_t *)data;

	if (pars->count > 0)
	{
		/* Error */
		printf("ERROR begin_cmd!\r\n");
		return 1; /* ERR#1 */
	}

	counter_step = 0;
	emu_stat = BEGIN_EMU_ST;

	return 0; /* OK' */
}

/* Func 'pause_cmd' */
char pause_cmd(char *buf, void *data)
{
	cmd_data_t *pars = (cmd_data_t *)data;

	if (pars->count > 0)
	{
		/* Error */
		printf("ERROR pause_cmd!\r\n");
		return 1; /* ERR#1 */
	}

	/* Новое состояние пауза */
	emu_stat = PAUSE_EMU_ST;

	return 0; /* OK' */
}

/* Func 'run_cmd' */
char run_cmd(char *buf, void *data)
{
	cmd_data_t *pars = (cmd_data_t *)data;

	if (pars->count > 0)
	{
		/* Error */
		printf("ERROR run_cmd!\r\n");
		return 1; /* ERR#1 */
	}

	/* Новое состояние выполнять операцииы */
	emu_stat = LOOP_WORK_EMU_ST;

	return 0; /* OK' */
}

/* Func 'step_cmd' */
char step_cmd(char *buf, void *data)
{
	cmd_data_t *pars = (cmd_data_t *)data;

	if (pars->count > 1)
	{
		/* Error */
		printf("ERROR step_cmd!\r\n");
		return 1; /* ERR#1 */
	}

	sscanf(pars->par2, "%ul", &STEP);

	emu_stat = STEP_EMU_ST;

	return 0; /* OK' */
}

/* Func 'break_cmd' */
char break_cmd(char *buf, void *data)
{
	cmd_data_t *pars = (cmd_data_t *)data;

	if (pars->count > 1)
	{
		/* Error */
		printf("ERROR break_cmd!\r\n");
		return 1; /* ERR#1 */
	}

	if (pars->count == 0)
	{
	//viv- 
		//BREAKPOINT = 0;
		//BREAKPOINT_TRS.l = 0;
	}
	else
	{
	//viv- 
		//BREAKPOINT = trs2digit(smtr(pars->par2));
		//BREAKPOINT_TRS = smtr(pars->par2);
	}

	return 0; /* OK' */
}

/* Func 'break_drum_cmd' */
char break_drum_cmd(char *buf, void *data)
{
	cmd_data_t *pars = (cmd_data_t *)data;

	if (pars->count > 1)
	{
		/* Error */
		printf("ERROR break_drum_cmd!\r\n");
		return 1; /* ERR#1 */
	}

	if (pars->count == 0)
	{
	//viv- 
		//BREAKPOINT_MB = 0;
		//BREAKPOINT_MB_TRS.l = 0;
	}
	else
	{
	//viv- 
		//BREAKPOINT_MB = trs2digit(smtr(pars->par2));
		//BREAKPOINT_MB_TRS = smtr(pars->par2);
	}

	return 0; /* OK' */
}

/* Func 'reg_cmd' */
char reg_cmd(char *buf, void *data)
{
	cmd_data_t *pars = (cmd_data_t *)data;

	if ((pars->count < 1) || (pars->count > 2))
	{
		/* Error */
		printf("ERROR reg_cmd!\r\n");
		return 1; /* ERR#1 */
	}

	int rg = toupper(pars->par2[0]);
	//viv-
	//trs_t trs = smtr(pars->par3);
	switch (rg)
	{
	case 'K':
		//viv- 
		//copy_trs(&trs, &K);
		break;
	case 'F':
		//viv- 
		//copy_trs(&trs, &F);
		break;
	case 'C':
		//viv- 
		//copy_trs(&trs, &C);
		break;
	case 'W':
		//viv- 
		//copy_trs(&trs, &W);
		break;
	case 'S':
		//viv- 
		//copy_trs(&trs, &S);
		break;
	case 'R':
		//viv- 
		//copy_trs(&trs, &R);
		break;
	case 'M':
		//viv- 
		//copy_trs(&trs, &MB);
		break;
	default:
		break;
	}

	return 0; /* OK' */
}

/* Func 'view_cmd' */
char view_cmd(char *buf, void *data)
{
	cmd_data_t *pars = (cmd_data_t *)data;

	printf("[ View registers Emu ]\r\n");

	if (pars->count > 0)
	{
		/* Error */
		printf("ERROR view_cmd!\r\n");
		return 1; /* ERR#1 */
	}

	/* Prints REGS */
	//viv- 
	//view_short_regs();
	printf("\r\n");
	printf("[ Tools ]\r\n");

	switch (emu_stat)
	{
	case NOREADY_EMU_ST:
		printf("  status: noready\r\n");
		break;
	case BEGIN_EMU_ST:
		printf("  status: begin\r\n");
		break;
	case LOOP_WORK_EMU_ST:
		printf("  status: loop\r\n");
		break;
	case STEP_EMU_ST:
		printf("  status: step\r\n");
		break;
	case ERROR_EMU_ST:
		printf("  status: error\r\n");
		break;
	case ERROR_MB_NUMBER_EMU_ST:
		printf("  status: error drum\r\n");
		break;
	case CLI_WELCOM_EMU_ST:
		printf("  status: welcom\r\n");
		break;
	case WAIT_EMU_ST:
		printf("  status: wait\r\n");
		break;

	default:
		break;
	}

	if (russian_latin_sw)
	{
		printf("  print: 'lat'\r\n");
	}
	else
	{
		printf("  print: 'rus'\r\n");
	}
	if (LOGGING)
	{
		printf("  debug: on\r\n");
	}
	else
	{
		printf("  debug: off\r\n");
	}
	//viv- 
	//if (BREAKPOINT_TRS.l != 0)
	//{
	//	//viv- 
	//	//view_short_reg(&BREAKPOINT_TRS, "  breakpoint");
	//}
	//else
	//{
	//	printf("  breakpoint fram: no used\r\n");
	//}
	//viv- 
	//if (BREAKPOINT_MB_TRS.l != 0)
	//{
	//	view_short_reg(&BREAKPOINT_MB_TRS, "  breakpoint drum");
	//}
	//else
	//{
	//	printf("  breakpoint drum: no used\r\n");
	//}
	printf("  steps: %d\r\n", counter_step);

	return 0; /* OK' */
}

/* Func 'fram_cmd' */
char fram_cmd(char *buf, void *data)
{
	cmd_data_t *pars = (cmd_data_t *)data;

	if (pars->count > 1)
	{
		/* Error */
		printf("ERROR fram_cmd!\r\n");
		return 1; /* ERR#1 */
	}

	if(1) //viv-  (valid_smtr(pars->par2) == 0)
	{
		//viv- 
		//dump_fram_zone(smtr(pars->par2));
	}
	else
	{
		int8_t len;
		uint8_t pr[2] = {0};
		uint8_t sm[1] = {0};
		len = strlen(pars->par2);
		if (len < 1 || len > 1)
		{
			/* Error */
			printf("ERROR fram_cmd!\r\n");
			return 1; /* ERR#1 */
		}
		//viv- 
		//memcpy(pr, lt2symtrs(*(pars->par2)), 2);
		//sm[0] = pr[1];
		//dump_fram_zone(smtr(sm));
	}

	return 0;
}

/* Func 'drum_cmd' */
char drum_cmd(char *buf, void *data)
{
	cmd_data_t *pars = (cmd_data_t *)data;

	if (pars->count > 1)
	{
		/* Error */
		printf("ERROR drum_cmd!\r\n");
		return 1; /* ERR#1 */
	}

	if (1) //viv- (valid_smtr(pars->par2) == 0)
	{
		//viv- 		
		//view_drum_zone(smtr(pars->par2));
	}
	else
	{
		int8_t len;
		uint8_t sm[4] = {0};

		len = strlen(pars->par2);

		if (len < 1 || len > 2)
		{
			/* Error */
			printf("ERROR drum_cmd!\r\n");
			return 1; /* ERR#1 */
		}

		switch (len)
		{
		case 1:
		{
			//viv- 
			//memcpy(sm, lt2symtrs(*(pars->par2 + 0)), 2);
		}
		break;
		case 2:
		{
			//viv- 
			//memcpy(sm, lt2symtrs(*(pars->par2 + 0)), 2);
			//memcpy(sm + 2, lt2symtrs(*(pars->par2 + 1)), 2);
		}
		break;
		}
		//viv- 
		//view_drum_zone(smtr(sm));
	}

	return 0; /* OK' */
}

/* Func 'help_cmd' */
char help_cmd(char *buf, void *data)
{
	cmd_data_t *pars = (cmd_data_t *)data;

	if (pars->count > 0)
	{
		/* Error */
		printf("ERROR help_cmd!\r\n");
		return 1; /* ERR#1 */
	}

	help_print();

	return 0; /* OK' */
}

/* Func nmea_next_field */
char *ascii_next_field(char *buf)
{
	/* Eсли следующее поле не найдено, то она возвращает NULL */
	while (*buf++ != ',')
	{
		if (*buf == '\0')
		{
			return NULL;
		}
	}
	return buf;
}

/* Func 'exit_cmd' */
char exit_cmd(char *buf, void *data)
{
	cmd_data_t *msg = (cmd_data_t *)data;

	printf("dbg: exit_cmd() \r\n");

	fflush(stdout);

	return 0;
}


void Emu_Stop(void)
{

	/* Prints REGS and FRAM */
	if (LOGGING > 0)
	{
	}

	/* Печать завершения работы "Emu" */
	if (STEP == 0)
	{
		/* Новое состояние */
		emu_stat = PAUSE_EMU_ST;

		printf("\r\n[ Stop Emu ]\r\n");
	}
	else
	{
		printf("\r\n[ Step = %d : Stop Emu ]\r\n", STEP);
	}
}


/* Func 'quit_cmd' */
char quit_cmd(char *buf, void *data)
{
  	cmd_data_t *pars = (cmd_data_t *)data;    
  	
	if (pars->count > 0)
	{
		/* Error */
		printf("ERROR quit_cmd!\r\n");
		return 1; /* ERR#1 */
	}

	Emu_Stop();

	fflush(stdout);

	exit(0);

	return 0; 
}
/** -------------------
 *   Func 'ascii parser'
 */
int ascii_parser(char *buf)
{
	int i;

	memset(par1, 0, sizeof(par1));
	memset(par2, 0, sizeof(par2));
	memset(par3, 0, sizeof(par3));
	memset(par4, 0, sizeof(par4));

	sscanf(buf, "%s %s %s %s", par1, par2, par3, par4);
	/*
	 viv- old  printf("par1=%s par2=%s par3=%s par3=%s\r\n",par1,par2,par3,par4);
	*/
	for (i = 0; i < ARRAY_SIZE(command); i++)
	{
		if ((strncmp(par1, command[i].name_cmd, strlen(command[i].name_cmd)) == 0) &&
			(strlen(par1) == strlen(command[i].name_cmd)))
		{
			if (command[i].parser)
			{
				int cnt = 0;
				cmd_data.par2 = par2;
				if (strlen(par2) > 0)
					cnt++;
				cmd_data.par3 = par3;
				if (strlen(par3) > 0)
					cnt++;
				cmd_data.par4 = par4;
				if (strlen(par4) > 0)
					cnt++;
				cmd_data.count = cnt;

				command[i].data = (cmd_data_t *)&cmd_data;
				return command[i].parser(par1, command[i].data);
			}
		}
	}
	return -1;
}

/* Process ascii string + <LF><CR> */
void Process_ascii_string(char c)
{
	static char *p_beg = NULL;
	static char *p_cur = NULL;
	static char *p_end = NULL;

	static char status = 0;
	/*
	 * Сканер строк ascii
	 */
	switch (status)
	{
	case 0: /* Иницализация сканера */
	{
		memset(buf, 0, ARRAY_SIZE(buf));
		p_beg = &buf[0];
		p_cur = p_beg;
		p_end = p_beg + ARRAY_SIZE(buf) - 1;
		status = 1;
	}
	case 1: /* Ввод символов и прверка 'LF' или 'CR' */
	{
		if (c == '\r' || c == '\n')
		{
			static char res = 0;

			res = ascii_parser(buf);
			if (res >= 0)
			{
			}
			else
			{
				printf("no command\r\n");
			}
			cli_ascii();

			/* Новый статус сканера */
			status = 0;
		}
		else
		{
			if (p_cur < p_end)
			{
				*p_cur++ = c; /* Сохранить байт в буфере */
			}
			else
			{
				*p_cur = c;
			}
		}
	}
	break;
	default:		/* Что-то не так !*/
		status = 0; /* Сброс сканера фысшш */
		break;
	} /* end switch() */
}  

int Process_Work_Emulation(void)
{
	int test = 0;
	int convert = 0;
	int dump = 0;
	char *output = "-";
	int ret = 0;

	/* ------------------------
	 *  Переменные emulator Emu
	 */
	uint8_t cmd[20] = {0};
	uint8_t ret_exec = 0;

	/* Состояние после включения питания */
	if (emu_stat == NOREADY_EMU_ST)
	{

		//viv- old code  
		//Emu_Open_Files();
		/* Инициализация таблиц символов ввода и вывода "Сетунь-1958" */
		//viv- old code  
		//init_tab4();

		/* Новое состоняие */
		emu_stat = WAIT_EMU_ST;
	}

	if (emu_stat == BEGIN_EMU_ST)
	{
		/* Начальный старт */
		//viv- old code  
		//Emu_Begin();

		/* Новое состоняие */
		emu_stat = LOOP_WORK_EMU_ST;
	}

	if (emu_stat == STEP_EMU_ST)
	{
		//viv- old code  
		//ret_exec = Emu_Step();
		
		ret_exec = STOP; //viv+ dbg
		
		if ((ret_exec == STOP))
		{
			printf("\r\n<STOP>\r\n");
			emu_stat = STOP_EMU_ST;
		}
		else if (ret_exec == STOP_OVER)
		{
			printf("\r\n<STOP_OVER>\r\n");
			emu_stat = STOP_EMU_ST;
		}
		else if (ret_exec == STOP_ERROR)
		{
			printf("\r\nERR#:%i<STOP_ERROR>\r\n", ret_exec);
			emu_stat = ERROR_EMU_ST;
		}
		else if (ret_exec == STOP_ERROR_MB_NUMBER)
		{
			printf("\r\nERR#:%i<STOP_ERROR_MB_NUMBER>\r\n", ret_exec);
			cli_ascii();
			emu_stat = ERROR_MB_NUMBER_EMU_ST;
		}

		counter_step++;
		if (STEP > 0)
			STEP--;
		if (STEP == 0)
		{
			/* Новое состояние */
			emu_stat = PAUSE_EMU_ST;
		}

		//viv- old code  		
		//if (BREAKPOINT == trs2digit(C) && (BREAKPOINT <= INT32_MAX))
		//{
		//	emu_stat = PAUSE_EMU_ST;
		//}

		//viv- old code  
		//if (BREAKPOINT_MB == trs2digit(MB) && (BREAKPOINT_MB <= INT32_MAX))
		//{
		//	emu_stat = PAUSE_EMU_ST;
		//}

		// viv- test  view_short_regs();
		
		/* Новое состояние */
		emu_stat = WAIT_EMU_ST;
	}

	if (emu_stat == PAUSE_EMU_ST)
	{
		emu_stat = WAIT_EMU_ST;
	}

	/* Состояние готов к работе */
	if (emu_stat == LOOP_WORK_EMU_ST)
	{

		//viv- old code  
		//if (BREAKPOINT_TRS.l != 0)
		//{
		//	if (BREAKPOINT == trs2digit(C) && (BREAKPOINT <= INT32_MAX))
		//	{
		//		view_short_regs();
		//		view_short_reg(&BREAKPOINT_TRS, "\r\n[ BREAK ] ");
		//		emu_stat = PAUSE_EMU_ST;
		//	}
		//}

		//viv- old code  
		//if (BREAKPOINT_MB_TRS.l != 0)
		//{
		//	if (BREAKPOINT_MB == trs2digit(MB) && (BREAKPOINT_MB <= INT32_MAX))
		//	{
		//		view_short_regs();
		//		view_short_reg(&BREAKPOINT_MB_TRS, "\r\n[ BREAK ADDR DRUM] ");
		//		emu_stat = PAUSE_EMU_ST;
		//	}
		//}

		//viv- old code  
		//ret_exec = Emu_Step();

		counter_step++;

		if ((ret_exec == STOP))
		{
			printf("\r\n<STOP>\r\n");
			emu_stat = STOP_EMU_ST;
			/*
			break;
			*/

			//viv- old code  
			//view_next_start();
		}
		else if (ret_exec == STOP_OVER)
		{
			printf("\r\n<STOP_OVER>\r\n");
			emu_stat = STOP_EMU_ST;
			/*
			 break;
			 */

			//viv- old code  
			//view_next_start();
		}
		else if (ret_exec == STOP_ERROR)
		{
			printf("\r\nERR#:%i<STOP_ERROR>\r\n", ret_exec);
			emu_stat = ERROR_EMU_ST;
			/*
			 break;
			*/

			//viv- old code  
			//view_next_start();
		}
		else if (ret_exec == STOP_ERROR_MB_NUMBER)
		{
			printf("\r\nERR#:%i<STOP_ERROR_MB_NUMBER>\r\n", ret_exec);
			cli_ascii();
			emu_stat = ERROR_MB_NUMBER_EMU_ST;
			/*
			 break;
			*/

			//viv- old code  
			//view_next_start();
		}
	}

	/* Состояние  */
	if (emu_stat == STOP_EMU_ST)
	{
		emu_stat = CLI_WELCOM_EMU_ST;
	} /* if(...) */

	/* Состояние  */
	if (emu_stat == ERROR_EMU_ST)
	{
		emu_stat = CLI_WELCOM_EMU_ST;
	} /* if(...) */

	/* Состояние  */
	if (emu_stat == ERROR_MB_NUMBER_EMU_ST)
	{
		emu_stat = CLI_WELCOM_EMU_ST;
	} /* if(...) */

	/* Состояние  */
	if (emu_stat == CLI_WELCOM_EMU_ST)
	{
		/* Prints REGS */
		//viv- old code  
		//view_short_regs();
		//printf("\r\n");

		cli_ascii();
		emu_stat = WAIT_EMU_ST;
	} /* if(...) */

	/* Состояние  */
	if (emu_stat == WAIT_EMU_ST)
	{

	} /* if(...) */

} /*  */

/*
 * Func 'calc_cmd' - реализация калькулятора Emu 
 */
char calc_cmd(char *buf, void *data)
{

	char operator;
	double num1, num2, result;

	printf("\r\nEmu\r\n");

	printf("calc: ");

	cmd_data_t *pars = (cmd_data_t *)data;
	if ((pars->count < 1) || (pars->count > 3))
	{
		/* Error */
		printf("\r\ndbg: ERR#1\r\n");
		return 1; /* ERR#1 */
	}

	sscanf(pars->par2, "%lf", &num1);
	sscanf(pars->par3, "%c", &operator);
	sscanf(pars->par4, "%lf", &num2);

	switch (operator)
	{
	case '+':
		result = num1 + num2;
		break;
	case '-':
		result = num1 - num2;
		break;
	case '*':
		result = num1 * num2;
		break;
	case '/':
		result = num1 / num2;
		break;
	default:
		printf("ERROR: Invalid operator\n");
		return 1;
	}

	printf("%.8lf %c %.8lf = %.8lf\n", num1, operator, num2, result);

	printf("\r\nTODO: add codes for calculate trits !\r\n\r\n");

	return 0;
}

