#include <iostream>
#include <string>

#include "index_manager.h"

/*
* implementation of Entry 
*/

Entry::Entry(std::string str) : word(str){	
	current_tf = 1;
	doc_freq = 0;
}

void Entry::add_term_freq(){
	current_tf++;
}

void Entry::merge_posting(unsigned long song_id){
	if(current_tf == 0)
		return;

	Posting* post = new Posting;
	post->term_freq = current_tf;
	post->song_id = song_id;
	List.push_back(post);
	
	doc_freq++;
	current_tf = 0;	
}

void Entry::sort_postings(){
	compare cmp;
	std::sort(List.begin(), List.end(), cmp);
}

void Entry::print_postings(){
	std::vector<Posting*>::iterator it;
	for(it = List.begin(); it != List.end(); it++){
		std::cout<<(*it)->song_id<<':'<<(*it)->term_freq<<' ';
	}
	std::cout<<std::endl;
}


/* 
* implementation of IndexManager 
*/
IndexManager::IndexManager(){
	doc_cnt = 0;
}

void IndexManager::finish_doc(unsigned long song_id){
	/* merge old doc's postings */
	if(doc_cnt > 0){
		std::map<std::string, Entry*>::iterator it;
		for(it = Dict.begin(); it != Dict.end(); it++){
			it->second->merge_posting(song_id);
		}
	}
	doc_cnt++;
}

void IndexManager::add_posting(std::string str){
	std::map<std::string, Entry*>::iterator it;
	it = Dict.find(str);
	if(it == Dict.end()){
		Entry* entry = new Entry(str);
		Dict.insert(std::pair<std::string, Entry*>(str, entry));
	}	
	else{
		it->second->add_term_freq();
	}
}

void IndexManager::sort_postings(){
	std::map<std::string, Entry*>::iterator ent;

	for(ent = Dict.begin(); ent != Dict.end(); ent++)
		ent->second->sort_postings();
}

void IndexManager::print_postings(){
	std::map<std::string, Entry*>::iterator ent;

	for(ent = Dict.begin(); ent != Dict.end(); ent++){
		std::cout<<ent->first<<":"<<ent->second->doc_freq<<' ';
		ent->second->print_postings();
		
	}
}