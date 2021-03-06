
#include "lexical_analyzer.h"
#include "jsmn/jsmn.h"
#include "THULAC/include/thulac.h"

LexicalAnalyzer::LexicalAnalyzer(const char* models_path){
	song_id = 0;
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

	lyr_token = new THULAC_result;
	lyr_pos = 0;

}

/* load input file */
int LexicalAnalyzer::load(const char* file_path, unsigned long id){
	song_id = id;
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
	song_id = 0;
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
	if(lyr_token != NULL){
		delete lyr_token;
		lyr_token = (THULAC_result*) new THULAC_result;
	}
	lyrics_len = 0;
	lyr_pos = 0;
}

/* parse lyrics into member variable (lyrics_en, lyrics_cn) */
int LexicalAnalyzer::parse_lyrics(){
	/* get the start & end pos of lyrics */
	int lyrics_start, lyrics_end;
	if(!find_lyrics(&lyrics_start, &lyrics_end)){
		printf("Error: no lyrics found in song: %ul\n", song_id);
		return 1;
	}

	/* extract the string lyrics */
	lyrics_len = lyrics_end - lyrics_start;
	lyrics = new char[lyrics_len + 1];
	strncpy(lyrics, (char*)(input + lyrics_start), lyrics_len);
	lyrics[lyrics_len] = '\0';

	purify_lyrics();

	/* segment lyrics into token sequence */	
	((THULAC*)lac)->cut(lyrics, *(THULAC_result*)lyr_token);

	return 0;
}

/*  */
std::string LexicalAnalyzer::poll_lyrics(){
	THULAC_result* lyr = (THULAC_result*)lyr_token;
	if(lyr->size() == lyr_pos){
		std::string str("");
		return str;
	}
	else{
		std::string str((*lyr)[lyr_pos].first);
		lyr_pos++;
		return str;
	}
}

/* print lyrics */
void LexicalAnalyzer::print_lyrics() {
	bool seg_only = true;
	THULAC_result* lyr = (THULAC_result*)lyr_token;
	std::cout<<"song_id="<<song_id<<std::endl;
   	std::cout<<"lyrics=";
	if(lyr->size() == 0){
		std::cout<<std::endl;
		return;
	}
    for(int i = 0; i < lyr->size() - 1; i++) {
        if(i != 0) 
        	std::cout<<" ";
        if(seg_only) {
           	std::cout << (*lyr)[i].first;
        }
        else {
           	std::cout << (*lyr)[i].first << '/' << (*lyr)[i].second;
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


/* purify lyrics, remove useless symbols */
void LexicalAnalyzer::purify_lyrics(){
	if(lyrics == NULL || lyrics[0] == '\0'){
		return;
	}	

	char* lyr = new char[lyrics_len + 1];
	int pos = 0;
	for(int i = 0; i < lyrics_len; i++){
		/* utf-8 1-byte code, ascii */
		if(!(lyrics[i] & 0x80)){
			char c = lyrics[i];

			if('a' <= c && c <= 'z' || '0' <= c && c <= '9'){
				lyr[pos++] = c;
			}
			/* change capital to small letter */
			else if('A' <= c && c <= 'Z'){
				lyr[pos++] = c + ('a' - 'A');
			}
			/* special symbols begin with '\\' */
			else if(c == '\\'){
				/* omit '\n', '\\' */	
				if(lyrics[i+1] == 'n' || lyrics[i+1] == '\\')
					i++;
				/* omit '\uXXX' */
				else if(lyrics[i+1] == 'u' && i + 5 < lyrics_len)
					i += 5;
				
				lyr[pos++] = ' ';
			}
			/* omit meanless symbols not begin with '\\' */
			else
				lyr[pos++] = ' '; 				
		}
		/*utf-8* 3-byte code, chinese */
		else if((lyrics[i] & 0xF0) == 0xE0 && (i + 2 < lyrics_len)){
			char c0 = lyrics[i], c1 = lyrics[i+1], c2 = lyrics[i+2];

			/* english captial letter in chinese coding */
			if( c0 == char(0xEF) && c1 == char(0xBC) && 
				char(0xA1) <= c2 && c2 <= char(0xBA) ){
				lyr[pos++] = 'a' + c2 - char(0xA1);
				i += 2;
			}
			/* english small letter in chinese coding */
			else if( c0 == char(0xEF) && c1 == char(0xBD) && 
				char(0x81) <= c2 && c2 <= char(0x9A)){
				lyr[pos++] = 'a' + c2 - char(0x81);
				i += 2;
			}
			/* arabic number in chinese coding */
			else if( c0 == char(0xEF) && c1 == char(0xBC) &&
				char(0x90) <= c2 && c2 <= char(0x99)){
				lyr[pos++] == '0' + c2 - char(0x90);
				i += 2;
			}
			/* useless symbols in chinese coding */
			else if( 
				(c0 == char(0xE3) && c1 == char(0x80)) || 	/* seg 0 */

				(c0 == char(0xEF) && c1 == char(0xB9)) ||	/* seg 1 */
				
				(c0 == char(0xEF) && c1 == char(0xBC) && (	/* seg 2 */
				(char(0x80) <= c2 && c2 <= char(0x8F)) ||
				(char(0x9A) <= c2 && c2 <= char(0xA0)) ||
				(char(0xBB) <= c2 && c2 <= char(0xBF)) )) ||
				
				(c0 == char(0xEF) && c1 == char(0xBD) && 	/* seg 3 */
				(char(0x9B) <= c2 && c2 <= char(0xA5)) || c2 == char(0x80)) ||
				
				(c0 == char(0xEF) && c1 == char(0xBF)) ){   /* seg 4  */
				
				lyr[pos++] = ' ';
				i += 2;
			}
			else{
				lyr[pos++] = lyrics[i++];
				lyr[pos++] = lyrics[i++];
				lyr[pos++] = lyrics[i];
			}
		}
		else
			lyr[pos++] = lyrics[i];
	}

	lyr[pos++] = '\0';
	lyrics_len = pos;
	char* tmp = lyrics;
	lyrics = lyr;
	delete tmp;
}