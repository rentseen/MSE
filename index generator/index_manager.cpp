#include <iostream>
#include <string>

#include "index_manager.h"


Entry::Entry(std::string str, unsigned long song_id) : word(str){
	Posting* post = new Posting;
	post->count = 1;
	List.insert(std::pair<unsigned long, Posting*>(song_id, post));
}

void Entry::add_posting(unsigned long song_id){
	std::map<unsigned long, Posting*>::iterator it;
	it = List.find(song_id);
	if(it == List.end()){
		Posting* post = new Posting;
		post->count = 1;
		List.insert(std::pair<unsigned long, Posting*>(song_id, post));
	}
	else{
		it->second->count++;
	}
}


IndexManager::IndexManager(){

}


void IndexManager::add_posting(std::string str, unsigned long song_id){
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


void IndexManager::print_postings(){
	std::map<std::string, Entry*>::iterator dict;
	std::map<unsigned long, Posting*>::iterator ent;
	std::map<unsigned long, Posting*>* list;
	for(dict = Dict.begin(); dict != Dict.end(); dict++){
		std::cout<<dict->first<<" ";
		list = &(dict->second->List);
		for(ent = list->begin(); ent != list->end(); ent++){
			std::cout<<ent->first<<':'<<ent->second->count<<" ";
		}
		std::cout<<std::endl;
	}

}