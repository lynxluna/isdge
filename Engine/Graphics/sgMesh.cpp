//
//	sgMesh.cpp
//	iSDGE
//
//	Created by Nils Daumann on 17.04.10.
//	Copyright (c) 2010 Nils Daumann

//	Permission is hereby granted, free of charge, to any person obtaining a copy
//	of this software and associated documentation files (the "Software"), to deal
//	in the Software without restriction, including without limitation the rights
//	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//	copies of the Software, and to permit persons to whom the Software is
//	furnished to do so, subject to the following conditions:

//	The above copyright notice and this permission notice shall be included in
//	all copies or substantial portions of the Software.

//	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
//	THE SOFTWARE.

#include "sgMesh.h"

#include <cmath>
#include <cstring>
#include <cstdlib>
#include <OpenGLES/ES1/gl.h>
#include <OpenGLES/ES1/glext.h>
#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/glext.h>
#include "sgVector3.h"
#include "sgDebug.h"

sgMesh::sgMesh()
{
	vbo = -1;
	ivbo = -1;
	vbo0 = -1;
	ivbo0 = -1;
	vbo1 = -1;
	ivbo1 = -1;
	vbotoggle = false;
	dynamic = false;
	
	vertexnum = 0;
	indexnum = 0;
	vertices = NULL;
	indices = NULL;
	
	culled = false;
}

sgMesh::~sgMesh()
{
	if(vbo0 != -1)
		glDeleteBuffers(1, &vbo0);
	if(ivbo0 != -1)
		glDeleteBuffers(1, &ivbo0);
	if(vbo1 != -1)
		glDeleteBuffers(1, &vbo1);
	if(ivbo1 != -1)
		glDeleteBuffers(1, &ivbo1);
	
	free(vertices);
	delete[] indices;
}

