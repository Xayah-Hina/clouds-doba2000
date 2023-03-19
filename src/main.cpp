//---------------------------------------------------------------------------
//
// Copyright (c) 2012 Taehyun Rhee
//
// This software is provided 'as-is' for assignment of COMP308
// in ECS, Victoria University of Wellington,
// without any express or implied warranty.
// In no event will the authors be held liable for any
// damages arising from the use of this software.
//
// The contents of this file may not be copied or duplicated in any form
// without the prior permission of its owner.
//
//----------------------------------------------------------------------------

#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <GL/glut.h>

// My definition
#include "define.h"
#include "G308_Geometry.h"
#include <iostream>
#include "Simulation.h"

using namespace std;

// Global Variables
GLuint g_mainWnd;
GLuint g_nWinWidth  = G308_WIN_WIDTH;
GLuint g_nWinHeight = G308_WIN_HEIGHT;

int xSimulationSize = 32;
int ySimulationSize = 32;
int zSimulationSize = sizeof(unsigned long)*8;

bool boundingBoxToggle = false;
bool ellipsoidToggle = false;
bool playing = true;

float xRotation = 0.0;

Simulation *s = NULL;

void renderClouds();
void boundingBox();

void mouseClicked(int button, int state, int x, int y);
void keyPressed(unsigned char key, int mouseX, int mouseY);
void G308_Display() ;
void G308_Reshape(int w, int h) ;
void G308_SetCamera();
void G308_SetLight();
void G308_ChangeFrame(int button, int state, int x, int y);

int main(int argc, char** argv)
{
	
	glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowSize(g_nWinWidth, g_nWinHeight);
    g_mainWnd = glutCreateWindow("COMP308 Final Project");
	glClearColor(0.67f, 0.87f, 0.98f, 1.0f);

    glutDisplayFunc(G308_Display);
    glutReshapeFunc(G308_Reshape);
    glutMouseFunc(mouseClicked);
	glutKeyboardFunc(keyPressed);

	s = new Simulation(xSimulationSize, ySimulationSize);


	G308_SetLight();
	G308_SetCamera();
	glutMainLoop();


    return 0;
}



//Detecting mouse clicks
void mouseClicked(int button, int state, int x, int y){
	G308_Point clickPos = {x, y, 0};

	if(button == GLUT_LEFT_BUTTON && state ==GLUT_DOWN){
		
	}
}

//Listening for keyboard presses
void keyPressed(unsigned char key, int mouseX, int mouseY){ 
	int mod = glutGetModifiers(); 
	if (key == 'p') {
		playing = !playing;
	}

	if(key == '1'){
		boundingBoxToggle = !boundingBoxToggle;
	}

	if(key == '2'){
		ellipsoidToggle = !ellipsoidToggle;
	}

	//Reset to new simulation
	if (key == 'r') {;
		s = new Simulation(xSimulationSize, ySimulationSize);
		s->tick();
		renderClouds();
		glutPostRedisplay();
		
	}

	if ( key == 'd'){
		//xRotation = xRotation +1.0;
		glTranslatef(-xSimulationSize/2, -ySimulationSize/2, zSimulationSize/2);
		glRotatef(1.0, 0.0, 0.0, 1.0);
		glTranslatef(xSimulationSize/2, ySimulationSize/2, zSimulationSize/2);
		//printf("d pressed, %f\n", xRotation);
		glutPostRedisplay();
	}

	if ( key == 'a'){
		//xRotation = xRotation +1.0;
		glRotatef(-1.0, 0.0, 0.0, 1.0);
		glutPostRedisplay();
	}

	if ( key == 'f'){
		glRotatef(1.0, 0.0, 1.0, 0.0);
		glutPostRedisplay();
	}
} 

// Display function
void G308_Display()
{
	//printf("################################## REDRAWN ####################################\n");
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_COLOR_MATERIAL);

	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
	glColor3f(1.0f,1.0f,1.0f);
	//glutSolidCube(1);
    glClear(GL_COLOR_BUFFER_BIT);
	
	renderClouds();

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);
	glDisable(GL_COLOR_MATERIAL);
	
	if(playing)
		s->tick();
	glutPostRedisplay();

	glutSwapBuffers();
}

void renderClouds(){
	if(boundingBoxToggle)
		boundingBox();

	if(ellipsoidToggle)
		s->renderEllipsoids();

	s->render();
}

void boundingBox(){
	//Red is x Axis
	glColor3f(1.0f,0.0f,0.0f);
	glBegin(GL_LINES);
		glVertex3f(0,0,0);
		glVertex3f(xSimulationSize,0,0);	
			
		glVertex3f(0,0,zSimulationSize);			
		glVertex3f(xSimulationSize,0,zSimulationSize);
			
		glVertex3f(0,ySimulationSize,0);
		glVertex3f(xSimulationSize,ySimulationSize,0);	
			
		glVertex3f(0,ySimulationSize,zSimulationSize);			
		glVertex3f(xSimulationSize,ySimulationSize,zSimulationSize);
	glEnd();
		
	// Green is y axis
	glColor3f(0.0f,1.0f,0.0f);
		glBegin(GL_LINES);
			glVertex3f(0,0,0);
			glVertex3f(0,ySimulationSize,0);
			
			glVertex3f(xSimulationSize,0,0);
			glVertex3f(xSimulationSize,ySimulationSize,0);
			
			glVertex3f(0,0,zSimulationSize);
			glVertex3f(0,ySimulationSize,zSimulationSize);
			
			glVertex3f(xSimulationSize,0,zSimulationSize);
			glVertex3f(xSimulationSize,ySimulationSize,zSimulationSize);
		glEnd();

	// Blue is z axis
	glColor3f(0.0f,0.0f,1.0f);
		glBegin(GL_LINES);
			glVertex3f(0,0,0);
			glVertex3f(0,0,zSimulationSize);
			
			glVertex3f(xSimulationSize,0,0);
			glVertex3f(xSimulationSize,0,zSimulationSize);
			
			glVertex3f(0,ySimulationSize,0);
			glVertex3f(0,ySimulationSize,zSimulationSize);
			
			glVertex3f(xSimulationSize,ySimulationSize,0);			
			glVertex3f(xSimulationSize,ySimulationSize,zSimulationSize);
		glEnd();
}


// Reshape function
void G308_Reshape(int w, int h)
{
    if (h == 0) h = 1;

	g_nWinWidth = w;
	g_nWinHeight = h;

    glViewport(0, 0, g_nWinWidth, g_nWinHeight);
}

// Set Light
void G308_SetLight()
{
	float direction[]	  = {0.0f, 0.0f, 1.0f, 0.0f};
	float diffintensity[] = {0.67f, 0.87f, 0.98f, 1.0f};
	float ambient[]       = {0.4f, 0.4f, 0.4f, 1.0f};

	glLightfv(GL_LIGHT0, GL_POSITION, direction);
	glLightfv(GL_LIGHT0, GL_DIFFUSE,  diffintensity);
	glLightfv(GL_LIGHT0, GL_AMBIENT,  ambient);

	glEnable(GL_LIGHT0);
}

// Set Camera Position
void G308_SetCamera()
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(G308_FOVY, (double) g_nWinWidth / (double) g_nWinHeight, G308_ZNEAR_3D, G308_ZFAR_3D);
	//glOrtho (0, g_nWinWidth, g_nWinHeight, 0, 0, 1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(xSimulationSize*2, ySimulationSize*2, zSimulationSize*3, xSimulationSize/2, ySimulationSize/2, zSimulationSize/2, 0.0, 1.0, 0.0);
}

