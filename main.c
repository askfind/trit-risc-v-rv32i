/*
* Проект 'TRIT-RISC-V : T-RV32I' на языке C
*
* Дата созд.   : 24.10.2025
* Дата редакт. : 27.11.2025
*
* Версия:        0.04
*
* Автор: 		 Vladimir V.
* E-mail: 	  	 askfind@ya.ru   
* 
*/                              

#include <assert.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <unistd.h>
#include <dirent.h>
#include <fcntl.h>
#include <sys/select.h>
#include <errno.h>  

#include "cpu_trit.h"
#include "elf_parser.h"
#include "bit_trit_parser.h"
#include "inst.h"
#include "ternary.h"
#include "inst_trit.h"
#include "cli.h"
#include "log.h"

extern int flag_quiet;
extern int flag_inst_trits;

void print_usage(FILE *fh)
{
    fprintf(fh, "Usage: Emulator trit-rv32i-sim [-q] [-m] [-t ROM] [-d RAM] [FILENAME] NCYCLES\n");
    fprintf(fh, "Options:\n");
    fprintf(fh, "  -q     : No log print\n");
    fprintf(fh, "  -m     : Dump memory\n");
    fprintf(fh, "  -t ROM : Initial ROM data\n");
    fprintf(fh, "  -d RAM : Initial RAM data\n");
    fprintf(fh, "  -a     : Set cpu trits\n");
}

_Noreturn void print_usage_to_exit(void)
{
    print_usage(stderr);
    exit(1);
}

void print_flags(struct cpu_trit *c){
     //viv- error  log_printf("FLAGS(SZCV) => %c%c%c%c ", signch(c->flag_sign), signch(c->flag_zero), signch(c->flag_carry), signch(c->flag_overflow));
}

void init_cpu(struct cpu_trit *c){

    c->inst_rom = &c->inst_rom_arr[INST_ROM_SIZE/2];
    c->data_ram = &c->data_ram_arr[DATA_RAM_SIZE/2];    
    
    for(int i=0;i<REGISTER_SIZE;i++){        
         uint32_to_tr32(&c->reg[i], i);         
    }

    for(int i=0;i<TRIT32_SIZE-1;i++){        
         c->csr.t[i] = 0;
    }

    for(int i=-INST_ROM_SIZE/2;i<INST_ROM_SIZE/2;i++){
        uint8_to_tr8(&c->inst_rom[i],0);
    }

    for(int i=-DATA_RAM_SIZE/2;i<=DATA_RAM_SIZE/2;i++){                
		for (int j = TRIT8_SIZE-1; j >= 0 ; j--) {                	
			c->data_ram[i].t[j] = 0; //sign(1-(rand()%3));
		}        
    }

    for(int j=0;j<TRIT16_SIZE;j++){
        c->pc.t[j] = 0;
    }

}

void dump_memory(FILE *fh, tr8 *mem, int size)
{
	static uint8_t buf4[4] = {0};
	
	fprintf(fh, "\nDump ternary 8-trits RAM[%0i]\n",size);	
    
    int ii = 0; 
    
    for(int i=-size/2;i<=size/2;i++){                

#if 1
	/* Вывод в девятиричном виде */
	if ( ii % 4 == 0 ) { 
		fprintf(fh,"% 9d :n ",i);
	}       	
	tr8_to_nine_string(buf4,mem[i]);
	buflog4(buf4);
   	fprintf(fh, " ");

	if ( ii % 4 == 3 ) fprintf(fh, "\n");			 
	ii++;
#else /* Вывод в троичном виде */

		if ( ii % 8 == 0)  ) { 
			fprintf(fh,"% 5d : ",i);
		}       	
       	fprintf(fh, "t");
		for (int j = TRIT8_SIZE-1; j >= 0 ; j--) {                	
        	fprintf(fh,"%c", signch(mem[i].t[j]));
		}
		fprintf(fh," ");

		if ( ii % 8 == 7 ) fprintf(fh, "\n");
		ii++;
#endif		
	}
	fprintf(fh, "\n");
}

void set_bytes_from_str(tr8 *dst, const char * const src, int N)
{
    char *buf = (char *)malloc(strlen(src) + 1);
    strcpy(buf, src);
		
    int idst = 0;
    char *tp;
    tp = strtok(buf, " ");
    while (tp != NULL) {
        assert(idst < N && "Too large data!");
        uint8_t val = strtol(tp, NULL, 16);
        
		uint8_to_tr8(&dst[idst++],val);
		
        tp = strtok(NULL, " ");
    }

    free(buf);
}

