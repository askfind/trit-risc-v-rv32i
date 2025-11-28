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
#include <string.h>

#include "cpu_trit.h"
#include "ternary.h"
#include "bit_trit_parser.h"
#include "inst.h"
#include "log.h"


/** 
 *  ---------------------------------------------------------------------- 
 *  Функции для совместимости троичных типов данных троичного процессора с
 *  бинарными типами данных в двоичном процессоре.   
 */


int64_t abs_int64( int64_t v) {
	if( v < 0 ) v = 0 - v; 
	return v;
}

/* 
 * Получить бинарное состояние трита равное '0' or '1' для бинарной совместимости памяти.  
 */
int getbool(trit t) {
     if (t == 1) {
        return 1;
     }
     else {
        return 0;
     }
}

/* 
 * Преобразовать число trit8 -> uint8
 */
uint8_t trits_to_uint8( tr8 * ts ) {
		uint8_t r;		
	    for(int i=0; i<TRIT8_SIZE; i++) {		
			if( getbool(ts->t[i]) > 0 ) { 
				r |= (1<<i);
			}
			else {
				r &= ~(1<<i);
			}
		}
		return r;
}

/* 
 * Преобразовать число uint8 -> trit8
 */
void uint8_to_tr8(tr8 * rt, uint8_t v ) {
		
	    for(int i=0; i<TRIT8_SIZE; i++) {		
			if( (v & (1<<i)) > 0 ) { 
				rt->t[i] = 1;
			}
			else {
				rt->t[i] = 0;
			}			
		}		
}

/* 
 * Преобразовать число trit16 -> uint16
 */
uint16_t trits_to_uint16( tr16 * ts ) {
		uint16_t r = 0;		
	    for(int i=0; i<TRIT16_SIZE; i++) {		
			if( getbool(ts->t[i]) > 0 ) { 
				r |= (1<<i);
			}
			else {
				r &= ~(1<<i);
			}
		}
		return r;
}

/* 
 * Преобразовать число uint16 -> trit16
 */
void uint16_to_tr16( tr16 * rt, uint16_t v ) {
				
	    for(int i=0; i<TRIT16_SIZE; i++) {		
			if( (v & (1<<i)) > 0 ) { 
				rt->t[i] = 1;
			}
			else {
				rt->t[i] = 0;
			}			
		}		
}

/* 
 * Преобразовать число uint32 -> trit32
 */
void uint32_to_tr32( tr32 * rt, uint32_t v ) {
				
	    for(int i=0; i<TRIT32_SIZE; i++) {		
			if( (v & (1<<i)) > 0 ) { 
				rt->t[i] = 1;
			}
			else {
				rt->t[i] = 0;
			}			
		}			
}


/** 
 *  ------------------------------------------------------
 *  Функции для троичных типов данных троичного процессора
 */

/*
 * Возведение в степень по модулю 3
 */
int64_t pow3(int16_t x)
{
	int8_t i;
	int64_t r = 1;
	
	if( x == 0) return r;
	
	for (i = 0; i < x; i++)
	{
		r *= 3;
	}
	return r;
}

/**
 * Преобразование троичного целого числа в целое со знаком 3 -> 10
 */
int32_t tr16_to_int32(tr16 tr)
{
	int32_t n = 0; 	
	
	for (int i = 0; i < TRIT16_SIZE ; i++)
	{
		trit x = get_trit16(tr, i);
		if (x != 0)
		{
			n += pow3(i) * x;
		}
	}

	return n;
}  

int64_t tr32_to_int64(tr32 tr)
{
	int64_t n = 0; 	
	for (int i = 0; i < TRIT32_SIZE ; i++)
	{
		trit x = get_trit32(tr, i);
		if (x != 0)
		{
			n += pow3(i) * x;
		}
	}
	return n;
}  

/*
 * Преобразовать число со знаком в трит {-1, 0, +1}
 */
trit sign(int x) {
    return (x > 0) - (x < 0);
}

/* 
 * Получить значение трита как символ {'-',0,'+'}
 */
uint8_t signch(int x) {
    int r = (x > 0) - (x < 0);
    if( r > 0 ) return '+';
    if( r < 0 ) return '-';
    return '0';
}

/* 
 * Получить целое со знаком трита в позиции троичного числа
 */
