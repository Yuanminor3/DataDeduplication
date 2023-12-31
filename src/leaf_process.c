#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "../include/hash.h"
#include "../include/utils.h"

#define BUFFER_SIZE 1024

char *output_file_folder = "output/inter_submission/";

int main(int argc, char* argv[]) {
    if (argc != 3) {
        printf("Usage: Inter Submission --> ./leaf_process <file_path> 0\n");
        printf("Usage: Final Submission --> ./leaf_process <file_path> <pipe_write_end>\n");
        return -1;
    }

    //TODO(): get <file_path> <pipe_write_end> from argv[]
    char *file_path = argv[1];
    int pipe_write_end = atoi(argv[2]);

    //TODO(): create the hash of given file
    char hashValue[1024];
    hash_data_block(hashValue, file_path);

    //TODO(): construct string write to pipe. The format is "<file_path>|<hash_value>"
    char pipe_message[1024];
    snprintf(pipe_message, sizeof(pipe_message), "%s|%s|", file_path, hashValue);


    if(pipe_write_end == 0){
        //TODO(inter submission)
        //TODO(overview): create a file in output_file_folder("output/inter_submission/root*") and write the constructed string to the file
        

        //TODO(step1): extract the file_name from file_path using extract_filename() in utils.c
        char *file_name = extract_filename(file_path);
        //TODO(step2): extract the root directory(e.g. root1 or root2 or root3) from file_path using extract_root_directory() in utils.c
        char *root_directory = extract_root_directory(file_path);
        //TODO(step3): get the location of the new file (e.g. "output/inter_submission/root1" or "output/inter_submission/root2" or "output/inter_submission/root3")
        char file_location[PATH_MAX];
        snprintf(file_location, PATH_MAX, "%s%s", output_file_folder, root_directory);

        //TODO(step4): create and write to file, and then close file

        // Create and write to the file
        char output_file[PATH_MAX];
        snprintf(output_file, PATH_MAX, "%s/%s", file_location, file_name);
        FILE *file = fopen(output_file, "w");
        if (file == NULL) {
            perror("fopen");
            exit(1);
        }

        fputs(pipe_message, file);

        fclose(file);

        //TODO(step5): free any arrays that are allocated using malloc!! Free the string returned from extract_root_directory()!! It is allocated using malloc in extract_root_directory()
        // Free allocated memory
        //free(file_name);
        free(root_directory);

    }else{
        //TODO(final submission): write the string to pipe
        write(pipe_write_end, pipe_message, strlen(pipe_message));
        exit(0);

    }

    exit(0);
}
