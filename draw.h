/**
 * Lorenzo Zenitsky, 10/11/19
 * 
 * This is the header class for draw.c
 * 
 * These methods are for printing basic
 * output to the screen that is unrelated
 * to any executed commands.
 */

#ifndef DRAW_H
# define DRAW_H

/**
 * Prints out the command line prompt 
 * to the user after every completed command.
 *
 * Return value: void
 */
void prompt();

/**
 * Prints out some shell info to the user.
 *
 * Return value: void
 */
void print_info();

/**
 * Removes the first two characters from any string.
 *
 * exe: 
 *    the character pointer to remove the characters from.
 *
 * Return value: void
 */
void remove2(char* exe);

#endif
