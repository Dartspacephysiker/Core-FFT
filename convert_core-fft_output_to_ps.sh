#!/bin/bash

while [[ $# -gt 1 ]]
do
    key="$1"

    case $key in
	-d|--def)
	    GRAYDEFFILE="$2"
	    shift # past argument
	    ;;
	-i|--input)
	    INFILE="$2"
	    shift # past argument
	    ;;
	-o|--output)
	    OUTFILE="$2"
	    shift # past argument
	    ;;
	-r|--outdir)
	    OUTDIR="$2"
	    shift # past argument
	    ;;
	-n|--noprompt)
	    NOPROMPT="$2"
	    shift # past argument
	    ;;
	# --default)
	#     DEFAULT=YES
	#     ;;
	*)
	    # unknown option
	    ;;
    esac
    shift # past argument or value
done

# [ -z "${GRAYDEFFILE}" ] && GRAYDEFFILE="gray.ngdef"

# if [ \( -z "${INFILE}" \) -o \( -z "${GRAYDEFFILE}" \) ]; then

if [ -z "${INFILE}" ]; then

    while true; do
	read -p "Enter name of input file (or type 'quit' or 'ls [dir]'):" yn
	case $yn in
	    ls*)
		ls "${yn##ls }";;
            Q|quit|Quit|exit)
		echo "OK! Exiting ..."
		exit
		break;;
            *)
		INFILE=${yn};
		break;;
	esac
    done
fi

if [ -z "${GRAYDEFFILE}" ]; then

    while true; do
	read -p "Enter name of gray definition file (or type 'quit' or 'ls [dir]' or 'default):" yn
	case $yn in
	    ls*)
		ls "${yn##ls }";;
            Q|quit|Quit|exit)
		echo "OK! Exiting ..."
		exit
		break;;
	    default)
		GRAYDEFFILE="gray.ngdef";
		break;;
            *)
		GRAYDEFFILE=${yn};
		break;;
	esac
    done
fi

[ -z "${OUTFILE}"     ] && OUTFILE="`basename ${INFILE%%.data}`.pdf"

[ -n "${GRAYDEFFILE}" ] && echo "GRAY DEF FILE   = ${GRAYDEFFILE}"
[ -n "${INFILE}"      ] && echo "IN  FILE        = ${INFILE}     "
[ -n "${OUTFILE}"     ] && echo "OUT FILE        = ${OUTFILE}    "
[ -n "${OUTDIR}"      ] && echo "OUT DIR         = ${OUTDIR}     "

datafile=${INFILE[*]}
[ -z "${OUTDIR}"      ] && OUTDIR="./"

while true; do

    if [ ! -z "$NOPROMPT" ]; then
    	# echo "\$NOPROMPT is NOT empty"
	case $NOPROMPT in
	    [1]*) yn=Y;;
	    *) 	read -p "Do it?" yn;;
	esac
    else
	# echo "\$NOPROMPT is empty"
	read -p "Do it?" yn
    fi

    case $yn in
        [Yy]* )    for file in ${INFILE}; do
		       echo "Processing $file..."
		       gray -d ${GRAYDEFFILE} ${file}
		       ps2pdf gray.ps ${OUTDIR}/${OUTFILE}
		       mv gray.ps ${OUTDIR}/lastgray.ps
		       # echo "gray -d ${GRAYDEFFILE} ${file}"
		       # echo "ps2pdf gray.ps ${OUTDIR}/${OUTFILE}"
		       # echo "rm gray.ps			  "
		       echo "Check out ${OUTDIR}/${OUTFILE}"
		   done; break;;
        [Nn]* ) break;;
        * ) echo "It's not that simple.";;
    esac
done
