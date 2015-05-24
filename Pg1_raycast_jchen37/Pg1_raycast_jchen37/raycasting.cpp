#include "stdafx.h"
#include "raycasting.h"



/*setting the default parameters given in the project 1*/
void raycasting::setDefaultParameters(){
	this->EYE.x = 0;
	this->EYE.y = 0;
	this->EYE.z = -2;

	this->LIGHT.x = 0;
	this->LIGHT.y = 5;
	this->LIGHT.z = 0;

	this->LIGHT_COLOR.r = this->LIGHT_COLOR.g = this->LIGHT_COLOR.b = 1;

	this->WINDOW_SIZE_WIDTH= 256;
	this->WINDOW_SIZE_HEIGHT = 256;

	this->WINDOW_UPPER_LEFT.x = -1;
	this->WINDOW_UPPER_LEFT.y = 1;
	this->WINDOW_UPPER_LEFT.z = -1;

	this->WINDOW_LOWER_RIGHT.x = 1;
	this->WINDOW_LOWER_RIGHT.y = -1;
	this->WINDOW_LOWER_RIGHT.z = -1;

	return;
}

float dotProductO(const vector_3D A, const vector_3D B){
	return A.x * B.x + A.y * B.y + A.z * B.z;
}

float raycasting::dotProduct(const vector_3D A, const vector_3D B){
	float l1 = sqrt(A.x * A.x + A.y * A.y + A.z * A.z);
	float l2 = sqrt(B.x * B.x + B.y * B.y + B.z * B.z);
	float result = A.x * B.x + A.y * B.y + A.z * B.z;
	return result / (l1	* l2);
}

vector_3D raycasting::calcR(const vector_3D L, const vector_3D N){
	//R = 2N(N.L)-L  normalization is required
	//NOTE: the normal vector has been normalized in the objLoader
	float temp = 2 * dotProduct(L, N);

	vector_3D result;
	result.x = temp*N.x - L.x;
	result.y = temp*N.y - L.y;
	result.z = temp*N.z - L.z;

	//normalization
	float length = sqrt(result.x * result.x + result.y * result.y + result.z * result.z);
	result.x /= length;
	result.y /= length;
	result.z /= length;

	return result;
}

/*calcaulate the vector L: from hit point to the light*/
vector_3D raycasting::calcL(const vector_3D light, const vector_3D hit){
	vector_3D result;
	result.x = light.x - hit.x;
	result.y = light.y - hit.y;
	result.z = light.z - hit.z;

	//normalization
	float length = sqrt(result.x * result.x + result.y * result.y + result.z * result.z);
	result.x /= length;
	result.y /= length;
	result.z /= length;

	return result;
}

/*calcaulate the vector V: from hit point to the eye*/
vector_3D raycasting::calcV(const vector_3D eye, const vector_3D hit){
	vector_3D result;
	result.x = eye.x - hit.x;
	result.y = eye.y - hit.y;
	result.z = eye.z - hit.z;

	//normalization
	float length = sqrt(result.x * result.x + result.y * result.y + result.z * result.z);
	result.x /= length;
	result.y /= length;
	result.z /= length;

	return result;
}

vector_3D raycasting::negativeVector(const vector_3D A){
	vector_3D result;
	result.x = (-1) * A.x;
	result.y = (-1) * A.y;
	result.z = (-1) * A.z;

	return result;
}


/*give the 3D position of a specific point in window
a and b should be no greater than WINDOW_SIZE
assume that the view window is parallel to the xOy plane
*/
vector_3D raycasting::threeDPositionInWindow(int a, int b){
	vector_3D result;
	result.x = a * (this->WINDOW_LOWER_RIGHT.x - this->WINDOW_UPPER_LEFT.x) / this->WINDOW_SIZE_WIDTH + this->WINDOW_UPPER_LEFT.x;
	result.y = b * (this->WINDOW_LOWER_RIGHT.y - this->WINDOW_UPPER_LEFT.y) / this->WINDOW_SIZE_HEIGHT + this->WINDOW_UPPER_LEFT.y;
	result.z = this->WINDOW_LOWER_RIGHT.z;
	return result;
}

