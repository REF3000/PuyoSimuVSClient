
enum eSCENE{
	NONE, TITLE, NETWORK_PLAY, 
};

class SceneManager{

	int m_scene_id;
	int m_next_scene_id;

public:
	SceneManager();
	void setNextScene( int scene_id );

	int  update(); // 0à»äOÇ≈èIóπ
	void draw();
	void fin();

};
