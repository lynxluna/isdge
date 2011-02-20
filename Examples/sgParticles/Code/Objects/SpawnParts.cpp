//
//	SpawnParts.cpp
//	Engine
//
//	Created by Nils Daumann on 15.02.11.
//	Copyright (c) 2011 Nils Daumann

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

#include "SpawnParts.h"

#include <OpenGLES/ES2/gl.h>

void SpawnParts::onInit(sgEntity *e)
{
	ent = e;
	currtime = 0;
	
	ent->emitt->material->blendsource = GL_SRC_ALPHA;
	ent->emitt->material->blenddestination = GL_ONE;
}

void SpawnParts::onDraw(float timestep)
{
	currtime += timestep;
	
	if((currtime-lastparttime) > (0.2) && ent->obj != NULL)
	{
		ent->emitt->emitPart((sgParticle*)new TestPart(ent->obj));
		lastparttime = currtime;
	}
}
