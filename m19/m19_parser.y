%{
//-- don't change *any* of these: if you do, you'll break the compiler.
#include <cdk/compiler.h>
#include "ast/all.h"
#define LINE               compiler->scanner()->lineno()
#define yylex()            compiler->scanner()->scan()
#define yyerror(s)         compiler->scanner()->error(s)
#define YYPARSE_PARAM_TYPE std::shared_ptr<cdk::compiler>
#define YYPARSE_PARAM      compiler
//-- don't change *any* of these --- END!
%}

%union {
  int                   i;	/* integer value */
  double                d;  /* real value */
  std::string          *s;	/* symbol name or string literal */
  cdk::basic_node      *node;	/* node pointer */
  cdk::sequence_node   *sequence;
  cdk::expression_node *expression; /* expression nodes */
  cdk::lvalue_node     *lvalue;
  basic_type           *type;

  m19::block_node       *block;
};

%token <i> tINTEGER
%token <s> tIDENTIFIER tSTRING
%token <d> tREAL
%token tPRINT_LINE tPOINTER tINITIALSEC tFINALSEC

%token tBREAK tCONTINUE tRETURN tINT
%token tAND tOR tNOT

%nonassoc tIFX
%nonassoc tELSE

%right '='
%left tOR
%left tAND
%nonassoc tNOT
%left tEQ tNE
%left tLE tGE '>' '<'
%left '+' '-'
%left '*' '/' '%'
%nonassoc tUNARY

%type <node> instruction program body section declaration vardecl fundef fundecl normalvardecl initialsection finalsection
%type <sequence> instructions expressions sections declarations normalvardecls listvardecls
%type <expression> expr funcall literal
%type <lvalue> lval
%type <type> type exclamation
%type <block> block
%type <s> string


%{
//-- The rules below will be included in yyparse, the main parsing function.
%}
%%

program         : declarations                                                          { compiler->ast($$ = new cdk::sequence_node(LINE, $1)); }
                ;

declarations    :              declaration                                              { $$ = new cdk::sequence_node(LINE, $1); }
                | declarations declaration                                              { $$ = new cdk::sequence_node(LINE, $2, $1); }
                ;

declaration     : vardecl ';'                                                           { $$ = $1; }
                | fundecl                                                               { $$ = $1; }
                | fundef                                                                { $$ = $1; }
                ;

normalvardecls  :                    normalvardecl                                      { $$ = new cdk::sequence_node(LINE, $1); }
                | normalvardecls ',' normalvardecl                                      { $$ = new cdk::sequence_node(LINE, $3, $1); }
                ;

normalvardecl   : type tIDENTIFIER                                                      { $$ = new m19::variable_declaration_node(LINE, 0, $1, *$2, NULL); delete $2; } 
                | type tIDENTIFIER '=' expr                                             { $$ = new m19::variable_declaration_node(LINE, 0, $1, *$2, $4); delete $2; }
                ;

listvardecls    :              normalvardecl ';'                                        { $$ = new cdk::sequence_node(LINE, $1); }
                | listvardecls normalvardecl ';'                                        { $$ = new cdk::sequence_node(LINE, $2, $1); }
                ;

vardecl         : type tIDENTIFIER                                                      { $$ = new m19::variable_declaration_node(LINE, 0, $1, *$2, NULL); delete $2; }
                | type tIDENTIFIER '!'                                                  { $$ = new m19::variable_declaration_node(LINE, 1, $1, *$2, NULL); delete $2; }
                | type tIDENTIFIER '?'                                                  { $$ = new m19::variable_declaration_node(LINE, 2, $1, *$2, NULL); delete $2; }
                | type tIDENTIFIER     '=' literal                                      { $$ = new m19::variable_declaration_node(LINE, 0, $1, *$2, $4); delete $2; }                
                | type tIDENTIFIER '!' '=' literal                                      { $$ = new m19::variable_declaration_node(LINE, 1, $1, *$2, $5); delete $2; }
                ;

