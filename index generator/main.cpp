#include <iostream>
#include "time.h"
#include "lexical_analyzer.h"


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

	if(argc < 3){
		printf("Error: no enough directory parameter given, \
			DATA_DIRECTORY, MODELS_DIRECTORY needed\n");
		return 1;
	}

	/* parsing files in the dedicated directory */	
	DIR* pDir = opendir(args[1]);		
	if(pDir == NULL){
		printf("Error: cannot open directory %s\n", args[1]);
		return 1;
	}

	/* init analyzer with THULAC models */
	LexicalAnalyzer analyzer(args[2]);

	clock_t clock_start = clock();
	int status;
	int cnt = 0;
	struct dirent* pEnt;
	while((pEnt = readdir(pDir)) != NULL){
		/* skip non-use files */
		if(!suffix(pEnt->d_name, DATA_FILE_SUFFIX))
			continue;

		/* open useful file */
		char path[128];
		char song_id[DATA_FILE_ID_SIZE];
		sprintf(path, "%s/%s", args[1], pEnt->d_name);
		sprintf(song_id, "%s", pEnt->d_name);
		song_id[strlen(pEnt->d_name) - strlen(DATA_FILE_SUFFIX)] = '\0';
		
		status = analyzer.load(path, song_id);
		if(status != 0){
			continue;
		}

		status = analyzer.parse_lyrics();
		analyzer.print_lyrics(Chinese);

		//std::string word_cn("a");
		while(1){
			std::string word_cn = analyzer.poll_lyrics_cn();
			if(word_cn.size() == 0)
				break;
			std::cout<<word_cn<<std::endl;
		}


		analyzer.clean();
		cnt ++;
	}
	clock_t clock_end = clock();
	double duration = (double)(clock_end - clock_start) / CLOCKS_PER_SEC;
	printf("Totally, taken %lf seconds\n\n", duration);
	closedir(pDir);
	return 0;
}