#!/bin/bash

# requires="ffprobe bc awk" # these should be standard
requires="ffprobe bc gawk|awk" # these should be standard

# known file extensions -- all others are ignored
VIDEXTS="mkv mp4 mpv avi flv webm wmv mpg ogv VOB wmv ts"
AUDEXTS="flac mp3 ac3 pcm mp2 mpa mpc ogg oga ape  m4a aac aiff wma wav wv"

# these are partial matches but wrong
PROBLEMEXT="m4 c"              # these are anomalies


vext=("${VIDEXTS}") # arrays
aext=("${AUDEXTS}")
problemext=("${PROBLEMEXT}")

check_req()
{
    OFS=$IFS

    local found
    local _OR
    
    for r in ${requires[@]}
    do	
	# if A|B clause, find one
	if [[ "${r}" =~ "|" ]] ; then
	    IFS="$IFS|" # add separator
	    
	    arr=("${r}") # make an array of possibilities
	    found=

	    for f in ${arr[@]} 
	    do
		# echo "f: ${f}" >2
		found=${found:-$(which "$f")} # try a few times	  
	    done

	    IFS=$OFS #restore separator

	else
	    found=$(which "$r")
	fi
	
	[[ ! $found ]] && echo "${r} not found in path. Must fix." && exit 1 
    done
    
} # check_req



function duration()
{
    KEY="Duration"
    AWK=$(which gawk)
    AWK=${AWK:-$(which awk)}
    FFP=$(which ffprobe)
    DURATION=$( ${FFP} -i "${1}"  2>&1  | grep "${KEY}" |  ${AWK} '{print $2}' | tr -d , )

    # echo $DURATION

    OLDIFS=$IFS
    IFS=':.' read -ra T <<< $(echo $DURATION)
    IFS=$OLDIFS

    HOURS="${T[0]}"
    MINUTES="${T[1]}"
    SECONDS="${T[2]}"
    FRAMES="${T[3]}"

    # for i in "${T[@]}"; do
    #     # process "$i"
    #     echo -e "$i"
    # done

    # echo "HOURS  : $HOURS"
    # echo "MINUTES: $MINUTES"
    # echo "SECONDS: $SECONDS"
    # echo "FRAMES : $FRAMES"

    DURSTR="(${HOURS} *3600) + (${MINUTES} * 60) + ${SECONDS} + (${FRAMES} / 75)"

    #    echo "DURSTR:  ${DURSTR}"

    NDUR=$(echo "scale=3; ${DURSTR}" | bc -l )

    echo "${NDUR}"

} #function duration


function sec_to_timecode()
{
    tt=$sum

    hrs=$(echo "$tt / 3600" | bc)

    tt=$(echo "$tt - ( $hrs * 3600 )" | bc )

    min=$(echo "$tt / 60" | bc )

    tt=$(echo "$tt - ($min *60)" | bc )

    sec=$(echo "$tt/1" | bc )

    tt=$(echo "$tt - $sec" | bc )

    frames=$(echo "$tt * 75"  | bc )
    frames=$(echo "$frames/1" | bc )


    hrs=$(printf "%02d\n" $hrs)

    min=$(printf "%02d\n" $min)

    sec=$(printf "%02d\n" $sec)

    frames=$(printf "%02d\n" $frames)

    timecode="$hrs:$min:$sec:$frames"

    echo "${timecode}"

} # function timecode




main ()
{
    check_req

    FileList=( "$@" ) # make an array from the args 

    n=${#FileList[@]} # number of elements

    # oops no files
    [ "${n}" -eq 0 ] && >&2 printf "no file(s) specified."   && exit 1

    #clean out non-files 
    for (( i=0; i<n; i++ ))
    do
	M="${FileList[$i]}"
	ext="${M##*.}"
	
	# not a file name
	[ ! -f "${M}" ] &&  unset FileList[$i]  && continue # make entry blank (remove element)

	# case of file name  with no extension
	test "${ext}" = "${M}" && unset FileList[$i]  && continue 

	# clean out problem files
	for P in ${problemext[@]}
	do
	    # file is on problematic filename  list
            [[ "$P" =~ "${ext}" ]] && unset FileList[$i] && break # make entry blank (remove element)
	done

    done # for i


    # add up  the durations 
    declare -a total  # store durations in array

    VDUR=
    ADUR=

    for F in "${FileList[@]}"
    do
	ext="${F##*.}"

	if [[ "${VIDEXTS}" =~ "${ext}" ]] ; then

	    VDUR=$(duration "$F")
	    total+=(${VDUR})
	    echo "${F} :  ${VDUR}"

	elif [[ "${AUDEXTS}" =~ "${ext}" ]] ; then

	    ADUR=$(duration "${F}")
	    total+=(${ADUR})
	    echo "${F} : ${ADUR}"

	else
	    continue
	fi

    done


    # for T in "${total[@]}"
    # do
    #     echo "$T"
    # done
    
    # use bc to add all durations together
    # by setting IFS to '+'
    # by building a string of 'a+b+c..'
    # the * in arr[*] means "all of arr separated by IFS"

    arr=( "${total[@]}" ) 
    IFS='+' sum=$(echo "scale=3; ${arr[*]} "|bc -l)
    echo $sum  # in seconds

    tc=$(sec_to_timecode ${sum}) # calculate timecode string

    echo "Timecode: ${tc}"  # this is the output 

    return 0
} # main


# execute main function
main "$@"



#END
