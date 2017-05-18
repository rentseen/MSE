#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

#include "jsmn/jsmn.h"


#define DATA_FILE_SUFFIX	".data"
#define JSON_TOKEN_NUM		512



/* parse a line in json type, set the position of lyrics */
/* return 1 if lyrics found, otherwise return 0 */
int find_lyrics(const char* line, int* start, int* end){
	jsmn_parser parser;
	jsmntok_t tokens[JSON_TOKEN_NUM];

	jsmn_init(&parser);

	int cnt = jsmn_parse(&parser, line, strlen(line), tokens, JSON_TOKEN_NUM);

	if(cnt < 0){
		printf("Error: jsmn parsing failed, returned %d\n", cnt);
		return 0;
	}

	int i;
	for(i = 0; i < cnt; i++){
		int size = tokens[i].end - tokens[i].start + 1;
		char* str = (char*) malloc(size + 1);
		snprintf(str, size, "%s", (char*)(line + tokens[i].start));
	
		/* check if the find the token "lyrics" */
		if(!strncmp(str, "lyrics", 6)){
			*start = tokens[i+1].start;
			*end = tokens[i+1].end;
			free(str);
			return 1;
		}
		
		free(str);
	}

	return 0;
}


/* check whether str ended with suffix */
int suffix(const char* str, const char* suffix){
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
		if(!suffix(pEnt->d_name, DATA_FILE_SUFFIX))
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

		/* start & end pos of lyrics */
		int lyrics_start, lyrics_end;

		if(!find_lyrics(line, &lyrics_start, &lyrics_end)){
			printf("Error: no lyrics found in file: %s\n", pEnt->d_name);
			continue;
		}

		int lyrics_len = lyrics_end - lyrics_start + 1;
		char* str = (char*) malloc(lyrics_len + 1);
		snprintf(str, lyrics_len, "%s", (char*)(line + lyrics_start));
		printf("%s has lyrics as:\n%s\n\n", pEnt->d_name, str);
		free(str);
		
		free(line);
		fclose(pFile);

		cnt ++;
		
	}

	closedir(pDir);
	return 0;
}