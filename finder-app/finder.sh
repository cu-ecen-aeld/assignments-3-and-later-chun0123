#!/bin/bash

filesdir=$1
searchstr=$2

#if parameters are not NULL
if [ ! -z "${filesdir}" ] && [ ! -z ${searchstr} ];
then
	#if direction exist
	if [ -d "${filesdir}" ];
	then
		echo "The number of files are $(ls "${filesdir}" | wc -l) and the number of matching lines are $(grep -r ${searchstr} "${filesdir}" | wc -l)"
		exit 0
	else
		echo "direction doesn't exist!"
		exit 1
	fi
else
       	echo "Please input the filesdir and the searcher!"
	echo "$ 1 = $1, $ 2 = $2, filesdir = $filesdir, searchstr = $searchstr"
	exit 1
fi
