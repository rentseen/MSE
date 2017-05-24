#ifndef LEXICAL_ANALYZER_H
#define LEXICAL_ANALYZER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <string>
#include <iostream>


#define DATA_FILE_SUFFIX	".data"
#define JSON_TOKEN_NUM		512


class LexicalAnalyzer{
private:
	unsigned long song_id;
	char* input;
	FILE* pFile;

	/* THULAC*, use void pointer to not include THULAC/*.h in main */
	/* to avoid multiple definition link error */
	void* lac;

	char* lyrics;
	int lyrics_len;

	/* pointer of THULAC_result to contain segmented lyrics */
	void* lyr_token;
	int lyr_pos;

	int find_lyrics(int* start, int* end);
	void purify_lyrics();


public:
	LexicalAnalyzer(const char* models_path);

	/* init with input file */
	int load(const char* file_path, unsigned long id);
	
	/* parse lyrics into english and chinese */
	int parse_lyrics();

	/* poll one word from lyrics */
	std::string poll_lyrics();

	/* print lyrics */
	void print_lyrics();

	/* clean to get input again */
	void clean();
};


#endif