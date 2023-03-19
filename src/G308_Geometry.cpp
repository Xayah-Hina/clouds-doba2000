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

#include "G308_Geometry.h"
#include <stdio.h>
#include <math.h>
#include <GL/glut.h>

#include <iostream>

using namespace std;

int numVert, numNorm, numUV, numFace;

G308_Geometry::G308_Geometry(void) {
	m_pVertexArray = NULL;
	m_pNormalArray = NULL;
	m_pUVArray = NULL;
	m_pTriangles = NULL;

	mode = G308_SHADE_POLYGON;

	m_nNumPoint = m_nNumUV = m_nNumPolygon = 0;
	m_glGeomListPoly = m_glGeomListWire = 0;
}

G308_Geometry::~G308_Geometry(void) {
	if (m_pVertexArray != NULL)
		delete[] m_pVertexArray;
	if (m_pNormalArray != NULL)
		delete[] m_pNormalArray;
	if (m_pUVArray != NULL)
		delete[] m_pUVArray;
	if (m_pTriangles != NULL)
		delete[] m_pTriangles;
}

//-------------------------------------------------------
// This function read obj file having
// triangle faces consist of vertex v, texture coordinate vt, and normal vn
// e.g. f v1/vt1/vn1 v2/vt1/vn2 v3/vt3/vn3
//
// [Assignment1]
// You can revise the following function for reading other variations of obj file
// 1) f v1//vn1 v2//vn2 v3//vn3 ; no texture coordinates such as bunny.obj
// 2) f v1 v2 v3 ; no normal and texture coordinates such as dragon.obj
// The case 2) needs additional functions to build vertex normals
//--------------------------------------------------------
void G308_Geometry::ReadOBJ(const char *filename) {
	FILE* fp;
	char mode, vmode;
	char str[200];
	int v1, v2, v3, n1, n2, n3, t1, t2, t3;

	float x, y, z;
	float u, v;

	numVert = numNorm = numUV = numFace = 0;

	fp = fopen(filename, "r");
	if (fp == NULL)
		printf("Error reading %s file\n", filename);
	else
		printf("Reading %s file\n", filename);

	// Check number of vertex, normal, uvCoord, and Face
	while (fgets(str, 200, fp) != NULL) {
		sscanf(str, "%c%c", &mode, &vmode);
		switch (mode) {
		case 'v': /* vertex, uv, normal */
			if (vmode == 't') // uv coordinate
				numUV++;
			else if (vmode == 'n') // normal
				numNorm++;
			else if (vmode == ' ') // vertex
				numVert++;
			break;
		case 'f': /* faces */
			numFace++;
			break;
		}
	}

	m_nNumPoint = numVert;
	m_nNumUV = numUV;
	m_nNumPolygon = numFace;
	m_nNumNormal = numNorm;

	printf("Number of Point %d, UV %d, Normal %d, Face %d\n", numVert, numUV,
			numNorm, numFace);
	//-------------------------------------------------------------
	//	Allocate memory for array
	//-------------------------------------------------------------

	if (m_pVertexArray != NULL)
		delete[] m_pVertexArray;
	m_pVertexArray = new G308_Point[m_nNumPoint];

	if (m_pNormalArray != NULL)
		delete[] m_pNormalArray;
	m_pNormalArray = new G308_Normal[m_nNumNormal];

	if (m_pUVArray != NULL)
		delete[] m_pUVArray;
	m_pUVArray = new G308_UVcoord[m_nNumUV];

	if (m_pTriangles != NULL)
		delete[] m_pTriangles;
	m_pTriangles = new G308_Triangle[m_nNumPolygon];

	//-----------------------------------------------------------
	//	Read obj file
	//-----------------------------------------------------------
	numVert = numNorm = numUV = numFace = 0;

	fseek(fp, 0L, SEEK_SET);
	while (fgets(str, 200, fp) != NULL) {
		sscanf(str, "%c%c", &mode, &vmode);
		switch (mode) {
		case 'v': /* vertex, uv, normal */
			if (vmode == 't') // uv coordinate
			{
				sscanf(str, "vt %f %f", &u, &v);
				m_pUVArray[numUV].u = u;
				m_pUVArray[numUV].v = v;
				numUV++;
			} else if (vmode == 'n') // normal
			{
				sscanf(str, "vn %f %f %f", &x, &y, &z);
				m_pNormalArray[numNorm].x = x;
				m_pNormalArray[numNorm].y = y;
				m_pNormalArray[numNorm].z = z;
				numNorm++;
			} else if (vmode == ' ') // vertex
			{
				sscanf(str, "v %f %f %f", &x, &y, &z);
				m_pVertexArray[numVert].x = x;
				m_pVertexArray[numVert].y = y;
				m_pVertexArray[numVert].z = z;
				numVert++;
			}
			break;
		case 'f': /* faces : stored value is index - 1 since our index starts from 0, but obj starts from 1 */

			if (numUV !=0){
				sscanf(str, "f %d/%d/%d %d/%d/%d %d/%d/%d", &v1, &t1, &n1, &v2, &t2, &n2, &v3, &t3, &n3);
			} else if (numNorm != 0){
				sscanf(str, "f %d//%d %d//%d %d//%d", &v1, &n1, &v2, &n2, &v3, &n3);
			} else {
				sscanf(str, "f %d %d %d", &v1, &v2, &v3);
			}


			// Vertex indicies for triangle:
			if (numVert != 0) {
				m_pTriangles[numFace].v1 = v1 - 1;
				m_pTriangles[numFace].v2 = v2 - 1;
				m_pTriangles[numFace].v3 = v3 - 1;
			}

			// Normal indicies for triangle
			if (numNorm != 0) {
				m_pTriangles[numFace].n1 = n1 - 1;
				m_pTriangles[numFace].n2 = n2 - 1;
				m_pTriangles[numFace].n3 = n3 - 1;
			}

			// UV indicies for triangle
			if (numUV != 0) {
				m_pTriangles[numFace].t1 = t1 - 1;
				m_pTriangles[numFace].t2 = t2 - 1;
				m_pTriangles[numFace].t3 = t3 - 1;
			}

			numFace++;
			break;
		default:
			break;
		}
	}


	fclose(fp);
	printf("Reading OBJ file is DONE.\n");

}

