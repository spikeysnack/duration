/* duration.c */

#include "duration.h"

#include "help.h"

#include "options.h"


typedef enum {SEC = 1, MIN=60, HOUR=3600} tsec; 


#define FFPROBE "ffprobe -i \"%s\" 2>&1 | grep Duration | awk \"{print $2}\" | tr -d , "

#define FILECMD "file -b "


/* boolean options bdtrhVv */
bool optbr_only    = false;
bool optdur_only   = false;
bool opttc_only    = false;
bool optrate_only  = false;
bool opthelp       = false;
bool optquiet      = false;
bool optversion    = false;
bool optverbose    = false;


/* if we have file  (optional) */

#ifndef HAVEFILE
bool have_file_bin = false; */
#else
bool have_file_bin = true;
#endif 


size_t array_count(char** a)
{
  size_t count = 0;
  while (a[count] != NULL) count++;
  return count;
}




/* check if we can know the extension
   based upon a list */

bool check_ext( char* fn, char** list , size_t sz)
{

  size_t i = 0;
  char* fext = NULL;
  char* ext  = NULL;
  bool ret = false;

  if ( !fn || !list) return false;

  /* get extension from filename */

  fext = strrchr( fn , '.');

  /* no extention */
  if (! fext )  return false;

  /* skip "." */
  fext = fext + 1;

  for (i=0; i<sz; i++)
    {
      ext = list[i];

      if ( ext[0] == fext[0] )
	{
	  if ( strncmp( fext, ext , strlen(ext)) == 0 ) { ret = true; break; }
	}
    }//for
return ret;

}// check_ext



/* invoke "file" binary for file info */
void fileinfo(char* f)
{
  //char fcmd[80] = {0};
char fcmd[256] = {0};
char** fileinfo = NULL;
size_t i,n;

  //fprintf(stdout, "%-50.50s\t" , f);

if (have_file_bin)
  {

      snprintf(fcmd , 255, "%s \"%s\"\n" , FILECMD , f );

      fileinfo = parse_output(fcmd);

      n = array_count(fileinfo);

      fprintf( stdout , "\t%s" , fileinfo[0] );

      for( i = 0; i < n; i++) 
        free(fileinfo[i]);
      free(fileinfo);
      }

} /* fileinfo */



/*
  run a command and capture its output
  up to MAXLINES lines of BUFSIZE chars per line
*/


char** parse_output(char* cmd ) 
{
  char buf[BUFSIZE];
  char ** ret = NULL;
  size_t i = 0;
  FILE *fp;

  errno = 0;
  /* 50 strings */
  ret = (char**)calloc(MAXLINES, sizeof(char*));
  
  /* run the command */
  if ((fp = popen(cmd, "r")) == NULL) 
    {
      fprintf(stderr, "Error opening pipe! {%s}\n", strerror(errno) );
      exit(errno);
    }


  while (fgets(buf, BUFSIZE, fp) != NULL) 
    {
      /* collect lines of output */
      ret[i++] = strndup(buf, strlen(buf));

      //      fprintf( stderr, "LINE: %s\n", buf);

      /* maxed out */
      if (i == MAXLINES) break;
    }


  errno = 0;

  /* close process and release resources */
  if(pclose(fp) != 0)  
    {
      fprintf( stderr, "Command not found or exited with error status {%s}\n", strerror(errno) );
      exit(errno);
    }
 
  return ret;
}



/* Find program for a given path and name.

   windows posix honors / in paths 
   but uses semicolon (;) 
   instead of colon (:) 
   for PATH variable 
*/

#ifdef _WIN32
    #define PATH_SEP ";"
#else
    #define PATH_SEP   ":"
#endif


char *find_file(const char* path, const char *file)
{
  char *p     = NULL; 
  char *s     = NULL;
  char* fname = NULL;

  char prog[256] = {0};
  size_t len = 0;
  struct stat st;
  int err;
  errno = 0;
  
 
  if(!path && !file ) return NULL;

/* strtok is destructive of its string */
  p = strndup(path, strlen(path));   

  for(s = strtok(p, PATH_SEP); s; s = strtok(NULL, PATH_SEP)) 
    {

      if (  sprintf(prog, "%s/%s", s,  file ) < 0 ) 
	{
	  fprintf(stderr, "sprintf failed\n");
	  exit(-1);
	}

      err = stat(prog, &st);

      if ( !err ) break;

      if (err == -1)
      	{
	  if( errno != ENOENT ) /* ignore FILE NOT FOUND */
	    { 
      	      fprintf(stderr, "stat %s failed {%s} \n", prog ,strerror(errno) );
      	      exit(errno);
	    }
      	}
           
      memset(prog , 0, 256); /* clear prog */
    } //for


  if (prog)
    { 
      len = strlen(prog)+1;

      fname = (char*)calloc(len , sizeof(char) );

      if (!fname)
	{
	  fprintf(stderr, "calloc of %zu  failed\n", len );
	  exit(-1);
	}

      fname = strncpy( fname, prog, len);   
    }


  free(p);

  return fname;
} /* find_file */



/* check we have the proper files in 
   our path to run this */