void sgMesh::generateVBO(bool dyn)
{
	dynamic = dyn;
	
	if(vbo0 != -1)
		glDeleteBuffers(1, &vbo0);
	if(ivbo0 != -1)
		glDeleteBuffers(1, &ivbo0);
	if(vbo1 != -1)
		glDeleteBuffers(1, &vbo1);
	if(ivbo1 != -1)
		glDeleteBuffers(1, &ivbo1);
	
	vbo = -1;
	ivbo = -1;
	vbo0 = -1;
	ivbo0 = -1;
	vbo1 = -1;
	ivbo1 = -1;
	vbotoggle = false;
	
	if(!dynamic)
	{
		glGenBuffers(1, &vbo0);
		glBindBuffer(GL_ARRAY_BUFFER, vbo0);
		glBufferData(GL_ARRAY_BUFFER, vertexnum*vtxsize, vertices, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		
		glGenBuffers(1, &ivbo0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ivbo0);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexnum*sizeof(unsigned short), indices, GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		vbo = vbo0;
		ivbo = ivbo0;
	}else
	{
		glGenBuffers(1, &vbo0);
		glBindBuffer(GL_ARRAY_BUFFER, vbo0);
		glBufferData(GL_ARRAY_BUFFER, vertexnum*vtxsize, vertices, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		
		glGenBuffers(1, &ivbo0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ivbo0);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexnum*sizeof(unsigned short), indices, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		
		glGenBuffers(1, &vbo1);
		glBindBuffer(GL_ARRAY_BUFFER, vbo1);
		glBufferData(GL_ARRAY_BUFFER, vertexnum*vtxsize, vertices, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		
		glGenBuffers(1, &ivbo1);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ivbo1);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexnum*sizeof(unsigned short), indices, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		vbo = vbo0;
		ivbo = ivbo0;
	}
}

void sgMesh::updateVBO()
{
	if(!dynamic)
		return;

	vbotoggle = !vbotoggle;
	
	if(vbotoggle)
	{
		vbo = vbo1;
		ivbo = ivbo1;
		
		glBindBuffer(GL_ARRAY_BUFFER, vbo0);
		glBufferSubData(GL_ARRAY_BUFFER, 0, vertexnum*vtxsize, vertices);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		
		glBindBuffer(GL_ARRAY_BUFFER, ivbo0);
		glBufferSubData(GL_ARRAY_BUFFER, 0, indexnum*sizeof(unsigned short), indices);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}else
	{
		vbo = vbo0;
		ivbo = ivbo0;
		
		glBindBuffer(GL_ARRAY_BUFFER, vbo1);
		glBufferSubData(GL_ARRAY_BUFFER, 0, vertexnum*vtxsize, vertices);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		
		glBindBuffer(GL_ARRAY_BUFFER, ivbo1);
		glBufferSubData(GL_ARRAY_BUFFER, 0, indexnum*sizeof(unsigned short), indices);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
}

void sgMesh::calculateNormals()
{
	int i;
	int n;
	sgVertex *tempvtx;
	sgVertex *tempverts = (sgVertex*)malloc(vtxsize*vertexnum);
	
	//Reset all normals
	for(i = 0; i < vertexnum; i++)
	{
		if(vtxform == BASIC)
		{
			tempvtx = &vertices[i];
		}else if(vtxform == SECONDUV)
		{
			tempvtx = (sgVertex*)&((sgVertexUV*)vertices)[i];
		}else if(vtxform == COLOR)
		{
			tempvtx = (sgVertex*)&((sgVertexCol*)vertices)[i];
		}else if(vtxform == SECONDUVCOLOR)
		{
			tempvtx = (sgVertex*)&((sgVertexUVCol*)vertices)[i];
		}else if(vtxform == TANGENT)
		{
			tempvtx = (sgVertex*)&((sgVertexTan*)vertices)[i];
		}else if(vtxform == TANGENTSECONDUV)
		{
			tempvtx = (sgVertex*)&((sgVertexTanUV*)vertices)[i];
		}else if(vtxform == TANGENTCOLOR)
		{
			tempvtx = (sgVertex*)&((sgVertexTanCol*)vertices)[i];
		}else if(vtxform == TANGENTSECONDUVCOLOR)
		{
			tempvtx = (sgVertex*)&((sgVertexTanUVCol*)vertices)[i];
		}
		
		tempvtx->normal.x = 0;
		tempvtx->normal.y = 0;
		tempvtx->normal.z = 0;
	}
	memcpy(tempverts, vertices, vtxsize*vertexnum);
	
	//Calculate the normal of each vertex
	for(i = 0; i < indexnum; i += 3)
	{
		if(vtxform == BASIC)
		{
			calculateFaceNormal(&vertices[indices[i]], &vertices[indices[i+2]], &vertices[indices[i+1]]);
		}else if(vtxform == SECONDUV)
		{
			calculateFaceNormal((sgVertex*)&((sgVertexUV*)vertices)[indices[i]], (sgVertex*)&((sgVertexUV*)vertices)[indices[i+2]], (sgVertex*)&((sgVertexUV*)vertices)[indices[i+1]]);
		}else if(vtxform == COLOR)
		{
			calculateFaceNormal((sgVertex*)&((sgVertexCol*)vertices)[indices[i]], (sgVertex*)&((sgVertexCol*)vertices)[indices[i+2]], (sgVertex*)&((sgVertexCol*)vertices)[indices[i+1]]);
		}else if(vtxform == SECONDUVCOLOR)
		{
			calculateFaceNormal((sgVertex*)&((sgVertexUVCol*)vertices)[indices[i]], (sgVertex*)&((sgVertexUVCol*)vertices)[indices[i+2]], (sgVertex*)&((sgVertexUVCol*)vertices)[indices[i+1]]);
		}else if(vtxform == TANGENT)
		{
			calculateFaceNormal((sgVertex*)&((sgVertexTan*)vertices)[indices[i]], (sgVertex*)&((sgVertexTan*)vertices)[indices[i+2]], (sgVertex*)&((sgVertexTan*)vertices)[indices[i+1]]);
		}else if(vtxform == TANGENTSECONDUV)
		{
			calculateFaceNormal((sgVertex*)&((sgVertexTanUV*)vertices)[indices[i]], (sgVertex*)&((sgVertexTanUV*)vertices)[indices[i+2]], (sgVertex*)&((sgVertexTanUV*)vertices)[indices[i+1]]);
		}else if(vtxform == TANGENTCOLOR)
		{
			calculateFaceNormal((sgVertex*)&((sgVertexTanCol*)vertices)[indices[i]], (sgVertex*)&((sgVertexTanCol*)vertices)[indices[i+2]], (sgVertex*)&((sgVertexTanCol*)vertices)[indices[i+1]]);
		}else if(vtxform == TANGENTSECONDUVCOLOR)
		{
			calculateFaceNormal((sgVertex*)&((sgVertexTanUVCol*)vertices)[indices[i]], (sgVertex*)&((sgVertexTanUVCol*)vertices)[indices[i+2]], (sgVertex*)&((sgVertexTanUVCol*)vertices)[indices[i+1]]);
		}
	}
	
	//Normalize all normals
	for(i = 0; i < vertexnum; i++)
	{
		if(vtxform == BASIC)
		{
			tempvtx = &vertices[i];
		}else if(vtxform == SECONDUV)
		{
			tempvtx = (sgVertex*)&((sgVertexUV*)vertices)[i];
		}else if(vtxform == COLOR)
		{
			tempvtx = (sgVertex*)&((sgVertexCol*)vertices)[i];
		}else if(vtxform == SECONDUVCOLOR)
		{
			tempvtx = (sgVertex*)&((sgVertexUVCol*)vertices)[i];
		}else if(vtxform == TANGENT)
		{
			tempvtx = (sgVertex*)&((sgVertexTan*)vertices)[i];
		}else if(vtxform == TANGENTSECONDUV)
		{
			tempvtx = (sgVertex*)&((sgVertexTanUV*)vertices)[i];
		}else if(vtxform == TANGENTCOLOR)
		{
			tempvtx = (sgVertex*)&((sgVertexTanCol*)vertices)[i];
		}else if(vtxform == TANGENTSECONDUVCOLOR)
		{
			tempvtx = (sgVertex*)&((sgVertexTanUVCol*)vertices)[i];
		}
		tempvtx->normal.normalize();
	}
	
	//Interpolate normals between doublicated vertices
	for(i = 0; i < vertexnum; i++)
	{
		for(n = 0; n < vertexnum; n++)
		{
			if(vtxform == BASIC)
			{
				if(vertices[i].position.dist(vertices[n].position) < 0.00001 && vertices[i].normal.dot(vertices[n].normal) > 0.3)
					tempverts[i].normal += vertices[n].normal;
			}else if(vtxform == SECONDUV)
			{
				if(((sgVertexUV*)vertices)[i].position.dist(((sgVertexUV*)vertices)[n].position) < 0.000001 && ((sgVertexUV*)vertices)[i].normal.dist(((sgVertexUV*)vertices)[n].normal) > 0)
					((sgVertexUV*)tempverts)[i].normal += ((sgVertexUV*)vertices)[n].normal;
			}else if(vtxform == COLOR)
			{
				if(((sgVertexCol*)vertices)[i].position.dist(((sgVertexCol*)vertices)[n].position) < 0.000001 && ((sgVertexCol*)vertices)[i].normal.dist(((sgVertexCol*)vertices)[n].normal) > 0)
					((sgVertexCol*)tempverts)[i].normal += ((sgVertexCol*)vertices)[n].normal;
			}else if(vtxform == SECONDUVCOLOR)
			{
				if(((sgVertexUVCol*)vertices)[i].position.dist(((sgVertexUVCol*)vertices)[n].position) < 0.000001 && ((sgVertexUVCol*)vertices)[i].normal.dist(((sgVertexUVCol*)vertices)[n].normal) > 0)
					((sgVertexUVCol*)tempverts)[i].normal += ((sgVertexUVCol*)vertices)[n].normal;
			}else if(vtxform == TANGENT)
			{
				if(((sgVertexTan*)vertices)[i].position.dist(((sgVertexTan*)vertices)[n].position) < 0.000001 && ((sgVertexTan*)vertices)[i].normal.dist(((sgVertexTan*)vertices)[n].normal) > 0)
					((sgVertexTan*)tempverts)[i].normal += ((sgVertexTan*)vertices)[n].normal;
			}else if(vtxform == TANGENTSECONDUV)
			{
				if(((sgVertexTanUV*)vertices)[i].position.dist(((sgVertexTanUV*)vertices)[n].position) < 0.000001 && ((sgVertexTanUV*)vertices)[i].normal.dist(((sgVertexTanUV*)vertices)[n].normal) > 0)
					((sgVertexTanUV*)tempverts)[i].normal += ((sgVertexTanUV*)vertices)[n].normal;
			}else if(vtxform == TANGENTCOLOR)
			{
				if(((sgVertexTanCol*)vertices)[i].position.dist(((sgVertexTanCol*)vertices)[n].position) < 0.000001 && ((sgVertexTanCol*)vertices)[i].normal.dist(((sgVertexTanCol*)vertices)[n].normal) > 0)
					((sgVertexTanCol*)tempverts)[i].normal += ((sgVertexTanCol*)vertices)[n].normal;
			}else if(vtxform == TANGENTSECONDUVCOLOR)
			{
				if(((sgVertexTanUVCol*)vertices)[i].position.dist(((sgVertexTanUVCol*)vertices)[n].position) < 0.000001 && ((sgVertexTanUVCol*)vertices)[i].normal.dist(((sgVertexTanUVCol*)vertices)[n].normal) > 0)
					((sgVertexTanUVCol*)tempverts)[i].normal += ((sgVertexTanUVCol*)vertices)[n].normal;
			}
		}
	}
	free(vertices);
	vertices = tempverts;
	
	//Normalize all normals
	for(i = 0; i < vertexnum; i++)
	{
		if(vtxform == BASIC)
		{
			tempvtx = &vertices[i];
		}else if(vtxform == SECONDUV)
		{
			tempvtx = (sgVertex*)&((sgVertexUV*)vertices)[i];
		}else if(vtxform == COLOR)
		{
			tempvtx = (sgVertex*)&((sgVertexCol*)vertices)[i];
		}else if(vtxform == SECONDUVCOLOR)
		{
			tempvtx = (sgVertex*)&((sgVertexUVCol*)vertices)[i];
		}else if(vtxform == TANGENT)
		{
			tempvtx = (sgVertex*)&((sgVertexTan*)vertices)[i];
		}else if(vtxform == TANGENTSECONDUV)
		{
			tempvtx = (sgVertex*)&((sgVertexTanUV*)vertices)[i];
		}else if(vtxform == TANGENTCOLOR)
		{
			tempvtx = (sgVertex*)&((sgVertexTanCol*)vertices)[i];
		}else if(vtxform == TANGENTSECONDUVCOLOR)
		{
			tempvtx = (sgVertex*)&((sgVertexTanUVCol*)vertices)[i];
		}
		tempvtx->normal.normalize();
	}
}

void sgMesh::calculateTangents()
{
	if(vtxform == BASIC || vtxform == SECONDUV || vtxform == COLOR || vtxform == SECONDUVCOLOR)
		return;
	
	int i;
	int n;
	sgVertexTan *tempvtx;
	sgVertexTan *tempverts = (sgVertexTan*)malloc(vtxsize*vertexnum);
	sgVector3 temptangent;
	sgVector3 *bitangents = new sgVector3[vertexnum];
	
	//Reset all tangents
	for(i = 0; i < vertexnum; i++)
	{
		if(vtxform == TANGENT)
		{
			tempvtx = (sgVertexTan*)&((sgVertexTan*)vertices)[i];
		}else if(vtxform == TANGENTSECONDUV)
		{
			tempvtx = (sgVertexTan*)&((sgVertexTanUV*)vertices)[i];
		}else if(vtxform == TANGENTCOLOR)
		{
			tempvtx = (sgVertexTan*)&((sgVertexTanCol*)vertices)[i];
		}else if(vtxform == TANGENTSECONDUVCOLOR)
		{
			tempvtx = (sgVertexTan*)&((sgVertexTanUVCol*)vertices)[i];
		}
		
		tempvtx->tangent.x = 0;
		tempvtx->tangent.y = 0;
		tempvtx->tangent.z = 0;
		tempvtx->tangent.w = 0;
	}
	memcpy(tempverts, vertices, vtxsize*vertexnum);
	
	//Calculate the tangent of each vertex
	for(i = 0; i < indexnum; i += 3)
	{
		if(vtxform == TANGENT)
		{
			calculateFaceTangent((sgVertexTan*)&((sgVertexTan*)vertices)[indices[i]], (sgVertexTan*)&((sgVertexTan*)vertices)[indices[i+2]], (sgVertexTan*)&((sgVertexTan*)vertices)[indices[i+1]], &bitangents[indices[i]], &bitangents[indices[i+2]], &bitangents[indices[i+1]]);
		}else if(vtxform == TANGENTSECONDUV)
		{
			calculateFaceTangent((sgVertexTan*)&((sgVertexTanUV*)vertices)[indices[i]], (sgVertexTan*)&((sgVertexTanUV*)vertices)[indices[i+2]], (sgVertexTan*)&((sgVertexTanUV*)vertices)[indices[i+1]], &bitangents[indices[i]], &bitangents[indices[i+2]], &bitangents[indices[i+1]]);
		}else if(vtxform == TANGENTCOLOR)
		{
			calculateFaceTangent((sgVertexTan*)&((sgVertexTanCol*)vertices)[indices[i]], (sgVertexTan*)&((sgVertexTanCol*)vertices)[indices[i+2]], (sgVertexTan*)&((sgVertexTanCol*)vertices)[indices[i+1]], &bitangents[indices[i]], &bitangents[indices[i+2]], &bitangents[indices[i+1]]);
		}else if(vtxform == TANGENTSECONDUVCOLOR)
		{
			calculateFaceTangent((sgVertexTan*)&((sgVertexTanUVCol*)vertices)[indices[i]], (sgVertexTan*)&((sgVertexTanUVCol*)vertices)[indices[i+2]], (sgVertexTan*)&((sgVertexTanUVCol*)vertices)[indices[i+1]], &bitangents[indices[i]], &bitangents[indices[i+2]], &bitangents[indices[i+1]]);
		}
	}
	
	//Normalize all tangents
	for(i = 0; i < vertexnum; i++)
	{
		if(vtxform == TANGENT)
		{
			tempvtx = (sgVertexTan*)&((sgVertexTan*)vertices)[i];
		}else if(vtxform == TANGENTSECONDUV)
		{
			tempvtx = (sgVertexTan*)&((sgVertexTanUV*)vertices)[i];
		}else if(vtxform == TANGENTCOLOR)
		{
			tempvtx = (sgVertexTan*)&((sgVertexTanCol*)vertices)[i];
		}else if(vtxform == TANGENTSECONDUVCOLOR)
		{
			tempvtx = (sgVertexTan*)&((sgVertexTanUVCol*)vertices)[i];
		}
		temptangent = tempvtx->tangent;
		temptangent -= tempvtx->normal*tempvtx->normal.dot(tempvtx->tangent);
		temptangent.normalize();
		tempvtx->tangent = temptangent;
	}
	
	//Interpolate tangents between doublicated vertices
	for(i = 0; i < vertexnum; i++)
	{
		for(n = 0; n < vertexnum; n++)
		{
			if(vtxform == TANGENT)
			{
				if(((sgVertexTan*)vertices)[i].position.dist(((sgVertexTan*)vertices)[n].position) < 0.000001 && ((sgVertexTan*)vertices)[i].normal.dot(((sgVertexTan*)vertices)[n].normal) > 0.9)
					((sgVertexTan*)tempverts)[i].tangent += ((sgVertexTan*)vertices)[n].tangent;
			}else if(vtxform == TANGENTSECONDUV)
			{
				if(((sgVertexTanUV*)vertices)[i].position.dist(((sgVertexTanUV*)vertices)[n].position) < 0.000001 && ((sgVertexTanUV*)vertices)[i].normal.dot(((sgVertexTanUV*)vertices)[n].normal) > 0.9)
					((sgVertexTanUV*)tempverts)[i].tangent += ((sgVertexTanUV*)vertices)[n].tangent;
			}else if(vtxform == TANGENTCOLOR)
			{
				if(((sgVertexTanCol*)vertices)[i].position.dist(((sgVertexTanCol*)vertices)[n].position) < 0.000001 && ((sgVertexTanCol*)vertices)[i].normal.dot(((sgVertexTanCol*)vertices)[n].normal) > 0.9)
					((sgVertexTanCol*)tempverts)[i].tangent += ((sgVertexTanCol*)vertices)[n].tangent;
			}else if(vtxform == TANGENTSECONDUVCOLOR)
			{
				if(((sgVertexTanUVCol*)vertices)[i].position.dist(((sgVertexTanUVCol*)vertices)[n].position) < 0.000001 && ((sgVertexTanUVCol*)vertices)[i].normal.dot(((sgVertexTanUVCol*)vertices)[n].normal) > 0.9)
					((sgVertexTanUVCol*)tempverts)[i].tangent += ((sgVertexTanUVCol*)vertices)[n].tangent;
			}
		}
	}
	free(vertices);
	vertices = (sgVertex*)tempverts;
	
	//Normalize all tangents
	for(i = 0; i < vertexnum; i++)
	{
		if(vtxform == TANGENT)
		{
			tempvtx = (sgVertexTan*)&((sgVertexTan*)vertices)[i];
		}else if(vtxform == TANGENTSECONDUV)
		{
			tempvtx = (sgVertexTan*)&((sgVertexTanUV*)vertices)[i];
		}else if(vtxform == TANGENTCOLOR)
		{
			tempvtx = (sgVertexTan*)&((sgVertexTanCol*)vertices)[i];
		}else if(vtxform == TANGENTSECONDUVCOLOR)
		{
			tempvtx = (sgVertexTan*)&((sgVertexTanUVCol*)vertices)[i];
		}
		tempvtx->tangent.normalize();
	}
	
	//Calculate bitangent direction
	for(i = 0; i < vertexnum; i++)
	{
		if(vtxform == TANGENT)
		{
			tempvtx = (sgVertexTan*)&((sgVertexTan*)vertices)[i];
		}else if(vtxform == TANGENTSECONDUV)
		{
			tempvtx = (sgVertexTan*)&((sgVertexTanUV*)vertices)[i];
		}else if(vtxform == TANGENTCOLOR)
		{
			tempvtx = (sgVertexTan*)&((sgVertexTanCol*)vertices)[i];
		}else if(vtxform == TANGENTSECONDUVCOLOR)
		{
			tempvtx = (sgVertexTan*)&((sgVertexTanUVCol*)vertices)[i];
		}
		temptangent = tempvtx->tangent;
		tempvtx->tangent.w = (tempvtx->normal.cross(temptangent).dot(bitangents[i]) < 0.0f)? -1.0f : 1.0f;
	}
	
	delete[] bitangents;
}

void sgMesh::calculateFaceNormal(sgVertex *vertex, sgVertex *neighbour_a, sgVertex *neighbour_b)
{
	float a[3];
	float b[3];
	float normal[3];
	
	a[0] = vertex->position.x - neighbour_a->position.x;
	a[1] = vertex->position.y - neighbour_a->position.y;
	a[2] = vertex->position.z - neighbour_a->position.z;
	
	b[0] = neighbour_a->position.x - neighbour_b->position.x;
	b[1] = neighbour_a->position.y - neighbour_b->position.y;
	b[2] = neighbour_a->position.z - neighbour_b->position.z;
	
	normal[0] = (a[1] * b[2]) - (a[2] * b[1]);
	normal[1] = (a[2] * b[0]) - (a[0] * b[2]);
	normal[2] = (a[0] * b[1]) - (a[1] * b[0]);
	
	float len = normal[0]*normal[0]+normal[1]*normal[1]+normal[2]*normal[2];
	if(len == 0.0f) len = 1.0;
	len = sqrt(len);
	
	normal[0] /= -len;
	normal[1] /= -len;
	normal[2] /= -len;
	
	vertex->normal.x = normal[0];
	vertex->normal.y = normal[1];
	vertex->normal.z = normal[2];
	
	neighbour_a->normal.x = normal[0];
	neighbour_a->normal.y = normal[1];
	neighbour_a->normal.z = normal[2];
	neighbour_b->normal.x = normal[0];
	neighbour_b->normal.y = normal[1];
	neighbour_b->normal.z = normal[2];
}

void sgMesh::calculateFaceTangent(sgVertexTan *vertex, sgVertexTan *neighbour_a, sgVertexTan *neighbour_b, sgVector3 *bitangent0, sgVector3 *bitangent1, sgVector3 *bitangent2)
{
	sgVector3 posdir1 = neighbour_a->position-vertex->position;
	sgVector3 posdir2 = neighbour_b->position-vertex->position;
	sgVector2 uvdir1 = neighbour_a->uv-vertex->uv;
	sgVector2 uvdir2 = neighbour_b->uv-vertex->uv;
	
	float r = (uvdir1.x*uvdir2.y-uvdir2.x*uvdir1.y);
	if(r != 0)
		r = 1.0/r;
	else
		r = 1.0;
	sgVector4 tangent((uvdir2.y*posdir1.x-uvdir1.y*posdir2.x)*r, (uvdir2.y*posdir1.y-uvdir1.y*posdir2.y)*r, (uvdir2.y*posdir1.z-uvdir1.y*posdir2.z)*r, 0.0);
	sgVector3 bitangent((uvdir1.x*posdir2.x-uvdir2.x*posdir1.x)*r, (uvdir1.x*posdir2.y-uvdir2.x*posdir1.y)*r, (uvdir1.x*posdir2.z-uvdir2.x*posdir1.z)*r);
	
	vertex->tangent += tangent;
	neighbour_a->tangent += tangent;
	neighbour_b->tangent += tangent;
	
	*bitangent0 += bitangent;
	*bitangent1 += bitangent;
	*bitangent2 += bitangent;
}

void sgMesh::invertTexCoordsX()
{
	if(vtxform == BASIC)
	{
		for(int i = 0; i < vertexnum; i++)
		{
			vertices[i].uv.x = 1.0f-vertices[i].uv.x;
		}
	}else if(vtxform == SECONDUV)
	{
		for(int i = 0; i < vertexnum; i++)
		{
			((sgVertexUV*)vertices)[i].uv.x = 1.0f-((sgVertexUV*)vertices)[i].uv.x;
		}
	}else if(vtxform == COLOR)
	{
		for(int i = 0; i < vertexnum; i++)
		{
			((sgVertexCol*)vertices)[i].uv.x = 1.0f-((sgVertexCol*)vertices)[i].uv.x;
		}
	}else if(vtxform == SECONDUVCOLOR)
	{
		for(int i = 0; i < vertexnum; i++)
		{
			((sgVertexUVCol*)vertices)[i].uv.x = 1.0f-((sgVertexUVCol*)vertices)[i].uv.x;
		}
	}else if(vtxform == TANGENT)
	{
		for(int i = 0; i < vertexnum; i++)
		{
			((sgVertexTan*)vertices)[i].uv.x = 1.0f-((sgVertexTan*)vertices)[i].uv.x;
		}
	}else if(vtxform == TANGENTSECONDUV)
	{
		for(int i = 0; i < vertexnum; i++)
		{
			((sgVertexTanUV*)vertices)[i].uv.x = 1.0f-((sgVertexTanUV*)vertices)[i].uv.x;
		}
	}else if(vtxform == TANGENTCOLOR)
	{
		for(int i = 0; i < vertexnum; i++)
		{
			((sgVertexTanCol*)vertices)[i].uv.x = 1.0f-((sgVertexTanCol*)vertices)[i].uv.x;
		}
	}else if(vtxform == TANGENTSECONDUVCOLOR)
	{
		for(int i = 0; i < vertexnum; i++)
		{
			((sgVertexTanUVCol*)vertices)[i].uv.x = 1.0f-((sgVertexTanUVCol*)vertices)[i].uv.x;
		}
	}
}

void sgMesh::invertTexCoordsY()
{
	if(vtxform == BASIC)
	{
		for(int i = 0; i < vertexnum; i++)
		{
			vertices[i].uv.y = 1.0f-vertices[i].uv.y;
		}
	}else if(vtxform == SECONDUV)
	{
		for(int i = 0; i < vertexnum; i++)
		{
			((sgVertexUV*)vertices)[i].uv.y = 1.0f-((sgVertexUV*)vertices)[i].uv.y;
		}
	}else if(vtxform == COLOR)
	{
		for(int i = 0; i < vertexnum; i++)
		{
			((sgVertexCol*)vertices)[i].uv.y = 1.0f-((sgVertexCol*)vertices)[i].uv.y;
		}
	}else if(vtxform == SECONDUVCOLOR)
	{
		for(int i = 0; i < vertexnum; i++)
		{
			((sgVertexUVCol*)vertices)[i].uv.y = 1.0f-((sgVertexUVCol*)vertices)[i].uv.y;
		}
	}else if(vtxform == TANGENT)
	{
		for(int i = 0; i < vertexnum; i++)
		{
			((sgVertexTan*)vertices)[i].uv.y = 1.0f-((sgVertexTan*)vertices)[i].uv.y;
		}
	}else if(vtxform == TANGENTSECONDUV)
	{
		for(int i = 0; i < vertexnum; i++)
		{
			((sgVertexTanUV*)vertices)[i].uv.y = 1.0f-((sgVertexTanUV*)vertices)[i].uv.y;
		}
	}else if(vtxform == TANGENTCOLOR)
	{
		for(int i = 0; i < vertexnum; i++)
		{
			((sgVertexTanCol*)vertices)[i].uv.y = 1.0f-((sgVertexTanCol*)vertices)[i].uv.y;
		}
	}else if(vtxform == TANGENTSECONDUVCOLOR)
	{
		for(int i = 0; i < vertexnum; i++)
		{
			((sgVertexTanUVCol*)vertices)[i].uv.y = 1.0f-((sgVertexTanUVCol*)vertices)[i].uv.y;
		}
	}
}

void sgMesh::calcCullSphere()
{
	//Find the center
	sgVector3 vmax(-1000000000.0, -1000000000.0, -1000000000.0);
	sgVector3 vmin(1000000000.0, 1000000000.0, 1000000000.0);
	sgVector3 center;
	sgVertex *tempvtx;
	for(int i = 0; i < vertexnum; i++)
	{
		if(vtxform == BASIC)
		{
			tempvtx = &vertices[i];
		}else if(vtxform == SECONDUV)
		{
			tempvtx = (sgVertex*)&((sgVertexUV*)vertices)[i];
		}else if(vtxform == COLOR)
		{
			tempvtx = (sgVertex*)&((sgVertexCol*)vertices)[i];
		}else if(vtxform == SECONDUVCOLOR)
		{
			tempvtx = (sgVertex*)&((sgVertexUVCol*)vertices)[i];
		}else if(vtxform == TANGENT)
		{
			tempvtx = (sgVertex*)&((sgVertexTan*)vertices)[i];
		}else if(vtxform == TANGENTSECONDUV)
		{
			tempvtx = (sgVertex*)&((sgVertexTanUV*)vertices)[i];
		}else if(vtxform == TANGENTCOLOR)
		{
			tempvtx = (sgVertex*)&((sgVertexTanCol*)vertices)[i];
		}else if(vtxform == TANGENTSECONDUVCOLOR)
		{
			tempvtx = (sgVertex*)&((sgVertexTanUVCol*)vertices)[i];
		}
		
		if(tempvtx->position.x > vmax.x)
			vmax.x = tempvtx->position.x;
		if(tempvtx->position.y > vmax.y)
			vmax.y = tempvtx->position.y;
		if(tempvtx->position.z > vmax.z)
			vmax.z = tempvtx->position.z;
		
		if(tempvtx->position.x < vmin.x)
			vmin.x = tempvtx->position.x;
		if(tempvtx->position.y < vmin.y)
			vmin.y = tempvtx->position.y;
		if(tempvtx->position.z < vmin.z)
			vmin.z = tempvtx->position.z;
	}
	center = vmin+vmax;
	center *= 0.5;
	
	//Find the radius
	float radius = center.dist(vmax);
	cullsphere = sgVector4(center.x, center.y, center.z, radius);
}
