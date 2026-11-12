#include "comment_removal.h"

void remove_comments(FILE *src, FILE *dest)
{
    int ch, next;
    int state = 0; /* 0=normal, 1=string, 2=character, 3=line comment, 4=block comment */

    while ((ch = fgetc(src)) != EOF) {
        if (state == 0) {
            if (ch == '"') {
                state = 1;
                fputc(ch, dest);
            } else if (ch == '\'') {
                state = 2;
                fputc(ch, dest);
            } else if (ch == '/') {
                next = fgetc(src);
                if (next == '/') {
                    state = 3;
                } else if (next == '*') {
                    state = 4;
                } else {
                    fputc(ch, dest);
                    if (next != EOF) ungetc(next, src);
                }
            } else {
                fputc(ch, dest);
            }
        } else if (state == 1 || state == 2) {
            fputc(ch, dest);
            if (ch == '\\') {
                next = fgetc(src);
                if (next != EOF) fputc(next, dest);
            } else if ((state == 1 && ch == '"') ||
                       (state == 2 && ch == '\'')) {
                state = 0;
            }
        } else if (state == 3) {
            if (ch == '\n') {
                fputc('\n', dest);
                state = 0;
            }
        } else if (state == 4) {
            if (ch == '*') {
                next = fgetc(src);
                if (next == '/') {
                    state = 0;
                } else if (next != EOF) {
                    if (next == '\n') fputc('\n', dest);
                    ungetc(next, src);
                }
            } else if (ch == '\n') {
                fputc('\n', dest);
            }
        }
    }
}
