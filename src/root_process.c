#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include "../include/utils.h"

#define WRITE (O_WRONLY | O_CREAT | O_TRUNC)
#define PERM (S_IRUSR | S_IWUSR)
#define BUFFER_SIZE 4098

char *output_file_folder = "output/final_submission/";

void redirection(char **dup_list, int size, char* root_dir){
    // TODO(overview): redirect standard output to an output file in output_file_folder("output/final_submission/")
    // TODO(step1): determine the filename based on root_dir. e.g. if root_dir is "./root_directories/root1", the output file's name should be "root1.txt"

    // Determine the filename based on root_dir.
    char output_filename[1024];
    char* last_slash = strrchr(root_dir, '/'); // get out last char after final "/" to determine the filename
    snprintf(output_filename, sizeof(output_filename), "%s.txt", last_slash + 1); // last_slash is the pointer of "/" and we need move one more to get file name

    // Create the full output file path.
    char output_file_path[1024];
    snprintf(output_file_path, sizeof(output_file_path), "%s%s", output_file_folder, output_filename);

    //TODO(step2): redirect standard output to output file (output/final_submission/root*.txt)    
    int output_fd = open(output_file_path, WRITE, PERM);
    if (output_fd == -1) {
        perror("open");
        exit(1);
    }
    if (dup2(output_fd, STDOUT_FILENO) == -1) {
        perror("dup2");
        exit(1);
    }

    //TODO(step3): read the content each symbolic link in dup_list, write the path as well as the content of symbolic link to output file(as shown in expected)

    for (int i = 0; i < size; i++) {
        char target_path[PATH_MAX] = "";
        ssize_t len = readlink(dup_list[i], target_path, sizeof(target_path));
        if (len == -1){
            perror("readlink");
            return 1;
        }
        printf("[%s --> %s] : [%s --> %s]\n", "<path of symbolic link>", "<path of retained file>", dup_list[i], target_path);
    }

    close(output_fd);

}

void create_symlinks(char **dup_list, char **retain_list, int size) {
    //TODO(): create symbolic link at the location of deleted duplicate file
    //TODO(): dup_list[i] will be the symbolic link for retain_list[i]
    for (int i = 0; i < size; i++){
        if (symlink(retain_list[i], dup_list[i]) == -1){
            perror("symlink");
            exit(1);
        }
    }
}

void delete_duplicate_files(char **dup_list, int size) {
    //TODO(): delete duplicate files, each element in dup_list is the path of the duplicate file
    for (int i = 0; i < size; i++){
        //delete file
        if (remove(dup_list[i]) == -1){
            perror("remove");
            exit(1);
        }
    }

}

// ./root_directories <directory>
int main(int argc, char* argv[]) {
    if (argc != 2) {
        // dir is the root_directories directory to start with
        // e.g. ./root_directories/root1
        printf("Usage: ./root <dir> \n");
        return 1;
    }


    //TODO(overview): fork the first non_leaf process associated with root directory("./root_directories/root*")
    char* root_directory = argv[1];
    char all_filepath_hashvalue[4098]; //buffer for gathering all data transferred from child process
    memset(all_filepath_hashvalue, 0, sizeof(all_filepath_hashvalue));// clean the buffer
    
    //TODO(step1): construct pipe
    int pipe_fd[2];
    if (pipe(pipe_fd) == -1){
        perror("pipe");
        exit(1);
    }
    //TODO(step2): fork() child process & read data from pipe to all_filepath_hashvalue
    pid_t child_pid = fork();
    if (child_pid == -1){
        perror("fork");
        exit(1);
    }

    // if child process
    if (child_pid == 0){
        close(pipe_fd[0]); // close read end of the pipe
        char writePipe[sizeof(pipe_fd[1])];
        sprintf(writePipe, "%d", pipe_fd[1]);
        execl("./nonleaf_process", "./nonleaf_process", root_directory, writePipe, NULL);
        
        //execl("./leaf_process", "./leaf_process", "./root_directories/root1/sub_1/WorldStage_2.txt", "0", NULL);
        perror("execl"); 
        exit(1); // exec failed

    }else{ // if parent process

        close(pipe_fd[1]); // close write end of the pipe

        //Read date from the pipe and store it in all_filepath_hashvalue
        ssize_t bytes_read = read(pipe_fd[0], all_filepath_hashvalue, sizeof(all_filepath_hashvalue)); 
        if (bytes_read < 0) { 
            perror("read"); 
            exit(1); 
        } 
        close(pipe_fd[0]); // Close the read end of the pipe
        wait(NULL);
    }


    //TODO(step3): malloc dup_list and retain list & use parse_hash() in utils.c to parse all_filepath_hashvalue
    // dup_list: list of paths of duplicate files. We need to delete the files and create symbolic links at the location
    // retain_list: list of paths of unique files. We will create symbolic links for those files

    // malloc dup_list
    char* dup_list = (char*)malloc(sizeof(int)*100); 

    if (dup_list == NULL) {
        perror("malloc");
        exit(1);
    }

    // malloc retain_list
    char* retain_list = (char*)malloc(sizeof(int)*100);

    if (retain_list == NULL) {
        perror("malloc");
        exit(1);
    }
    int size; 
    size = parse_hash(all_filepath_hashvalue, dup_list, retain_list); 

    //TODO(step4): implement the functions
    delete_duplicate_files(dup_list,size);
    create_symlinks(dup_list, retain_list, size);
    redirection(dup_list, size, argv[1]);

    //TODO(step5): free any arrays that are allocated using malloc!

    free(dup_list);
    free(retain_list);


}