vector_3D subtract_V(vector_3D A, vector_3D B){
	vector_3D result;
	result.x = A.x - B.x;
	result.y = A.y - B.y;
	result.z = A.z - B.z;
	return result;
}


//calculate the intersection point of the given line and given plane.
//set parallel is the line is parallel to the plane
vector_3D calulateIntersectPoint(vector_3D planeVector, vector_3D planePoint, vector_3D lineVector, vector_3D linePoint, bool &parallel)
{
	vector_3D returnResult;
	parallel = false;
	float vp1, vp2, vp3, n1, n2, n3, v1, v2, v3, m1, m2, m3, t, vpt;
	vp1 = planeVector.x;
	vp2 = planeVector.y;
	vp3 = planeVector.z;
	n1 = planePoint.x;
	n2 = planePoint.y;
	n3 = planePoint.z;
	v1 = lineVector.x;
	v2 = lineVector.y;
	v3 = lineVector.z;
	m1 = linePoint.x;
	m2 = linePoint.y;
	m3 = linePoint.z;
	vpt = v1 * vp1 + v2 * vp2 + v3 * vp3;
	//parallel?
	if (vpt == 0)
	{
		parallel = true;
		returnResult.x = returnResult.y = returnResult.z = 0;
		return returnResult;
	}
	else
	{
		t = ((n1 - m1) * vp1 + (n2 - m2) * vp2 + (n3 - m3) * vp3) / vpt;
		returnResult.x = m1 + v1 * t;
		returnResult.y = m2 + v2 * t;
		returnResult.z = m3 + v3 * t;
	}
	return returnResult;
}

/*determine whether the surface is hit by the ray EYE->One Dot in view window*/
bool raycasting::detectHit(const vector_3D eye, const vector_3D windowPoint, const face target, vector_3D &hit){

	/*float ddd = (windowPoint.x - eye.x)*target.norm.x +
	(windowPoint.y - eye.y)*target.norm.y +
	(windowPoint.z - eye.z)*target.norm.z;
	if (abs(ddd) < 0.001)
	return false;
	float t = (obj->vertex[target.v1_index].x * target.norm.x +
	obj->vertex[target.v1_index].y * target.norm.y +
	obj->vertex[target.v1_index].z * target.norm.z -
	eye.x * target.norm.x -
	eye.y * target.norm.y -
	eye.z * target.norm.z) /
	ddd;

	hit.x = eye.x + t*(windowPoint.x - eye.x);
	hit.y = eye.y + t*(windowPoint.y - eye.y);
	hit.z = eye.z + t*(windowPoint.z - eye.z);*/



	vector_3D A = obj->vertex[target.v1_index];
	vector_3D B = obj->vertex[target.v2_index];
	vector_3D C = obj->vertex[target.v3_index];
	vector_3D N = target.norm;

	bool tem;

	hit = calulateIntersectPoint(N, A, subtract_V(EYE, windowPoint), EYE, tem);
	if (tem)
		return false;

	vector_3D v0 = subtract_V(C, A);
	vector_3D v1 = subtract_V(B, A);
	vector_3D v2 = subtract_V(hit, A);

	float dot00 = dotProductO(v0, v0);
	float dot01 = dotProductO(v0, v1);
	float dot02 = dotProductO(v0, v2);
	float dot11 = dotProductO(v1, v1);
	float dot12 = dotProductO(v1, v2);

	float temp = 1 / (dot00*dot11 - dot01*dot01);
	float u = (dot11 * dot02 - dot01 * dot12) * temp;
	if (u < 0 || u > 1)
		return false;

	float v = (dot00 * dot12 - dot01 * dot02) * temp;
	if (v < 0 || v > 1)
		return false;
	if (u + v > 1)
		return false;
	else
		return true;
}

