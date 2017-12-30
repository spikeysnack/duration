/* options.h */
#include <unistd.h>

#include "help.h"

#ifndef OPTIONS_H
#define OPTIONS_H

extern char* delim;

inline void options( int argc, char** argv);
void usage(FILE* f);
void version(FILE* f);

/* options bdhqv */

extern bool optbr_only;
extern bool optdur_only;
extern bool opttc_only;
extern bool optrate_only;
extern bool opthelp;
extern bool optquiet; 
extern bool optversion;
extern bool optverbose;



/* check options  (unistd.h)*/
void options( int argc, char** argv)
{

  int opt = 0; 

  while ((opt = getopt(argc, argv, "bdhqrtvV")) != -1) {
    switch (opt) 
      {

      case 'b':
	optbr_only = true;
	break;
      
      case 'd':
	optdur_only = true;
	break;

      case 'h':
	opthelp = true;
	break;

      case 'q':
	optquiet = true;
	break;

      case 'r':
	optrate_only = true;
	break;

      case 't':
	opttc_only = true;
	break;

      case 'v':
	optverbose = true;
	break;

      case 'V':
	optversion = true;
	break;

      default: /* '?' */
	break;
      }
  }

  /* logic overrides */
  if(optquiet)
    {
      optverbose = false;
    }

  if(optverbose)
    {
      optquiet = optbr_only = optdur_only = opttc_only = optrate_only = false;      
    }



  errno = 0;
}/* options */
 
#endif
