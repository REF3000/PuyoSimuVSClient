#include "PuyoAIUtility.h"
#include <vector>
#include <algorithm>
#include <numeric>
using namespace std;

bool canFireField( Field &field ){
	for( int x=1; x<=6; ++x ){
		for( int y=1; y<=12; ++y ){
			if( field.countConnection(x,y)>=4 ) return true;
		}
	}
	return false;
}

struct StepInfo{
	int num;
	vector<int> connection;
	int color;
	StepInfo(){ num=0; color=0; }
};
StepInfo stepFireField( Field &field ){
	StepInfo info;
	int c[] = {0,0,0,0,0};
	for( int x=1; x<=6; ++x ){
		for( int y=1; y<=12; ++y ){
			if( field.countConnection(x,y)>=4 ){
				int n = field.deleteConnection(x,y);
				info.connection.push_back( n );
				info.num += n;
				c[field.get(x,y)]=1;
			}
		}
	}
	info.color = accumulate( c, c+5, 0 );
	return info;
}

FireInfo fireField( Field &field ){
	const int A[] = {0,8,16,32,64,96,128,160,192,224,256,288,320,352,384,416,448,480,512};
	const int B[] = {0,3,6,12,24};
	const int C[] = {0,2,3,4,5,6,7,10};

	FireInfo fi = {-1,0};

	for( int i=1; i<=20; ++i ){
		field.fall();
		StepInfo info = stepFireField( field );
		if( info.num==0 ){
			fi.limit = i-2;
			break;
		}
		// ‚¨‚¶‚á‚ÜŽZŠJŽn
		int base  = info.num*10;
		int mag   = 0;
		mag += A[i-1];
		mag += B[info.color-1];
		for( size_t j=0; j<info.connection.size(); ++j ){
			int n = info.connection[j];
			if( n>11 ) n=11;
			mag += C[n-4];
		}
		if( mag==0 ) mag=1;
		fi.score += base*mag;
	}

	return fi;
}