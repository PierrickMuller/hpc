#pragma once

#define NB_CHARS	62 // [a-z][A-Z][0-9] -> 62 characters

struct char_stats {
  /* NOTE : Don't remove this struct member, as it is needed
   * for veryfiying your results */
  size_t count;
  
  /* NOTE : From here you can structure as you'd like
   * as long as you effectively count each character of 
   * the alphabet [a-z][A-Z][0-9]. */
  size_t char_count[NB_CHARS];
};


/* Opens a (potentially huge) text file to be processed,
 * initializes threads and returns a pointer to a struct char_stats handle. */
struct char_stats *stats_init(const char *path);

/* Parse the file opened by stats_init() and gathers per-character statistics.
 * The alphabet processed is [a-z][A-Z][0-9]. Other symbols are ignored.
 * Returns the number of characters from the alphabet processed.
 */
size_t stats_count(struct char_stats *stats);

/* Clears everything initialized by char_init() */
void stats_clear(struct char_stats *stats);
