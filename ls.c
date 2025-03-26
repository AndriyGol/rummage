/******************************************************************************
  Title          : rummage.c
  Author         : Andriy Goltsev
  Created on     : March 27, 2011
  Description    :  rummage recursively searches through the directory hierarchy whose root is the directory specified
                    on the command line, for all non-directory files that have not been accessed within the specified
                    time period, inclusively. If no directory is specified, the current working directory is used. The
                    time period is determined from the option letters, as described below.

  Usage          : rummage -m|h|d n [directory_name]
                   -m <minutes>
                    The specified time is given in minutes.
                    -h <hours>
                    The specified time is given in hours.
                    -d <days>
                    The specifieed time is given in days.

  Build with     : gcc -o rummage rummage.c
                   gcc -o rummage rummage.c -D MAC_OS (on mac machine)
  Modifications  :
  Notes          : rammage_recursive() based on ls_s() from ls4.c written by Stewart Weiss
                   the program does not follow symbolic links
                   assumes there is no hard linked directories
******************************************************************************/


#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

//compile on MAC OS
#ifdef MAC_OS
	#include<sys/syslimits.h>
#endif /*MAC_OS*/


#define   HERE      "."
#define   SEC_MIN   60
#define   SEC_HOUR  60*SEC_MIN
#define   SEC_DAY   24*SEC_HOUR
#define   TIME_FUNCTION() time(NULL) //define time nunction

double _total_seconds; //time frame in seconds
time_t _curr_time; //current time

char _opts[] = "m:h:d:"; //all possible options for getopt()

/*****************************************************************************
  setOpts(int, char**)
  sets _total_seconds variable according to given options. 
  on success returns the index in argv where file names start.
  on failure returns -1
******************************************************************************/
int setOpts(int, char**);

/****************************************************************************
 recursively process directories.
 based on ls_s from ls4.c written by Stewart Weiss
 ****************************************************************************/
void rummage_recursive              ( char dirname[] );

/****************************************************************************
 rummage() checks if dirname is a directory. if it is, starts rummage_recursive,
 if not prints file name to stderr
 ****************************************************************************/
void rummage              ( char dirname[] );

/****************************************************************************
  print file name followed by the time of last access
*****************************************************************************/
void print_file  ( char *path, time_t);


/*****************************************************************************
                               Main Program
*****************************************************************************/
int main(int argc, char *argv[])
{
    if ( argc == 1 )    // no arguments; return error code 1
        return 1;

    _curr_time = TIME_FUNCTION();
    int i;

    if((i = setOpts(argc, argv)) == -1) {
            return 1;
        };

    if(i == argc)
        rummage(HERE);
    else {
       // for each command line argument, display contents
        while ( i < argc ){
            rummage( argv[i] );
            i++;
        }
    }
    return 0;
}


int setOpts(int argc, char* argv[]){

    int c = getopt(argc, argv, _opts);
 
	int return_val = -1;
	
    switch(c){
        case 'm' :
            _total_seconds = SEC_MIN * atoi(optarg);
            return_val = optind;
			break;
        case 'h' :
            _total_seconds = SEC_HOUR * atoi(optarg);
            return_val = optind;
			break;
        case 'd' :
            _total_seconds = SEC_DAY * atoi(optarg);
            return_val = optind;
			break;
        case -1 : //no options found assume default 
            _total_seconds = SEC_DAY * atoi(argv[1]);
            return_val = 1 + optind;
			break;
        default:
            //fprintf(stderr, "Unknown option -%c", c);
            //getopt will print error message for us
            return_val = -1;
    }
	
        //check if other options are still present or _total_seconds is negative
        if( getopt(argc, argv, _opts) != -1 || _total_seconds < 0)
                return_val = -1; //set return value to -1
	
	return return_val;
}



void rummage (char dirname[]){

    struct  stat     statbuf;         //store stat results
    //check if dirname is a directory
    if (lstat(dirname, &statbuf) != -1
         && S_ISDIR(statbuf.st_mode)){
             rummage_recursive(dirname);
             return;
    }
    fprintf(stderr, "%s\n", dirname);
    return;
}



void rummage_recursive   ( char dirname[] )
{
    DIR              *dir;            // pointer to directory struct
    struct dirent    *dp;             // pointer to directory entry
    char             fname[PATH_MAX]; // string to hold full path name
    struct  stat     statbuf;         // to store stat results

    if ( ( dir = opendir( dirname ) ) == NULL )
        fprintf(stderr, "%s\n", dirname);
    else {
        // Loop through directory entries
        while ((dp = readdir(dir)) != NULL) {
            if (strcmp(dp->d_name, ".") == 0 || strcmp(dp->d_name,"..") == 0 )
                // skip dot and dot-dot entries
                continue;

            // construct a pathname for the file using the directory name
            // passed to the program and the directory entry            
            if(dirname[(strlen(dirname) - 1)] == '/') /*Test if dirname ands with '/' */
                sprintf(fname,"%s%s",dirname, dp->d_name);
            else
                sprintf(fname,"%s/%s",dirname, dp->d_name);


            // fill the stat buffer
            if (lstat(fname, &statbuf) == -1) {
                fprintf(stderr, "%s\n", fname);
                continue;      // stat call failed but we go on
            }


            if(S_ISDIR(statbuf.st_mode)) {
                rummage_recursive(fname); //recusive call to process directories
             }
            else { //if it is not a directory print it
                 print_file(fname, statbuf.st_atime);
             }

        }
        closedir(dir);
    }        
}



void print_file(char* path, const time_t t){
    if(difftime(_curr_time,t) > _total_seconds)
        printf("%s \t %s",path, ctime(&t));
    else return;
}

