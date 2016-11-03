%{
#include "sub_create.h"
#include "symbol.h"

%}

%union
{
CSymbol * p_symbol;
string * p_string;
}

%token <p_string> STRING

%type <p_symbol> func_state
%type <p_symbol> param_items
%type <p_symbol> param_other
%type <p_symbol> var_define

%%

statements:	/* empty */
			| statements statement
			| error					{ yyerrok; }
			;

statement:	';'
			| func_state ';'
			| error ';'				{ yyerrok; }
			;

func_state:	STRING STRING STRING '(' param_items ')'		{
														$$ = ParserFuncState($1, $2, $3, $5);
														}
			| STRING STRING '(' param_items ')'			{
														$$ = ParserFuncState($1, nullptr, $2, $4);
														}

param_items:	/* empty */								{
														$$ = ParserParamItems(nullptr, nullptr);
														}
				| var_define param_other				{
														$$ = ParserParamItems($1, $2);
														}

var_define:		STRING STRING							{
														$$ = ParserVarDefine($1, $2);
														}

param_other:	/* empty */								{
														$$ = ParserParamOther(nullptr, nullptr);
														}
				| ',' var_define param_other			{
														$$ = ParserParamOther($2, $3);
														}

%%
