#include <iostream>
#include <string>

#include "index_manager.h"

/*
* implementation of Entry 
*/

Entry::Entry(std::string str) : word(str){	
	current_id = 0;
	doc_freq = 0;
}

void Entry::add_posting(unsigned long song_id){
	if(current_id != song_id){
		Posting* post = new Posting;
		post->term_freq = 1;
		post->song_id = song_id;
		List.push_back(post);

		current_id = song_id;
		doc_freq++;
	}
	else{
		std::vector<Posting*>::iterator it = List.end() - 1;
		(*it)->term_freq++;
	}
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
	current_id = 0;
	doc_cnt = 0;
}

void IndexManager::new_doc(unsigned long song_id){
	current_id = song_id;
	doc_cnt++;
}

void IndexManager::add_posting(std::string str){
	std::map<std::string, Entry*>::iterator it;
	it = Dict.find(str);
	if(it == Dict.end()){
		Entry* ent = new Entry(str);
		ent->add_posting(current_id);
		Dict.insert(std::pair<std::string, Entry*>(str, ent));
	}	
	else{
		it->second->add_posting(current_id);
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