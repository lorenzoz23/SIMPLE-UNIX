/**
 * Lorenzo Zenitsky, 10/11/19
 *
 * The C file contains all the functions
 * for executing any built-in/system commands
 * entered in by the user through this shell's 
 * command line.
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

#include "commands.h"

extern char** environ;

/**
 * Prints out the help screen for the user.
 * The system help is displayed after a brief
 * help screen for all the special built-in 
 * commands specific to this shell. All output
 * is piped through the more filter.
 *
 * Return value: void
 */
void help()
{
  puts("User manual for MYSHELL\n"
       "\n"
       "The following commands are supported by this shell:\n\n"
       "cal - Returns a calendar with the current day highlighted\n"
       "cd <directory> - Change the current default directory to <directory>\n"
       "clr - Clear the screen\n"
       "date - Returns the current date\n"
       "dir <directory> - List the contents of directory <directory>\n"
       "echo <comment> - Display <comment> on the display, followed by a new line\n"
       "environ - List all the environment strings\n"
       "help - Display the user manual\n"
       "ls - Lists the content of a directory\n"
       "pause - Pause the operation of the shell until \"ENTER/RETURN\" key is pressed\n"
       "ps - Returns list of currently running processes\n"
       "quit - Quit the shell\n"
       "time - Returns the current time\n"
       "who - Returns various information on current user\n"
       "\n"
       "The following is the user manual for the shell on this computer...\n"
       );

  // since help command isn't supported through execing
  system("bash -c help | more");
}

/**
 * Prints out all the environment strings.
 *
 * Return value: void
 */
void environment_strings()
{
  int i;
  char *s = *environ;
  
  for(i = 1; s; i++) {
    printf("%s\n", s);
    s = *(environ + i);
  }
}

/**
 * Clears the screen. 
 * One could also use the clear system
 * command to achieve the same functionality.
 *
 * Return value: void
 */
void clr()
{
  printf("\033[H\033[J");
}

/**
 * Pauses operation of the shell
 * until the user presses the ENTER/RETURN
 * key.
 *
 * Return value: void
 */
void pause_program()
{
  while(getchar() != '\n');
}

/**
 * Quits the shell.
 *
 * Return value: void
 */
void quit()
{
  printf("Goodbye...\n");
  exit(0);
}

/**
 * Executes any built-in command that the
 * user entered. 
 *
 * parsed_input:
 *             the commands and arguments entered in by the user
 *
 * Return value: void
 */
void execute_built_in_command(char** parsed_input)
{
  char* ownCommands[12];
  int argSwitch = 0;
  int i;

  ownCommands[0] = "cd";
  ownCommands[1] = "cd;";
  ownCommands[2] = "clr";
  ownCommands[3] = "clr;";
  ownCommands[4] = "environ";
  ownCommands[5] = "environ;";
  ownCommands[6] = "help";
  ownCommands[7] = "help;";
  ownCommands[8] = "pause";
  ownCommands[9] = "pause;";
  ownCommands[10] = "quit";
  ownCommands[11] = "quit;";

  for(i = 0; i < 12; i++) { 
    if(strcmp(parsed_input[0], ownCommands[i]) == 0) { 
      argSwitch = i; 
      break; 
    } 
  }

  switch(argSwitch) {
  case 0:
    if(parsed_input[1]) {
      if(chdir(parsed_input[1]) < 0) {
	fprintf(stderr, "Error encountered while trying to change directories...\n");
      }
    }
    else {
      if(chdir(getenv("HOME")) < 0) {
	fprintf(stderr, "Error encountered while trying to change directories...\n");
      }
    }
    break;
  case 1:
    if(chdir(getenv("HOME")) < 0) {
      fprintf(stderr, "Error encountered while trying to change directories...\n");
    }
    break;
  case 2:
  case 3:
    clr();
    break;
  case 4:
  case 5:
    environment_strings();
    break;
  case 6:
  case 7:
    help();
    break;
  case 8:
  case 9:
    pause_program();
    break;
  case 10:
  case 11:
    quit();
    break;
  default:
    break;
  }
}

/**
 * Executes any system commands that the
 * user entered.
 *
 * parsed_input:
 *             the commands and arguments entered in by the user
 * bg:
 *   1 if an & symbol was given, 0 otherwise
 *
 * Return value: void
 */
void execute_unix_command(char** parsed_input, int bg)
{
  pid_t pid = fork();
  int status;
  
  if(pid < 0) { 
    printf("Error: Failed forking child...\n"); 
    return; 
  }
  else if(pid == 0) {
    if(execvp(parsed_input[0], parsed_input) < 0) { 
      printf("Error: Could not execute command...\n");
      return;
    } 
  }
  else {
    // if there is an & symbol in input, return to command
    // line immediately - only wait if bg = 0
    if(!bg) wait(NULL);
    else {
      // if process is supposed to be run in background,
      // wait for process to finish but still continue to execute
      // other commands in foreground
      waitpid(-1, &status, WNOHANG);
    }
    return; 
  } 
}

/**
 * Executes any system command line input that separated by pipes.
 *
 * parsed1:
 *        the left side of the pipe
 * parsed2:
 *        the right side of the pipe
 * bg:
 *   1 if an & symbol was given, 0 otherwise
 *
 * Return value: void
 */
void execute_pipe(char** parsed1, char** parsed2, int bg)
{
  // 0 read, 1 write
  int pipefd[2];  
  pid_t p1, p2;
  int status;
  
  if(pipe(pipefd) < 0) { 
    printf("Error: Pipe could not be initialized\n"); 
    return; 
  }
  
  p1 = fork(); 
  if(p1 < 0) { 
    printf("Error: Could not fork\n"); 
    return; 
  } 
  
  if(p1 == 0) { 
    // child 1 executing...
    // only needs to write at write end 
    close(pipefd[0]); 
    dup2(pipefd[1], STDOUT_FILENO); 
    close(pipefd[1]); 
    
    if(execvp(parsed1[0], parsed1) < 0) { 
      printf("Error: Could not execute command 1...\n"); 
      return;
    } 
  }
  else { 
    // parent executing...
    p2 = fork(); 
    
    if(p2 < 0) { 
      printf("Error: Could not fork\n"); 
      return; 
    } 
    
    // child 2 executing... 
    // only needs to read at read end 
    if (p2 == 0) { 
      close(pipefd[1]); 
      dup2(pipefd[0], STDIN_FILENO); 
      close(pipefd[0]); 
      if(execvp(parsed2[0], parsed2) < 0) { 
	printf("Error: Could not execute command 2...\n"); 
	return;
      } 
    }
    else { 
      // parent executing, waiting for two children 
      if(!bg) {
        close(pipefd[0]);
        close(pipefd[1]);
        waitpid(p1, &status, 0);
	waitpid(p2, &status, 0);
      }
      else {
	waitpid(-1, &status, WNOHANG);
	waitpid(-1, &status, WNOHANG);
      }
    } 
  } 
}