trit get_trit8(tr8 tr, uint8_t pos)
{
	trit r;	
	
	if( pos > TRIT8_SIZE-1) {				
		r = 0;		
	}
	else {		
		r = tr.t[pos];
	}	

	return r;
} 


/* 
 * Получить целое со знаком трита в позиции троичного числа
 */
trit get_trit16(tr16 tr, uint8_t pos)
{
	trit r;	
	
	if( pos > TRIT16_SIZE-1) {				
		r = 0;		
	}
	else {		
		r = tr.t[pos];
	}	

	return r;
} 

trit get_trit32(tr32 tr, uint8_t pos)
{
	trit r;	
	
	if( pos > TRIT32_SIZE-1) {				
		r = 0;		
	}
	else {		
		r = sign(tr.t[pos]);
	}	

	return r;
} 

/*
 * Установить значение трита в троичном числе.
 */
tr8 set_trit8(tr8 tr, uint8_t pos, trit t)
{
	uint8_t p;
	tr8 res;
	
	if( pos > TRIT8_SIZE-1) {				
		res = tr;
	}
	else {
		res = tr;
		res.t[pos] = t;	
	}
	
	return res;
}


/*
 * Установить значение трита в троичном числе.
 */
tr16 set_trit16(tr16 tr, uint8_t pos, trit t)
{
	uint8_t p;
	tr16 res;
	
	if( pos > TRIT16_SIZE-1) {				
		res = tr;
	}
	else {
		res = tr;
		res.t[pos] = t;	
	}
	
	return res;
}

tr32 set_trit32(tr32 tr, uint8_t pos, trit t)
{
	uint8_t p;
	tr32 res = {0};
	
	if( pos > TRIT32_SIZE-1) {				
		res = tr;
	}
	else {
		res = tr;
		res.t[pos] = t;	
	}
	
	return res;
}

uint8_t symb_nine_form(tr16 v) {
	
	trit t1 = sign(v.t[1]);
	trit t0 = sign(v.t[0]);
	
	switch (3*t1 + t0) {
		case +4: return '4';
		case +3: return '3';
		case +2: return '2';
		case +1: return '1';
		case  0: return '0';
		case -1: return 'Z';
		case -2: return 'Y';
		case -3: return 'X';
		case -4: return 'W';
		default: return ' '; 	
	}
}

/**
 *  Char = {'-','0','+'} to trit
 */
trit char_to_trit(uint8_t c)
{
	switch (c) {
		case '+': return +1;
		case '0': return  0;
		case '-': return -1;
		default:  return  0; 	
	}
}

void tr8_to_nine_string(uint8_t * buf,tr8 v) {
	
	uint8_t index = 0;	
	tr16 t;	

	for(int i=0;i<4;i++) {
		t.t[0] = get_trit8(v,index++);
		t.t[1] = get_trit8(v,index++);
		buf[i] = symb_nine_form(t);
	}	
}

/**
 * Cтроку вида {'-','0','+'} в троичное число tr16
 */
tr8 smtr8(uint8_t *s)
{
	int8_t i;
	trit v;
	int8_t len;
	tr8 t;

	len = strlen(s);
	len = min(len, TRIT8_SIZE);
	for (i=0; i<len; i++)
	{
		v = char_to_trit(*(s + len - i - 1));
		t = set_trit8(t, i, v);
	}
	return t;
} 


/**
 * Cтроку вида {'-','0','+'} в троичное число tr16
 */
tr16 smtr16(uint8_t *s)
{
	int8_t i;
	trit v;
	int8_t len;
	tr16 t;

	len = strlen(s);
	len = min(len, TRIT16_SIZE);
	for (i=0; i<len; i++)
	{
		v = char_to_trit(*(s + len - i - 1));
		t = set_trit16(t, i, v);
	}
	return t;
} 

void tr16_to_nine_string(uint8_t * buf,tr16 v) {
	
	uint8_t index = 0;	
	tr16 t;	

	for(int i=0;i<8;i++) {
		t.t[0] = get_trit16(v,index++);
		t.t[1] = get_trit16(v,index++);
		buf[i] = symb_nine_form(t);
	}	
}

/**
 * Cтроку вида {'-','0','+'} в троичное число tr16
 */
tr32 smtr32(uint8_t *s)
{
	int8_t i;
	trit v;
	int8_t len;
	tr32 t;

	len = strlen(s);
	len = min(len, TRIT32_SIZE);
	for (i=0; i<len; i++)
	{
		v = char_to_trit(*(s + len - i - 1));
		t = set_trit32(t, i, v);
	}
	return t;
} 

