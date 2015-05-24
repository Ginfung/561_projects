#include "stdafx.h"
#include "objLoader.h"

struct color{
	float r, g, b;
};

class raycasting{
public:
	vector_3D EYE; //eye position
	vector_3D LIGHT; //light position
	color LIGHT_COLOR; //color of light

	int WINDOW_SIZE_WIDTH;
	int WINDOW_SIZE_HEIGHT;
	vector_3D WINDOW_UPPER_LEFT;
	vector_3D WINDOW_LOWER_RIGHT;

	objLoader *obj;

	void setObj(objLoader *obj){ this->obj = obj; }

	/*setting the default parameters given in the project 1*/
	void setDefaultParameters();

	void setWindowSize(char *name);

	void setProjectionRect(char *name);

	/*set the position and color of the light. ONLY ONE LIGHT CAN BE READ AT THIS TIME*/
	void setLight(char *name);

	/*give the 3D position of a specific point in window
	a and b should be no greater than WINDOW_SIZE
	*/
	vector_3D threeDPositionInWindow(int a, int b);

	/*determine whether the surface is hit by the ray EYE->One Dot in view window*/
	bool detectHit(const vector_3D eye, const vector_3D windowPoint, const face target, vector_3D &hit);

	/*calculation for vector R*/
	vector_3D calcR(const vector_3D L, const vector_3D N);

	/*calcaulate the vector L: from hit point to the light*/
	vector_3D calcL(const vector_3D light, const vector_3D hit);

	/*calcaulate the vector V: from hit point to the eye*/
	vector_3D calcV(const vector_3D eye, const vector_3D hit);

	/*ray casting for a specific window point, without shadow*/
	bool castingOnePoint(int a, int b, color &c);

	/*ray casting for a specific window point. Enable shadow*/
	bool castingOnePointWithShadow(int a, int b, color &c);

private:
	/*return the dot product for A and B. That is cos<A,B>. Normailization will be performed in this function*/
	float dotProduct(const vector_3D A, const vector_3D B);

	/* return -A*/
	vector_3D negativeVector(const vector_3D A);

	bool isEqual(vector_3D A, vector_3D B);

};