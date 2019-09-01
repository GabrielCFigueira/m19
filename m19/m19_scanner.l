%option c++ prefix="m19_scanner_" outfile="m19_scanner.cpp"
%option stack noyywrap yylineno 8bit
%{ 
// make relevant includes before including the parser's tab file
#include <string>
#include <cmath>
#include <limits>
#include <cdk/ast/sequence_node.h>
#include <cdk/ast/expression_node.h>
#include <cdk/ast/lvalue_node.h>
#include "m19_parser.tab.h"

static int count_lt = 0;
static int comment_flag = 0;

// don't change this
#define yyerror LexerError

inline double convert_to_real(std::string s) {
    std::map<char, int> exprs({{'.', 0}, {'a', -18}, {'f', -15},
    {'p', -12}, {'n', -9}, {'u', -6}, {'m', -3}, {'k', 3}, {'M', 6},
    {'G', 9}, {'T', 12}, {'P', 15}, {'E', 18}});
    double return_number = 0;
  	for(auto el: exprs){
          size_t pos = s.find(el.first);
          if(pos != std::string::npos){
            const char * number = s.replace(pos, 1, ".").c_str();
			return_number = strtod(number, nullptr) * pow(10, el.second);
            if (errno == ERANGE) throw std::out_of_range(std::to_string(errno));
            break;
          }
    }
    return return_number;
}
%}

DECIMAL                             [0-9]+
HEXACONTROL                         [0-9a-fA-F]
HEXADECIMAL                         0x{HEXACONTROL}+
REAL                                {DECIMAL}[.afpnumkMGTPE]{DECIMAL}
TYPE                                [#%$]
SPACE                               [ \t\n]

%x X_STRING X_COMMENT X_BACKSLASH X_IGNORE X_LT X_GT
%%

    {set_debug(1);}

"//".*\n                            ;  /*IGNORE COMMENTS*/

">="                                return tGE; 
"<="                                return tLE; 
"=="                                return tEQ; 
"!="                                return tNE;
"&&"                                return tAND;
"||"                                return tOR;
"~"                                 return tNOT;

":"                                 return tELSE;

">>@"                               return tBREAK;
"@>>"                               return tCONTINUE;
">@<"                               return tRETURN;

">>"                                return tFINALSEC;
"!!"                                return tPRINT_LINE;

"<"                                 count_lt++; yy_push_state(X_LT);
<X_LT>"<"                           count_lt++;
<X_LT>{TYPE}                        yy_pop_state(); yy_push_state(X_GT); return *yytext;
<X_LT,X_GT>"/*"                     comment_flag = 1; yy_push_state(X_COMMENT);
<X_LT,X_GT>"//".*\n                 comment_flag = 1;
<X_GT>">"                           if(count_lt-- == 1) { comment_flag = 0; yy_pop_state(); } return tPOINTER;
<X_LT,X_GT>{SPACE}                  ;
<X_GT>.                             {
                                            std::string error = "Unexpected character at line " + std::to_string(yylineno);
                                            yyerror(error.c_str());
                                    }
<X_LT>.                             {
                                        if (count_lt == 2 && comment_flag == 0) {
                                            yy_pop_state();
                                            yyless(yyleng-1);
                                            count_lt = 0;
                                            return tINITIALSEC;
                                        }
                                        else if (count_lt == 1) {
                                            yy_pop_state();
                                            comment_flag = 0;
                                            yyless(yyleng-1);
                                            count_lt = 0;
                                            return '<';
                                        }
                                        else {
                                            std::string error = "Unexpected character at line " + std::to_string(yylineno);
                                            yyerror(error.c_str());
                                        }
                                    }


[A-Za-z][A-Za-z0-9_]*               yylval.s = new std::string(yytext); return tIDENTIFIER;



\"                                  yy_push_state(X_STRING); yylval.s = new std::string("");
<X_STRING>\\			            yy_push_state(X_BACKSLASH);
<X_STRING,X_IGNORE>\"               yy_pop_state(); return tSTRING;
<X_STRING>.                         *yylval.s += yytext;

<X_BACKSLASH>0 		                yy_pop_state(); yy_pop_state(); yy_push_state(X_IGNORE);
<X_BACKSLASH>n 		                *yylval.s += "\n"; yy_pop_state();
<X_BACKSLASH>r 		                *yylval.s += "\r"; yy_pop_state();
<X_BACKSLASH>t 		                *yylval.s += "\t"; yy_pop_state();
<X_BACKSLASH>\" 		            *yylval.s += "\""; yy_pop_state();
<X_BACKSLASH>\\ 		            *yylval.s += "\\"; yy_pop_state();
<X_BACKSLASH>{HEXACONTROL}{1,2}     *yylval.s += (char)std::stoi(yytext, NULL, 16); yy_pop_state();
<X_BACKSLASH>. 		                *yylval.s += yytext; yy_pop_state();

<X_IGNORE>.                         ;


"/*"                                yy_push_state(X_COMMENT);
<X_COMMENT>"/*"                     yy_push_state(X_COMMENT);
<X_COMMENT>"*/"                     yy_pop_state();
<X_COMMENT>.|\n                     ;



{DECIMAL}                           {   
                                        try{
                                            yylval.i = std::stoi(yytext, nullptr, 10);
                                            return tINTEGER;
                                        } catch(std::out_of_range& e){
                                            std::string error = "Overflow at line " + std::to_string(yylineno);
                                            yyerror(error.c_str());
                                        }
                                    }

{HEXADECIMAL}                       {
                                        try{
                                            yylval.i = std::stoi(yytext, nullptr, 16);
                                            return tINTEGER;
                                        } catch(std::out_of_range& e){
                                            std::string error = "Overflow at line " + std::to_string(yylineno);
                                            yyerror(error.c_str());
                                        }
                                    }
                                    
{REAL}                              {
                                        try {
                                            double number=convert_to_real(yytext);
                                            yylval.d = number; return tREAL; 
                                        } catch(std::out_of_range& e) {
                                            std::string error = "Overflow at line " + std::to_string(yylineno);
                                            yyerror(error.c_str());
                                        }
                                    }

[-()<>=+*/%;{},!?\[\]#$@]           return *yytext;

{SPACE}+                            ; /* ignore whitespace */

.                                   {
                                        std::string error = "Unknown character at line " + std::to_string(yylineno);
                                        yyerror(error.c_str());
                                    }

%%
