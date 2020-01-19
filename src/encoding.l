%{
#include <iostream>    
#include <cstdlib>
#include <cstring>
#include <cstdio>

#include "ttf.h"
#include "ttmkfdir.h"    
#include "encoding.h"    

#define DEFAULT_SIZE 0x100 /* 8 bit encoding */
#define YY_DECL int yylex (char *current_file, Encodings_t &dest)

static unsigned int line_number = 1;
static inline char *
strip_first (char *s)
{
    char *res = s + std::strcspn (s, " \t");
    return res + std::strspn (res, " \t");
}

static Encoding       *cur_enc; 
static NumericMapping *cur_map;

%}

DIGIT       [[:digit:]]
HEXDIGIT    [[:xdigit:]]
HEXNUM      0x{HEXDIGIT}+
DECNUM      {DIGIT}+
NUMBER      {DECNUM}|{HEXNUM}
STRING      [[:alnum:][:punct:]]+
COMMENT     #.*
LINEEND     {WHITESPACES}*{COMMENT}*\n
WHITESPACES [ \t]+

%s INSIDE_ENC_BLOCK
%s INSIDE_MAP_BLOCK
%s ERROR_STATE
%s INSIDE_UNKNOWN_MAP

%%
STARTENCODING{WHITESPACES}{STRING} {
    cur_enc = new Encoding;
    cur_enc->names.push_back (strip_first (yytext));
    BEGIN(INSIDE_ENC_BLOCK);
}

<INSIDE_ENC_BLOCK>ALIAS{WHITESPACES}{STRING} {
    cur_enc->names.push_back (strip_first (yytext));
}

<INSIDE_ENC_BLOCK>SIZE{WHITESPACES}{NUMBER}({WHITESPACES}{NUMBER})? {
    int i1, i2;
    char *startptr = strip_first (yytext);
    char *endptr;

    i1 = std::strtol (startptr, &endptr, 0);
    startptr = endptr;
    
    i2 = std::strtol (startptr, &endptr, 0);

    cur_enc->size = (startptr == endptr) ? i1: (i1  << 8) + i2;
}

<INSIDE_ENC_BLOCK>FIRSTINDEX{WHITESPACES}{NUMBER}({WHITESPACES}{NUMBER})? {
}

<INSIDE_ENC_BLOCK>STARTMAPPING{WHITESPACES}unicode {
    cur_map = new NumericMapping (cur_enc->size,
				  TT_PLATFORM_MICROSOFT,
				  TT_MS_ID_UNICODE_CS);
    
    cur_enc->enc_size = 0;
    cur_enc->start_code = 0xffff;
    BEGIN(INSIDE_MAP_BLOCK);
}

<INSIDE_ENC_BLOCK>STARTMAPPING{WHITESPACES}postscript {
    BEGIN(INSIDE_UNKNOWN_MAP);
}

<INSIDE_ENC_BLOCK>STARTMAPPING{WHITESPACES}CMAP{WHITESPACES}{NUMBER}{WHITESPACES}{NUMBER} {

    BEGIN(INSIDE_UNKNOWN_MAP);
    /*char *startptr = strip_first (strip_first (yytext));
    char *endptr;
    
    TT_UShort cmap_plat = strtol (startptr, &endptr, 0);
    startptr = endptr;
    TT_UShort cmap_enc = strtol (startptr, &endptr, 0);
    
    cur_map = new NumericMapping (cur_enc->size, cmap_plat, cmap_enc);
    BEGIN(INSIDE_MAP_BLOCK); */
}

<INSIDE_MAP_BLOCK>UNDEFINE{WHITESPACES}{NUMBER}({WHITESPACES}{NUMBER})? {
    char *startptr = strip_first (yytext);
    char *endptr;

    int i1 = std::strtol (startptr, &endptr, 0);
    startptr = endptr;
    
    int i2 = std::strtol (startptr, &endptr, 0);

    if (startptr == endptr) {
	i2 = i1;
    }
    
    /* now mark all the unassigned codes */
    for (long i = i1; i <= i2; i++) {
	(*cur_map)[i] = -1;
    }
}

<INSIDE_MAP_BLOCK>{NUMBER}({WHITESPACES}{NUMBER}){0,2} {
    int numbers[3], i = 0, start_range, end_range, target, res;
    char *startptr;
    char *endptr = yytext;

    for (i = 0;;i++) {
	startptr = endptr;
	res = std::strtol (startptr, &endptr, 0);
	if (startptr != endptr) numbers[i] = res; else break;
    }

    switch (i) {
    case 1:
	start_range = end_range = numbers[0];
	target = -1;
	break;
    case 2:
	start_range = end_range = numbers[0];
	target = numbers[1];
	break;
    case 3:
	start_range = numbers[0];
	end_range   = numbers[1];
	target      = numbers[2];
	break;
    }

    for (i = start_range; i <= end_range; i++, target++) {
	(*cur_map)[i] = target;
    }
    cur_enc->enc_size = cur_enc->enc_size + end_range - start_range + 1;   
    if (start_range < cur_enc->start_code) 
	cur_enc->start_code = start_range;
}


<INSIDE_MAP_BLOCK>ENDMAPPING {
    cur_enc->AddMapping (cur_map);
    dest.insert (std::make_pair(cur_map->cmapkey(), cur_enc));;
    BEGIN(INSIDE_ENC_BLOCK);
}

<INSIDE_UNKNOWN_MAP>ENDMAPPING {
    BEGIN(INSIDE_ENC_BLOCK);
}

<INSIDE_ENC_BLOCK>ENDENCODING {
    BEGIN(INITIAL);
}

{WHITESPACES}

{LINEEND} {
    line_number++;
}

[[:alnum:]]+ {
    if (YY_START != INSIDE_UNKNOWN_MAP) {
	std::cerr << "unexpected token " << yytext << " in file " <<
	    current_file << ", line " << line_number << std::endl;
	switch (YY_START) {
	case INSIDE_MAP_BLOCK:
	    BEGIN(INSIDE_MAP_BLOCK);
	    break;
	case INSIDE_ENC_BLOCK:
	    BEGIN(INSIDE_ENC_BLOCK);
	    break;
	default:
	    BEGIN(ERROR_STATE);
	}
    }
}
%%

int
yywrap (void)
{
    line_number = 1;
    return 1;
}

#ifdef STANDALONE

int
main (int argc, char*argv[])
{
    yyrestart (std::fopen (argv[1], "rb"));

    yylex (argv[1]);

    for (unsigned int i = 0; i < cur_enc->names.size(); i++) {
	std::cout << "name " << i << ": " << cur_enc->names[i] << std::endl;
    }

    std::cout << "size " << cur_enc->size << std::endl;
    
    for (Encoding::MappingMap_t::iterator j = cur_enc->mappings.begin (); j != cur_enc->mappings.end(); j++) {
	NumericMapping *m = j->second;
	
	std::cout << "mapping to cmap table " << m->platform << " " << m->encoding << std::endl;
	
	for (unsigned int i = 0; i < cur_enc->size; i++) {
	    std::printf ("%3d (0x%04x) -> %4d (0x%04x)\n", i, i,
			 (*m)[i], (*m)[i]);
	}
    }

    return 0;
}
#endif
