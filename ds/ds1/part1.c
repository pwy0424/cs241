#include <stdio.h>
#include <stdlib.h>

/**
 * Compute the length of a string s.
 * @param s a string
 * @return the number of characters preceding the NULL terminator
 */
size_t strlen(const char *s)
{
  return 0;
}

/**
 * Lexicographically compare strings s1 and s2.
 * @param s1 a string
 * @param s2 a string
 * @return an integer greater than, equal to, or less than zero if s1 is
 *         greater than, equal to, or less than s2, respectively.
 */
int strcmp(const char *s1, const char *s2)
{
  return 0;
}

/**
 * Copy the string s2 to s1 (including the terminating `\0').
 * @param s1 a mutable string
 * @param s2 a constant string
 * @return a pointer to s1
 */
char *strcpy(char *restrict s1, const char *restrict s2)
{
  return s1;
}

/**
 * Find the first occurence of (char)c in s.
 * The terminating NULL is considered part of the string, 
 * therefore if c is `\0', strchr should locate the 
 * terminating `\0'.
 * @param s a string
 * @param c a character to search for
 * @return a pointer to the located character or NULL if it could not be found
 */
char *strchr(const char *s, int c)
{
  return NULL;
}

