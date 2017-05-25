#include <iostream>
#include <string>

#include "index_manager.h"

/*
* implementation of Entry 
*/

Entry::Entry(std::string str) : word(str){	
	//Posting* post = new Posting;
	//post->term_freq = 1;
	//List.insert(std::pair<unsigned long, Posting*>(song_id, post));
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
	List.push(post);
	
	doc_freq++;
	current_tf = 0;	
}


/* 
* implementation of IndexManager 
*/
IndexManager::IndexManager(){
	doc_cnt = 0;
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

void IndexManager::print_postings(){
	std::map<std::string, Entry*>::iterator ent;
	std::priority_queue<Posting*, std::vector<Posting*>, compare>* list;
	Posting* post;

	for(ent = Dict.begin(); ent != Dict.end(); ent++){
		std::cout<<ent->first<<":"<<ent->second->doc_freq<<' ';
		list = &(ent->second->List);
		while(!list->empty()){
			post = list->top();
			std::cout<<post->song_id<<':'<<post->term_freq<<" ";
			list->pop();
		}
		std::cout<<std::endl;
	}
}