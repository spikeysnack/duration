#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <regex.h>
#include <errno.h>
#include <error.h>



#ifndef DURATION_H
#define DURATION_H

#define DURATION_VERSION  "1.0"
#define DURATION_DATE     "1 Jan 2018"

#define DEBUG 1
#define BUFSIZE 256
#define MAXLINES 50

/* some C suites do not define bool */
#ifndef bool
typedef enum{false, true} bool;
#endif



typedef struct 
{ 
  char* fname;
  char* durstr;  
  unsigned duration;
  unsigned rate;
}f_data;


bool check_ext( char* fn, char** list, size_t sz);
bool check_req(char* fname);
size_t array_count(char** a);
char** parse_output(char* cmd );  
char *find_file(const char* path, const char *file);
char* regex_match(char* in, char* match );
void fileinfo(char* f);
void get_dur_and_rate(f_data* result );
void print_results( f_data* result );



/* required binaries */
char* required[]={ "ffprobe", "awk", NULL};


/* rearrange arrays for the format you
   have most of first and searches will
   be generally faster */

/* video extensions */
char* vidext[]={ "mkv",  "mp4", "mpv", "avi", "flv", 
		 "webm", "wmv", "mpg", "ogv", "VOB", 
		 "wmv",  "ts" ,  NULL};



/* audio extensions */
char* audext[] = { "flac", "mp3", "ac3", "pcm", "mp2", 
		   "mpa",  "mpc", "ogg", "oga", "ape",  
		   "m4a", "aac", "aiff", "wma", "wav", 
		   "wv",  NULL };


#endif
