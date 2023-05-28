#!/bin/bash

writefile=$1
writestr=$2

filename=$(basename "${writefile}")
filedir=$(dirname "${writefile}")

#echo "filename = ${filename}";
#echo "writefile = ${writefile}";
#echo "writestr = ${writestr}";


if [ -z ${writefile} ] && [ -z ${writestr} ];
then
	echo "Please input the writefile and writestr";
	exit 1
else
	mkdir -p ${filedir}
	touch ${filename}
	echo ${writestr} >> ${filename}
	mv -f ${filename} ${filedir}

	if [ -d "${writefile}" ]
	then
		echo "Created failed";
		exit 1
	else
		#echo "Created successfully";
		exit 0

	fi
fi