bool raycasting::castingOnePoint(int a, int b, color &c){
	vector_3D viewPoint = threeDPositionInWindow(a, b);

	int frontmostK = -1;
	float minDepth;
	vector_3D hit;
	vector_3D finalHit;

	for (int k = 0; k < this->obj->face_count; k++){ //for each face
		minDepth = 100; //temporary variant to save the depth
		bool isHit = detectHit(this->EYE, viewPoint, this->obj->facedata[k], hit);
		if (isHit){ //hit
			if (hit.z <= minDepth){ //is the frontmost
				frontmostK = k;
				finalHit = hit;
			}
		}
	}

	if (frontmostK == -1)
		return false;
	else{ //calculate the color
		//ambient LaKa
		int mtlI = obj->facegroupdata[obj->facedata[frontmostK].group_index].mtl_index;
		float aR = (this->LIGHT_COLOR.r) * (obj->materialdata[mtlI].KaR);
		float aG = (this->LIGHT_COLOR.g) * (obj->materialdata[mtlI].KaG);
		float aB = (this->LIGHT_COLOR.b) * (obj->materialdata[mtlI].KaB);

		//diffusion LdKd(N.L)
		vector_3D L = calcL(this->LIGHT, finalHit);
		float NL;
		NL = dotProduct(obj->facedata[frontmostK].norm, L);
		if (NL < 0){
			NL = dotProduct(negativeVector(obj->facedata[frontmostK].norm), L);
		}



		float dR = this->LIGHT_COLOR.r * obj->materialdata[mtlI].KdR * NL;
		float dG = this->LIGHT_COLOR.g * obj->materialdata[mtlI].KdG * NL;
		float dB = this->LIGHT_COLOR.b * obj->materialdata[mtlI].KdB * NL;

		//specular LsKs(R.V)^N
		vector_3D R;
		R = calcR(calcL(LIGHT, finalHit), obj->facedata[frontmostK].norm);

		vector_3D V = calcV(EYE, finalHit);

		float RV;
		float sR, sG, sB;
		if ((RV = dotProduct(R, V)) < 0){
			sR = sG = sB = 0;
		}
		else{
			sR = this->LIGHT_COLOR.r * obj->materialdata[obj->facedata[frontmostK].group_index].KsR * pow(RV, obj->materialdata[obj->facedata[frontmostK].group_index].N);
			sG = this->LIGHT_COLOR.g * obj->materialdata[obj->facedata[frontmostK].group_index].KsG * pow(RV, obj->materialdata[obj->facedata[frontmostK].group_index].N);
			sB = this->LIGHT_COLOR.b * obj->materialdata[obj->facedata[frontmostK].group_index].KsB * pow(RV, obj->materialdata[obj->facedata[frontmostK].group_index].N);
		}


		c.r = aR +dR + sR;
		c.g = aG +dG + sG;
		c.b = aB +dB + sB;
		if (c.r > 1)
			c.r = 1;
		if (c.g > 1)
			c.g = 1;
		if (c.b > 1)
			c.b = 1;
		return true;
	}
}