//--------------------------------------------------------------
// [Assignment1]
// Fill the following function to create display list
// of the obj file to show it as polygon
//--------------------------------------------------------------
void G308_Geometry::CreateGLPolyGeometry() {
	cout << "\nCreating PolyGeometry!\n";
	if (m_glGeomListPoly != 0){
		glDeleteLists(m_glGeomListPoly, 1);
	}
	// Assign a display list; return 0 if err
	m_glGeomListPoly = glGenLists(1);
	glNewList(m_glGeomListPoly, GL_COMPILE);


		glBegin(GL_TRIANGLES);
			// Looping over the triangle array
			for (int i=0; i<numFace; i++) {

				//glTexCoord2f(m_pUVArray[m_pTriangles[i].t1].u, m_pUVArray[m_pTriangles[i].t1].v);
				glNormal3f(m_pNormalArray[m_pTriangles[i].n1].x, m_pNormalArray[m_pTriangles[i].n1].y, m_pNormalArray[m_pTriangles[i].n1].z);
				glVertex3f(m_pVertexArray[m_pTriangles[i].v1].x, m_pVertexArray[m_pTriangles[i].v1].y, m_pVertexArray[m_pTriangles[i].v1].z);

				//glTexCoord2f(m_pUVArray[m_pTriangles[i].t2].u, m_pUVArray[m_pTriangles[i].t2].v);
				glNormal3f(m_pNormalArray[m_pTriangles[i].n2].x, m_pNormalArray[m_pTriangles[i].n2].y, m_pNormalArray[m_pTriangles[i].n2].z);
				glVertex3f(m_pVertexArray[m_pTriangles[i].v2].x, m_pVertexArray[m_pTriangles[i].v2].y, m_pVertexArray[m_pTriangles[i].v2].z);

				//glTexCoord2f(m_pUVArray[m_pTriangles[i].t3].u, m_pUVArray[m_pTriangles[i].t3].v);
				glNormal3f(m_pNormalArray[m_pTriangles[i].n3].x, m_pNormalArray[m_pTriangles[i].n3].y, m_pNormalArray[m_pTriangles[i].n3].z);
				glVertex3f(m_pVertexArray[m_pTriangles[i].v3].x, m_pVertexArray[m_pTriangles[i].v3].y, m_pVertexArray[m_pTriangles[i].v3].z);
			}
		glEnd();


	glEndList();
}