type            : '#'                                                                   { $$ = new basic_type(4, basic_type::TYPE_INT); }
                | '%'                                                                   { $$ = new basic_type(8, basic_type::TYPE_DOUBLE); }
                | '$'                                                                   { $$ = new basic_type(4, basic_type::TYPE_STRING); }
                | type tPOINTER                                                         { $$ = new basic_type(4, basic_type::TYPE_POINTER); $$->_subtype = $1; }
                ;

exclamation     : '!'                                                                   { $$ = new basic_type(0, basic_type::TYPE_VOID); }
                ;

fundecl         : type        tIDENTIFIER     '(' normalvardecls ')'                    { $$ = new m19::function_declaration_node(LINE, 0, $1, *$2, $4); delete $2; }
                | exclamation tIDENTIFIER     '(' normalvardecls ')'                    { $$ = new m19::function_declaration_node(LINE, 0, $1, *$2, $4); delete $2; }
                | type        tIDENTIFIER '!' '(' normalvardecls ')'                    { $$ = new m19::function_declaration_node(LINE, 1, $1, *$2, $5); delete $2; }
                | exclamation tIDENTIFIER '!' '(' normalvardecls ')'                    { $$ = new m19::function_declaration_node(LINE, 1, $1, *$2, $5); delete $2; }
                | type        tIDENTIFIER '?' '(' normalvardecls ')'                    { $$ = new m19::function_declaration_node(LINE, 2, $1, *$2, $5); delete $2; }
                | exclamation tIDENTIFIER '?' '(' normalvardecls ')'                    { $$ = new m19::function_declaration_node(LINE, 2, $1, *$2, $5); delete $2; }
                | type        tIDENTIFIER     '('                ')'                    { $$ = new m19::function_declaration_node(LINE, 0, $1, *$2, NULL); delete $2; }
                | exclamation tIDENTIFIER     '('                ')'                    { $$ = new m19::function_declaration_node(LINE, 0, $1, *$2, NULL); delete $2; }
                | type        tIDENTIFIER '!' '('                ')'                    { $$ = new m19::function_declaration_node(LINE, 1, $1, *$2, NULL); delete $2; }
                | exclamation tIDENTIFIER '!' '('                ')'                    { $$ = new m19::function_declaration_node(LINE, 1, $1, *$2, NULL); delete $2; }
                | type        tIDENTIFIER '?' '('                ')'                    { $$ = new m19::function_declaration_node(LINE, 2, $1, *$2, NULL); delete $2; }
                | exclamation tIDENTIFIER '?' '('                ')'                    { $$ = new m19::function_declaration_node(LINE, 2, $1, *$2, NULL); delete $2; }          
                ;

fundef          : type        tIDENTIFIER     '(' normalvardecls ')' '=' literal body   { $$ = new m19::function_definition_node(LINE, 0, $1, *$2, $4, $7, $8); delete $2; }
                | type        tIDENTIFIER     '(' normalvardecls ')'             body   { $$ = new m19::function_definition_node(LINE, 0, $1, *$2, $4, $6); delete $2; }
                | exclamation tIDENTIFIER     '(' normalvardecls ')'             body   { $$ = new m19::function_definition_node(LINE, 0, $1, *$2, $4, $6); delete $2; }
                | type        tIDENTIFIER '!' '(' normalvardecls ')' '=' literal body   { $$ = new m19::function_definition_node(LINE, 1, $1, *$2, $5, $8, $9); delete $2; }
                | type        tIDENTIFIER '!' '(' normalvardecls ')'             body   { $$ = new m19::function_definition_node(LINE, 1, $1, *$2, $5, $7); delete $2; }
                | exclamation tIDENTIFIER '!' '(' normalvardecls ')'             body   { $$ = new m19::function_definition_node(LINE, 1, $1, *$2, $5, $7); delete $2; }
                | type        tIDENTIFIER     '('                ')' '=' literal body   { $$ = new m19::function_definition_node(LINE, 0, $1, *$2, NULL, $6, $7); delete $2; }
                | type        tIDENTIFIER     '('                ')'             body   { $$ = new m19::function_definition_node(LINE, 0, $1, *$2, NULL, $5); delete $2; }
                | exclamation tIDENTIFIER     '('                ')'             body   { $$ = new m19::function_definition_node(LINE, 0, $1, *$2, NULL, $5); delete $2; }
                | type        tIDENTIFIER '!' '('                ')' '=' literal body   { $$ = new m19::function_definition_node(LINE, 1, $1, *$2, NULL, $7, $8); delete $2; }
                | type        tIDENTIFIER '!' '('                ')'             body   { $$ = new m19::function_definition_node(LINE, 1, $1, *$2, NULL, $6); delete $2; }
                | exclamation tIDENTIFIER '!' '('                ')'             body   { $$ = new m19::function_definition_node(LINE, 1, $1, *$2, NULL, $6); delete $2; }
                ;

