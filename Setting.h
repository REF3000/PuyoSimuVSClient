#include <string>

class Setting{

	int port;
	std::string nickname;
	std::string hostname;

public:

	void load();
	int getPort();
	std::string getNickName();
	std::string getHostName();
};
