/**
 * Lorenzo Zenitsky, 10/11/19
 * 
 * This C file contains functions for parsing
 * and processing user input.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "process.h"
#include "commands.h"
#include "draw.h"

/**
 * Helper function for determining if input string
 * has a certain character.
 *
 * input:
 *      input string to check
 * c:
 *  character to check string with
 *
 * Return value: 1 if true, 0 otherwise
 */
int has_character(char* input, char c)
{
  if(strchr(input, c)) {
    return 1;
  }
  else {
    return 0;
  }
}

/**
 * Helper function to determine whether command is
 * built-in, system, or invalid input.
 *
 * command:
 *        command to check
 * 
 * Return value: 1 if built-in, 0 otherwise
 */
int is_own_command(char* command)
{
  if(strcmp("clr", command) == 0 || strcmp("environ", command) == 0 ||
     strcmp("pause", command) == 0 || strcmp("quit", command) == 0 ||
     strcmp("cd", command) == 0 || strcmp("help", command) == 0) {
    return 1;
  }
  else {
    return 0;
  }
}

/**
 * Helper function to remove leading whitespace.
 *
 * input:
 *      input to remove leading whitespace from
 *
 * Return value: void
 */
void remove_leading_whitespace(char** input)
{
  if(*input[0] == ' ') {
    while(*input[0] == ' ') {
      strsep(input, " ");
    }
  }
}

/**
 * Takes input and parses it by spaces and inserts
 * each parsed input into the parsed char pointer array.
 *
 * input:
 *      input to parse spaces from
 * parsed:
 *       char pointer array to put parsed strings into
 * 
 * Return value: void
 */
void parse_spaces(char* input, char** parsed)
{ 
  char* cp = input;
  int i = 0;
  while(has_character(cp, ' ')) {
    parsed[i] = strsep(&cp, " ");
    i++;
    // if more than one space is used as a delimiter,
    // remove until there is no more space
    if(cp[0] == ' ') {
      while(cp[0] == ' ') {
	strsep(&cp, " ");
      }
    }
    // edge case for last string in input with length > 0 and no more spaces
    else if(strlen(cp) > 0 && !has_character(cp, ' ')) {
      parsed[i] = cp;
    }
  }
}

/**
 * Reads any command line input given from 
 * the filename provided when executing the shell.
 *
 * filename:
 *         name of the text file to read from
 * commands: 
 *         array of character pointers to store commands in
 *
 * Return value: void
 */
void read_input_from_file(char* filename, char** commands)
{
  FILE* file;
  char* ext = strrchr(filename, '.');
  char** parsed_input;

  // adds .txt extension if not present
  if(!ext) {
    strcat(filename, ".txt");
  }

  file = fopen(filename, "r");
  if(!file) {
    printf("Error: Could not find file in current working directory.\n");
    printf("Checking parent directory...\n");

    // if file is not found in cwd, the parent directory is checked
    char* parent = malloc(sizeof(char) * (strlen(filename) + 10));
    parent = strcpy(parent, "../");
    strcat(parent, filename);
    
    if(!(file = fopen(parent, "r"))) {
      fprintf(stderr, "Error: Could not find file in parent directory...\n");
      exit(1);
    }
    
    printf("File has been opened for reading.\n\n");
    free(parent);
  }

  int i = 0;
  commands[i] = malloc(sizeof(char) * 100);
  parsed_input = malloc(sizeof(char*) * 100);
  while(fgets(commands[i], 1000, file) != NULL) {
    commands[i][strcspn(commands[i], "\n")] = 0; // remove newline
    
    parse_string(commands[i]);
    free(commands[i]);
    i++;
    if(i > 100) {
      printf("Too many lines of commands.\n");
      break;
    }
    commands[i] = malloc(sizeof(char) * 100);
  }

  printf("\n");
  if(i < 100) free(commands[i]);
  free(parsed_input);
  fclose(file);
}

/**
 * Consumes whatever input the user gives
 * through the command line.
 *
 * input:
 *      variable to store input given by user
 * char_count:
 *           maximum length of user input program can take
 *
 * Return value: 1 if input length is greater than 0, 0 otherwise
 */
int take_user_input(char* input, int char_count)
{
  fgets(input, char_count, stdin); // copies user input into variable with newline

  input[strcspn(input, "\n")] = 0; // remove newline from input
  if(strlen(input) != 0) {
    return 1;
  }
  else {
    return 0;
  }
}

/**
 * Helper method that goes through each 
 * char pointer in arr and memsets it back
 * to an empty state.
 *
 * arr:
 *    char pointer array to have its contents emptied
 * 
 * Return value: void
 */
void double_init(char** arr)
{
  int i = 0;
  while(arr[i]) {
    memset(arr[i], '\0', sizeof(char) * strlen(arr[i]));
    i++;
  }
}