literal         : tINTEGER                                                              { $$ = new cdk::integer_node(LINE, $1); }
	              | string                                                                { $$ = new cdk::string_node(LINE, $1); }
                | tREAL                                                                 { $$ = new cdk::double_node(LINE, $1); }
                ;

body            : initialsection sections finalsection                                  { $$ = new m19::function_body_node(LINE, $1, $3, $2); }
                | initialsection          finalsection                                  { $$ = new m19::function_body_node(LINE, $1, $2, NULL); }
                | initialsection sections                                               { $$ = new m19::function_body_node(LINE, $1, NULL, $2); }
                |                sections finalsection                                  { $$ = new m19::function_body_node(LINE, NULL, $2, $1); }
                |                sections                                               { $$ = new m19::function_body_node(LINE, NULL, NULL, $1); }
                | initialsection                                                        { $$ = new m19::function_body_node(LINE, $1, NULL, NULL); }
                |                         finalsection                                  { $$ = new m19::function_body_node(LINE, NULL, $1, NULL); }
                ;

initialsection  : tINITIALSEC block                                                     { $$ = new m19::initial_section_node(LINE, $2); }
                ;

finalsection    : tFINALSEC block                                                       { $$ = new m19::final_section_node(LINE, $2); }
                ;

sections        :          section                                                      { $$ = new cdk::sequence_node(LINE, $1); }
                | sections section                                                      { $$ = new cdk::sequence_node(LINE, $2, $1); }
                ;

section         :              block                                                    { $$ = new m19::section_node(LINE, false, $1); }
                | '['      ']' block                                                    { $$ = new m19::section_node(LINE, true, $3); }
                | '('      ')' block                                                    { $$ = new m19::section_node(LINE, false, $3); }
                | '[' expr ']' block                                                    { $$ = new m19::section_node(LINE, true, $4, $2); }
                | '(' expr ')' block                                                    { $$ = new m19::section_node(LINE, false, $4, $2); }
                ;

block           : '{' listvardecls              '}'                                     { $$ = new m19::block_node(LINE, $2, NULL); }
                | '{'              instructions '}'                                     { $$ = new m19::block_node(LINE, NULL, $2); }
                | '{' listvardecls instructions '}'                                     { $$ = new m19::block_node(LINE, $2, $3); }
                | '{'                           '}'                                     { $$ = new m19::block_node(LINE, NULL, NULL); }
                ;

instructions    :              instruction                                              { $$ = new cdk::sequence_node(LINE, $1); }
	              | instructions instruction                                              { $$ = new cdk::sequence_node(LINE, $2, $1); }
	              ;

instruction     : expr ';'                                                              { $$ = new m19::evaluation_node(LINE, $1); }
 	              | expr '!'                                                              { $$ = new m19::print_node(LINE, $1, false); }
                | expr tPRINT_LINE                                                      { $$ = new m19::print_node(LINE, $1, true); }
                | '[' expressions ';' expressions ';' expressions ']' instruction       { $$ = new m19::for_node(LINE, $2, $4, $6, $8); }
                | '[' normalvardecls ';' expressions ';' expressions ']' instruction    { $$ = new m19::for_node(LINE, $2, $4, $6, $8); }
                | '[' expr ']' '#' instruction %prec tIFX                               { $$ = new m19::if_node(LINE, $2, $5); }
                | '[' expr ']' '?' instruction %prec tIFX                               { $$ = new m19::if_node(LINE, $2, $5); }
                | '[' expr ']' '?' instruction tELSE instruction                        { $$ = new m19::if_else_node(LINE, $2, $5, $7); }
                | tBREAK                                                                { $$ = new m19::break_node(LINE); }  //FIXME
                | tCONTINUE                                                             { $$ = new m19::continue_node(LINE); }
                | tRETURN                                                               { $$ = new m19::return_node(LINE); }
                | block                                                                 { $$ = $1; }
                ;

