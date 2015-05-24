/*
* \file main.cpp
* \brief the main file for assgnment 2 for CSC 561 2014 fall
* \author Jianfeng Chen (jchen37)
* \email jchen37@ncsu.edu
* \version 1.0
* \date Oct 12, 2014
*/

#include <windows.h>
#include <cstdio>
#include <iostream>
#include <fstream>
#include <memory.h>
#include <Windows.h>
#include "gl.h"
#include "glm.h"
#include "glu.h"
#include "glut.h"


using namespace std;

#pragma warning(disable: 4996)


unsigned char* data;
unsigned int width, height;
static GLuint texName;
static GLMmodel* object;
static GLfloat lookat[9];
static bool isTexture;

/*code source from the teaching assistant*/
int loadBMP(const char* path, unsigned char*& data, unsigned int& width, unsigned int& height)
{
	delete data;
	// Data read from the header of the BMP file
	unsigned char header[54]; // Each BMP file begins by a 54-bytes header
	unsigned int dataPos;     // Position in the file where the actual data begins
	unsigned int imageSize;   // = width*height*3
	// Open the file in binary mode
	FILE* fp = fopen(path, "rb");
	if (!fp)
	{
		cout << "Image could not be opened\n";
		return 0;
	}

	if (fread(header, 1, 54, fp) != 54)
	{
		// If not 54 bytes read : problem
		cout << "Not a correct BMP file\n";
		return 0;
	}

	if (header[0] != 'B' || header[1] != 'M')
	{
		cout << "Not a correct BMP file\n";
		return 0;
	}

	// Read ints from the byte array
	dataPos = *(int*)&(header[0x0A]);
	imageSize = *(int*)&(header[0x22]);
	width = *(int*)&(header[0x12]);
	height = *(int*)&(header[0x16]);

	// Some BMP files are misformatted, guess missing information
	if (imageSize == 0)
		imageSize = (width)*(height)* 3; // 3 : one byte for each Red, Green and Blue component
	if (dataPos == 0)
		dataPos = 54; // The BMP header is done 

	// Create a buffer
	data = new unsigned char[imageSize];

	// Read the actual data from the file into the buffer
	if (fread(data, 1, imageSize, fp) != imageSize)
	{
		fclose(fp);
		return 0;
	}

	//Everything is in memory now, the file can be closed
	fclose(fp);
	return 1;
}

void load8bitBMP(const char* path, unsigned char*& data, unsigned int& width, unsigned int& height){
	delete data;
	BITMAPFILEHEADER bmfh;
	BITMAPINFOHEADER bmih;
	unsigned short bpp;
	RGBQUAD *colours;
	int byteWidth; //the width in bytes of the image
	int padWidth; //the width in bytes of the added image

	FILE *in;                  //file stream for reading
	char *tempData;       //temp storage for image data
	int numColours;            //total available colours

	//open the file for reading in binary mode
	in = fopen(path, "rb");

	//if the file does not exist return in error
	if (in == NULL) {
		cout << "ERROR: cannot open " << path << endl;
		fclose(in);
		return;
	}

	//read in the entire BITMAPFILEHEADER
	fread(&bmfh, sizeof(BITMAPFILEHEADER), 1, in);

	//read in the entire BITMAPINFOHEADER
	fread(&bmih, sizeof(BITMAPINFOHEADER), 1, in);

	//save the width, height and bits per pixel for external use
	width = bmih.biWidth;
	height = bmih.biHeight;
	bpp = bmih.biBitCount;

	//calculate the size of the image data with padding
	unsigned int dataSize = (width*height*(unsigned int)(bmih.biBitCount / 8.0));

	//calculate the number of available colours
	numColours = 1 << bmih.biBitCount;

	//load the palette for 8 bits per pixel
	colours = new RGBQUAD[numColours];
	fread(colours, sizeof(RGBQUAD), numColours, in);
	

	//set up the temporary buffer for the image data
	tempData = new char[dataSize];

	//read in the entire image
	fread(tempData, sizeof(char), dataSize, in);
	fclose(in);

	//calculate the witdh of the final image in bytes
	byteWidth = padWidth = (int)((float)width*(float)bpp / 8.0);

	//adjust the width for padding as necessary
	while (padWidth % 4 != 0)
		padWidth++;

	int offset, diff;

	diff = width*height * 3;
	//allocate the buffer for the final image data
	data = new unsigned char[diff];

	if (height>0) {
		offset = padWidth - byteWidth;
		int j = 0;
		//count backwards so you start at the front of the image
		for (int i = 0; i<dataSize * 3; i += 3) {
			//jump over the padding at the start of a new line
			if ((i + 1) % padWidth == 0) {
				i += offset;
			}
			//transfer the data
			*(data + i) = colours[*(tempData + j)].rgbRed;
			*(data + i + 1) = colours[*(tempData + j)].rgbGreen;
			*(data + i + 2) = colours[*(tempData + j)].rgbBlue;
			j++;
		}
	}

	//image parser for a forward image
	else {
		offset = padWidth - byteWidth;
		int j = dataSize - 1;
		//count backwards so you start at the front of the image
		for (int i = 0; i<dataSize * 3; i += 3) {
			//jump over the padding to a new line
			if ((i + 1) % padWidth == 0) {
				i += offset;
			}
			//transfer the data
			*(data + i) = colours[*(tempData + j)].rgbRed;
			*(data + i + 1) = colours[*(tempData + j)].rgbGreen;
			*(data + i + 2) = colours[*(tempData + j)].rgbBlue;
			j--;
		}
	}
	delete[] tempData;
	return;
}


