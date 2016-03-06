#include "Setting.h"
#include <windows.h>
#include <iostream>
#include <fstream>
using namespace std;


void Setting::load(){
	char path[] = "./setting.ini";
	char buf[256];
	port = GetPrivateProfileInt( "NETWORK", "PORT", 2424, path );
	GetPrivateProfileString( "NETWORK", "HOSTNAME", "localhost", buf, sizeof(buf), path );
	hostname = buf;
	GetPrivateProfileString( "NETWORK", "NICKNAME", "NO_NAME", buf, sizeof(buf), path );
	nickname = buf;
}


int Setting::getPort(){
	return port;
}

string Setting::getNickName(){
	return nickname;
}

string Setting::getHostName(){
	return hostname;
}





