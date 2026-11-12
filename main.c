#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "comment_removal.h"
#include "file_inclusion.h"
#include "macro_handler.h"

int main(int argc, char *argv[])
{
    FILE *input, *comments_removed, *includes_processed, *output;
    char *output_name;
    char *dot, *slash;

    if (argc != 2) {
        fprintf(stderr, "Usage: %s inputfile.c\n", argv[0]);
        return 1;
    }

    input = fopen(argv[1], "r");
    if (input == NULL) {
        perror("Cannot open input file");
        return 1;
    }

    comments_removed = tmpfile();
    includes_processed = tmpfile();
    if (comments_removed == NULL || includes_processed == NULL) {
        fprintf(stderr, "Cannot create temporary files\n");
        fclose(input);
        if (comments_removed != NULL) fclose(comments_removed);
        if (includes_processed != NULL) fclose(includes_processed);
        return 1;
    }

    output_name = malloc(strlen(argv[1]) + 3);
    if (output_name == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        fclose(input);
        fclose(comments_removed);
        fclose(includes_processed);
        return 1;
    }

    strcpy(output_name, argv[1]);
    dot = strrchr(output_name, '.');
    slash = strrchr(output_name, '/');
    if (dot != NULL && (slash == NULL || dot > slash))
        strcpy(dot, ".i");
    else
        strcat(output_name, ".i");

    output = fopen(output_name, "w");
    if (output == NULL) {
        perror("Cannot create output file");
        free(output_name);
        fclose(input);
        fclose(comments_removed);
        fclose(includes_processed);
        return 1;
    }

    remove_comments(input, comments_removed);
    rewind(comments_removed);

    process_includes(comments_removed, includes_processed);
    rewind(includes_processed);

    process_macros(includes_processed, output);

    printf("Preprocessing completed successfully!\n");
    printf("Output file: %s\n", output_name);

    fclose(input);
    fclose(comments_removed);
    fclose(includes_processed);
    fclose(output);
    free(output_name);
    return 0;
}