//--------------------------------------------------------------
// [Assignment1]
// Fill the following function to create display list
// of the obj file to show it as wireframe
//--------------------------------------------------------------
void G308_Geometry::CreateGLWireGeometry() {
	cout << "\nCreating WireGeometry!\n";

	if (m_glGeomListWire != 0){
		glDeleteLists(m_glGeomListWire, 1);
	}
	// Assign a display list; return 0 if err
	m_glGeomListWire = glGenLists(1);
	glNewList(m_glGeomListWire, GL_COMPILE);

		for (int i=0; i<numFace; i++) {
			glBegin(GL_LINE_LOOP);
				//glTexCoord2f(m_pUVArray[m_pTriangles[i].t1].u, m_pUVArray[m_pTriangles[i].t1].v);
				glNormal3f(m_pNormalArray[m_pTriangles[i].n1].x, m_pNormalArray[m_pTriangles[i].n1].y, m_pNormalArray[m_pTriangles[i].n1].z);
				glVertex3f(m_pVertexArray[m_pTriangles[i].v1].x, m_pVertexArray[m_pTriangles[i].v1].y, m_pVertexArray[m_pTriangles[i].v1].z);

				//glTexCoord2f(m_pUVArray[m_pTriangles[i].t2].u, m_pUVArray[m_pTriangles[i].t2].v);
				glNormal3f(m_pNormalArray[m_pTriangles[i].n2].x, m_pNormalArray[m_pTriangles[i].n2].y, m_pNormalArray[m_pTriangles[i].n2].z);
				glVertex3f(m_pVertexArray[m_pTriangles[i].v2].x, m_pVertexArray[m_pTriangles[i].v2].y, m_pVertexArray[m_pTriangles[i].v2].z);

				//glTexCoord2f(m_pUVArray[m_pTriangles[i].t3].u, m_pUVArray[m_pTriangles[i].t3].v);
				glNormal3f(m_pNormalArray[m_pTriangles[i].n3].x, m_pNormalArray[m_pTriangles[i].n3].y, m_pNormalArray[m_pTriangles[i].n3].z);
				glVertex3f(m_pVertexArray[m_pTriangles[i].v3].x, m_pVertexArray[m_pTriangles[i].v3].y, m_pVertexArray[m_pTriangles[i].v3].z);

			glEnd();
		}
	glEndList();
}


void G308_Geometry::toggleMode() {
	if (mode == G308_SHADE_POLYGON) {
		mode = G308_SHADE_WIREFRAME;
	} else {
		mode = G308_SHADE_POLYGON;
	}
}


void G308_Geometry::RenderGeometry() {

	if (mode == G308_SHADE_POLYGON) {
		//-------------------------------------------------------------
		// [Assignment1] :
		// Polygon mesh by glut. Comment this out to when moving onto displaying your .obj
		glShadeModel(GL_SMOOTH);
		//glutSolidTeapot(5.0);

		// [Assignment1] :
		// Uncomment the next line to display the polygon of your own .obj file
		glCallList(m_glGeomListPoly);


	} else if (mode == G308_SHADE_WIREFRAME) {
		//-------------------------------------------------------------
		// [Assignment1] :
		// Wireframe mesh by glut. Comment this out to when moving onto displaying your .obj
		glShadeModel(GL_SMOOTH);
		//glutWireTeapot(5.0);

		// [Assignment1] :
		// Uncomment the next line to display the wireframe of your own .obj file
		glCallList(m_glGeomListWire);

	} else {
		printf("Warning: Wrong Shading Mode. \n");
	}

}
