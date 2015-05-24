#include <windows.h>
#include <cstdio>
#include <iostream>
#include <memory.h>
#include "gl.h"
#include "glm.h"
#include "glu.h"
#include "glut.h"
#include "Bullet.h"
using namespace std;
#pragma warning(disable: 4996)


GLMmodel* Bullet::loadModel(char *objFileName){
	GLMmodel* object = glmReadOBJ(objFileName);
	glmUnitize(object);
	glmFacetNormals(object);
	glmVertexNormals(object, 90);
	//cout << "Obj file " << objFileName << " sucessfully loaded." << endl;
	return object;
}

Bullet::Bullet(float offsetX, GLMmodel* o){
//	loadModel(".//inputs//missile.obj");
	this->object = o;
	this->offsetX = offsetX;
	this->offsetZ = 22;
	this->exist = true;
}

Bullet::~Bullet(void){
	delete object;
}

void Bullet::displayBullet(){
	GLMgroup *group = object->groups;
	while (group){
		GLMmaterial *material = &object->materials[group->material];
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, material->ambient);
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, material->diffuse);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, material->specular);
		glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, material->shininess);
		glBegin(GL_TRIANGLES);
		for (GLuint i = 0; i < group->numtriangles; i++){
			GLMtriangle *triangle = &object->triangles[group->triangles[i]];
			GLfloat *v1 = &object->vertices[3 * triangle->vindices[0]];
			GLfloat *v2 = &object->vertices[3 * triangle->vindices[1]];
			GLfloat *v3 = &object->vertices[3 * triangle->vindices[2]];


			glVertex3f(v1[0] + offsetX, v1[1]+2, v1[2] + offsetZ);
			glVertex3f(v2[0] + offsetX, v2[1]+2, v2[2] + offsetZ);
			glVertex3f(v3[0] + offsetX, v3[1]+2, v3[2] + offsetZ);

		}
		glEnd();
		group = group->next;
	}
}