void loadOBJ(char* objFileName){
	object = glmReadOBJ(objFileName);
	glmUnitize(object);
	glmFacetNormals(object);
	glmVertexNormals(object, 90);
	cout << "Obj file " << objFileName << " sucessfully loaded." << endl;
	return;
}
void display();
void setTexture_24_bmp(char *);
void setTexture_8_bmp(char *);
void mymenu(int value){
	switch (value){
	case 1:
		glLoadIdentity();
		gluLookAt(lookat[0], lookat[1], lookat[2], lookat[3], lookat[4], lookat[5], lookat[6], lookat[7], lookat[8]);
		loadOBJ(".//input//al.obj");
		glDisable(GL_TEXTURE_2D);
		display();
		break;
	case 2:
		glLoadIdentity();
		gluLookAt(lookat[0], lookat[1], lookat[2], lookat[3], lookat[4], lookat[5], lookat[6], lookat[7], lookat[8]);
		loadOBJ(".//input//car.obj");
		glDisable(GL_TEXTURE_2D);
		display();
		break;
	case 3:
		glLoadIdentity();
		gluLookAt(lookat[0], lookat[1], lookat[2], lookat[3], lookat[4], lookat[5], lookat[6], lookat[7], lookat[8]);
		loadOBJ(".//input//dolphins.obj");
		glDisable(GL_TEXTURE_2D);
		display();
		break;
	case 4:
		setTexture_24_bmp(".//input//Koala.bmp");
		isTexture = true;
		display();
		break;
	case 5:
		setTexture_8_bmp(".//input//lena_8.bmp");
		setTexture_8_bmp(".//input//lena_8.bmp");
		setTexture_8_bmp(".//input//lena_8.bmp");
		setTexture_8_bmp(".//input//lena_8.bmp");
		setTexture_8_bmp(".//input//lena_8.bmp");//to fix a unknown reason bug---
		isTexture = true;
		display();
		break;
	}
}

void setTexture_24_bmp(char *textureFileName){
	if (!loadBMP(textureFileName, data, width, height))
		cout << "Error loading bmp\n";
	else
		cout << "Loaded bmp successfully!\n";
	/*set the texture*/
	//specify pixels are byte-aligned while unpacking from memory
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	glGenTextures(1, &texName);//generate
	glBindTexture(GL_TEXTURE_2D, texName);//bind to a 2-D texture

	//wrap the image along s coordinate (width)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	//wrap the image along t coordinate (height)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	//how to treat magnification
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	//how to treat de-magnification
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	//bmp stores BGR instead of RGB
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, data);

	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
	glBindTexture(GL_TEXTURE_2D, texName);
}

void setTexture_8_bmp(char *textureFileName){
	load8bitBMP(textureFileName, data, width, height);
	/*set the texture*/
	//specify pixels are byte-aligned while unpacking from memory
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	glGenTextures(1, &texName);//generate
	glBindTexture(GL_TEXTURE_2D, texName);//bind to a 2-D texture

	//wrap the image along s coordinate (width)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	//wrap the image along t coordinate (height)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	//how to treat magnification
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	//how to treat de-magnification
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, data);

	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
	glBindTexture(GL_TEXTURE_2D, texName);
}


