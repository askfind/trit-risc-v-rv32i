#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "ternary.h"
#include "bit_trit_parser.h"


bool bitpat_match_s(uint32_t t, const char* s){
    int idx = 0;
    int bit_cnt = 0;

    if(s[idx++] != '0'){
        fprintf(stderr, "Invalid pattern\n");
        return false;
    }
    if(s[idx++] != 'b'){
        fprintf(stderr, "Invalid pattern\n");
        return false;
    }

    while(bit_cnt < 32){
        if(s[idx] == '_') {
            idx++;
            continue;
        }
        if(s[idx] == 'x'){
            idx++;
            bit_cnt++;
            continue;
        }
        if(s[idx] == '1'){
            if((t>>(31-bit_cnt))&1) {
                idx++;
                bit_cnt++;
                continue;
            }
            return false;
        }
        else if(s[idx] == '0'){
            if(((t>>(31-bit_cnt))&1) == 0) {
                idx++;
                bit_cnt++;
                continue;
            }
            return false;
        }
    }
    return true;
}

bool tritpat_match_s(tr32 t, const char* s){
    
    int idx = 0;
    int trit_cnt = 0;

    if(s[idx++] != '0'){
        fprintf(stderr, "Invalid pattern\n");
        return false;
    }
    if(s[idx++] != 't'){
        fprintf(stderr, "Invalid pattern\n");
        return false;
    }

    while(trit_cnt < 32){
        if(s[idx] == '_') {
            idx++;
            continue;
        }
        if(s[idx] == 'x'){
            idx++;
            trit_cnt++;
            continue;
        }
        if(s[idx] == '+'){
            if( get_trit32(t,31-trit_cnt) == 1 ) {
                idx++;
                trit_cnt++;
                continue;
            }
            return false;
        }
        else if(s[idx] == '0'){
            if( get_trit32(t,31-trit_cnt) == 0 ) {
                idx++;
                trit_cnt++;
                continue;
            }
            return false;
        }
        else if(s[idx] == '-'){
            if( get_trit32(t,31-trit_cnt) == -1 ) {
                idx++;
                trit_cnt++;
                continue;
            }
            return false;
        }
    }
    return true;
}
