//
//	Camera.cpp
//	Engine
//
//	Created by Nils Daumann on 10.05.10.
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

#include "CameraFree.h"
#include "sgMain.h"

CameraFree::CameraFree()
{
	stereo_cam = NULL;
}

void CameraFree::makeStereo(float dist)
{
	stereodist = dist;
	
	stereo_cam = ent->cam->createCamera();
	
	switch(ent->sgmain->renderer->orientation)
	{
		case 0:
		{
			ent->cam->size = sgVector2(sgRenderer::backingWidth*0.5, sgRenderer::backingHeight);
			stereo_cam->screenpos = sgVector2(ent->cam->size.x, 0.0f);
			break;
		}
		
		case 1:
		{
			ent->cam->size = sgVector2(sgRenderer::backingWidth*0.5, sgRenderer::backingHeight);
			ent->cam->screenpos = sgVector2(ent->cam->size.x, 0.0f);
			stereo_cam->screenpos = sgVector2(0.0f, 0.0f);
			break;
		}
		
		case 2:
		{
			ent->cam->size = sgVector2(sgRenderer::backingWidth, sgRenderer::backingHeight*0.5);
			stereo_cam->screenpos = sgVector2(0.0f, ent->cam->size.y);
			break;
		}
			
		case 3:
		{
			ent->cam->size = sgVector2(sgRenderer::backingWidth, sgRenderer::backingHeight*0.5);
			ent->cam->screenpos = sgVector2(0.0f, ent->cam->size.y);
			stereo_cam->screenpos = sgVector2(0.0f, 0.0f);
			break;
		}
	}
	
	ent->cam->aspect = ent->cam->size.x/ent->cam->size.y;
	ent->cam->updateProj();
	
	stereo_cam->size = ent->cam->size;
	stereo_cam->aspect = ent->cam->aspect;
	stereo_cam->updateProj();
}

void CameraFree::makeAnaglyph(float dist, const char *fs)
{
	stereodist = dist;
	
	ent->cam->size = 512;
	
	stereo_cam = ent->cam->createCamera();
	stereo_cam->size = ent->cam->size;
	stereo_cam->arc = ent->cam->arc;
	stereo_cam->aspect = ent->cam->aspect;
	stereo_cam->updateProj();
	
	//Attach a texture and make it a rendertarget
	ent->cam->rendertarget = sgTexture::getTexture2D(512, 512);
	ent->cam->rendertarget->makeRendertarget();
	
	//Attach a texture and make it a rendertarget
	stereo_cam->rendertarget = sgTexture::getTexture2D(512, 512);
	stereo_cam->rendertarget->makeRendertarget();
	
	sgMaterial *mat = sgMaterial::getMaterial("default", "RedCyanStereo");
	mat->setTexture2D(-1, ent->cam->rendertarget);
	mat->setTexture2D(-1, stereo_cam->rendertarget);
	
	//Display it on the HUD
	sgEntity *hud = ent->sgmain->first_ent->createPanEntity();
	hud->pan->fixorientation = true;
	hud->pan->addImage(mat, sgVector2(0, 0), sgVector2(sgRenderer::backingWidth, sgRenderer::backingHeight));
}

void CameraFree::onDraw(float timestep)
{
	timestep *= 16.0f;
	
	sgVector2 toll;
	if(sgAccelerometer::curracc.x > 0.1f || sgAccelerometer::curracc.x < -0.1f)
	{
		toll.x = sgAccelerometer::curracc.x;
	}else
	{
		toll.x = 0.0;
	}
	if(sgAccelerometer::curracc.y > 0.1f || sgAccelerometer::curracc.y < -0.1f)
	{
		toll.y = sgAccelerometer::curracc.y;
	}else
	{
		toll.y = 0.0;
	}

	if(sgTouches::touches.size() == 1)
	{
		sgVector3 rot(sgTouches::touches[0]->direction.x, 0.0f, -sgTouches::touches[0]->direction.y);
		ent->cam->rotation += rot;
		sgTouches::touches[0]->direction.x = 0;
		sgTouches::touches[0]->direction.y = 0;
	}
	if(sgTouches::touches.size() == 2)
		toll.y = 1.0;
	
	sgVector3 dir(toll.x, 0.0f, -toll.y);
	dir = ent->cam->rotation.rotate(dir);
	ent->cam->position += dir*timestep;
	
	if(stereo_cam != NULL)
	{
		stereo_cam->rotation = ent->cam->rotation;
		
		dir = ent->cam->rotation.rotate(sgVector3(1.0f, 0.0f, 0.0f));
		stereo_cam->position = ent->cam->position+dir*stereodist;
	}
}