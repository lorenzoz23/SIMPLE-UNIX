/**
 * Lorenzo Zenitsky, 10/11/19
 * 
 * This is the header class for process.c
 * 
 * These methods are for processing user input
 * given to the program through the command line.
 */

#ifndef PROCESS_H
# define PROCESS_H

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
int take_user_input(char* input, int char_count);

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
void parse_string(char* input);

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
void read_input_from_file(char* filename, char** commands);

#endif
