/*
	CSC561 2014 Fall extra project
	Jianfeng Chen
	jchen37
	200065465
	Dec 2014
*/
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <iostream>
#include "glew.h"
#include "glut.h"
#include "textfile.h"
#include "glm.h"

using namespace std;


GLMmodel* object;

void init()
{
	object = glmReadOBJ(".//inputs//al.obj");
	glmUnitize(object);
	glmFacetNormals(object);
	glmVertexNormals(object, 90);
	cout << "OBJ file load successfully." << endl;

	glClearColor(0, 0, 0, 0);
	glShadeModel(GL_SMOOTH);
	glEnable(GL_DEPTH_TEST);

}

void reshape(int w, int h)
{
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0, (GLfloat)w / (GLfloat)h, 1, 8);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0, 0, 2, 0, 0, 1, 0, 1, 0);
}

void setLighting()
{
	GLfloat pos[3] = { 0, 5, 0 };
	GLfloat whiteLight[3] = { 1, 1, 1 };

	glEnable(GL_LIGHTING);
	glLightfv(GL_LIGHT0, GL_AMBIENT, whiteLight);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, whiteLight);
	glLightfv(GL_LIGHT0, GL_SPECULAR, whiteLight);
	glLightfv(GL_LIGHT0, GL_POSITION, pos);
	glEnable(GL_LIGHT0);
}

void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	GLMgroup *group = object->groups;
	while (group){
		for (GLuint i = 0; i < group->numtriangles; i++){
			glBegin(GL_TRIANGLES);
			GLMmaterial *material = &object->materials[group->material];
			glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, material->ambient);
			glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, material->diffuse);
			glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, material->specular);
			glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, material->shininess);
			GLMtriangle *triangle = &object->triangles[group->triangles[i]];
			GLfloat *v1 = &object->vertices[3 * triangle->vindices[0]];
			GLfloat *v2 = &object->vertices[3 * triangle->vindices[1]];
			GLfloat *v3 = &object->vertices[3 * triangle->vindices[2]];

			/*calculate the triangle normal vector*/
			GLfloat v[3];
			GLfloat u[3];
			GLfloat n[3];
			v[0] = v1[0] - v2[0];
			v[1] = v1[1] - v2[1];
			v[2] = v1[2] - v2[2];
			u[0] = v1[0] - v3[0];
			u[1] = v1[1] - v3[1];
			u[2] = v1[2] - v3[2];
			n[0] = u[1] * v[2] - u[2] * v[1];
			n[1] = u[2] * v[0] - u[0] * v[2];
			n[2] = u[0] * v[1] - u[1] * v[0];
			GLfloat l;
			l = (GLfloat)sqrt(n[0] * n[0] + n[1] * n[1] + n[2] * n[2]);
			n[0] /= -l;
			n[1] /= -l;
			n[2] /= -l;

			//glNormal3fv(n);
			glNormal3fv(&object->normals[3*triangle->nindices[0]]);
			glVertex3fv(v1);
			glNormal3fv(&object->normals[3*triangle->nindices[1]]);
			glVertex3fv(v2);
			glNormal3fv(&object->normals[3*triangle->nindices[2]]);
			glVertex3fv(v3);
		glEnd();
		}

		group = group->next;
	}

	//glmDraw(object, GLM_SMOOTH | GLM_MATERIAL);

	glFlush();
	glutSwapBuffers();
}
GLuint v, f, p;
GLfloat eyePosition[3] = { 0, 0, -2 };
void setBPShaders()
{
	char *vs = NULL, *fs = NULL, *fs2 = NULL;

	v = glCreateShader(GL_VERTEX_SHADER);
	f = glCreateShader(GL_FRAGMENT_SHADER);

	vs = textFileRead("BlinnPhong.vert");
	fs = textFileRead("BlinnPhong.frag");

	char* vv = vs;
	char* ff = fs;

	glShaderSource(v, 1, &vv, NULL);
	glShaderSource(f, 1, &ff, NULL);

	free(vs); 
	free(fs);

	glCompileShader(v);
	glCompileShader(f);

	p = glCreateProgram();
	glAttachShader(p, v);
	glAttachShader(p, f);

	glLinkProgram(p);

	glUseProgram(p);

	GLint eye;
	eye = glGetUniformLocation(p, "eyePosition");
	glUniform3fv(eye, 1, eyePosition);
}

void setToonShading()
{
	char *vs = NULL, *fs = NULL, *fs2 = NULL;

	v = glCreateShader(GL_VERTEX_SHADER);
	f = glCreateShader(GL_FRAGMENT_SHADER);

	vs = textFileRead("toon.vert");
	fs = textFileRead("toon.frag");

	char* vv = vs;
	char* ff = fs;

	glShaderSource(v, 1, &vv, NULL);
	glShaderSource(f, 1, &ff, NULL);

	free(vs);
	free(fs);

	glCompileShader(v);
	glCompileShader(f);

	p = glCreateProgram();
	glAttachShader(p, v);
	glAttachShader(p, f);
	glLinkProgram(p);
	glUseProgram(p);

	GLint eye;
	eye = glGetUniformLocation(p, "eyePosition");
	glUniform3fv(eye, 1, eyePosition);
}

void keyboard(unsigned char key, int x, int y){
	switch (key){
	case 't':
		//apply toon shading
		setToonShading();
	}
}


int main(int argc, char **argv)
{
	init();
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(512, 512);
	glutCreateWindow("PgExtra_glsl_jchen37");

	glutDisplayFunc(display);
	glutIdleFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);

	glEnable(GL_DEPTH_TEST);
	glewInit();
	setBPShaders();
	setLighting();
	glutMainLoop();

	return 0;
}