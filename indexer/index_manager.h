#ifndef INDEX_MANAGER_H
#define INDEX_MANAGER_H

#include <map>
#include <vector>
#include <algorithm>

/* posting records term-doc association */
struct Posting{
	unsigned long song_id;
	unsigned term_freq;
};

struct compare{
	bool operator()(Posting* a, Posting* b) const{
		return (a->song_id < b->song_id);
	}
};

/* entry records a term and its posting list */
class Entry{
public:	
	unsigned long current_id;	/* current song id */
	unsigned doc_freq; 	/* number of docs the word appears in */
	std::string word;

	std::vector<Posting*> List;	
	Entry(std::string str);
	
	void add_posting(unsigned long song_id);

	/* sort postings according to tf */
	void sort_postings();

	void print_postings();
};


/* indexmanager contains dictionary and manages it */
class IndexManager{
private:
	unsigned doc_cnt;
	unsigned long current_id;
	std::map<std::string, Entry*> Dict;


public:

	IndexManager();

	void new_doc(unsigned long song_id);
	
	void add_posting(std::string str);

	void sort_postings();

	void print_postings();

};






#endif