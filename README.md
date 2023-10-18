
Project group number: 33

Group Member:
    Yuan Jiang ( jian0655 )
    Matthew Olson ( olso9444 )
    Thomas Nicklaus ( nickl098 )

CSELabs computer: csel-kh1250-35.cselabs.umn.edu

Any changes you made to the Makefile or existing files that would affect grading:

    Add the following in Makefile:
        outfolder:
        mkdir -p -m 0777 output/inter_submission
        mkdir -p -m 0777 output/final_submission
        mkdir -p -m 0777 output/inter_submission/root1
        mkdir -p -m 0777 output/inter_submission/root2
        mkdir -p -m 0777 output/inter_submission/root3

    Before submission we run "make outfolder" to create folders first.

    Assume:
        #define PATH_MAX 1024
        #define BUFFER_SIZE 1024

    Add some code in nonleaf_process.c but haven't tested it.
    

Plan outlining individual contributions for each member of your group:
    Yuan: 3.1.1 3.1.2 
    Matt: 3.1.2
    Thomas:

Plan on how you are going to construct the pipes and inter-process communication.  (high-level pseudocode would be acceptable/preferred for this part):

    First, do "nonleaf_process.c":

        data[][]
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

    Second, do "root_process.c":
        have collected a long string that contains all leaf files names as well as their hashes.
        Using helper function "parse_hash()" in "util.c". Iterate them and retain the file with smallest file number

    Third, delete duplicate files and create a symbolic link for each of them pointing to unique file and free all malloc memory