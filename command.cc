
/*
 * CS354: Shell project
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>

#include "command.h"

#include <time.h>

void log_child_termination(int pid, int status)
{
	FILE *logFile;
	//change the path here
	char logFilePath[] = "/mnt/sda1/University/Level 3/Subj/Term 7/Operating Systems/Labs/lab3/lab3-src/lab2-src/log.txt";
	logFile = fopen(logFilePath, "a");
	
	if (!logFile)
	{
		perror("Failed to open log file");
		return;
	}

	time_t current_time = time(NULL);
	fprintf(logFile, "Child process %d terminated at %s", pid, ctime(&current_time));

	if (WIFEXITED(status))
	{
		fprintf(logFile, " - Exit status: %d\n", WEXITSTATUS(status));
	}
	else if (WIFSIGNALED(status))
	{
		fprintf(logFile, " - Terminated by signal: %d\n", WTERMSIG(status));
	}

	fclose(logFile);
}

void child_signal_handler(int sig)
{
	int pid;
	int status;

	while ((pid = waitpid(-1, &status, WNOHANG)) > 0)
	{
		log_child_termination(pid, status);
	}
}

SimpleCommand::SimpleCommand()
{
	// Creat available space for 5 arguments
	_numberOfAvailableArguments = 5;
	_numberOfArguments = 0;
	_arguments = (char **)malloc(_numberOfAvailableArguments * sizeof(char *));
	signal(SIGCHLD, child_signal_handler);
}

void SimpleCommand::insertArgument(char *argument)
{
	if (_numberOfAvailableArguments == _numberOfArguments + 1)
	{
		// Double the available space
		_numberOfAvailableArguments *= 2;
		_arguments = (char **)realloc(_arguments,
									  _numberOfAvailableArguments * sizeof(char *));
	}

	_arguments[_numberOfArguments] = argument;

	// Add NULL argument at the end
	_arguments[_numberOfArguments + 1] = NULL;

	_numberOfArguments++;
}

Command::Command()
{
	// Create available space for one simple command
	_numberOfAvailableSimpleCommands = 1;
	_simpleCommands = (SimpleCommand **)
		malloc(_numberOfSimpleCommands * sizeof(SimpleCommand *));

	_numberOfSimpleCommands = 0;
	_outFile = 0;
	_inputFile = 0;
	_errFile = 0;
	_background = 0;
	_append = 0;
	_currentDir = NULL;
}

void Command::insertSimpleCommand(SimpleCommand *simpleCommand)
{
	if (_numberOfAvailableSimpleCommands == _numberOfSimpleCommands)
	{
		_numberOfAvailableSimpleCommands *= 2;
		_simpleCommands = (SimpleCommand **)realloc(_simpleCommands,
													_numberOfAvailableSimpleCommands * sizeof(SimpleCommand *));
	}

	_simpleCommands[_numberOfSimpleCommands] = simpleCommand;
	_numberOfSimpleCommands++;
}

void Command::clear()
{
	for (int i = 0; i < _numberOfSimpleCommands; i++)
	{
		for (int j = 0; j < _simpleCommands[i]->_numberOfArguments; j++)
		{
			free(_simpleCommands[i]->_arguments[j]);
		}

		free(_simpleCommands[i]->_arguments);
		free(_simpleCommands[i]);
	}

	if (_outFile)
	{
		free(_outFile);
	}

	if (_inputFile)
	{
		free(_inputFile);
	}

	// if ( _errFile ) {
	// 	free( _errFile );
	// }

	_numberOfSimpleCommands = 0;
	_outFile = 0;
	_inputFile = 0;
	_errFile = 0;
	_background = 0;
	_append = 0;
}

void Command::print()
{
	printf("\n\n");
	printf("              COMMAND TABLE                \n");
	printf("\n");
	printf("  #   Simple Commands\n");
	printf("  --- ----------------------------------------------------------\n");

	for (int i = 0; i < _numberOfSimpleCommands; i++)
	{
		printf("  %-3d ", i);
		for (int j = 0; j < _simpleCommands[i]->_numberOfArguments; j++)
		{
			printf("\"%s\" \t", _simpleCommands[i]->_arguments[j]);
		}
	}

	printf("\n\n");
	printf("  Output       Input        Error        Background\n");
	printf("  ------------ ------------ ------------ ------------\n");
	printf("  %-12s %-12s %-12s %-12s\n", _outFile ? _outFile : "default",
		   _inputFile ? _inputFile : "default", _errFile ? _errFile : "default",
		   _background ? "YES" : "NO");
	printf("\n\n");
}

void Command::exitshell()
{
	printf("  Good bye!!\n");
	exit(0);
}

void Command::execute()
{
	// Don't do anything if there are no simple commands
	if (_numberOfSimpleCommands == 0)
	{
		prompt();
		return;
	}

	// Print contents of Command data structure
	print();

	int defaultin = dup(0);
	int defaultout = dup(1);
	int defaulterr = dup(2);

	int outfd = defaultout;
	int infd = defaultin;

	int fdpipes[_numberOfSimpleCommands - 1][2];

	for (int i = 0; i < _numberOfSimpleCommands - 1; i++)
	{

		if (pipe(fdpipes[i]) == -1)
		{
			perror("piping error: ");
			exit(2);
		}
	}

	for (int i = 0; i < _numberOfSimpleCommands; i++)
	{

		if (i == 0 && _inputFile)
		{

			infd = open(_inputFile, O_RDONLY);

			if (infd < 0)
			{
				perror(" error opening the input file");
				exit(2);
			}
			dup2(infd, 0);
			close(infd);
		}

		else
			dup2(fdpipes[i - 1][0], 0);

		if (i == _numberOfSimpleCommands - 1)
		{
			if (_outFile)
			{
				if (!_append)
					outfd = creat(_outFile, 0666);

				else
					outfd = open(_outFile, O_WRONLY | O_APPEND | O_CREAT, 0666);

				if (outfd < 0)
				{
					perror(" create output file!");
					exit(2);
				}

				if (_errFile)
				{
					dup2(outfd, 2);
					close(outfd);
				}

				dup2(outfd, 1);
				close(outfd);
			}
			else
			{
				dup2(defaultout, 1);
			}
		}

		else
		{
			dup2(fdpipes[i][1], 1);
		}

		int pid = fork();

		if (pid < 0)
		{
			perror("Failed to fork\n");
			exit(2);
		}

		else if (pid == 0)
		{
			// child

			for (int j = 0; j < _numberOfSimpleCommands - 1; j++)
			{
				// close(fdpipes[j][0]);
				// close(fdpipes[j][1]);
			}

			execvp(_simpleCommands[i]->_arguments[0], _simpleCommands[i]->_arguments);

			perror("	execution error: ");
			exit(2);
		}
		else
		{
			if (i > 0)
			{
				close(fdpipes[i - 1][0]);
			}
			if (i < _numberOfSimpleCommands - 1)
			{
				close(fdpipes[i][1]);
			}

			if (_background == 0)
			{
				int status;
				waitpid(pid, &status, 0);
				log_child_termination(pid, status);
			}
		}
	}

	dup2(defaultin, 0);
	dup2(defaultout, 1);
	dup2(defaulterr, 2);

	close(defaultin);
	close(defaultout);
	close(defaulterr);

	clear();

	// Print new prompt
	prompt();
}

void sigint_handler(int sig)
{
	printf("\n");
	Command::_currentCommand.prompt();
}

// Shell implementation

void Command::prompt()
{
	char cwd[900];

	printf("\033[31mmyshell:\033[0m"); // color: red

	if (getcwd(cwd, sizeof(cwd)) != NULL)
	{

		printf("\x1b[32m%s\x1b[0m$ ", cwd); // color: blue
	}

	else
	{
		perror("");
	}

	fflush(stdout);
}

void Command::change_dir()
{
	char *homeDir = getenv("HOME");

	if (_currentDir == NULL)
		_currentDir = homeDir;

	if (chdir(Command::_currentCommand._currentDir) == -1)
	{
		perror(" Failed to change directory: ");
		Command::_currentCommand._currentDir = (char *)".";
	}

	prompt();
}

Command Command::_currentCommand;
SimpleCommand *Command::_currentSimpleCommand;

int yyparse(void);

int main()
{
	signal(SIGINT, sigint_handler);

	Command::_currentCommand.prompt();
	yyparse();
	return 0;
}
