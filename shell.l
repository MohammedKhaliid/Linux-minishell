
/*
 *
 * CS-413 Spring 98
 * shell.l: lexical analyzer for shell
 * You have to extend it.
 *
 */

%{

#include <string.h>
#include "y.tab.h"

%}

%%

\n 	{
		return NEWLINE;
	}

[ \t] 	{
		/* Discard spaces and tabs */
     	}	

">" 	{
		return GREAT;
	}


"&>"	{
		return ERRGREAT;
	}

">&"	{
		return ERRGREAT;
	}

"&>>"	{
		return ERRAPPEND;
	}

">>&"	{
		return ERRAPPEND;
	}
"<" 	{
		return LESS;
	}

">>" {
		return APPEND;
}

"|" {
		return PIPE;
}

"&" {
		return BACKGROUND;
}

"exit"	{
		return EXIT;
}

"cd"	{
		return CDIR;
}

\'[^\']*\'	{
		yylval.string_val = strdup(yytext + 1); 
		yylval.string_val[strlen(yylval.string_val) - 1] = '\0'; 
		return PATH;
}

\"[^\"]*\"	{
		yylval.string_val = strdup(yytext + 1); 
		yylval.string_val[strlen(yylval.string_val) - 1] = '\0'; 
		return PATH;
}


[^ &<>|\"\t\n][^ &<>|\"\t\n]*  {
		/* Assume that file names have only alpha chars */
		yylval.string_val = strdup(yytext);
		return WORD;
	}

%%
