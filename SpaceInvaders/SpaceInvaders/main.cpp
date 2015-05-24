#include <windows.h>
#include <cstdio>
#include <iostream>
#include <memory.h>
#include <vector>
#include <stdlib.h>
#include <string>
#include "gl.h"
#include "glm.h"
#include "glu.h"
#include "glut.h"
#include "Invaders.h"
#include "Cannon.h"
#include "Bullet.h"
#include "AlienShot.h"
using namespace std;

#pragma warning(disable: 4996)

#define DIRECTION_LEFT 0
#define DIRECTION_RIGHT 1

/*Attributes for the space invaders*/
struct invaderOffset{
	float offsetX;
	float offsetZ;
	bool isAlive;
};


int currentDirection;
Invaders *invader;
invaderOffset invaLocation[3][7];
Cannon *cannon;
float cannonOffset;
GLMmodel *alienshotmodel;
GLMmodel *bulletmodel;
vector<Bullet*> bullets;
vector<AlienShot*> alienshots;
float barrier1size, barrier2size, barrier3size;
int score;
int lives;
bool gameover;
bool success;

void init()
{
	invader = new Invaders();
	invader->loadModel(".//inputs//Space_Invader.obj");
	cannon = new Cannon();
	cannon->loadModel(".//inputs//A-6_Intruder.obj");
	alienshotmodel = AlienShot::loadModel(".//inputs//bulletfrominvader.obj");
	bulletmodel = Bullet::loadModel(".//inputs//missile.obj");
	barrier1size = 1.5;
	barrier2size = 1.5;
	barrier3size = 1.5;
	score = 0;
	lives = 3;
	gameover = false;
	success = false;
	cannonOffset = 5;
	/*moving left at the beginning*/
	currentDirection = DIRECTION_LEFT;

	glClearColor(0, 0.2, 0.4, 0);
	glEnable(GL_LIGHTING);
	glEnable(GL_DEPTH_TEST);
	
	/*set up the lightings*/
	GLfloat white_light[] = { 1, 1, 1, 1 };
	GLfloat light_position[] = {0,5,0};
	glLightfv(GL_LIGHT0, GL_AMBIENT, white_light);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, white_light);
	glLightfv(GL_LIGHT0, GL_SPECULAR, white_light);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	glEnable(GL_LIGHT0);
	
	/*initial the location offset for 3*7 invaders */
	for (int row = 0; row < 3; row++)
		for (int col = 0; col < 7; col++){
			invaLocation[row][col].isAlive = true;
			invaLocation[row][col].offsetZ = row * (-5);
			invaLocation[row][col].offsetX = 3.5 * col - 9;
		}
}

/*All the space invaders will move left*/
void movingLeft(){
	for (int i = 0; i < 3; i++)
	for (int j = 0; j < 7; j++)
		invaLocation[i][j].offsetX -= 0.5;
}

/*All the space invaders will move right*/
void movingRight(){
	for (int i = 0; i < 3; i++)
	for (int j = 0; j < 7; j++)
		invaLocation[i][j].offsetX += 0.5;
}

/*All the space invaders will go down*/
void movingDown(){
	for (int i = 0; i < 3; i++)
	for (int j = 0; j < 7; j++)
		invaLocation[i][j].offsetZ += 1;
}

/*detect whether any space invader has hit the left edge*/
bool hitLeftEdgeDetect(){
	for (int j = 0; j < 7; j++)
	for (int i = 0; i < 3; i++)
	if (invaLocation[i][j].isAlive && invaLocation[i][j].offsetX <= -22)
		return true;
	return false;
}

/*detect whether any space invader has hit the right edge*/
bool hitRightEdgeDetect(){
	for (int j = 6; j >= 0; j--)
	for (int i = 0; i < 3; i++)
	if (invaLocation[i][j].isAlive && invaLocation[i][j].offsetX >= 22)
		return true;
	return false;
}

/*return true if ant space invader is alive*/
bool isAnyInvaderAlive(){
	for (int i = 0; i < 3; i++)
	for (int j = 0; j < 7; j++)
	if (invaLocation[i][j].isAlive)
		return true;
	return false;
}

/*print the score on the screen*/
void printStatus()
{
	glDisable(GL_DEPTH_TEST);
	glColor3f(1, 0, 0);
	glRasterPos2f(-30, 14);

	string str = "SCORE: ";
	str.append(to_string(score));
	str.append("    LIVES: ");
	str.append(to_string(lives));
	int len = str.length();
	for (int i = 0; i < len; i++) {
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, str[i]);
	}
	glEnable(GL_DEPTH_TEST);
}

void printGameOver(){
	glDisable(GL_DEPTH_TEST);
	glColor3f(1, 0, 0);
	glRasterPos2f(-10, 0);

	string str = "GAME OVER. Final Score: ";
	str.append(to_string(score));
	int len = str.length();
	for (int i = 0; i < len; i++) {
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, str[i]);
	}
	glEnable(GL_DEPTH_TEST);
}