void init()
{
	loadOBJ(".//input//al.obj");
	glClearColor(0.7, 0.6, 0.8, 0); //background

	/*read light position from the light.txt*/
	fstream fp;
	/*read the file view.txt*/
	fp.open(".//input//view.txt");
	fp >> lookat[0] >> lookat[1] >> lookat[2] >> lookat[6] >> lookat[7] >> lookat[8] >> lookat[3] >> lookat[4] >> lookat[5];
	fp.close();

	glShadeModel(GL_SMOOTH);
	/*setting the light*/
	GLfloat light_color[3];
	GLfloat light_position[3];
	fp.open(".//input//lights.txt");
	GLenum currentLight = GL_LIGHT0;
	while (!fp.eof()){
		fp >> light_position[0] >> light_position[1] >> light_position[2];
		fp >> light_color[0] >> light_color[1] >> light_color[2];
		glLightfv(currentLight, GL_AMBIENT, light_color);
		glLightfv(currentLight, GL_DIFFUSE, light_color);
		glLightfv(currentLight, GL_SPECULAR, light_color);
		glLightfv(currentLight, GL_POSITION, light_position);
		currentLight = currentLight + 1;
	}
	//cout << currentLight -GL_LIGHT0 << endl; //light count
	fp.close();

	/*add the menu*/
	glutCreateMenu(mymenu);
	glutAddMenuEntry("Al", 1);
	glutAddMenuEntry("Car", 2);
	glutAddMenuEntry("Dolphins", 3);
	glutAddMenuEntry("Koala.bmp(24bits)", 4);
	glutAddMenuEntry("lena_8.bmp(8bits)", 5);
	glutAttachMenu(GLUT_RIGHT_BUTTON);

}

/*mapping the 3D vertex into 2D texture.
assuming that v is inside the cude(2^3) whose center is the origin*/
GLfloat* texture_mapping(GLfloat* v){
	GLfloat result[2];
	result[0] = (v[0] + 1) / 2;
	result[1] = (v[1] + 1) / 2;
	return result;
}

void display(){
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glEnable(GL_LIGHTING);
		glEnable(GL_LIGHT0);
		glEnable(GL_DEPTH_TEST);

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

				glNormal3fv(n);
				if (isTexture) glTexCoord2fv(texture_mapping(v1));
				//glNormal3fv(&object->normals[triangle->nindices[0]]);
				glVertex3fv(v1);
				if (isTexture) glTexCoord2fv(texture_mapping(v2));
				//glNormal3fv(&object->normals[triangle->nindices[1]]);
				glVertex3fv(v2);
				if (isTexture) glTexCoord2fv(texture_mapping(v3));
				//glNormal3fv(&object->normals[triangle->nindices[2]]);
				glVertex3fv(v3);
			
		}
		glEnd();
		group = group->next;
	}
	glFlush();
	glutSwapBuffers();
}



void reshape(int w, int h)
{
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0, (GLfloat)w / (GLfloat)h, 1.0, 8.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(lookat[0], lookat[1], lookat[2], lookat[3], lookat[4], lookat[5], lookat[6], lookat[7], lookat[8]);
}

void keyboard1(int key, int x, int y)
{
	switch (key)
	{
	case GLUT_KEY_UP:
		//move the object in +Y
		glTranslatef(0, 0.01, 0);
		display();
		break;
	case GLUT_KEY_DOWN:
		//move the object in -Y
		glTranslatef(0, -0.01, 0);
		display();
		break;
	case GLUT_KEY_RIGHT:
		//move the object in +X
		glTranslatef(0.01, 0, 0);
		display();
		break;
	case GLUT_KEY_LEFT:
		//move the object in -Y
		glTranslatef(-0.01, 0, 0);
		display();
		break;
	default:
		cout << key << endl;
	}
}

void keyboard2(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 27:
		//exit the program
		exit(0);
		break;
	case 'z':
		//zoom in
		glScalef(1.01, 1.01, 1.01);
		display();
		break;
	case 'x':
		//zoom out
		glScalef(0.99, 0.99, 0.99);
		display();
		break;
	case '[':
		//move the object in +Z
		glTranslatef(0, 0, 0.01);
		display();
		break;
	case ']':
		//move the object in -Z
		glTranslatef(0, 0, -0.01);
		display();
		break;
	case 'q':
		//rothe clockwise
		glRotatef(3, 0, 1, 0);
		display();
		break;
	case 'w':
		//rotate anti-clockwise
		glRotatef(3, 0, -1, 0);
		display();
		break;
	default:
		cout << key << endl;
	}
}


int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
	glutInitWindowSize(512, 512);
	glutInitWindowPosition(50, 50);
	glutCreateWindow("Pg2_opengl_jchen37");
	isTexture = false;
	init();
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutSpecialFunc(keyboard1);
	glutKeyboardFunc(keyboard2);
	glutMainLoop();

	delete data;
	return 0;
}