#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "draw.h"
#include "process.h"
#include "commands.h"

#define MAXINPUT 1000
#define MAXCOUNT 100

/**
 * Main method - It first clears the screen and prints some 
 * shell info for the user. Next, if there is a file present
 * in the arguments, it is read from and the shell exits. If not, 
 * the shell goes into an infinite loop reading in and asking for
 * input from the user.
 */
int main(int argc, char* argv[]) {
  char input[MAXINPUT];
  char* commands[MAXCOUNT];

  // clear the screen
  clr();
  // print some shell info
  print_info(argv[0]);

  // read from file if provided
  if(argv[1]) {
    read_input_from_file(argv[1], commands);
    exit(0); 
  }

  // give the user the prompt, take input, display results
  // also, check the status of any background processes
  while(1) {
    int status;
    waitpid(-1, &status, WNOHANG);
    prompt();

    if(take_user_input(input, MAXINPUT) && strlen(input) > 1) {
      parse_string(input);
    }
  }
}
