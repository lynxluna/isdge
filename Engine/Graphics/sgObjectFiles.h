//
//	sgObjectFiles.h
//	iSDGE
//
//	Created by Nils Daumann on 19.04.10.
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

#ifndef __SGOBJECTFILES_H__
#define __SGOBJECTFILES_H__

#include <vector>
#include <string>

#include "sgVertex.h"
class sgObjectContainer;

/**
 * Object file namespace. Contains functionality to loat object files.
 */
namespace sgObjectFiles
{
	enum LOAD_FLAGS
	{
		NONE = 0x00,
		GEN_TANGENT = 0x01
	};
	
	/**
	 * Material. Used to load the objects material information into.
	 */
	struct sgSGMMaterial
	{
		unsigned char id_;
		std::vector<std::string> texnames;
	};
	
	/**
	 * Mesh. Used to load the objects mesh information into.
	 */
	struct sgSGMMesh
	{
		unsigned char id_;
		sgSGMMaterial *material;
		unsigned long vtxfeatures;
		unsigned char vtxsize;
		
		unsigned short vertexnum;
		unsigned short indexnum;
		float *vertices;
		unsigned short *indices;
		unsigned short *bonemapping;
	};


	/**
	 *	Load sgm.
	 *	Loads the information from a file into an object.
	 * @param obj object to load the file into
	 * @param filename the name of the file to load the object from
	 * @return TRUE if it was successfull, FALSE otherwise
	 */
	bool loadSGM(sgObjectContainer *obj, const char *filename, unsigned long flags = NONE);
}

#endif
