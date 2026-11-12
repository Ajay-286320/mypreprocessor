#include "macro_handler.h"
#include <ctype.h>
#include <stdio.h>
#include <string.h>

#define MAX_MACROS 100
#define MAX_NAME 64
#define MAX_VALUE 512

struct Macro {
    char name[MAX_NAME];
    char value[MAX_VALUE];
    char parameter[MAX_NAME];
    int function_like;
};

static int find_macro(struct Macro macros[], int count, const char *name)
{
    int i;
    for (i = 0; i < count; i++)
        if (strcmp(macros[i].name, name) == 0)
            return i;
    return -1;
}

static void replace_parameter(const char *body, const char *parameter,
                              const char *argument, FILE *dest)
{
    const char *p = body;
    size_t n = strlen(parameter);

    while (*p) {
        if ((isalpha((unsigned char)*p) || *p == '_')) {
            char token[MAX_NAME];
            size_t i = 0;
            const char *start = p;

            while ((isalnum((unsigned char)*p) || *p == '_') &&
                   i < sizeof token - 1)
                token[i++] = *p++;
            token[i] = '\0';

            if (strcmp(token, parameter) == 0)
                fputs(argument, dest);
            else
                fwrite(start, 1, (size_t)(p - start), dest);
        } else {
            fputc(*p++, dest);
        }
    }
    (void)n;
}

void process_macros(FILE *src, FILE *dest)
{
    struct Macro macros[MAX_MACROS];
    int count = 0;
    char line[2048];

    while (fgets(line, sizeof line, src) != NULL) {
        char *p = line;
        while (isspace((unsigned char)*p)) p++;

        if (strncmp(p, "#define", 7) == 0 &&
            isspace((unsigned char)p[7])) {
            struct Macro *m;
            size_t i = 0;

            if (count >= MAX_MACROS)
                continue;

            p += 7;
            while (isspace((unsigned char)*p)) p++;

            m = &macros[count];
            while ((isalnum((unsigned char)*p) || *p == '_') &&
                   i < sizeof m->name - 1)
                m->name[i++] = *p++;
            m->name[i] = '\0';

            if (m->name[0] == '\0')
                continue;

            m->function_like = 0;
            m->parameter[0] = '\0';

            /* In C, a function-like macro has no whitespace before '(' */
            if (*p == '(') {
                size_t j = 0;
                m->function_like = 1;
                p++;
                while (isspace((unsigned char)*p)) p++;
                while ((isalnum((unsigned char)*p) || *p == '_') &&
                       j < sizeof m->parameter - 1)
                    m->parameter[j++] = *p++;
                m->parameter[j] = '\0';
                while (isspace((unsigned char)*p)) p++;
                if (*p != ')')
                    continue;
                p++;
            }

            while (*p == ' ' || *p == '\t') p++;
            strncpy(m->value, p, sizeof m->value - 1);
            m->value[sizeof m->value - 1] = '\0';
            m->value[strcspn(m->value, "\r\n")] = '\0';
            count++;
            continue;
        }

        p = line;
        while (*p) {
            if (*p == '"' || *p == '\'') {
                int quote = *p;
                fputc(*p++, dest);
                while (*p) {
                    int ch = *p++;
                    fputc(ch, dest);
                    if (ch == '\\' && *p) {
                        fputc(*p++, dest);
                    } else if (ch == quote) {
                        break;
                    }
                }
            } else if (isalpha((unsigned char)*p) || *p == '_') {
                char token[MAX_NAME];
                size_t i = 0;
                int index;

                while ((isalnum((unsigned char)*p) || *p == '_') &&
                       i < sizeof token - 1)
                    token[i++] = *p++;
                token[i] = '\0';

                index = find_macro(macros, count, token);
                if (index < 0) {
                    fputs(token, dest);
                } else if (!macros[index].function_like) {
                    fputs(macros[index].value, dest);
                } else {
                    char *q = p;
                    char argument[512];
                    size_t a = 0;

                    while (isspace((unsigned char)*q)) q++;
                    if (*q != '(') {
                        fputs(token, dest);
                        continue;
                    }
                    q++;
                    while (*q && *q != ')' && a < sizeof argument - 1)
                        argument[a++] = *q++;
                    if (*q != ')') {
                        fputs(token, dest);
                        continue;
                    }
                    argument[a] = '\0';
                    p = q + 1;
                    replace_parameter(macros[index].value,
                                      macros[index].parameter,
                                      argument, dest);
                }
            } else {
                fputc(*p++, dest);
            }
        }
    }
}