void tr32_to_nine_string(uint8_t *buf, tr32 v) {
	
	uint8_t index = 0;	
	tr16 t;	

	for(int i=0;i<16;i++) {
		t.t[0] = get_trit32(v,index++);
		t.t[1] = get_trit32(v,index++);
		buf[15-i] = symb_nine_form(t);		
	}	
}
 
/*
 * Очистить поле битов троичного числа
 */
void clear_tr8(tr8 *tr)
{
	for(int i=0; i<TRIT8_SIZE; i++) {
		tr->t[i] = 0;
	}
}  
 
void clear_tr16(tr16 *tr)
{
	for(int i=0; i<TRIT16_SIZE; i++) {
		tr->t[i] = 0;
	}
}  

void clear_tr32(tr32 *tr)
{
	for(int i=0; i<TRIT32_SIZE; i++) {
		tr->t[i] = 0;
	}
}  

/*
 * Получить трит в младшей позиции троичного числа v.t[0]
 */
trit tr16pt0_to_trit(tr16 v)
{
	return sign(v.t[0]);
}

/*
 *   Операции с тритами
 *   trit = {-1,0,1}  - трит значение трита
 *   Троичное сложение двух тритов с переносом
 */
void sum_t(trit *a, trit *b, trit *p0, trit *s, trit *p1)
{
	*a = sign(*a);
	*b = sign(*b);
	*p0 = sign(*p0);
	*s = sign(*s);
	*p1 = sign(*p1);
		
	switch (*a + *b + *p0)
	{
	case -3:
	{
		*s = 0;
		*p1 = -1;
	}
	break;
	case -2:
	{
		*s = 1;
		*p1 = -1;
	}
	break;
	case -1:
	{
		*s = -1;
		*p1 = 0;
	}
	break;
	case 0:
	{
		*s = 0;
		*p1 = 0;
	}
	break;
	case 1:
	{
		*s = 1;
		*p1 = 0;
	}
	break;
	case 2:
	{
		*s = -1;
		*p1 = 1;
	}
	break;
	case 3:
	{
		*s = 0;
		*p1 = 1;
	}
	break;
	default:
	{
		*s = 0;
		*p1 = 0;
	}
	break;
	}
}  

/* 
 * Троичное умножение тритов
 */
void and_t(trit *a, trit *b, trit *s)
{
	*a = sign(*a);
	*b = sign(*b);
	*s = sign(*s);
		
	if ((*a * *b) > 0)
	{
		*s = 1;
	}
	else if ((*a * *b) < 0)
	{
		*s = -1;
	}
	else
	{
		*s = 0;
	}
}   

/*
 *  Троичное отрицание трита
 */
void not_t(trit *a, trit *s)
{
	trit tr = sign(*a);
	
	if (tr > 0)
	{
		*s = -1;
	}
	else if (tr < 0)
	{
		*s = 1;
	}
	else
	{
		*s = 0;
	}
} 

/*
 * Троичное xor тритов
 */
void xor_t(trit *a, trit *b, trit *s)
{
	*a = sign(*a);
	*b = sign(*b);
	
	if (*a == -1 && *b == -1)
	{
		*s = -1;
	}
	else if (*a == 1 && *b == -1)
	{
		*s = 1;
	}
	else if (*a == -1 && *b == 1)
	{
		*s = 1;
	}
	else if (*a == 1 && *b == 1)
	{
		*s = -1;
	}
	else if (*a == 0 && *b == 1)
	{
		*s = 0;
	}
	else if (*a == 0 && *b == -1)
	{
		*s = 0;
	}
	else if (*a == 1 && *b == 0)
	{
		*s = 0;
	}
	else if (*a == -1 && *b == 0)
	{
		*s = 0;
	}
	else
	{
		*s = 0;
	}
}  

/* 
 * Троичное or тритов
 * OR (A, B)
 *   A ∨ B
 *       B
 *	  - 0 +
 *   - - 0 +
 * A	0 0	0 +
 *   + + + +
 */
