%{
#include <stdio.h>
#include <boost/format.hpp>
using boost::format;

#include "sub_create.h"			// SetTokenProperty
#include "sub_create.tab.h"		// yytokentype
#include "symbol.h"				// COutput

#define ECHO do { GetOutput()->Write(yytext); } while(0)

%}

%option noyywrap

%x CODE

CHARACTOR	[_a-zA-Z0-9]
SPACE		[ \t\r\n]
EATER		.

COMMENT1	"/*"
COMMENT2	"//"

STRING		{CHARACTOR}+

/*
ASSIGN		=
SEM			;
WELL		#
COMMA		,
LEFT_BRACKET	"("
RIGHT_BRACKET	")"
*/

%%

"//%{"		{ BEGIN(CODE); }
.*			{ ECHO; }
\n			{ ECHO; }

<CODE>"//%}"	{ BEGIN(INITIAL); }

<CODE>{SPACE}		{/*nothing to do*/}
<CODE>{COMMENT1}	{
				int c;
				int n_begin_line = yylineno;
				while(1) {
					c = yyinput();
					if(c == EOF) {
						LexicalReportError(
							(format("find unexcepted end of file: %d") % 
								n_begin_line).str()
							);
						AbortParse();
						break;
					} else if(c != '*') {
						continue;
					} else {
						c = yyinput();
						while(c == '*') c = yyinput();
						if(c == EOF) {
							LexicalReportError(
								(format("find unexcepted end of file: %d") % 
									n_begin_line).str()
							);
							AbortParse();
							break;
						} else if(c == '/') {
							break;		// end comment
						}
					}
				}
			}

<CODE>{COMMENT2}	|
<CODE>#			{
				int c;
				do {
					c = yyinput();
				} while(c != '\n' && c != EOF);
			}

<CODE>{STRING}	{
				SetTokenProperty(yytext);
				return STRING;
			}

<CODE>"="			{
				SetTokenProperty("=");
				return yytext[0];
			}

<CODE>";"			{
				SetTokenProperty(";");
				return yytext[0];
			}

<CODE>","			{
				SetTokenProperty(",");
				return yytext[0];
			}

<CODE>")"			{
				SetTokenProperty(")");
				return yytext[0];
			}

<CODE>"("			{
				SetTokenProperty("(");
				return yytext[0];
			}

<CODE>"{"			{
				SetTokenProperty("{");
				return yytext[0];
			}

<CODE>"}"			{
				SetTokenProperty("}");
				return yytext[0];
			}


<CODE>{EATER}		{ /*do nothing*/ }

%%

bool lexical_init(const char * psz_file)
{
	yyin = fopen(psz_file, "rt");
	return yyin != NULL;
}
void lexical_close()
{
	fclose(yyin);
}