void printThankyou(){
	glDisable(GL_DEPTH_TEST);
	glColor3f(1, 0, 0);
	glRasterPos2f(-10, 0);

	string str = "Thank you. Final Score: ";
	str.append(to_string(score));
	int len = str.length();
	for (int i = 0; i < len; i++) {
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, str[i]);
	}
	glEnable(GL_DEPTH_TEST);
}

void wariningmessage(){
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDisable(GL_DEPTH_TEST);
	glColor3f(1, 0, 0);
	glRasterPos2f(0, 0);

	string str = "LIVES LEFT: ";
	str.append(to_string(lives));
	int len = str.length();
	for (int i = 0; i < len; i++) {
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, str[i]);
	}
	glEnable(GL_DEPTH_TEST);
	/*display the cannon*/
	cannon->displayCannon(cannonOffset);
	glFlush();
	glutSwapBuffers();
	Sleep(200);
	cannon->loadModel(".//inputs//A-6_Intruder.obj");
	cannonOffset = 0;
}




void display(){


	if (success){
		glClearColor(0.0, 0.7, 0, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		printThankyou();
		glFlush();
		glutSwapBuffers();
		return;
	}
	if (gameover){
		if (lives == 0){
			glClearColor(0.7, 0, 0, 0);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			printGameOver();
			glFlush();
			glutSwapBuffers();
			return;
		}
		else{
			cannon->loadModel(".//inputs//cannonexplosion.obj");
			gameover = false;
			lives--;
			wariningmessage();
		}
	}
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	/*display the game staus*/
	printStatus();
	/*display the space invaders*/
	for (int i = 0; i < 3; i++)
	for (int j = 0; j < 7; j++){
		if (invaLocation[i][j].isAlive)
			invader->displayInvader(invaLocation[i][j].offsetX, invaLocation[i][j].offsetZ);
	}
	/*display the cannon*/
	cannon->displayCannon(cannonOffset);
	/*display the bullets*/
	for (int i = 0; i < bullets.size(); i++)
	if (bullets[i]->exist)
		bullets[i]->displayBullet();
	/*display the alien shootings*/
	for (int i = 0; i < alienshots.size(); i++)
	if (alienshots[i]->exist)
		alienshots[i]->displayAlienShot();
	/*display the barrier 1*/
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glPushMatrix();
	glTranslatef(-10, 2, 18);
	glutSolidSphere(barrier1size, 10, 10);
	glPopMatrix();
	glPopAttrib();
	/*display the barrier 2*/
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glPushMatrix();
	glTranslatef(0, 2, 18);
	glutSolidSphere(barrier2size, 15, 15);
	glPopMatrix();
	glPopAttrib();
	/*display the barrier 3*/
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glPushMatrix();
	glTranslatef(10, 2, 18);
	glutSolidSphere(barrier3size, 15, 15);
	glPopMatrix();
	glPopAttrib();

	glFlush();
	glutSwapBuffers();
}

/*detect whether any space invaders hit the buttom line*/
bool buttomLineDetect(){
	float maxZ = -10;
	for (int i = 0; i < 3; i++)
	for (int j = 0; j < 7; j++){
		if (invaLocation[i][j].isAlive && invaLocation[i][j].offsetZ > maxZ){
			maxZ = invaLocation[i][j].offsetZ;
		}
	}
	if (maxZ > 18)
		return true;
	else
		return false;
}

/*success detect*/
bool successDetect(){
	for (int i = 0; i < 3; i++)
	for (int j = 0; j < 7; j++)
		if (invaLocation[i][j].isAlive)
			return false;
	
	return true;
}

/*game running*/
void rungame(int value){
	if (gameover){
		glutPostRedisplay();
		glutTimerFunc(70, rungame, 1);
		return;
	}
	if (success){
		glutPostRedisplay();
		glutTimerFunc(70, rungame, 1);
		return;
	}
	/*moving the space invaders*/
	if (currentDirection == DIRECTION_LEFT){
		if (!hitLeftEdgeDetect())
			movingLeft();
		else{
			movingDown();
			currentDirection = DIRECTION_RIGHT;
		}
	}
	else{
		if (!hitRightEdgeDetect())
			movingRight();
		else{
			movingDown();
			currentDirection = DIRECTION_LEFT;
		}
	}
	/*handle the bullet issue*/
	//std::vector<Bullet*>::iterator it = bullets.begin();
	L:for (int k = 0; k < bullets.size(); k++){
		if (!bullets[k]->exist)
			continue;
		bullets[k]->offsetZ -= 1.2;
		if (bullets[k]->offsetZ < -20) // bullet fly away
			bullets[k]->exist = false;
		else{
			//detect whether bullets[k] is blocked by the barriers
			if (abs(bullets[k]->offsetZ-13) < 1 &&(abs(bullets[k]->offsetX + 10) < barrier1size || 
				abs(bullets[k]->offsetX ) < barrier2size ||
				abs(bullets[k]->offsetX - 10) < barrier3size)){
				bullets[k]->exist = false;
				continue;
			}
			//detect whether bullets[k] hit any space invader in first line
			for (int row = 0; row < 7; row++)
			if (invaLocation[0][row].isAlive && abs(invaLocation[0][row].offsetX - bullets[k]->offsetX) < 1 &&  bullets[k]->offsetZ - invaLocation[0][row].offsetZ < 0.5){ //bullets k hit invader[0][row]
				score += 5;
				invaLocation[0][row].isAlive = false;
				bullets[k]->exist = false;
				continue;
			}
			//detect whether bullets[k] hit any space invader in second line
			for (int row = 0; row < 7; row++)
			if (!invaLocation[0][row].isAlive && invaLocation[1][row].isAlive && abs(invaLocation[1][row].offsetX - bullets[k]->offsetX) < 1 && bullets[k]->offsetZ - invaLocation[1][row].offsetZ < 0.5){//bullets k hit invader[1][row]
				score += 5;
				invaLocation[1][row].isAlive = false;
				bullets[k]->exist = false;
				continue;
			}
			//detect whether bullets[k] hit any space invader in third line
			for (int row = 0; row < 7; row++)
			if (!invaLocation[0][row].isAlive && !invaLocation[1][row].isAlive && invaLocation[2][row].isAlive && abs(invaLocation[2][row].offsetX - bullets[k]->offsetX) < 1 && bullets[k]->offsetZ - invaLocation[2][row].offsetZ < 0.5){//bullets k hit invader[2][row]
				score += 5;
				invaLocation[2][row].isAlive = false;
				bullets[k]->exist = false;
				continue;
			}
		}
	}
	/*generate the alien shotings*/
	 P: for (int j = 0; j < 7; j++){
		  if (rand() % 1000 < 15){
			  for (int i = 0; i < 3; i++)
			  if (invaLocation[i][j].isAlive){
				  alienshots.push_back(new AlienShot(invaLocation[i][j].offsetX, invaLocation[i][j].offsetZ,alienshotmodel));
				  goto P;
			  }
		  }
	  }

	/*moving the existed alien shootings*/
	//std::vector<AlienShot*>::iterator it2 = alienshots.begin();
	for (int x = 0; x < alienshots.size(); x++){
		alienshots[x]->offsetZ += 1;
		if (!alienshots[x]->exist)
			continue;
		//detect whether allienshots[x] is blocked by the barriers
		if (barrier1size > 0.3 && abs(alienshots[x]->offsetZ - 19) < 1 && abs(alienshots[x]->offsetX + 10) < barrier1size){
			barrier1size -= 0.4;
			alienshots[x]->exist = false;
		
		}
		else if (barrier1size > 0.3 && abs(alienshots[x]->offsetZ - 19) < 1 && abs(alienshots[x]->offsetX) < barrier2size){
			barrier2size -= 0.4;
			alienshots[x]->exist = false;
			
		}
		else if (barrier1size > 0.3 && abs(alienshots[x]->offsetZ - 19) < 1 && abs(alienshots[x]->offsetX - 10) < barrier3size){
			barrier3size -= 0.4;
			alienshots[x]->exist = false;
			
		}
		else
			//detect whether the alienshots[x] shots the cannon
		if (alienshots[x]->offsetZ < 24  && alienshots[x]->offsetZ > 22 && abs(alienshots[x]->offsetX - cannonOffset) < 0.2){
				alienshots[x]->exist = false;
				gameover = true;
				PlaySound(TEXT(".//inputs//explosion.wav"), NULL, SND_ASYNC | SND_FILENAME);
		}
	}

	/*Detect whether the space invader hit the bottom line*/
	if (buttomLineDetect()){
		lives = 0;
		gameover = true;
	}

	/*Detect whether whether success*/
	if (successDetect()){
		success = true;
	}
	glutPostRedisplay();
	glutTimerFunc(70, rungame, 1);
}

/*keyboard control function*/
void keyboard(unsigned char key, int x, int y){
	switch (key)
	{
	case ' ':
		//create a new bullet. X set as the same as that of cannon
		PlaySound(TEXT(".//inputs//shoot.wav"), NULL, SND_ASYNC | SND_FILENAME);
		bullets.push_back(new Bullet(cannonOffset,bulletmodel));
		break;
	case 27:
		exit(0);
	}
}
void keyboard2(int key, int x, int y){
	switch (key){
	case GLUT_KEY_LEFT:
		//left move the cannon
		if (cannonOffset >= -18){
			cannonOffset -= 1;
			glutPostRedisplay();
		}
		break;
	case GLUT_KEY_RIGHT:
		//right move the cannon
		if (cannonOffset <= 18){
			cannonOffset += 1;
			glutPostRedisplay();
		}
		break;
	}
}

void reshape(int w, int h)
{
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(30, (GLfloat)w / (GLfloat)h, 0, 60);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0, 30, 60, 0, 0, 0, 0, 1, 0);
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
	glutInitWindowSize(1200, 650);
	glutInitWindowPosition(10, 10);
	glutCreateWindow("Space Invader (jchen37)");
	init();
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(keyboard2);
	glutTimerFunc(500, rungame, 1);
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutMainLoop();

	return 0;
}