void or_t(trit *a, trit *b, trit *s)
{
	*a = sign(*a);
	*b = sign(*b);
	
	if (*a == -1 && *b == -1)
	{
		*s = -1;
	}
	else if (*a == 1 && *b == -1)
	{
		*s = 1;
	}
	else if (*a == -1 && *b == 1)
	{
		*s = 1;
	}
	else if (*a == 1 && *b == 1)
	{
		*s = 1;
	}
	else if (*a == 0 && *b == 1)
	{
		*s = 1;
	}
	else if (*a == 0 && *b == -1)
	{
		*s = -1;
	}
	else if (*a == 1 && *b == 0)
	{
		*s = 1;
	}
	else if (*a == -1 && *b == 0)
	{
		*s = 0;
	}
	else
	{
		*s = 0;
	}
}


/*
 * Троичное ADD сложение тритов
 */
//TODO info RISC-V bits
tr32 add_trs(tr32 x, tr32 y)
{
	int8_t i, j;
	trit a, b, s, p0, p1;
	tr32 r;

	/* Результат */
	uint8_t m = 0;

	j = TRIT32_SIZE;

	p0 = 0;
	p1 = 0;

	for (i = 0; i < j; i++)
	{
		a = get_trit32(x, i);
		b = get_trit32(y, i);
		sum_t(&a, &b, &p0, &s, &p1);

		if (s > 0)
		{
			r.t[m] = +1;			
		}
		else if (s < 0)
		{
			r.t[m] = -1;			
		}
		else
		{
			r.t[m] = 0;			
		}
		m += 1;
		p0 = p1;
		p1 = 0;
	}

	return r;
}

/* 
 * Троичное вычитание тритов
 */
tr32 sub_trs(tr32 x, tr32 y)
{
	uint8_t i, j;
	trit a, b, s, p0, p1;
	tr32 r;
	
	uint8_t m = 0;
	j = TRIT32_SIZE;

	p0 = 0;
	p1 = 0;

	for (i = 0; i < j; i++)
	{
		a = get_trit32(x, i);
		b = get_trit32(y, i);
		b = -b;
		sum_t(&a, &b, &p0, &s, &p1);

		if (s > 0)
		{
			r.t[m] = +1;
		}
		else if (s < 0)
		{
			r.t[m] = -1;
		}
		else
		{
			r.t[m] = 0;
		}
		m += 1;
		p0 = p1;
		p1 = 0;
	}
	
	return r;
}

/*
 * Троичная операция OR тритов 
 */
tr32 or_trs(tr32 x, tr32 y)
{
	tr32 r;
	uint8_t i, j;
	trit a, b, s;

	j = TRIT32_SIZE;

	for (i = 0; i < j; i++)
	{
		a = get_trit32(x, i);
		b = get_trit32(y, i);
		or_t(&a, &b, &s);
		r = set_trit32(x, i, s);
	}
    
	return r;
}  


/* 
 * Операции AND троичная
 */
tr32 and_trs(tr32 x, tr32 y)
{
	tr32 r;

	uint8_t i, j;
	trit a, b, s;

	j = TRIT32_SIZE;

	for (i = 0; i < j; i++)
	{
		a = get_trit32(x, i);
		b = get_trit32(y, i);
		and_t(&a, &b, &s);
		r = set_trit32(x, i, s);
	}
	
	return r;
}

/* 
 * Операции XOR trs
 */
tr32 xor_trs(tr32 x, tr32 y)
{
	tr32 r;

	int8_t i, j, ll;
	trit a, b, s;

	j = TRIT32_SIZE;

	for (i = 0; i < j; i++)
	{
		a = get_trit32(x, i);
		b = get_trit32(y, i);
		xor_t(&a, &b, &s);
		r = set_trit32(x, i, s);
	}
	
	return r;
}

/* 
 * Операция NOT троичная
 */
tr32 not_trs(tr32 x)
{	
	int8_t i,j;
	tr32 r;
	
	j = TRIT32_SIZE;
	for (i = 0; i < j; i++)
	{
		r.t[i] = -x.t[i];
	}	
	return r;
}

/* 
 * Троичное NEG отрицания тритов
 */
tr32 neg_trs(tr32 t)
{
	return not_trs(t);
}

#if 1 //TODO переписать для нового типа данных как массив тритов
	//TODO add code
#endif

/*
 *  Печать при отладке бинарных данных
 */
