#pragma once
#include "Puyo.h"
#include <utility>
#include <random>
#include <vector>
#include <iostream>
using namespace std;




struct Game{

	Next next;
	Field field[2];
	int turn;      // �o�߃^�[����
	vector<Action> history[2];
	Action action[2];
	int status[2]; // 0:��A���� 1:�A����
	int score[2];
	int ojama_score[2];
	int chain_count[2];
	int ojama_notice[2];
	int ojama_stock[2];
	int ojama_count[2];
	static const int OJAMA_TABLE[6];

	Game(){
		init();
	}
	void init(){
		turn = 0;
		for( int i=0; i<2; ++i ){
			status[i] = 0;
			score[i] = 0;
			ojama_score[i] = 0;
			action[i] = Action();
			chain_count[i] = 0;
			ojama_notice[i] = 0;
			ojama_stock[i] = 0;
			ojama_count[i] = 0;
			field[i].init();
			history[i].clear();

		}
	}
	Field &getMyField(){
		return field[0];
	};
	Field &getEnemyField(){
		return field[1];
	};
	Field &getField( int player_id ){
		return field[player_id-1];
	}
	void setNext( Next next ){
		this->next = next;
	}
	Tumo getNextTumo( int num, int player_id ){ // num���̃c����Ԃ�
		int cnt = 0; // ���݂܂ł̃p�X�łȂ��萔�𐔂���
		for( size_t i=0; i<history[player_id-1].size(); ++i ) cnt+=(history[player_id-1][i].id==1);
		return next.get(cnt+num);
	}
	void setAction( Action act, int player_id ){
		action[player_id-1] = act;
	}
	Action getAction( int player_id ){
		return action[player_id-1];
	}
	void addScore( int id, int score ){
		this->score[id] += score;
		ojama_score[id] += score;
	}
	void processOjamaScore( int id ){
		int ojama = ojama_score[id]/70;
		ojama_score[id] -= ojama*70;
		// ���E�inotice�j
		if( ojama<ojama_notice[id] ){
			ojama_notice[id] -= ojama;
			return;
		}
		ojama -= ojama_notice[id];
		ojama_notice[id] = 0;
		// ���E�istock�j
		if( ojama<ojama_stock[id] ){
			ojama_stock[id] -= ojama;
			return;
		}
		ojama -= ojama_stock[id];
		ojama_stock[id] = 0;
		// �G������ܗ\������
		ojama_stock[(id+1)%2] += ojama;
	}
	void forwardChain( int id ){
		++chain_count[id];
		const int A[] = {0,8,16,32,64,96,128,160,192,224,256,288,320,352,384,416,448,480,512};
		const int B[] = {0,3,6,12,24};
		const int C[] = {0,2,3,4,5,6,7,10};
		int score = 0;
		int bonas = A[chain_count[id]-1];
		int color_flag[5] = {};
		for( int x=1; x<=6; ++x ){
			for( int y=1; y<=12; ++y ){
				int n = field[id].countConnection(x,y);
				if( n>=4 ){
					score += n*10;
					color_flag[field[id].get(x,y)] = 1;
					field[id].deleteConnection(x,y);
					bonas += C[(n<11)?(n-4):10];
				}
			}
		}
		int color_cnt=0;
		for(int i=1; i<=4; ++i ) color_cnt+=color_flag[i];
		bonas += B[color_cnt-1];
		if( bonas==0 ) bonas=1;
		addScore( id, score*bonas );
		processOjamaScore( id );
	}
	bool canFire( int id ){
		for( int x=1; x<=6; ++x ){
			for( int y=1; y<=12; ++y ){
				int n = field[id].countConnection(x,y);
				if( n>=4 ) return true;
			}}
		return false;
	}
	void fallOjama( int id ){
		if( ojama_notice[id]<=0 ) return;
		int num = (ojama_notice[id]>30) ? 30 : ojama_notice[id];
		for( int i=0; i<num; ++i ){
			const int P = ojama_count[id]++%6;
			field[id].set(OJAMA_TABLE[P],9); // TODO:OJAMA�͒萔
		}
		ojama_notice[id] -= num;
	}
	void transmitOjama( int id ){
		if( ojama_stock[id]<=0 ) return;
		ojama_notice[id] += ojama_stock[id];
		ojama_stock[id] = 0;
	}
	bool checkAllClear( int id ){
		for( int x=1; x<=6; ++x )
			for( int y=1; y<=13; ++y ) if( field[id].get(x,y)!=0 ) return false;
		return true;
	}
	void goNextStep(){
		for( int i=0; i<2; ++i ){
			if( action[i].id==-1 ){
				printf("action���Z�b�g����Ă��Ȃ� :player%d\n",i+1);
				return;
			}
		}
		int ojama_trans_flag[2]={};
		for( int id=0; id<2; ++id ){
			if( status[id]==0 ){ // ��A����
				if( action[id].id==1 ){
					int fall_d = field[id].set(action[id],getNextTumo(0,id+1));
					if( !history[id].empty() && history[id].back().id==0 ) fall_d = 0;
					addScore( id, fall_d ); // �O�񂪃p�X�ȊO�Ȃ痎���{�[�i�X�t�^
					if( canFire(id) ){
						status[id] = 1;
						//continue;
					} else {
						// TODO:��������
						fallOjama( id );
						// TODO:��������2
					}
				}
			} else {             // �A����
				action[id].id = 0;
				forwardChain(id);
				field[id].fall();
				if( canFire(id) ){
					//continue;
				} else {
					if( checkAllClear(id) ) addScore( id, 2100 );
					// TODO:��������
					fallOjama( id );
					ojama_trans_flag[ (id+1)%2 ] = true;
					// TODO:��������2
					status[id] = 0;
					chain_count[id] = 0;
				}
			}
			history[id].push_back(action[id]);
			action[id] = Action();
		}
		for( int id=0; id<2; ++id ){
			if( ojama_trans_flag[id] ) transmitOjama( id );
		}
		turn++;
	}
	int getStatus( int player_id ){
		return status[player_id-1];
	}
	int getOjamaStock( int player_id ){
		return ojama_stock[player_id-1];
	}
	int getOjamaNotice( int player_id ){
		return ojama_notice[player_id-1];
	}
	int getScore( int player_id ){
		return score[player_id-1];
	}
	Action getHistory( int player_id, int num ){
		if( turn-num<0 ) return Action();
		return history[player_id-1][turn-num];
	}
	int getTurn(){
		return turn+1;
	}

	void debug(){
		int cnt = 0; // ���݂܂ł̃p�X�łȂ��萔�𐔂���
		int id = 0;
		cout<<"debug"<<endl;
		for( size_t i=0; i<history[id].size(); ++i ) cnt+=(history[id][i].id==1), cout<<history[id][i].id<<",";
		cout<<"sum:"<<cnt<<endl;
		//return next.get(cnt+num);
	}
};
