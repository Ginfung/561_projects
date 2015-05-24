#include <windows.h>
#include <cstdio>
#include <iostream>
#include <memory.h>
#include "gl.h"
#include "glm.h"
#include "glu.h"
#include "glut.h"
#include "Cannon.h"
using namespace std;
#pragma warning(disable: 4996)

void Cannon::loadModel(char *objFileName){
	object = glmReadOBJ(objFileName);
	glmUnitize(object);
	glmFacetNormals(object);
	glmVertexNormals(object, 90);
	cout << "Obj file " << objFileName << " sucessfully loaded." << endl;
}

void Cannon::displayCannon(float offsetX){
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

			glVertex3f(v1[0] + offsetX, v1[1], v1[2]+23);
			glVertex3f(v2[0] + offsetX, v2[1], v2[2]+23);
			glVertex3f(v3[0] + offsetX, v3[1], v3[2]+23);

		}
		glEnd();
		group = group->next;
	}
}