void vlog8( unsigned char * buf, uint8_t v ) {
		//log_printf("\n\r");
		log_printf("( %s : ",buf);
   		log_printf("x%02x",v);
   		log_printf(" [%d]",v);				
     	log_printf(" )\n\r");     	
} 
void vlog16( unsigned char * buf, uint16_t v ) {
		//log_printf("\n\r");
		log_printf("( %s : ",buf);
   		log_printf("x%04x",v);
   		log_printf(" [%d]",v);				
     	log_printf(" )\n\r");     	
}
void vlog32( unsigned char * buf, uint32_t v ) {
		//log_printf("\n\r");
		log_printf("( %s : ",buf);
   		log_printf("0x%04x%04x",v>>16, v & 0xFFFF);   		
   		log_printf(" [%d]",v);				
     	log_printf(" )\n\r");     	
}
void vlog64( unsigned char * buf, int64_t v ) {
		//log_printf("\n\r");
		log_printf("( %s : ",buf);
   		log_printf("0x%08x",(uint32_t)v);   		
   		log_printf(" [%lld]",v);				
     	log_printf(" )\n\r");     	
}

/* 
 * Печать при отладке троичных данных
 */
void tlog8( unsigned char * buf, tr8 ts ) {
		log_printf("[ %s : t",buf);
   		for(int j=0;j<8;j++) { 
			log_printf("%c",signch( ts.t[7-j]));
		};
     	log_printf(" ]\n\r");
}
void tlog16( unsigned char * buf, tr16 ts ) {
		log_printf("[ %s : t",buf);
   		for(int j=0;j<16;j++) { 
			log_printf("%c",signch( ts.t[15-j]));
		};
     	log_printf(" ]\n\r");
}
void tlog32( unsigned char * buf, tr32 ts ) {
		log_printf("[ %s : t",buf);
   		for(int j=0;j<32;j++) { 
			log_printf("%c",signch( ts.t[31-j]));
		};
     	log_printf(" ]\n\r");
}

void buflog4( uint8_t * buf ) {		
   		for(int j=0;j<4;j++) { 
			printf("%c",buf[j]);
		};     	
}

void buflog8( uint8_t * buf ) {		
   		for(int j=0;j<8;j++) { 
			printf("%c",buf[j]);
		};     	
}

void buflog16( uint8_t * buf ) {		
   		for(int j=0;j<16;j++) { 
			printf("%c",buf[j]);
		};     	
}

void memory_read(struct cpu_trit *c, tr32 addr_trit, tr8 * data) {

    int64_t addr_binary = tr32_to_int64(addr_trit) ;
	
    if (addr_binary >= ROM_BASE && addr_binary < ROM_BASE + ROM_SIZE) {
        
        for(int i=0;i<TRIT8_SIZE;i++) {  
			data->t[i] = c->inst_rom[addr_binary - ROM_BASE].t[i];
		}
		return;        
    }
    if (addr_binary >= RAM_BASE && addr_binary < RAM_BASE + RAM_SIZE) {
        
        for(int i=0;i<TRIT8_SIZE;i++) {  
			data->t[i] = c->data_ram[addr_binary - RAM_BASE].t[i];  // Чтение из RAM
		}
		return;
    }
    //viv+ TODO add code next version
    //else if (addr_binary >= MMIO_BASE && addr_binary < MMIO_BASE + MMIO_SIZE) {
    //    return mmio_read(addr_binary);  // Чтение из MMIO
    //}
    for(int i=0;i<TRIT8_SIZE;i++) {  
			data->t[i] = 0;  // Чтение из RAM
	}

    return;  // Invalid address
}

int8_t memory_write(struct cpu_trit *c, tr32 addr_trit, tr8 data) {

    int64_t addr_binary = tr32_to_int64(addr_trit) ;
	
	if( addr_binary < 0 ) {
		return 1;
	}
	
	uint32_t diff = (uint32_t)(addr_binary - (int64_t)RAM_BASE);
	
    if( diff < RAM_SIZE-1)
    {

        for(int i=0;i<TRIT8_SIZE;i++) {
                    trit d = data.t[i];
                    c->data_ram[addr_binary - RAM_BASE].t[i] = d;  // Чтение из RAM
        }
        return 0;
    }
    //viv+ TODO add code next version
    //else if (addr_binary >= MMIO_BASE && addr_binary < MMIO_BASE + MMIO_SIZE) {
    //    return mmio_read(addr_binary);  // Чтение из MMIO
    //}
    return 1;
}


/* EOF ternary.c */
