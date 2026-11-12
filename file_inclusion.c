#include "file_inclusion.h"
#include <ctype.h>
#include <stdio.h>
#include <string.h>

#define MAX_INCLUDE_DEPTH 20

static void process_file(FILE *src, FILE *dest, int depth)
{
    char line[2048];

    if (depth > MAX_INCLUDE_DEPTH) {
        fprintf(stderr, "Error: maximum include depth exceeded\n");
        return;
    }

    while (fgets(line, sizeof line, src) != NULL) {
        char *p = line;

        while (isspace((unsigned char)*p)) p++;

        if (strncmp(p, "#include", 8) == 0 &&
            (isspace((unsigned char)p[8]) || p[8] == '"' || p[8] == '<')) {
            char filename[512];
            char closing;
            size_t i = 0;
            FILE *header;

            p += 8;
            while (isspace((unsigned char)*p)) p++;

            if (*p == '"') {
                closing = '"';
            } else if (*p == '<') {
                /* Keep system headers such as stdio.h unchanged. */
                fputs(line, dest);
                continue;
            } else {
                fputs(line, dest);
                continue;
            }

            p++;
            while (*p != '\0' && *p != closing && i < sizeof filename - 1)
                filename[i++] = *p++;
            filename[i] = '\0';

            if (*p != closing || i == 0) {
                fputs(line, dest);
                continue;
            }

            header = fopen(filename, "r");
            if (header == NULL) {
                /* Preserve unresolved local include so the limitation is visible. */
                fputs(line, dest);
                continue;
            }

            process_file(header, dest, depth + 1);
            fclose(header);
        } else {
            fputs(line, dest);
        }
    }
}

void process_includes(FILE *src, FILE *dest)
{
    process_file(src, dest, 0);
}
