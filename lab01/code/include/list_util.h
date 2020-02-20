/* Xavier Ruppen / REDS / HEIG-VD
 * HPC - 17.02.16
 * array_util.h
 *
 * List API
 */

#pragma once

#include <stdint.h>

/* A list_element must at least contain a link to the next
 * element, as well as a uint64_t data value */
struct list_element;

/* Allocate "len" linked elements and initialize them
 * with random data.
 * Return list head */
struct list_element *list_init(size_t len);

/* Liberate list memory */
void list_clear(struct list_element *head);

/* Arrange a list in increasing order of value */
void list_sort(struct list_element *head);
