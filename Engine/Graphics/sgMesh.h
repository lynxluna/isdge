//
//	sgMesh.h
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

#ifndef __SGMESH_H__
#define __SGMESH_H__

#include "sgBase.h"

class sgVertex;
class sgVector3;

/**
 * Mesh class. This is the mesh format.
 */
class sgMesh : public sgBase
{
	public:
		/**
		 *	Constructor.
		 *	Initializes the instance.
		 */
		sgMesh();
		
		/**
		 *	Deconstructor.
		 *	Frees all memory allocated by this class.
		 */
		~sgMesh();
	
		/**
		 *	Generate VBO.
		 *	Generates a vertex buffer object of this mesh.
		 */
		void generateVBO(bool dyn = false);
	
		/**
		 *	Update VBO.
		 *	Updates the VBO to the current mesh, if it is dynamic and toggles the active VBO.
		 */
		void updateVBO();
	
		/**
		 *	Prepare for animation.
		 *	Transforms the vertexpositions into their bones space.
		 */
		void prepareAnimation();
	
		/**
		 *	Set animation.
		 *	Sets to a given position within the animation with the specified name.
		 * @param name name of the animation
		 * @param percent position within the animation in percent (0-100)
		 */
		void setAnimation(const char *name, float percent);
	
		/**
		 *	Calculate normals.
		 *	Calculates new normals for the mesh, which is needed for correct shading after the mesh was manipulated.
		 */
		void calculateNormals();
		
		/**
		 *	Calculate face normal.
		 *	Calculates the normal of a layer, streched between the three given vertices.
		 * @param vertex a first vertex
		 * @param neighbour_a a second vertex
		 * @param neighbour_b a third vertex
		 */
		void calculateFaceNormal(sgVertex *vertex, sgVertex *neighbour_a, sgVertex *neighbour_b);
	
		/**
		 *	Invert texcoords X.
		 *	Inverts the texture coordinates along the x axis.
		 */
		void invertTexCoordsX();
	
		/**
		 *	Invert texcoords Y.
		 *	Inverts the texture coordinates along the y axis.
		 */
		void invertTexCoordsY();
	
		/**
		 *	Vertex format.
		 *	The format of the vertices of this mesh (ranging from 0 to 3).
		 */
		unsigned int vertexformat;
	
		/**
		 *	Vertex number.
		 *	The number of vertices, this mesh consists of.
		 */
		unsigned int vertexnum;
	
		/**
		 *	Vertex array.
		 *	Pointer to the meshs array of vertices.
		 */
		sgVertex *vertices;
	
		/**
		 *	Index number.
		 *	The number of the meshs indices.
		 */
		unsigned int indexnum;
	
		/**
		 *	Index array.
		 *	Pointer to the meshs array of indices.
		 */
		unsigned short *indices;
	
		/**
		 *	VBO id.
		 *	The id of the meshs current vertex buffer object.
		 */
		unsigned int vbo;
	
		/**
		 *	Index VBO id.
		 *	The id of the meshs current vertex buffer object handling the indces.
		 */
		unsigned int ivbo;
	
		/**
		 *	Dynamic VBO.
		 *	True, if the mesh is meant to be dynamically changed.
		 */
		bool dynamic;
	
		/**
		 *	Bone number.
		 *	The number of the meshs bones.
		 */
//		unsigned int bonenum;
	
		/**
		 *	Bone array.
		 *	Array containing the bones of this model, which are used for animations.
		 */
//		sgBone *bones;
	
//		std::multimap<std::string, unsigned int*> animations;
	
	private:
		bool vbotoggle;
		unsigned int vbo0;
		unsigned int vbo1;
		unsigned int ivbo0;
		unsigned int ivbo1;
};

#endif