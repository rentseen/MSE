#include <iostream>
#include <string>

#include "index_manager.h"

/* implementation of Entry */
Entry::Entry(std::string str, unsigned long song_id) : word(str){	
	Posting* post = new Posting;
	post->term_freq = 1;
	List.insert(std::pair<unsigned long, Posting*>(song_id, post));
	doc_freq = 1;
}

void Entry::add_posting(unsigned long song_id){
	std::map<unsigned long, Posting*>::iterator it;
	it = List.find(song_id);
	if(it == List.end()){
		Posting* post = new Posting;
		post->term_freq = 1;
		List.insert(std::pair<unsigned long, Posting*>(song_id, post));
		doc_freq++;
	}
	else{
		it->second->term_freq++;
	}
}

/* implementation of IndexManager */
IndexManager::IndexManager(){
	doc_cnt = 0;
}

void IndexManager::add_doc(unsigned long song_id){
	doc_cnt++;
	current_id = song_id;
}

void IndexManager::add_posting(std::string str){
	std::map<std::string, Entry*>::iterator it;
	it = Dict.find(str);
	if(it == Dict.end()){
		Entry* entry = new Entry(str, song_id);
		Dict.insert(std::pair<std::string, Entry*>(str, entry));
	}	
	else{
		it->second->add_posting(song_id);
	}
}

void IndexManager::sort_postings(){
	std::map<std::string, Entry*>::iterator ent;
	std::map<unsigned long, Posting*>::iterator post;
	for(ent = Dict.begin(); ent != Dict.end(); ent++){


	}
}

void IndexManager::print_postings(){
	std::map<std::string, Entry*>::iterator ent;
	std::map<unsigned long, Posting*>::iterator post;
	std::map<unsigned long, Posting*>* list;
	for(ent = Dict.begin(); ent != Dict.end(); ent++){
		std::cout<<ent->first<<","<<ent->second->doc_freq<<' ';
		list = &(ent->second->List);
		for(post = list->begin(); post != list->end(); post++){
			std::cout<<post->first<<':'<<post->second->term_freq<<" ";
		}
		std::cout<<std::endl;
	}
}