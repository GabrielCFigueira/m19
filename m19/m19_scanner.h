#ifndef __M19SCANNER_H__
#define __M19SCANNER_H__

#undef yyFlexLexer
#define yyFlexLexer m19_scanner_FlexLexer
#include <FlexLexer.h>

typedef m19_scanner_FlexLexer m19_scanner;

#endif
