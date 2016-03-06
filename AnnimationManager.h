#pragma once
#include <list>
#include "Annimation.h"

class AnnimationManager{

	std::list< Annimation* > annimation_list;

public :

	AnnimationManager(){
		;
	}

	void add( Annimation *obj ){
		annimation_list.push_back( obj );
	}
	void update(){
		auto it = annimation_list.begin();
		while( it!=annimation_list.end() ){
			if( (*it)->update()!=0 ){
				delete (*it);
				it = annimation_list.erase(it);
				continue;
			}
			it++;
		}
	}
	void draw(){
		auto it = annimation_list.begin();
		while( it!=annimation_list.end() ){
			(*it)->draw();
			it++;
		}
	}

};