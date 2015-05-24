class Bullet{
public:
	GLMmodel *object;
	float offsetX,offsetZ;
	bool exist;
	void displayBullet();
	Bullet(float offsetX, GLMmodel* o);//constructor
	~Bullet(void);//deconstructor
	static GLMmodel* loadModel(char *objFileName);
};