/**
 * Processes the string by telling whether 
 * it is a built-in command or can be executed
 * by the system. If there is semi-colon detected,
 * the function loops until all semi-colon - separated 
 * commands have been executed.
 *
 * parsed_input:
 *             parsed input given by user
 * has_semi:
 *         1 if a semi-colon has been detected, 0 otherwise
 * bg: 
 *   1 if should be executed as a background process, 0 otherwise
 */
void process_string(char** parsed_input, int has_semi, int bg)
{
  char* command;
  char** semi_cmds;
  int i, j;

  i = 0, j = 0;
  
  command = parsed_input[0];
  // if there is a semi-colon present in parsed_input
  // then this loop executes each command when as soon
  // as a semi-colon is detected
  if(has_semi) {
    semi_cmds = malloc(sizeof(char*) * 100);
    while(parsed_input[i]) {
      semi_cmds[j] = parsed_input[i];
      if(has_character(parsed_input[i], ';')) {
	semi_cmds[j][strcspn(semi_cmds[j], ";")] = 0;
	if(is_own_command(semi_cmds[0])) {
	  execute_built_in_command(semi_cmds);
	}
	else {
	  execute_unix_command(semi_cmds, bg);
	}
	double_init(semi_cmds);
	j = -1;
      }
      j++;
      i++;
    }
    free(semi_cmds);
  }
  else if(is_own_command(command)) {
    execute_built_in_command(parsed_input);
  }
  else {
    execute_unix_command(parsed_input, bg);
  }
}

/**
 * Helper function for determining if 
 * input string contains a i/o redirect symbol.
 *
 * input:
 *      input string to check
 * 
 * Return value: 1 if true, 0 otherwise
 */
int has_redirect(char* input)
{
  return (strchr(input, '>') || strchr(input, '<'));
}

/**
 * Parses input string containing pipe redirection.
 * Splits the input into two commands, parses both, and
 * calls a method for executing commands by piping.
 *
 * input:
 *      input string to parse by pipes
 * bg:
 *   1 if commands should be run in background, 0 otherwise
 *
 * Return value: void
 */
void parse_pipe(char* input, int bg)
{
  char** commands;
  char** parsed_pipe1;
  char** parsed_pipe2;

  commands = malloc(sizeof(char*) * 5);
  parsed_pipe1 = malloc(sizeof(char*) * 10);
  parsed_pipe2 = malloc(sizeof(char*) * 10);
  
  commands[0] = strsep(&input, "|");
  commands[1] = input;
  
  remove_leading_whitespace(&commands[1]);
  
  if(has_character(commands[0], ' ')) {
    parse_spaces(commands[0], parsed_pipe1);
  }
  else {
    parsed_pipe1[0] = commands[0];
  }
  if(has_character(commands[1], ' ')) {
    parse_spaces(commands[1], parsed_pipe2);
  }
  else {
    parsed_pipe2[0] = commands[1];
  }
  
  execute_pipe(parsed_pipe1, parsed_pipe2, bg);
  
  free(parsed_pipe1);
  free(parsed_pipe2);
  free(commands);
}

/**
 * Parses input string that contains i/o redirection
 * symbols, same as for piping. This method can parse, 
 * process, and send out for execution strings that contain
 * '>', '<', ">>", or a combination of one input and one output.
 *
 * input:
 *      input string to check
 * bg:
 *   1 if process should be run in the background, 0 otherwise
 *
 * Return value: void
 *
 * PS - sorry for the length :)
 * I know, it's heinous
 */
