#ifndef _QUERY_H_
#define _QUERY_H_

#include <sys/types.h>

void remove_spaces(char *dest, char *src);
size_t copy_token(char *dest, char *src, char *delim);
int key_to_col(char *line, char *key);
size_t find_col(char *line, int column);

#endif