void set_trytes_from_str(tr8 *dst, const char * const src, int N)
{
    char *buf = (char *)malloc(strlen(src) + 1);
    strcpy(buf, src);	
	
	
    int idst = 0;
    char *tp;
    tp = strtok(buf, " ");
    while (tp != NULL) {
        assert(idst < N && "Too large data!");
        uint8_t val1 = strtol(tp, NULL, 16);
        
        tp = strtok(NULL, " ");
        if(tp == NULL) break;        
        uint8_t val0 = strtol(tp, NULL, 16);
		
	    for(int i=0; i<TRIT8_SIZE; i++) {		
						
			int8_t sel = 0;			 
			if( (val1 & (1<< i)) > 0 ) {
				sel += 2;
			}
			if( (val0 & (1<< i)) > 0 ) {
				sel += 1;
			}			
		
			trit tr = 0; 
			switch( sel ) {
				case 3: tr =  0; break;
				case 2: tr = +1; break;
				case 1: tr = -1; break;
				case 0: tr =  0; break;				
			}
			(dst+idst)->t[i] = tr;					
		}
		idst++;		
		
        tp = strtok(NULL, " ");
    }

    free(buf);
}


void set_cpu_support_trits(void)
{
     printf("TODO add code support cpu trit!\n\r\n\r");
}


int main(int argc, char *argv[]){

	int c;

	int test = 0;
	int convert = 0;
	int dump = 0;
	char *output = "-";
	int ret = 0;  

    static struct cpu_trit cput;
	
	/* Печать версии приложение */
	//print_version(); 
		
	/* Строка приглашение */
	//cli_ascii(); 

    init_cpu(&cput);
			
	//viv+ dbg ---------
    if( flag_inst_trits > 0 ) {            
            trs_dbg_oper(&cput);
            //return 0;
    }

#if 0 //viv+ TODO новый CLI из Emulator  
	
	/*
	 * Loop work CLI and Emu
	 */
	while (1)
	{
		char bufin[1024];

		/*
		 * Командный интерпретатор при работе Emu.
		 */
		fcntl(0, F_SETFL, fcntl(0, F_GETFL) | O_NONBLOCK);

		int numRead = read(0, bufin, 1);
		if (numRead > 0)
		{
			/* Проверить команду CLI */
			Process_ascii_string(bufin[0]);
		}
		else
		{
			/* Работа виртуальной машины */
			Process_Work_Emulation();
		}
	}

	printf("\r\n");
	return 0;

#else //viv- old code 

	    
	//------------------	
    int flag_load_elf = 1, flag_memory_dump = 0, opt;
    while((opt = getopt(argc, argv, "aqmt:d:")) != -1) {
        switch(opt) {
            case 'q':
                flag_quiet = 1;
                break;

            case 'm':
                flag_memory_dump = 1;
                break;

            case 't':
                flag_load_elf = 0;
                
				if( flag_inst_trits > 0 ) {                 
					set_trytes_from_str(cput.inst_rom, optarg, INST_ROM_SIZE);
				}
				else {
					set_bytes_from_str(cput.inst_rom, optarg, INST_ROM_SIZE);
				}					
                break;

            case 'd':
                flag_load_elf = 0;
                
				if( flag_inst_trits > 0 ) {                
					set_trytes_from_str(cput.data_ram, optarg, DATA_RAM_SIZE);
				}
				else {
					set_bytes_from_str(cput.data_ram, optarg, DATA_RAM_SIZE);
				}	
                
                break;

            case 'a':
                flag_inst_trits = 1;
                set_cpu_support_trits();
                break;

            default:
                print_usage_to_exit();
        }
    }
	
	//printf("\n[ Emulator TRIT-RISC-V: T-RV32I ]\n");
	
    if (optind >= argc) print_usage_to_exit();


    int iarg = optind;
    if (flag_load_elf)
        elf_parse(&cput, argv[iarg++]);


    int ncycles = 0;
    if (iarg >= argc) print_usage_to_exit();
    ncycles = atoi(argv[iarg]);


    for(int i=0;i<ncycles;i++){
        
        if( flag_inst_trits > 0 ) {
			
			/* троичные инструкции cpu_trit */
			
			//TODO ~ code 
			tr32 trs_inst;
			trs_rom_read_w(&cput, &trs_inst);                        
			for(int idx=0;trs_inst_list[idx].bit_pattern != NULL;idx++){        
				if(tritpat_match_s(trs_inst, trs_inst_list[idx].bit_pattern)){
					trs_inst_list[idx].func(&cput, trs_inst);
					break;
				}
			}
			
		}
		else {
			uint32_t inst = rom_read_w(&cput);

			for(int idx=0;inst_list[idx].bit_pattern != NULL;idx++){        
				if(bitpat_match_s(inst, inst_list[idx].bit_pattern)){
					inst_list[idx].func(&cput, inst);
					break;
				}                      	
				vlog32("b",inst); //viv+ test
			}

		}        


        print_flags(&cput);
        log_printf("\n");
        

        if (flag_memory_dump){
            dump_memory(stdout, cput.data_ram, DATA_RAM_SIZE);
            printf("\n");
        }
    }

        if( flag_inst_trits>0 ) {
			/* Вывод девятиричном виде  */
			for (int i = 0; i < REGISTER_SIZE; i++){
				tr32 r;
				trs_reg_read(&cput, i, &r);				
			}	
		}
		else {
			for (int i = 0; i < REGISTER_SIZE; i++){
				uint16_t val = reg_read(&cput, i);
				printf("tx%d=%d\n\r", i, val);
			}
		}
		
    puts("");
    
#endif

    return 0;
}