expressions      : /*empty*/                                                            { $$ = new cdk::sequence_node(LINE); }
                | expr                                                                  { $$ = new cdk::sequence_node(LINE, $1); }
                | expressions ',' expr                                                  { $$ = new cdk::sequence_node(LINE, $3, $1); }
                ;

expr            : literal                                                               { $$ = $1; }
                | '-' expr %prec tUNARY                                                 { $$ = new cdk::neg_node(LINE, $2); }
                | '+' expr %prec tUNARY                                                 { $$ = new m19::identity_node(LINE, $2); }
                | tNOT expr %prec tUNARY                                                { $$ = new cdk::not_node(LINE, $2); }                
                | expr '+' expr	                                                        { $$ = new cdk::add_node(LINE, $1, $3); }
                | expr '-' expr	                                                        { $$ = new cdk::sub_node(LINE, $1, $3); }
                | expr '*' expr	                                                        { $$ = new cdk::mul_node(LINE, $1, $3); }
                | expr '/' expr	                                                        { $$ = new cdk::div_node(LINE, $1, $3); }
                | expr '%' expr	                                                        { $$ = new cdk::mod_node(LINE, $1, $3); }
                | expr '<' expr	                                                        { $$ = new cdk::lt_node(LINE, $1, $3); }
                | expr '>' expr	                                                        { $$ = new cdk::gt_node(LINE, $1, $3); }
                | expr tGE expr	                                                        { $$ = new cdk::ge_node(LINE, $1, $3); }
                | expr tLE expr                                                         { $$ = new cdk::le_node(LINE, $1, $3); }
                | expr tNE expr	                                                        { $$ = new cdk::ne_node(LINE, $1, $3); }
                | expr tEQ expr	                                                        { $$ = new cdk::eq_node(LINE, $1, $3); }
                | expr tAND expr	                                                      { $$ = new cdk::and_node(LINE, $1, $3); }
                | expr tOR expr	                                                        { $$ = new cdk::or_node(LINE, $1, $3); }
                | '(' expr ')'                                                          { $$ = $2; }
                | lval                                                                  { $$ = new cdk::rvalue_node(LINE, $1); }  //FIXME
                | lval '=' expr                                                         { $$ = new cdk::assignment_node(LINE, $1, $3); }
                | lval '?'                                                              { $$ = new m19::address_node(LINE, $1); }
                | '@'                                                                   { $$ = new m19::read_node(LINE); }
                | '@' '=' expr                                                          { $$ = new cdk::assignment_node(LINE, new cdk::variable_node(LINE, new std::string("@")), $3); }
                | '@' '[' expr ']' '=' expr                                             { $$ = new cdk::assignment_node(LINE, new m19::index_node(LINE, new cdk::rvalue_node(LINE, new cdk::variable_node(LINE, new std::string("@"))), $3), $6); }
                | '[' expr ']'                                                          { $$ = new m19::memory_alloc_node(LINE, $2); }
                | funcall                                                               { $$ = $1; }
                ;

funcall         : tIDENTIFIER '(' expressions ')'                                       { $$ = new m19::function_call_node(LINE, *$1, $3); delete $1; }
                | '@'         '(' expressions ')'                                       { $$ = new m19::function_call_node(LINE, "@", $3); }
                ;

string          : tSTRING                                                               { $$ = $1; }
                | string tSTRING                                                        { $$ = new std::string(*$1 + *$2); delete $1; delete $2; }
                ;

lval            : tIDENTIFIER                                                           { $$ = new cdk::variable_node(LINE, $1); }
                | lval '[' expr ']'                                                     { $$ = new m19::index_node(LINE, new cdk::rvalue_node(LINE, $1), $3); }
                | '(' expr ')' '[' expr ']'                                             { $$ = new m19::index_node(LINE, $2, $5); }
                | funcall '[' expr ']'                                                  { $$ = new m19::index_node(LINE, $1, $3); }
                ;

%%
