/**
 * Lorenzo Zenitsky, 10/11/19
 * 
 * This is the header class for commands.c
 * 
 * These methods are for executing basic 
 * built-in/system commands and commands with
 * pipes.
 */


#ifndef COMMANDS_H
 # define COMMANDS_H

/**
 * Clears the screen. 
 * One could also use the clear system
 * command to achieve the same functionality.
 *
 * Return value: void
 */
void clr();

/**
 * Executes any built-in command that the
 * user entered. 
 *
 * parsed_input:
 *             the commands and arguments entered in by the user
 *
 * Return value: void
 */
void execute_built_in_command(char** parsed_input);

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
void execute_unix_command(char** parsed_input, int bg);

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
void execute_pipe(char** parsed1, char** parsed2, int bg);

#endif
