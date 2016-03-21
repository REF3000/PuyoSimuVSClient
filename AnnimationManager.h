#pragma once
#include <list>
#include <queue>
#include "Annimation.h"

class AnnimationManager{

	std::list< Annimation* > annimation_list;
	std::queue< Annimation* > annimation_queue;

public :

	AnnimationManager(){
		;
	}
	~AnnimationManager(){
		auto it = annimation_list.begin();
		while( it!=annimation_list.end() ){
			delete *it;
			it = annimation_list.erase(it);
		}
		while( !annimation_queue.empty() ){
			delete annimation_queue.front();
			annimation_queue.pop();
		}
	}

	void add( Annimation *obj ){
		annimation_list.push_back( obj );
	}
	void push( Annimation *obj ){
		annimation_queue.push( obj );
	}
	void update(){
		// list‚Ìˆ—
		auto it = annimation_list.begin();
		while( it!=annimation_list.end() ){
			if( (*it)->update()!=0 ){
				delete (*it);
				it = annimation_list.erase(it);
			}
			it++;
		}
		// queue‚Ìˆ—
		do {
			if( annimation_queue.empty() ) return;
			if( annimation_queue.front()->update()==0 ) break;
			delete annimation_queue.front();
			annimation_queue.pop();
		}while(1);
	}
	void draw(){
		auto it = annimation_list.begin();
		while( it!=annimation_list.end() ){
			(*it)->draw();
			it++;
		}
		if( !annimation_queue.empty() ) 
			annimation_queue.front()->draw();
	}

};