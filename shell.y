
/*
 * CS-413 Spring 98
 * shell.y: parser for shell
 *
 * This parser compiles the following grammar:
 *
 *	cmd [arg]* [> filename]
 *
 * you must extend it to understand the complete shell grammar
 *
 */

%token	<string_val> WORD

%token 	NOTOKEN GREAT LESS APPEND ERRGREAT ERRAPPEND PIPE BACKGROUND NEWLINE

%union	{
		char   *string_val;
	}

%{
extern "C" 
{
	int yylex();
	void yyerror (char const *s);
}
#define yylex yylex
#include <stdio.h>
#include "command.h"
%}

%%

goal:	
	commands
	;

commands: 
	command
	| commands command 
	;

command: simple_command
        ;

simple_command:	
	command_and_args pipe_list iomodifier_opt background NEWLINE {
		printf("   Yacc: Execute command\n");
		Command::_currentCommand.execute();
	}
	| NEWLINE 
	| error NEWLINE { yyerrok; }
	;

command_and_args:
	command_word arg_list {
		Command::_currentCommand.
			insertSimpleCommand( Command::_currentSimpleCommand );
	}
	;

arg_list:
	arg_list argument
	| /* can be empty */
	;

argument:
	WORD {
               printf("   Yacc: insert argument \"%s\"\n", $1);

	       Command::_currentSimpleCommand->insertArgument( $1 );\
	}
	;

command_word:
	WORD {
               printf("   Yacc: insert command \"%s\"\n", $1);
	       
	       Command::_currentSimpleCommand = new SimpleCommand();
	       Command::_currentSimpleCommand->insertArgument( $1 );
	}
	;

pipe_list:
	pipe_list pipe
	| /* can be empty */
	;

pipe:
	pipe_word command_and_args 

pipe_word:
	PIPE {
			printf("   Yacc: piping: \n");
	}
	;
iomodifier_opt:
	iomodifier_opt GREAT WORD { // > file
		printf("   Yacc: insert output \"%s\"\n", $3);
		Command::_currentCommand._outFile = $3;
	}
	| iomodifier_opt LESS WORD{ // < file
		printf("   Yacc: insert input \"%s\"\n", $3);
		Command::_currentCommand._inputFile = $3;
	}
	| iomodifier_opt APPEND WORD { // >> file
		printf("   Yacc: insert append output \"%s\"\n", $3);
		Command::_currentCommand._outFile = $3;
		Command::_currentCommand._append = 1;
	}
	| iomodifier_opt ERRGREAT WORD { // &> file OR >& file
		printf("   Yacc: insert output plus error \"%s\"\n", $3);
		Command::_currentCommand._outFile = $3;
		Command::_currentCommand._errFile = $3;
	}
	| iomodifier_opt ERRAPPEND WORD { // &>> file
		printf("   Yacc: insert append plus error \"%s\"\n", $3);
		Command::_currentCommand._outFile = $3;
		Command::_currentCommand._errFile = $3;
		Command::_currentCommand._append = 1;
	}
	| /* can be empty */ 

	;

background:
	BACKGROUND {
		printf("   Yacc: backgroud processing\n");
		Command::_currentCommand._background = 1;
	}
	| /* can be empty */ 

%%

void
yyerror(const char * s)
{
	fprintf(stderr,"%s\n", s);
	Command::_currentCommand.clear();
}

#if 0
main()
{
	yyparse();
}
#endif
