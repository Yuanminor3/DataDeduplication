Project group number: 33

Group Member:

    Yuan Jiang ( jian0655 )
    Matthew Olson ( olso9444 )
    Thomas Nicklaus ( nickl098 )

CSELabs computer:

    csel-kh1250-06.cselabs.umn.edu
    
Plan outlining individual contributions for each member of your group:

    Yuan: (ALL) 3.1.1 3.1.2 3.1 3.2

    Matt: 3.1.2 3.1.2

    Thomas:


Any changes you made to the Makefile or existing files that would affect grading:

    Add the following in Makefile:
            outfolder:
            mkdir -p -m 0777 output/inter_submission
            mkdir -p -m 0777 output/final_submission
            mkdir -p -m 0777 output/inter_submission/root1
            mkdir -p -m 0777 output/inter_submission/root2
            mkdir -p -m 0777 output/inter_submission/root3

        Before submission we run "make outfolder" to create folders first.


Any assumptions that you made that weren’t outlined in 7. Assumptions / Notes:

    pre-defined:
        #define PATH_MAX 1024
        #define BUFFER_SIZE 4098

How you designed your program for creating the process tree (again, high-level pseudocode would be acceptable/preferred for this part)

    "leaf_process.c":

        data[]
        write hash_data_block(hashValue, file_path) to data[]
        write data to pipe_write_end give atoi(argv[2]) from parent

    "nonleaf_process.c":

        malloc: int** data[int*][char*]
        open dir:
            if file:
                fork() and exec()
                child process: exec(leaf_process.c), read data(filename and hash contents)
                parent proces: write the read data to data[][]
            if dir:
                fork() and exec()
                child process: exec(nonleaf_process.c), do read data(filename and hash contents) from files or files in subDir
                parent proces: write the read data to data[][]

            combine data[][] to pipe_write_end give atoi(argv[2]) from parent

    "root_process.c":
        
        Have collected a long string that contains all leaf files names as well as their hashes
        fork() and exec()
                child process: exec(nonleaf_process.c), read data(all following filename and hash contents)
                parent proces: write the read data to all_filepath_hashvalue

        Using helper function "parse_hash()" in "util.c". Iterate them and retain the file with smallest file number
            malloc file_list[] and retain_list[]
            parse_hash()

        Delete duplicate files and create a symbolic link for each of them pointing to unique file and free all malloc memory
            delete_duplicate_files(dup_list,size);
            create_symlinks(dup_list, retain_list, size);
            redirection(dup_list, size, argv[1]); //redirection outputs to a txt file and print all messages needed

            free(...)

If your original design (intermediate submission) was different, explain how it changed Any code that you used AI helper tools to write with a clear justification and explanation of the code (Please see below for the AI tools acceptable use policy):

    Mostly following ideas of intermediate submission.
    
    Googled and learn how to malloc: int** data[int*][char*] in "nonleaf_process.c"
    Googled and learn how to read symbolic link contents by readlink in "root_process.c"