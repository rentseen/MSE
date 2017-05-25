#ifndef INDEX_MANAGER_H
#define INDEX_MANAGER_H

#include <map>
#include <queue>

/* posting records term-doc association */
struct Posting{

	unsigned long song_id;
	unsigned term_freq;

};

bool operator<(Posting a, Posting b){

}

/* entry records a term and its posting list */
class Entry{
public:	
	unsigned current_tf; /* term freq of current doc */
	unsigned doc_freq; 
	std::string word;
//	std::map<unsigned long, Posting*> List;
	std::priority_queue<Posting*> List;
	Entry(std::string str, unsigned long song_id);
	void add_posting(unsigned long song_id);
};


/* indexmanager contains dictionary and manages it */
class IndexManager{
private:
	unsigned doc_cnt;
	unsigned long current_id;
	std::map<std::string, Entry*> Dict;


public:

	IndexManager();

	void add_doc(unsigned long song_id);
	
	void add_posting(std::string str);

	void print_postings();

};










































#endif