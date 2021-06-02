#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "query.h"
#include "logger.h"

#define MAX_QUERY 128
#define MAX_LINE 5000

/* Forward declaration of print_usage for this 'private' function: */
void print_usage(char *argv[]);

/**
 * Copies a string, but removes any space characters.
 *
 * @param dest buffer to store the new string.
 * @param src  source string to copy to dest
 */
void remove_spaces(char *dest, char *src)
{
    // TODO
    if (src != NULL){
        int i = 0;
        int j = 0;
        for (i = 0; i < (int) strlen(src); i++) {
            if ( (int)(*(src + i)) == 32) {
                continue;
            } else {
                *(dest + j) = *(src + i);
                j++;
            }
        }
        *(dest + j) = '\0';
    } else {
        return;
    }


}

/**
 * Copies a token from a string (delimited by a set of characters) into a
 * destination buffer.
 *
 * @param dest buffer to store the token. Will be NUL terminated.
 * @param src  source string that contains the token
 * @param delim string containing characters to use as delimiters that signal
 * the end of the token.
 *
 * return the number of characters copied into dest, not including the NUL byte.
 */
size_t copy_token(char *dest, char *src, char *delim)
{
    // TODO
    if (dest == NULL) {
        return -1;
    }
    if (src == NULL) {
        return -1;
    }
    char* token = strtok(src, delim);
    memcpy(dest, token, sizeof(token));
    free(token);
    return strlen(dest);
}

/**
 * Given a search key, determine which column of the CSV it is associated with.
 *
 * @param line string to search for the key
 * @param key  search key
 *
 * @return the column associated with the key, or -1 on error.
 */
int key_to_col(char *line, char *key)
{
    // TODO
    if (line == NULL) {
        return -1;
    }
    if (key == NULL) {
        return -1;
    }
    int i = 0;
    char* delim = ",";
    char* token = strtok(line, delim);
    while (token != NULL) {
        if (strlen(token) == strlen(key)) {
            if (memcmp(token, key, strlen(token) == 0)) {
                free(delim);
                free(token);
                return i;
            } else {
                token = strtok(NULL, delim);
            }
        } else {
            token = strtok(NULL, delim);
        }
    }
    free(delim);
    free(token);
    return -1;
}

/**
 * Given a column number, determine its position within a string (as a number of
 * characters). Each comma in the CSV (,) represents one column.
 *
 * @param line string to search for the column
 * @param column column number to search for
 *
 * @return start of the column (in characters). Note that the remainder of the
 * string is still present; you will need to remove any following columns.
 */
size_t find_col(char *line, int column)
{
    // TODO
    if (line == NULL) {
        return -1;
    }
    if (column < 0) {
        return -1;
    }
    char *delim = ",";
    char *token = strsep(line, delim);
    int index = 0;
    size_t res = 0;
    while (token != NULL) {
        if (index == column) {
            free(delim);
            token = strsep(line, delim);
            line = token;
            free(token);
            return res;
        } else {
            token = strsep(line, delim);
            index++;
            res += strlen(token);
        }
    }
    return -1;
}

/**
 * Prints the usage information of the program to stderr.
 *
 * @param argv the argument array passed to the program (so we can display the
 * program's name in the help messages)
 */
void print_usage(char *argv[])
{
    fprintf(stderr, "Usage: %s '<query>'\n", argv[0]);
    fprintf(stderr, "%s expects input in CSV format from stdin.\n", argv[0]);
    fprintf(stderr, "<query> is in the format 'search_key [>|<|=] value'\n");
    fprintf(stderr, "Examples:\n");
    fprintf(stderr, "    - %s 'NOC > 3' < data.csv\n", argv[0]);
    fprintf(stderr, "    - cat data.csv | %s 'VEH = 2' | %s 'ST = 2'\n",
            argv[0], argv[0]);
}

int main(int argc, char *argv[])
{
    if (argc != 2) {
        fprintf(stderr, "Incorrect number of arguments.\n");
        print_usage(argv);
        return EXIT_FAILURE;
    }

    if (isatty(STDIN_FILENO)) {
        fprintf(stderr, "WARNING: No input file detected on stdin.\n");
    }

    char query[MAX_QUERY] = { 0 };
    char search_key[MAX_QUERY] = { 0 };
    char operator = 0;
    long value = 0;

    remove_spaces(query, argv[1]);
    LOG("Query string: %s\n", query);

    /* Grab just the first part (search key) of the query. For example,
     * BROADBND=1 would just return BROADBND */
    ssize_t result = copy_token(search_key, query, "><=");
    if (result == 0) {
        /* No search key? */
        fprintf(stderr, "Invalid query string: %s\n", query);
        print_usage(argv);
        return EXIT_FAILURE;
    }

    size_t op_loc = strcspn(query, "><=");
    if (op_loc == strlen(query)) {
        /* Didn't find the characters in the string so strcspn == its length */
        fprintf(stderr, "Invalid query string: %s\n", query);
        print_usage(argv);
        return EXIT_FAILURE;
    }
    operator = query[op_loc];

    char *convend;
    value = strtol(&query[op_loc + 1], &convend, 10);
    if (convend == &query[op_loc + 1]) {
        fprintf(stderr, "Invalid query string: %s\n", query);
        print_usage(argv);
        return EXIT_FAILURE;
    }

    LOG("Parsed query: [key=%s], [operator=%c], [value=%ld]\n",
            search_key, operator, value);

    /* Read the first line from stdin and print it out. This ensures we
     * reproduce the CSV header so the output can be piped to another invocation
     * of the program. */
    char line[MAX_LINE];
    char *lp = fgets(line, MAX_LINE, stdin);
    if (lp == NULL) {
        perror("fgets");
        return EXIT_FAILURE;
    }
    printf("%s", line);

    int key_col = key_to_col(line, search_key);
    LOG("Key column: %d\n", key_col);
    if (key_col == -1) {
        fprintf(stderr, "ERROR: Search key not found!\n");
        return EXIT_FAILURE;
    }

    /* TODO: Steps:
     *
     * 1. Loop through each line of the file
     *
     * 2. find the column of the search key you're looking for. For example, if
     *    your query is for BROADBND, then find what column that heading is
     *    associated with.
     *
     * 3. Convert the string in that column to a long integer. We'll assume all
     *    the entries in the dataset are long ints.
     *
     * 4. Based on the operator provided in the query string, evaluate the
     *    expression. If the query matches, then print out the line. If not,
     *    continue on to the next line.
     */

    /*
    while ( ... ) {

    }
    */
}
