/* help.h */

/* friendly messages */

#ifndef _HELP_H_

#include <stdio.h>

void usage   (FILE* f);
void help    (FILE* f);
void version (FILE* f);

void version(FILE* f)
{

#ifndef DURATION_VERSION
    #error DURATION_VERSION is not Defined yet
#endif
  fprintf( f, "%s version:\t %s  date:\t%s\n", "duration",  DURATION_VERSION, DURATION_DATE );
}



/* usage message */
void usage(FILE* f)
{
  version( f );

  char * usage = "usage: duration <filename1> <filename2> ...\n"
  "options:\n"
  "\t -b\tbitrate only\n"
  "\t -d\tduration only\n"
  "\t -h\tprint this message \n"
  "\t -q\tquiet \n"
  "\t -r\tbitrate only \n"
  "\t -t\ttimecode only \n"
  "\t -v\tverbose \n"
  "\t -V\tversion \n"
  "\n\tnote: (*) processes all files in path\n";

  fprintf( f, "%s" , usage ); 
}




#define _HELP_H_
#endif

/*END*/
