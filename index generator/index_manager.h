#ifndef INDEX_MANAGER_H
#define INDEX_MANAGER_H

#include <map>


struct Posting{

	//unsigned long song_id;
	unsigned count;
};


class Entry{
public:	
	std::string word;
	std::map<unsigned long, Posting*> List;

	Entry(std::string str, unsigned long song_id);
	void add_posting(unsigned long song_id);
};



class IndexManager{
private:

	std::map<std::string, Entry*> Dict;


public:

	IndexManager();
	
	void add_posting(std::string str, unsigned long song_id);

	void print_postings();

};










































#endif