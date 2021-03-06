#include <iostream>
#include <string>
#include "time.h"

#include "lexical_analyzer.h"
#include "index_manager.h"


#define DATA_FILE_ID_SIZE 	32

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
	LexicalAnalyzer Analyzer(args[2]);

	/* init index manager */
	IndexManager Indexer;

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
		char id[DATA_FILE_ID_SIZE];
		sprintf(path, "%s/%s", args[1], pEnt->d_name);
		sprintf(id, "%s", pEnt->d_name);
		id[strlen(pEnt->d_name) - strlen(DATA_FILE_SUFFIX)] = '\0';
		unsigned long song_id = std::stoul(id);
		status = Analyzer.load(path, song_id);
		if(status != 0){
			continue;
		}

		status = Analyzer.parse_lyrics();
		//Analyzer.print_lyrics();

		Indexer.new_doc(song_id);

		while(1){
			std::string token = Analyzer.poll_lyrics();
			if(token.size() == 0)
				break;
			
			Indexer.add_posting(token);

		}

		Analyzer.clean();
		cnt ++;
	
	}

	Indexer.sort_postings();

	clock_t clock_end = clock();
	double duration = (double)(clock_end - clock_start) / CLOCKS_PER_SEC;
	printf("Totally, taken %lf seconds\n\n", duration);
	
	Indexer.print_postings();

	closedir(pDir);
	return 0;
}