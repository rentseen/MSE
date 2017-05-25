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
		return (a->term_freq > b->term_freq);
	}
};

/* entry records a term and its posting list */
class Entry{
public:	
	unsigned current_tf;	/* term freq of current doc */
	unsigned doc_freq; 	/* number of docs the word appears in */
	std::string word;
//	std::map<unsigned long, Posting*> List;
//	std::priority_queue<Posting*, std::vector<Posting*>, compare> List;
	std::vector<Posting*> List;	
	Entry(std::string str);
	
	/* add term frequence */
	void add_term_freq();

	/* merge doc_id into posting list*/
	void merge_posting(unsigned long song_id);

	/* sort postings according to tf */
	void sort_postings();

	void print_postings();
};


/* indexmanager contains dictionary and manages it */
class IndexManager{
private:
	unsigned doc_cnt;
//	unsigned long current_id;
	std::map<std::string, Entry*> Dict;


public:

	IndexManager();

	/* merge current doc's postings */
	void finish_doc(unsigned long song_id);
	
	void add_posting(std::string str);

	void sort_postings();

	void print_postings();

};






#endif