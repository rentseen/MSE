#ifndef INDEX_MANAGER_H
#define INDEX_MANAGER_H

#include <map>

/* posting records term-doc association */
struct Posting{

	//unsigned long song_id;
	unsigned term_freq;
};


/* entry records a term and its posting list */
class Entry{
public:	
	unsigned doc_freq; 
	std::string word;
	std::map<unsigned long, Posting*> List;

	Entry(std::string str, unsigned long song_id);
	void add_posting(unsigned long song_id);
};


/* indexmanager contains dictionary and manages it */
class IndexManager{
private:

	std::map<std::string, Entry*> Dict;


public:

	IndexManager();
	
	void add_posting(std::string str, unsigned long song_id);

	void print_postings();

};










































#endif