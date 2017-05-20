
#include "lexical_analyzer.h"
#include "jsmn/jsmn.h"
#include "THULAC/include/thulac.h"

LexicalAnalyzer::LexicalAnalyzer(const char* models_path){
	file_id[0] = '\0';
	input = NULL;
	pFile = NULL;

	/* init the THULAC, load models */
	char* user_dict_name=NULL;
	bool seg_only = true;	/* only segment, no label need */
	bool useT2S = true; 	/* change chi-traditional into chi-simple */
	bool useFilter = false;	/* do not filte useless words */
	lac = (THULAC*) new THULAC;
	((THULAC*)lac)->init(models_path, user_dict_name, seg_only, useT2S, useFilter);

	lyrics = NULL;
	lyrics_len = 0;

	lyrics_en = new std::string("");
//	lyrics_cn = new std::string("");
	lyrics_cn = new THULAC_result;
	pos_en = 0;
	pos_cn = 0;


}

/* load input file */
int LexicalAnalyzer::load(const char* file_path, const char* id){
	sprintf(file_id, "%s", id);
	pFile = fopen(file_path, "r");	
	if(pFile == NULL){
		printf("Error: cannot open file %s\n", file_path);
		return 1;
	}

	/* read line in opened file */
	fseek(pFile, 0L, SEEK_END);
	int size = (size_t) ftell(pFile);
	size = (size / 8 + 1) * 8; /* align to 8 */ 
	fseek(pFile, 0L, SEEK_SET);
	if(size == 0){ /* skip empty file */
		fclose(pFile);
		return 1;
	}
	
	input = new char[size];
	if(fgets(input, size, pFile) == NULL){
		printf("Error: reading lines failed\n");
		delete input;
		return 1;
	}

	return 0;
}

/* reset member variables */
void LexicalAnalyzer::clean(){
	file_id[0] = '\0';
	if(input != NULL){
		delete input;
		input = NULL;
	}
	
	if(pFile != NULL){
		fclose(pFile);
		pFile = NULL;
	}

	if(lyrics != NULL){
		delete lyrics;
		lyrics = NULL;
	}

	if(lyrics_en != NULL){
		delete lyrics_en;
		lyrics_en = new std::string("");
	}
	
	if(lyrics_cn != NULL){
		delete lyrics_cn;
		lyrics_cn = (THULAC_result*) new THULAC_result;
	}

	lyrics_len = 0;
	pos_en = 0;
	pos_cn = 0;
}


int LexicalAnalyzer::parse_lyrics(){
	/* get the start & end pos of lyrics */
	int lyrics_start, lyrics_end;
	if(!find_lyrics(&lyrics_start, &lyrics_end)){
		printf("Error: no lyrics found in file: %s\n", file_id);
		return 1;
	}

	/* extract the string lyrics */
	lyrics_len = lyrics_end - lyrics_start;
	lyrics = new char[lyrics_len + 1];
	strncpy(lyrics, (char*)(input + lyrics_start), lyrics_len);
	lyrics[lyrics_len] = '\0';

	/* divide lyrics into english and chinese */
	std::string* lyr_cn = new std::string("");
	divide_language(lyr_cn);

	/* segment chinese words */	
	printf("lyr_cn.size=%d\n", lyr_cn->size());
	if(lyr_cn->size() > 0){
		/* lexical analysis for chinese using THULAC */
		
		((THULAC*)lac)->cut(*lyr_cn, *(THULAC_result*)lyrics_cn);
		delete lyr_cn;
	}

//	delete lyr_cn;
	//std::cout<<'#'<<cnt<<','<<file_id<<','<<std::endl;

	return 0;
}


/* print lyrics */
void LexicalAnalyzer::print_lyrics() {
	bool seg_only = true;
	THULAC_result* lyr_cn = (THULAC_result*)lyrics_cn;
    std::cout<<"lyrics_en="<<*lyrics_en<<std::endl;

    std::cout<<"lyrics_cn=";
    std::cout<<((THULAC_result*)lyrics_cn)->size()<<std::endl;
    std::cout<<lyr_cn->size()<<std::endl;
if(lyr_cn->size() == 0) return;
    for(int i = 0; i < lyr_cn->size() - 1; i++) {
        if(i != 0) 
        	std::cout<<" ";
        if(seg_only) {
            std::cout << (*lyr_cn)[i].first;
        }
        else {
            std::cout << (*lyr_cn)[i].first << '/' << (*lyr_cn)[i].second;
        }
    }
    std::cout<<std::endl<<std::endl;
}


/* parse a line in json type, set the position of lyrics */
/* return 1 if lyrics found, otherwise return 0 */
int LexicalAnalyzer::find_lyrics(int* start, int* end){
	/* init jsmn parser */
	jsmn_parser parser;
	jsmntok_t tokens[JSON_TOKEN_NUM];
	jsmn_init(&parser);

	/* call for jsmn to parse json string */
	int cnt = jsmn_parse(&parser, input, strlen(input), tokens, JSON_TOKEN_NUM);
	if(cnt < 0){
		printf("Error: jsmn parsing failed, returned %d\n", cnt);
		return 0;
	}

	/* finding token "lyrics" */
	int i;
	for(i = 0; i < cnt; i++){
		int size = tokens[i].end - tokens[i].start;
		char* str = new char[size + 1];
		strncpy(str, (char*)(input + tokens[i].start), size);
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


/* divide lyrics into english and chinese */
void LexicalAnalyzer::divide_language(std::string* lyr_cn){
	size_t len = strlen(lyrics);
	int i;
	int pos_en = 0;

	for(i = 0; i < len; i++){
		if(!(lyrics[i] & 0x80)){
			/* ascii code */ 
			*lyrics_en += lyrics[i];
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
			*lyr_cn += lyrics[i++];
			*lyr_cn += lyrics[i++];
			*lyr_cn += lyrics[i];
		}
		else if(!(lyrics[i] & 0x08)){
			/* skip 4-byte unicode */
			i += 3;
		}
	}
	//lyrics_en[pos_en] = '\0';
}


