#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

#include "jsmn/jsmn.h"


#define DATA_FILE_SUFFIX	".data"
#define JSON_TOKEN_NUM		256



/* parse a line in json type */
int parse(const char* str){
	jsmn_parser parser;
	jsmntok_t tokens[JSON_TOKEN_NUM];

	jsmn_init(&parser);

	int cnt = jsmn_parse(&parser, str, strlen(str), tokens, JSON_TOKEN_NUM);

	if(cnt < 0){
		printf("jsmn parse token cnt = %d\n", cnt);
		return 1;
	}

	return 0;
}


/* check whether str ended with suffix */
int EndWith(const char* str, const char* suffix){
	if(!str || !suffix)
		return 0;

	size_t lenstr = strlen(str);
	size_t lensuffix = strlen(suffix);
	if(lensuffix > lenstr)
		return 0;
	return strncmp(str + lenstr - lensuffix, suffix, lensuffix) == 0;
}


int main(int argc, char **args){
	
	if(argc == 1){
		printf("Error: no directory parameter given\n");
		return 1;
	}

	/* parsing files in the dedicated directory */	
	DIR* pDir = opendir(args[1]);		
	if(pDir == NULL){
		printf("Error: cannot open directory %s\n", args[1]);
		return 1;
	}

	int cnt = 0;
	struct dirent* pEnt;
	while((pEnt = readdir(pDir)) != NULL){
		/* skip non-use files */
		if(!EndWith(pEnt->d_name, DATA_FILE_SUFFIX))
			continue;

		/* open useful file */
		char path[128];
		sprintf(path, "%s/%s", args[1], pEnt->d_name);
		FILE* pFile = fopen(path, "r");	
		if(pFile == NULL){
			printf("Error: cannot open file %s\n", pEnt->d_name);
			continue;
		}

		/* read line in opened file */
		fseek(pFile, 0L, SEEK_END);
		int size = (size_t) ftell(pFile);
		size = (size / 8 + 1) * 8; /* align to 8 */ 
		fseek(pFile, 0L, SEEK_SET);

		char* line = (char*) malloc(size);
		if(fgets(line, size, pFile) == NULL){
			printf("Error: reading lines failed\n");
			free(line);
			continue;
		}
		
		//printf("aligned size:%d\n%s\n", size, line);

		if(parse(line)){
			printf("file name: %s\n", pEnt->d_name);
		}




		
		free(line);
		fclose(pFile);

		cnt ++;
		if(cnt == 128)
			break;
	}

	closedir(pDir);
	return 0;
}