/**
 * Lorenzo Zenitsky, 10/11/19
 * 
 * This C file contains various
 * functions for printing useful output
 * to the screen for the user.
 */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include "draw.h"

// various colors for use in print commands
#define GREEN "\x1B[32m"
#define RED  "\x1B[31m"
#define NORMAL  "\x1B[0m"
#define YELLOW  "\x1B[33m"
#define BLUE  "\x1B[34m"
#define MAGENTA  "\x1B[35m"
#define CYNAN  "\x1B[36m"
#define WHITE  "\x1B[37m"
#define RESET_COLOR "\033[0m"

/**
 * Removes the first two characters from any string.
 *
 * exe: 
 *    the character pointer to remove the characters from.
 *
 * Return value: void
 */
void remove2(char* exe)
{
  int i, j;
  char tmp[strlen(exe) - 1];

  j = 0;
  for(i = 2; i < strlen(exe); i++) {
    tmp[j] = exe[i];
    j++;
  }
  tmp[j] = '\0';
  strcpy(exe, tmp);
}


/**
 * Prints out some shell info to the user.
 *
 * Return value: void
 */
void print_info(char* executable)
{
  char cwd[1024];
  // removes the './' from command line arg
  remove2(executable);

  if(!getcwd(cwd, sizeof(cwd))) {
    fprintf(stderr, "size of array is not enough");
  }

  // adds the executable to the cwd path
  strcat(cwd, "/");
  strcat(cwd, executable);
  printf(GREEN
	 "COM S 352 - MYSHELL\n"
	 "Author: Lorenzo Zenitsky\n"
	 "Location of executable: %s\n\n"
	 RESET_COLOR, cwd);
}

/**
 * Prints out the command line prompt 
 * to the user after every completed command.
 *
 * Return value: void
 */
void prompt()
{
  char cwd[1024];

  if(!getcwd(cwd, sizeof(cwd))) {
    fprintf(stderr, "size of array is not enough");
  }
  else {
    // prints out the command line prompt in red
    printf(RED "%s" RESET_COLOR, cwd);
    printf("> ");
  }
}




