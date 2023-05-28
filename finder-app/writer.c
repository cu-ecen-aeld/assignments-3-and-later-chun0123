#include <stdio.h>
#include <stdlib.h>
#include <syslog.h>
#include <errno.h>

int main(int argc, char* argv[]){
	
	openlog("slog", LOG_DEBUG|LOG_ERR, LOG_USER);

	if(argc != 3){
		syslog(LOG_ERR, "Error: Incorrect format of input string");
		printf("Please input the writefile and writestr\n");
		return 1;
	}

	char *writefile = argv[1];
	char *writestr = argv[2];


	//open the file for writing
	FILE *fp = fopen(writefile, "w");


	if(fp == NULL){
		syslog(LOG_ERR, "Failed to open file for writing : %s", writefile);
		return 1;
	}


	//write the string to the file
	fprintf(fp, "%s\n", writestr);

	syslog(LOG_DEBUG, "Writing %s to %s", writestr, writefile);
	closelog();
	fclose(fp);

	return 0;

}