bool check_req(char* fname)
{

  char* prog = NULL;

  char *path = getenv("PATH");

  bool ret  = false;

  if ( (prog = find_file( path, fname)) )
    {
      
      errno = 0;

      if( access( prog, F_OK ) != -1 )
	{ 
	  ret = true;
	}
      else
	{
	  ret = false;
	  fprintf( stderr, "file  %s found but can't open it {%s}\n", prog, strerror(errno) );
	}
    }
  else
    {
      fprintf( stderr, "File %s not found \n", fname );
    }

  free(prog);

  return ret;
}



/* 
   search for a regex match
   extract the string and return a new one
   (must free)
*/


char* regex_match(char* in, char* match )
{ 
  int i;
  size_t beg;
  size_t end;  
  size_t sublen;
  size_t mlen;  
  char *start;
  char *fin; 
  
  regex_t re;
  regmatch_t rm[2];

  char substring[32] = {0};
  
  mlen = strlen(in);

  i = regcomp(&re, match, REG_EXTENDED);

  if ( i != 0)
    {
      fprintf(stderr, "regcomp failed to compile {%d}\n", i);
      exit(-1);
    }

  i = regexec(&re, in , 2, rm , 0);

  if ( i == REG_NOMATCH) 
    {
      regfree(&re);
      return NULL;
    }
  else
    {
      if ( i != 0)
	{
	  fprintf(stderr, "regexec failed\n");
	  exit(i);
	}
    }

  /* we got a match 
     the start of the match is rm.rm_so
     the end  of the match  is rm.rm_eo
     in bytes past in[0].
  */

  beg = rm[0].rm_so;  
  end = rm[0].rm_eo;

  /* fprintf(stderr , "beg %zu\n",  beg  ); */
  /* fprintf(stderr , "end  %zu\n", end ); */

  start = in + beg;
  fin   = in + end;
  sublen = fin - start;

  memcpy(substring , start , sublen );

  /* fprintf(stderr , "start %s\n", start ); */
  /* fprintf(stderr , "fin %s\n", fin ); */


  /* fprintf(stderr , "in %s\n", in ); */
  /* fprintf(stderr , "match %s\n", match ); */

  /* fprintf(stderr , "found %s\n", substring ); */


  regfree(&re);

  if (substring) 
    return strndup( substring, strlen(substring));
  else
    return NULL;
} //regex_match




/*
struct 
{ 
  char* fname;
  char* durstr;  
  unsigned duration;
  unsigned rate;
}f_data ;
*/


void get_dur_and_rate(  f_data* result )
{
 
  char ** ret = NULL;
  char cmd[256] = {0x0};
  char* timereg = "[0][0-9]:[0-9][0-9]:[0-9][0-9].[0-9][0-9]";
  char* ratereg = "[0-9]+ kb/s";
  char* durstr  = NULL;
  char* ratestr = NULL;

  size_t r;
  int hr, min, sec, ms;
  unsigned duration, rate;

  if (!result) 
    {
      fprintf(stderr, "%s result is null\n", __FUNCTION__);
      exit(-1);
    }

  
  sprintf(cmd, FFPROBE , result->fname ); 
  
  /* run ffrobe and capture output */
  ret = parse_output( cmd );  /* DO free this */

  /* int i = 0; */
  /* while ( ret[i] ) */
  /*   { */
  /*     fprintf(stdout , "line: %s \n" , ret[i]); */
  /*     i++; */
  /*   } */

  result->durstr  =  regex_match(ret[0], timereg  );

  sscanf( result->durstr, "%d:%d:%d.%d" , &hr , &min, &sec, &ms );

  result->duration = hr*3600 + min*60 + sec + ((ms>50)? 1:0);  
  /* fprintf(stdout, "time:\t%s\n" , durstr); */


  if(! optdur_only)
    {
      ratestr =  regex_match(ret[0], ratereg  );
      sscanf(ratestr, "%u kb/s", &result->rate);
  /* fprintf(stdout, "rate:\t%s\n" , ratestr); */
      if(ratestr) free(ratestr);
    }
  else
    result->rate = 0;


 cleanup:
  r = 0; while(ret[r]) free(ret[r++]); free(ret);

} /* get_dur_and_rate */





