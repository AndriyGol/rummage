# rummage

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

  Build with     : `gcc -o rummage rummage.c`
                   `gcc -o rummage rummage.c -D MAC_OS (on mac machine)`
  
  Modifications  :
  
  Notes          : rammage_recursive() based on ls_s() from ls4.c written by Stewart Weiss
                   the program does not follow symbolic links
                   assumes there is no hard linked directories
