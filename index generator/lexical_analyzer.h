#ifndef LEXICAL_ANALYZER_H
#define LEXICAL_ANALYZER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
//#include <cstdlib>
#include <string>
#include <iostream>

#define DATA_FILE_ID_SIZE	32
#define DATA_FILE_SUFFIX	".data"
#define JSON_TOKEN_NUM		512

enum lyrics_type{ English, Chinese, Both };

class LexicalAnalyzer
{
private:
	char song_id[DATA_FILE_ID_SIZE];
	char* input;
	FILE* pFile;

	/* THULAC*, use void pointer to not include THULAC/*.h in main */
	/* to avoid multiple definition link error */
	void* lac;

	char* lyrics;
	int lyrics_len;

	std::string* lyrics_en;
	/* pointer of THULAC_result to contain chinese lyrics */
	void* lyrics_cn;
	int pos_en;
	int pos_cn;


	int find_lyrics(int* start, int* end);
	void divide_language(std::string* lyr_cn);


public:
	LexicalAnalyzer(const char* models_path);

	/* init with input file */
	int load(const char* file_path, const char* id);
	
	/* parse lyrics into english and chinese */
	int parse_lyrics();

	/* poll one word from lyrics */
	std::string poll_lyrics_en();
	std::string poll_lyrics_cn();

	/* print lyrics */
	void print_lyrics(enum lyrics_type);

	/* clean to get input again */
	void clean();

};


#endif