void print_results( f_data* result )
  {
    char* durstr = NULL;
    char* basename = NULL;
    
    unsigned duration = 0;
    unsigned rate     = 0;


  if (!result) 
    {
      fprintf(stderr, "%s result is null\n", __FUNCTION__);
      return;
    }

  /* get basename of file */
  basename = strrchr(result->fname, '/');

  if (basename)
    {
      basename = basename +1;
    }
  else
    {
	basename  = result->fname;
    }


    durstr = result->durstr;
    duration = result->duration;
    rate = result->rate;
    

    if (!optquiet) fprintf(stdout, "%-50s\t" , basename);
 
    if(optverbose) fprintf(stdout, "\n");


  /* get string we want */

    if(opttc_only)
      {
	if (!optquiet)   
	  {
	    if ( durstr )fprintf(stdout, "\ttime:\t\t%s\n", durstr);
	  }
	else
	  {
	    if(durstr) fprintf(stdout, "%s\n" , durstr);
	  }
	
	goto done;
      }

 
    if (optbr_only)
      {
	
	if (!optquiet)
	  {
	    if (rate)  fprintf(stdout, "\tbitrate:\t%10u kb/s\n" , rate);
	  }
       else
	 {
	   if (rate)  fprintf(stdout, "%u kb/s\n" , rate);
	 }
	goto done;

      }

    if (optrate_only)
      {
	
	if (!optquiet)
	  {
	    if (rate)  fprintf(stdout, "\tbitrate:\t%10u\n" , rate*1000);
	  }
       else
	 {
	   if (rate)  fprintf(stdout, "%u\n" , rate*1000);
	 }
	goto done;

      }

    if (optdur_only)
      {
	
	if (!optquiet)
	  {
	    if (duration) fprintf(stdout, "\tduration:\t%10u sec\n" , duration);
	  }
       else
	 {
	   if (duration)  fprintf(stdout, "%u\n" , duration);
	 }
	goto done;

      }


      
      if (optverbose)
	{
	  if (durstr)   fprintf(stdout, "\ttimecode:\t%12s sec\t\n" , durstr);

	  if (duration) fprintf(stdout, "\tduration:\t%10u sec\n" , duration);

	  if (rate)  fprintf(stdout, "\tbitrate:\t%10u kb/s\n" , rate);
	}
      else
	{ /* no option option */
	  if (durstr)   fprintf(stdout, "\t%12.12s\t\t" , durstr);

	  if (duration) fprintf(stdout, "%10u s\t" , duration);

	  if (rate)  fprintf(stdout, "%10u kb/s\t" , rate);
	}
 

  fprintf(stdout, "\n");

  done:
  fflush(stdout);

} // print_results




int main( int argc, char** argv , char** env )
{

  f_data results = {NULL, NULL, 0, 0 };

  size_t audext_sz = 0;
  size_t vidext_sz = 0;

  unsigned hours     = 0;
  unsigned min       = 0;
  unsigned sec       = 0;
  unsigned totaltime = 0;

  int optargs; 
  extern int optind;

/* gotta have at least 2 arguments */
  if (argc  < 2 )
    {
      fprintf(stderr, " no arguments given\n" );
      usage(stderr);
      exit(-1);
    }

  /* set flags */  
  options(argc, argv);

  /* help and exit */
  if( opthelp )
    {
      //      version(stdout);
      usage(stdout);
      exit(0);
    }

  /* version and exit */
  if(optversion)
    {
      version(stdout);
      exit(0);
    }

  /* how many non option args? */
  optargs = optind; 
  /* fprintf(stderr, "optargs:\t%d\n" , optargs); */

  /* do we have file (magic info)? */
  have_file_bin = check_req("file");

  int f = 0;

  /* check requirements */
  while (required[f])
    {
      char* fn = required[f];

      if (! check_req(fn))
	{
	  fprintf(stderr, "check_req failed. %s not found in path. exiting.\n" , fn);
	  exit(-1);
	}
      /* else */
      /* 	  fprintf(stderr, "%s found OK\n" , fn); */
	
      f++;
    }

 
  audext_sz = array_count(audext);
  vidext_sz = array_count(vidext);

  size_t a;
  /* check files for existence, access, type */
  for ( a = optargs; a < argc; a++)
    {
      char* thefile = argv[a];  /* never free this */

      if(  access( thefile, R_OK ) != 0 )
	{
	  fprintf(stderr, "argument file  %s not found\n", thefile );
	  exit(-1);
	}
      else
	{
	  if ( check_ext(thefile, audext, audext_sz) || check_ext(thefile, vidext, vidext_sz) )
	    {
   	      
	      results.fname = thefile; /* never free this */
	      results.duration = 0;
	      results.rate     = 0;

	      get_dur_and_rate(&results);
	      
	      print_results(&results);
	      
	      totaltime += results.duration;

	      free(results.durstr); /* must free this */

	      if (optverbose)
		{
		  fileinfo(thefile);
		}	      

	    }
	  else
	    {
	      fprintf(stdout , "%-50.50s" , thefile);
	      fileinfo(thefile);
	    }
	}//else
    } //for
  

  if (a > 3)
    {

      
      hours = totaltime / 3600;
      
      min   = (totaltime - (hours * 3600) ) / 60;

      if (min > 59) { hours++; min = min % 60; }

      sec  = totaltime - ((hours*3600) + (min*60) );

      if( sec > 59) { min++; sec = sec % 60;}

      if ( optverbose )
	{
 
	  fprintf(stdout , "total duration:\t%u sec \t " , totaltime);
	  
	  fprintf(stdout , "( %02u hours\t" , hours);
	  
	  fprintf(stdout , "%02u minutes\t" , min );
	  
	  fprintf(stdout , "%02u seconds )\n" , sec);
	  
	}
      else
	{
	  char hms[9] = {'\0'};
	  
	  snprintf(hms , 8, "%02u:%02u:%02u", hours, min, sec); 
	  
	  if(!optquiet) fprintf(stdout , "total:\t%02u sec \t %s\n" , totaltime, hms );
	  
	}
    }

  return 0;
} //main



