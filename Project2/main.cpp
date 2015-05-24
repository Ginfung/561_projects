#include <windows.h>
#include <cstdio>
#include <iostream>
#include <Gl/GL.h>
#include <GL/GLU.h>
#include <GL/glut.h>

using namespace std;

#pragma warning(disable: 4996)

unsigned char* data;
unsigned int width, height;
static GLuint texName;

int loadBMP(const char* path, unsigned char*& data, unsigned int& width, unsigned int& height)
{

	// Data read from the header of the BMP file
	unsigned char header[54]; // Each BMP file begins by a 54-bytes header
	unsigned int dataPos;     // Position in the file where the actual data begins
	unsigned int imageSize;   // = width*height*3
	// Open the file in binary mode
	FILE* fp = fopen(path,"rb");
	if (!fp)                              
	{
		cout<<"Image could not be opened\n"; 
		return 0;
	}

	if ( fread(header, 1, 54, fp)!=54 )
	{ 
		// If not 54 bytes read : problem
		cout<<"Not a correct BMP file\n";
		return 0;
	}

	if ( header[0]!='B' || header[1]!='M' )
	{
		cout<<"Not a correct BMP file\n";
		return 0;
	}

	// Read ints from the byte array
	dataPos = *(int*)&(header[0x0A]);
	imageSize = *(int*)&(header[0x22]);
	width = *(int*)&(header[0x12]);
	height = *(int*)&(header[0x16]);

	// Some BMP files are misformatted, guess missing information
	if (imageSize==0)    
		imageSize=(width)*(height)*3; // 3 : one byte for each Red, Green and Blue component
	if (dataPos==0)      
		dataPos=54; // The BMP header is done 

	// Create a buffer
	data = new unsigned char [imageSize];

	// Read the actual data from the file into the buffer
	if (fread(data,1,imageSize,fp) != imageSize)
	{
		fclose(fp);
		return 0;
	}

	//Everything is in memory now, the file can be closed
	fclose(fp);
	return 1;
}

void init()
{
	if (!loadBMP("Koala.bmp",data, width, height))
		cout<<"Error loading bmp\n";
	else
		cout<<"Loaded bmp successfully!\n";

	glClearColor (0.0, 0.0, 0.0, 0.0);
	glShadeModel(GL_FLAT);	//to prevent color interpolation
	glEnable(GL_DEPTH_TEST);	

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
}

void display()
{
	static unsigned short count = 0;
	static unsigned short dir = 0;
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//tumble randomly
	glRotatef(count<640?1:count<1152?-1:(count=0,1),dir<768?1:0,1.0,dir<1792?0:(dir<2816?1:(dir=0,0)));
	count++;
	dir++;

	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
	glBindTexture(GL_TEXTURE_2D, texName);
	glBegin(GL_QUADS);
	//top
		glTexCoord2f(0.0, 0.0); glVertex3f(-1.0f, 1.0f, 1.0f);
		glTexCoord2f(1.0, 0.0); glVertex3f( 1.0f, 1.0f, 1.0f);
		glTexCoord2f(1.0, 1.0); glVertex3f( 1.0f, 1.0f,-1.0f);
		glTexCoord2f(0.0, 1.0); glVertex3f(-1.0f, 1.0f,-1.0f);
	
	//bottom
		glTexCoord2f(0.0, 0.0); glVertex3f(-1.0f,-1.0f,-1.0f);
		glTexCoord2f(1.0, 0.0); glVertex3f( 1.0f,-1.0f,-1.0f);
		glTexCoord2f(1.0, 1.0); glVertex3f( 1.0f,-1.0f, 1.0f);
		glTexCoord2f(0.0, 1.0); glVertex3f(-1.0f,-1.0f, 1.0f);

	//front	
		glTexCoord2f(0.0, 0.0); glVertex3f(-1.0f,-1.0f, 1.0f);
		glTexCoord2f(1.0, 0.0); glVertex3f( 1.0f,-1.0f, 1.0f);
		glTexCoord2f(1.0, 1.0); glVertex3f( 1.0f, 1.0f, 1.0f);
		glTexCoord2f(0.0, 1.0); glVertex3f(-1.0f, 1.0f, 1.0f);
	
	//back	
		glTexCoord2f(0.0, 0.0); glVertex3f(-1.0f,1.0f,-1.0f);
		glTexCoord2f(1.0, 0.0); glVertex3f( 1.0f, 1.0f,-1.0f);
		glTexCoord2f(1.0, 1.0); glVertex3f( 1.0f,-1.0f,-1.0f);
		glTexCoord2f(0.0, 1.0); glVertex3f(-1.0f,-1.0f,-1.0f);
		
	//left
		glTexCoord2f(0.0, 0.0); glVertex3f(-1.0f,-1.0f,-1.0f);
		glTexCoord2f(1.0, 0.0); glVertex3f(-1.0f,-1.0f, 1.0f);
		glTexCoord2f(1.0, 1.0); glVertex3f(-1.0f, 1.0f, 1.0f);
		glTexCoord2f(0.0, 1.0); glVertex3f(-1.0f, 1.0f,-1.0f);	
		
	//right
		glTexCoord2f(0.0, 0.0); glVertex3f( 1.0f,-1.0f, 1.0f);
		glTexCoord2f(1.0, 0.0); glVertex3f( 1.0f,-1.0f,-1.0f);
		glTexCoord2f(1.0, 1.0); glVertex3f( 1.0f, 1.0f,-1.0f);
		glTexCoord2f(0.0, 1.0); glVertex3f( 1.0f, 1.0f, 1.0f);
		
	glEnd();
	glFlush();
	glutSwapBuffers();
	glDisable(GL_TEXTURE_2D);
}

void rotate(int value)
{
	//we aren't doing anything with value here
	glutPostRedisplay();
	glutTimerFunc(10,rotate,0);
}

void reshape(int w, int h)
{
	glViewport(0, 0, (GLsizei) w, (GLsizei) h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0, (GLfloat) w/(GLfloat) h, 1.0, 8.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0,0,4,0,0,0,0,1,0);
}

void keyboard (unsigned char key, int x, int y)
{
	switch (key) 
	{
		case 27:
			exit(0);
			break;
		default:
			break;
	}
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
	glutInitWindowSize(512, 512);
	glutInitWindowPosition(50, 50);
	glutCreateWindow("Texture Shell");
	init();
	glutDisplayFunc(display);
	glutTimerFunc(60,rotate,0);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutMainLoop();
	
	delete data;
	return 0; 
}