//
//	sgMain.cpp
//	Engine
//
//	Created by Nils Daumann on 07.06.10.
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

#include "sgMain.h"

#include "sgRendererES1.h"
#include "sgRendererES2.h"

sgEvents *sgMain::eventhandler = NULL;

sgMain::sgMain(unsigned int oglvers)
{
	timestep = 0;
	if(oglvers == 1)
	{
		renderer = new sgRendererES1;
	}else if(oglvers >= 2)
	{
		renderer = new sgRendererES2;
	}
	
	setOrientation(0);
	first_ent = new sgEntity(NULL, NULL, this);
	
	timer.start();
}

sgMain::~sgMain()
{
	first_ent->destroyAll();
	delete renderer;
}

void sgMain::drawView()
{
	timer.stop();
	timestep = timestep*0.99f+timer.getElapsedTime()*0.01f;
	timer.start();
	
	if(eventhandler != NULL)
		eventhandler->onDraw(timestep);
	
	for(sgEntity *it = first_ent->next; it != NULL; it = it->next)
	{
		if(it->act != NULL)
		{
			it->act->onDraw(timestep);
		}
	}
	
	if(eventhandler != NULL)
		eventhandler->onDrawLate(timestep);
	
	renderer->render();
}

void sgMain::setOrientation(unsigned int interfaceOrientation)
{
	orientation = interfaceOrientation;
	renderer->setOrientation(orientation);
}