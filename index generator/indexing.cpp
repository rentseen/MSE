#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

#include "jsmn/jsmn.h"
#include "THULAC/include/thulac.h"


#define DATA_FILE_SUFFIX	".data"
#define JSON_TOKEN_NUM		512



/* parse a line in json type, set the position of lyrics */
/* return 1 if lyrics found, otherwise return 0 */
int find_lyrics(const char* line, int* start, int* end){
	/* init jsmn parser */
	jsmn_parser parser;
	jsmntok_t tokens[JSON_TOKEN_NUM];
	jsmn_init(&parser);

	/* call for jsmn to parse json string */
	int cnt = jsmn_parse(&parser, line, strlen(line), tokens, JSON_TOKEN_NUM);
	if(cnt < 0){
		printf("Error: jsmn parsing failed, returned %d\n", cnt);
		return 0;
	}

	/* finding token "lyrics" */
	int i;
	for(i = 0; i < cnt; i++){
		int size = tokens[i].end - tokens[i].start;
		char* str = new char[size + 1];
		strncpy(str, (char*)(line + tokens[i].start), size);
		str[size] = '\0';
		//std::cout<<"json: "<<str<<"#"<<std::endl;
		if(!strncmp(str, "lyrics", 7)){
			*start = tokens[i+1].start;
			*end = tokens[i+1].end;
			delete str;
			return 1;
		}
		
		delete str;
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

/* divide lyrics into english and chinese */
void divide_lang(const char* lyrics, char* lyrics_en, std::string* lyrics_cn){
	size_t len = strlen(lyrics);
	int i;
	int pos_en = 0;

	for(i = 0; i < len; i++){
		if(!(lyrics[i] & 0x80)){
			/* ascii code */ 
			lyrics_en[pos_en++] = lyrics[i];
		}
		else if(!(lyrics[i] & 0x20)){
			/* skip 2-byte unicode */
			i += 1;
		}
		else if(!(lyrics[i] & 0x10)){
			/* utf-8 for chinese character */
			if(i + 3 > len){
				/* skip garbage at the end */
				std::cout<<std::endl<<"skip garbage here"<<lyrics[i]<<std::endl;
				break;
			}
			*lyrics_cn += lyrics[i++];
			*lyrics_cn += lyrics[i++];
			*lyrics_cn += lyrics[i];
		}
		else if(!(lyrics[i] & 0x08)){
			/* skip 4-byte unicode */
			i += 3;
		}
	}
	lyrics_en[pos_en] = '\0';
}

/* print the result of THULAC */
void print(const THULAC_result& result, bool seg_only, char separator) {
    for(int i = 0; i < result.size() - 1; i++) {
        if(i != 0) 
        	std::cout<<" ";
        if(seg_only) {
            std::cout << result[i].first;
        }
        else {
            std::cout << result[i].first << separator << result[i].second;
        }
    }
    std::cout<<std::endl;
}


int main(int argc, char **args){

	if(argc < 3){
		printf("Error: no enough directory parameter given, \
			DATA_DIRECTORY, MODELS_DIRECTORY needed\n");
		return 1;
	}

	/* init the THULAC, load models */
	char* model_path = args[2];
	char* user_dict_name=NULL;
	bool seg_only = true;
	bool useT2S = false;
	bool useFilter = false;
	char separator = '/';
	THULAC lac;
	lac.init(model_path, user_dict_name, seg_only, useT2S, useFilter);

	/* parsing files in the dedicated directory */	
	DIR* pDir = opendir(args[1]);		
	if(pDir == NULL){
		printf("Error: cannot open directory %s\n", args[1]);
		return 1;
	}


	clock_t clock_start = clock();
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
		if(size == 0){ /* skip empty file */
			fclose(pFile);
			continue;
		}
		char* line = new char[size];
		if(fgets(line, size, pFile) == NULL){
			printf("Error: reading lines failed\n");
			delete line;
			continue;
		}
		
		/* get the start & end pos of lyrics */
		int lyrics_start, lyrics_end;
		if(!find_lyrics(line, &lyrics_start, &lyrics_end)){
			printf("Error: no lyrics found in file: %s\n", pEnt->d_name);
			continue;
		}

		/* extract the string lyrics */
		int lyrics_len = lyrics_end - lyrics_start;
		char* lyrics = new char[lyrics_len + 1];
		strncpy(lyrics, (char*)(line + lyrics_start), lyrics_len);
		lyrics[lyrics_len] = '\0';

		/* divide lyrics into english and chinese */
		char* lyrics_en = new char[lyrics_len + 1];
		std::string* lyrics_cn = new std::string("");
		divide_lang(lyrics, lyrics_en, lyrics_cn);
	
		/* lexical analysis for chinese using THULAC */
		if(lyrics_cn->size() > 0){
			THULAC_result* result = new THULAC_result;
			lac.cut(*lyrics_cn, *result);
			std::cout<<'#'<<cnt<<','<<pEnt->d_name<<", "<<result->size()<<std::endl;
			print(*result, seg_only, separator);
			
			delete result;
			delete lyrics_cn;

		}

		delete lyrics_en;


		delete lyrics;		
		delete line;
		fclose(pFile);

		cnt ++;
	}
	clock_t clock_end = clock();
	double duration = (double)(clock_end - clock_start) / CLOCKS_PER_SEC;
	printf("take seconds %lf\n", duration);
	closedir(pDir);
	return 0;
}