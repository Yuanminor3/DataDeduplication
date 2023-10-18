#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <string.h>
#include <dirent.h>

#define PATH_MAX 1024
#define BUFFER_SIZE 1024

int main(int argc, char* argv[]) {
    if (argc != 3) {
        printf("Usage: ./nonleaf_process <directory_path> <pipe_write_end> \n");
        return 1;
    }

    //TODO(overview): fork the child processes(non-leaf process or leaf process) each associated with items under <directory_path>
    //TODO(step1): get <file_path> <pipe_write_end> from argv[]
    char* directory_path = argv[1];
    int pipe_write_end = atoi(argv[2]);

    //TODO(step3): open directory
    DIR* dir = opendir(directory_path);
    if (dir == NULL) {
        perror("opendir");
        exit(1);
    }

    //TODO(step4): traverse directory and fork child process
    // Hints: Maintain an array to keep track of each read end pipe of child process
    char child_data[1024][BUFFER_SIZE];
    int num_child_data = 0;

    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL) {
        // Skip . and ..
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        // Construct the full path to the entry
        char full_path[PATH_MAX];
        sprintf(full_path, "%s/%s", directory_path, entry->d_name);

        // Check if the entry is a directory
        if (entry->d_type == DT_DIR) {
            // If it's a directory, create a non-leaf process
            int pipe_fd[2];
            if (pipe(pipe_fd) == -1) {
                perror("pipe");
                exit(1);
            }

            // Fork a child process
            pid_t child_pid = fork();

            if (child_pid == -1) {
                perror("fork");
                exit(1);
            }

            if (child_pid == 0) {
                // Child process
                close(pipe_fd[0]); // Close the read end of the pipe

                // Execute the non-leaf program using exec
                execl("./nonleaf_process", "./nonleaf_process", full_path, pipe_fd[1], NULL);
                perror("execl"); // exec failed
                exit(1);
            } else {
                // Parent process
                close(pipe_fd[1]); // Close the write end of the pipe

                //TODO(step5): read from pipe constructed for child process and write to pipe constructed for parent process

                // Read data from the pipe and store it in the child_data array
                ssize_t bytes_read = read(pipe_fd[0], child_data[num_child_data], BUFFER_SIZE);
                if (bytes_read < 0) {
                    perror("read");
                    exit(1);
                }

                num_child_data++;
                close(pipe_fd[0]); // Close the read end of the pipe
            }
        } else if (entry->d_type == DT_REG) {
            // If it's a file, create a leaf process (similar to non-leaf process)
            int pipe_fd[2];
            if (pipe(pipe_fd) == -1) {
                perror("pipe");
                exit(1);
            }

            // Fork a child process
            pid_t child_pid = fork();

            if (child_pid == -1) {
                perror("fork");
                exit(1);
            }

            if (child_pid == 0) {
                // Child process
                close(pipe_fd[0]); // Close the read end of the pipe

                // Execute the leaf program using exec (replace with the actual program)
                execl("./leaf_process", "./leaf_process", full_path, pipe_fd[1], NULL);
                perror("execl"); // exec failed
                exit(1);
            } else {
                // Parent process
                close(pipe_fd[1]); // Close the write end of the pipe
                // Read data from the pipe and store it in the child_data array
                ssize_t bytes_read = read(pipe_fd[0], child_data[num_child_data], BUFFER_SIZE);
                if (bytes_read < 0) {
                    perror("read");
                    exit(1);
                }

                num_child_data++;
                close(pipe_fd[0]); // Close the read end of the pipe
            }
        }

    }

    int total_bytes = 0;
    for (int i = 0; i < num_child_data; i++) {
        total_bytes += strlen(child_data[i]);
    }

    //TODO(step2): malloc buffer for gathering all data transferred from child process as in root_process.c
    // Create a buffer large enough to hold all the data
    char* buffer = (char*)malloc(total_bytes + 1); // An additional byte is needed to store the string null-terminator '\0'

    if (buffer == NULL) {
        perror("malloc");
        exit(1);
    }

    int buffer_offset = 0;
    for (int i = 0; i < num_child_data; i++) {
        // Copy the child process data into the buffer
        strcpy(buffer + buffer_offset, child_data[i]);

        // Update the buffer offset
        buffer_offset += strlen(child_data[i]);
    }
    // Ensure the buffer is null-terminated
    buffer[total_bytes] = '\0';

    // Write the concatenated buffer to pipe_write_end
    write(pipe_write_end, buffer, BUFFER_SIZE);

    free(buffer);// Free allocated memory
    closedir(dir); // Close the directory
    close(pipe_write_end); // close pipe write end from parent
    return 0;
}
