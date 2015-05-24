class AlienShot{
public:
	GLMmodel *object;
	float offsetX, offsetZ;
	void displayAlienShot();
	bool exist;
	AlienShot(float offsetX, float offsetZ,GLMmodel* o);//constructor
	~AlienShot(void);//deconstructor
	static GLMmodel* loadModel(char *objFileName);
};