void parse_redirect(char* input, int bg)
{
  int in = 0, is_in = 0;
  int out = 0, is_out = 0;
  int d_out = 0, is_d_out = 0;
  
  char* cmd;
  char* input_file_name;
  char* output_file_name;

  // determines what i/o redirect characters
  // the input string has
  if(has_character(input, '<')) in = 1;
  
  if(has_character(input, '>')) {
    if(strstr(input, ">>")) {
      d_out = 1;
    }
    else {
      out = 1;
    }
  }

  // sets the command and filename for input
  // with only one redirect character
  if(in && (!out && !d_out)) {
    cmd = strsep(&input, "<");
    input_file_name = input;
  }
  if(out && !in) {
    cmd = strsep(&input, ">");
    output_file_name = input;
  }
  if(d_out && !in) {
    cmd = strsep(&input, ">>");
    output_file_name = input;
  }

  // if there is both an input redirect symbol
  // and one for output, loop till first redirect
  // character is found and determine what it is
  // to make splitting up/parsing the line easier
  int i = 0;
  if(in && out && !d_out) {
    while(input[i]) {
      if(input[i] == '<') {
	is_in = 1;
	break;
      }
      else if(input[i] == '>') {
	is_out = 1;
	break;
      }
      i++;
    }

    if(is_in) {
      cmd = strsep(&input, "<");
      input_file_name = strsep(&input, ">");
      output_file_name = input;
    }
    else if(is_out) {
      cmd = strsep(&input, ">");
      output_file_name = strsep(&input, "<");
      input_file_name = input;
    }
  }
  if(in && d_out && !out) {
    while(input[i]) {
      if(input[i] == '<') {
	is_in = 1;
	break;
      }
      else if(strcmp(input, ">>") == 0) {
	is_d_out = 1;
	break;
      }
      i++;
    }

    if(is_in) {
      cmd = strsep(&input, "<");
      input_file_name = strsep(&input, ">>");
      output_file_name = input;
    }
    else if(is_d_out) {
      cmd = strsep(&input, ">>");
      output_file_name = strsep(&input, "<");
      input_file_name = input;
    }
  }

  int out_fd, in_fd;
  int saved_stdout, saved_stdin;

  // creates/opens file for output redirection - truncates if exists
  if(out && !in) {
    remove_leading_whitespace(&output_file_name);
    saved_stdout = dup(STDOUT_FILENO);
    
    if((out_fd = creat(output_file_name, 00666 )) == -1) {
      fprintf(stderr, "Error: Unable to create/open file\n");
      return;
    }
    
    dup2(out_fd, STDOUT_FILENO);
    close(out_fd);
  }
  // creates/opens file for input redirection
  else if(in && (!out && !d_out)) {
    remove_leading_whitespace(&input_file_name);
    saved_stdin = dup(STDIN_FILENO);
    
    if((in_fd = open(input_file_name, O_RDONLY)) == -1) {
      fprintf(stderr, "Error: Unable to create/open file\n");
      return;
    }
    
    dup2(in_fd, STDIN_FILENO);
    close(in_fd);
  }
  // creates/opens file for output redirection - appends if exists
  else if(d_out && !in) {
    remove2(output_file_name);
    remove_leading_whitespace(&output_file_name);
    saved_stdout = dup(STDOUT_FILENO);
    
    if((out_fd = open(output_file_name, O_WRONLY | O_CREAT | O_APPEND,
		      00666 )) == -1) {
      fprintf(stderr, "Error: Unable to create/open file\n");
      return;
    }
    
    dup2(out_fd, STDOUT_FILENO);
    close(out_fd);
  }

  // creates/opens file for both input and output redirection
  if(in && (out || d_out)) {
    remove_leading_whitespace(&input_file_name);
    if(has_character(input_file_name, ' ')) {
      input_file_name[strcspn(input_file_name, " ")] = 0;
    }
    saved_stdin = dup(STDIN_FILENO);
    
    if((in_fd = open(input_file_name, O_RDONLY)) == -1) {
      fprintf(stderr, "Error: Unable to create/open file\n");
      return;
    }
    
    dup2(in_fd, STDIN_FILENO);
    close(in_fd);

    // create/open file for output redirection - truncates
    if(out && !d_out) {
      remove_leading_whitespace(&output_file_name);
      if(has_character(output_file_name, ' ')) {
	output_file_name[strcspn(output_file_name, " ")] = 0;
      }
      saved_stdout = dup(STDOUT_FILENO);
      
      if((out_fd = creat(output_file_name, 00666 )) == -1) {
	fprintf(stderr, "Error: Unable to create/open file\n");
	return;
      }
      
      dup2(out_fd, STDOUT_FILENO);
      close(out_fd);
    }

    // craete/open file for output redirection - appends
    if(d_out && !out) {
      remove2(output_file_name);
      remove_leading_whitespace(&output_file_name);
      if(has_character(output_file_name, ' ')) {
	output_file_name[strcspn(output_file_name, " ")] = 0;
      }
      saved_stdout = dup(STDOUT_FILENO);
      
      if((out_fd = open(output_file_name, O_WRONLY | O_CREAT | O_APPEND,
			00666 )) == -1) {
	fprintf(stderr, "Error: Unable to create/open file\n");
	return;
      }
      
      dup2(out_fd, STDOUT_FILENO);
      close(out_fd);
    }
  }
  
  parse_string(cmd);

  // return stdout and stdin to its normal state
  if(out || d_out) {
    dup2(saved_stdout, STDOUT_FILENO);
    close(saved_stdout);
  }
  if(in) {
    dup2(saved_stdin, STDIN_FILENO);
    close(saved_stdin);
  }
}

/**
 * Determines if input has piping,
 * background execution, i/o redirection,
 * and/or spaces.
 *
 * input:
 *      input string that the user entered
 *
 * Return value: void
 */
void parse_string(char* input)
{
  char** parsed_input;
  int semi = 0;
  int bg = 0;

  if(has_character(input, '&')) {
    bg = 1;
    input[strcspn(input, "&")] = 0;
  }
    
  if(has_character(input, '|')) {
    parse_pipe(input, bg);
  }

  if(has_redirect(input)) {
    parse_redirect(input, bg);
  }
  
  else {
    parsed_input = malloc(sizeof(char*) * 100);
    
    if(has_character(input, ';')) semi = 1;
    if(has_character(input, ' ')) {
      parse_spaces(input, parsed_input);
    }
    else {
      parsed_input[0] = input;
    }
    process_string(parsed_input, semi, bg);
    free(parsed_input);
  }
}
