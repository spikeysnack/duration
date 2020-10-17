# duration 

#### Description:  get info on media files


#### 1.0  date:	1 Jan 2018 release 
#### 1.4  date:	16 Oct 2020 update 

#### Free for all non-commercial purposes.
#### Copyright 2018 Chris Reid <http://spikeysnack@gmail.com>

### Usage: 

	duration <filename1> <filename2> ...
	options:
		-b	bitrate only
		-d	duration only
		-h	print this message 
		-q	quiet 
		-r	bitrate only 
		-t	timecode only 
		-v	verbose 
		-V	version 

	note: (*) processes all files in path


### Instructions 

	duration <option> <filename> |  list


#### Files

    /usr/local/bin/duration

   

### Install
	test:  make test

	install make install
	    If the user has a home bin directory,
	    the files are placed there. Otherwise
	    they are put in /usr/local/bin.
			
			
### License
    
    Author: Chris Reid
    
    License: Free for all non-commercial purposes.
    
    License URL: <https://creativecommons.org/licenses/by-sa/3.0/>
    
    Modification: allowed but original attribution must be included.
    
    You are free to:
    
    Share — copy and redistribute the material in any medium or format
    
    Adapt — remix, transform, and build upon the material for any purpose,
    even commercially.
    
    Attribution — You must give appropriate credit,
    provide a link to the license, and indicate if changes were made.
    You may do so in any reasonable manner,
    but not in any way that suggests the licensor endorses you or your use.
    
    ShareAlike — If you remix, transform, or build upon the material,
    you must distribute your contributions under the same license
    as the original.
    
    Revocation — The licensor cannot revoke these freedoms as
    long as you follow the license terms.
    
    No Additional Restrictions — You may not apply legal terms
    or technological measures that legally restrict others
    from doing anything the license permits.


#### Output


     [normal]
     hours_of_work_for_nothing.mp3                     		 00:00:34.27		        34 s	       227 kb/s	
     
     [verbose]
     hours_of_work_for_nothing.mp3                     	
		 timecode:	 00:00:34.27 sec	
		 duration:	        34 sec
		 bitrate:	       227 kb/s

		Audio file with ID3 version 2.3.0, contains: MPEG ADTS, layer III, v1, 224 kbps, 48 kHz, JntStereo


	[duration]
	hours_of_work_for_nothing.mp3                     		duration:	        34 sec

	[bitrate]
	hours_of_work_for_nothing.mp3                     		bitrate:	       227 kb/s

	[rate]
	hours_of_work_for_nothing.mp3                     		bitrate:	    227000

	[timecode]
	hours_of_work_for_nothing.mp3                     		time:		00:00:34.27

	[QUIET]
		 00:00:34.27		        34 s	       227 kb/s	

	[QUIET DURATION]
	34

	[QUIET BITRATE]
	227 kb/s

	[QUIET RATE]
	227000

	[QUIET TIMECODE]
	00:00:34.27




**--- get the info you need the way you need it! ---**
