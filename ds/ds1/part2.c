#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include "part2.h"

/**
 * Initialize the list.
 * @param list the list to initialize
 */
void list_init(list_t *list)
{

}

/**
 * Insert an item into the list.
 * @param list the list to insert an item into
 * @param val the value to insert
 */
void list_insert(list_t *list, int val)
{

}

/**
 * Remove all items from a list.
 * @param list the list to clear
 */
void list_clear(list_t *list)
{

}

/**
 * Clone all items from a given list into a new list, removing any items
 * that may be contained in the target in the process.
 * @param source the list to copy
 * @param target the list to copy into
 */
void list_copy(list_t *source, list_t *target)
{

}

/**
 * Deallocate any memory associated with a list.
 * @param list the list to free
 */
void list_destroy(list_t *list)
{

}

/**
 * Compute the average of all items in the list.
 * @param list the list to compute the average of
 * @return the average or nan if the list is empty
 */
double list_average(list_t *list)
{
  return 0;
}

/**
 * Find the largest item in the list.
 * @param list the list to search
 * @return the largest value in the input list or INT_MIN if the list is empty
 */
int list_max(list_t *list)
{
  return 0;
}

/**
 * Find the smallest item in the list.
 * @param list the list to search
 * @return the smallest value in the input list or INT_MAX if the list is empty
 */
int list_min(list_t *list)
{
  return 0;
}

/**
 * Check if the list contains a given value.
 * @param list the list to search
 * @param value the value to look for in the list
 * @return 0 if the value is not in the list, 1 if it is
 */
int list_contains(list_t *list, int value)
{
  return 0;
}