bool raycasting::castingOnePointWithShadow(int a, int b, color &c){
	vector_3D viewPoint = threeDPositionInWindow(a, b);

	int frontmostK = -1;
	float minDepth;
	vector_3D hit;
	vector_3D finalHit;

	for (int k = 0; k < this->obj->face_count; k++){ //for each face
		minDepth = 100; //temporary variant to save the depth
		bool isHit = detectHit(this->EYE, viewPoint, this->obj->facedata[k], hit);
		if (isHit){ //hit
			if (hit.z <= minDepth){ //is the frontmost
				frontmostK = k;
				finalHit = hit;
			}
		}
	}

	if (frontmostK == -1)
		return false;
	else{ //calculate the color

		/*first, calculate the shadow binary*/
		bool shadow = false;
		for (int k = 0; k < this->obj->face_count; k++){ // for each face, detect whether the ray can be blocked by it.
			bool temp = detectHit(finalHit, this->LIGHT, this->obj->facedata[k], hit);
			if (k != frontmostK && (LIGHT.z-hit.z)*(hit.z-finalHit.z)>0){
				shadow = true;
				goto BEGIN_RENDER;
			}

		}

		BEGIN_RENDER:
		//ambient LaKa
		int mtlI = obj->facegroupdata[obj->facedata[frontmostK].group_index].mtl_index;
		float aR = (this->LIGHT_COLOR.r) * (obj->materialdata[mtlI].KaR);
		float aG = (this->LIGHT_COLOR.g) * (obj->materialdata[mtlI].KaG);
		float aB = (this->LIGHT_COLOR.b) * (obj->materialdata[mtlI].KaB);

		/*this the face is shadowed, only take the ambient into consideration*/
		if (shadow){
			c.r = aR;
			c.g = aG;
			c.b = aB;
			if (c.r > 1)
				c.r = 1;
			if (c.g > 1)
				c.g = 1;
			if (c.b > 1)
				c.b = 1;
			return true;
		}

		/*otherwise, continue calculating*/
		//diffusion LdKd(N.L)
		vector_3D L = calcL(this->LIGHT, finalHit);
		float NL;
		NL = dotProduct(obj->facedata[frontmostK].norm, L);
		if (NL < 0){
			NL = dotProduct(negativeVector(obj->facedata[frontmostK].norm), L);
		}



		float dR = this->LIGHT_COLOR.r * obj->materialdata[mtlI].KdR * NL;
		float dG = this->LIGHT_COLOR.g * obj->materialdata[mtlI].KdG * NL;
		float dB = this->LIGHT_COLOR.b * obj->materialdata[mtlI].KdB * NL;

		//specular LsKs(R.V)^N
		vector_3D R;
		R = calcR(calcL(LIGHT, finalHit), obj->facedata[frontmostK].norm);

		vector_3D V = calcV(EYE, finalHit);

		float RV;
		float sR, sG, sB;
		if ((RV = dotProduct(R, V)) < 0){
			sR = sG = sB = 0;
		}
		else{
			sR = this->LIGHT_COLOR.r * obj->materialdata[obj->facedata[frontmostK].group_index].KsR * pow(RV, obj->materialdata[obj->facedata[frontmostK].group_index].N);
			sG = this->LIGHT_COLOR.g * obj->materialdata[obj->facedata[frontmostK].group_index].KsG * pow(RV, obj->materialdata[obj->facedata[frontmostK].group_index].N);
			sB = this->LIGHT_COLOR.b * obj->materialdata[obj->facedata[frontmostK].group_index].KsB * pow(RV, obj->materialdata[obj->facedata[frontmostK].group_index].N);
		}


		c.r = aR + dR + sR;
		c.g = aG + dG + sG;
		c.b = aB + dB + sB;
		if (c.r > 1)
			c.r = 1;
		if (c.g > 1)
			c.g = 1;
		if (c.b > 1)
			c.b = 1;
		return true;
	}
}

void raycasting::setWindowSize(char *name){
	fstream in;
	in.open(name);
	int width, height;
	in >> width >> height;
	this->WINDOW_SIZE_WIDTH = width;
	this->WINDOW_SIZE_HEIGHT = height;
	in.close();
	return;
}

void raycasting::setProjectionRect(char *name){
	fstream in;
	in.open(name);
	float a, b, c, d;
	in >> a >> b >> c >> d;
	this->WINDOW_UPPER_LEFT.x = a;
	this->WINDOW_UPPER_LEFT.y = b;
	this->WINDOW_UPPER_LEFT.z = -1;
	this->WINDOW_LOWER_RIGHT.x = c;
	this->WINDOW_LOWER_RIGHT.y = d;
	this->WINDOW_LOWER_RIGHT.z = -1;
	in.close();
}

void raycasting::setLight(char *name){
	fstream in;
	in.open(name);
	in >> this->LIGHT.x >> this->LIGHT.y >> this->LIGHT.z;
	in >> this->LIGHT_COLOR.r >> this->LIGHT_COLOR.g >> this->LIGHT_COLOR.b;
	in.close();
}