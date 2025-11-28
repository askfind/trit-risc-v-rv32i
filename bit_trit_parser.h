#ifndef BIT_TRIT_PARSER_H
#define BIT_TRIT_PARSER_H

#include <stdbool.h>
#include <stdint.h>
#include "ternary.h"

bool bitpat_match_s(uint32_t t, const char* s);
bool tritpat_match_s(tr32 t, const char* s);

#endif /* BIT_TRIT_PARSER_H */
