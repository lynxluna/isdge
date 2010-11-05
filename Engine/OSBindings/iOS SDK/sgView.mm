//
//	sgView.mm
//	iSDGE
//
//	Created by Nils Daumann on 16.04.10.
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

#import "sgView.h"

#include <OpenGLES/ES1/gl.h>
#include <OpenGLES/ES1/glext.h>
#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/glext.h>

//#define DISABLE_OPENGL_ES_2_0

@implementation sgView

@synthesize animating;
@dynamic animationFrameInterval;

sgMain *sgmain;
sgView *sgview;

+ (sgMain*)main
{
	return sgmain;
}

+ (sgView*)view
{
	return sgview;
}

// You must implement this method
+ (Class)layerClass
{
	return [CAEAGLLayer class];
}

- (void)applicationDidFinishLaunching:(UIApplication*)application
{    
	//Step 0 - Create a fullscreen window that we can use for OpenGL ES output
	CGRect rect = [[UIScreen mainScreen] bounds];	
	appwind = [[UIWindow alloc] initWithFrame:rect];
	
	if(!(self = [super initWithFrame:rect])) 
	{
		[self release];
		return;
	}
	
	//Initialise EAGL.
	CAEAGLLayer* eaglLayer = (CAEAGLLayer*)[self layer];	
	[eaglLayer setDrawableProperties: [NSDictionary dictionaryWithObjectsAndKeys: [NSNumber numberWithBool:NO], kEAGLDrawablePropertyRetainedBacking, kEAGLColorFormatRGBA8, kEAGLDrawablePropertyColorFormat, nil]];
	eaglLayer.opaque = TRUE;
	
	//Find supported OpenGL ES version
	unsigned int oglvers;
	context = nil;
	
#ifndef DISABLE_OPENGL_ES_2_0
	if(context == nil)
	{
		oglvers = 2;
		context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];
		if(!context || ![EAGLContext setCurrentContext:context])
		{
			context = nil;
		}
	}
#endif
	
	if(context == nil)
	{
		oglvers = 1;
		context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES1];
		if(!context || ![EAGLContext setCurrentContext:context])
		{
			oglvers = 0;
			context = nil;
		}
	}
	
	//Initialize the renderer
	sgmain = new sgMain(oglvers);
	[self resizeFromLayer:(CAEAGLLayer*)self.layer];

	//Configure and start accelerometer
	[[UIAccelerometer sharedAccelerometer] setUpdateInterval:(1.0/30.0f)];
	[[UIAccelerometer sharedAccelerometer] setDelegate:self];

	//Initialize drawing
	animating = FALSE;
	animationFrameInterval = 1;
	displayLink = nil;
	
	//Add this view to the window.
	[appwind addSubview: self];
	
	//Show the window.
	[appwind makeKeyAndVisible];
	
	sgview = self;
	
	if(sgMain::eventhandler != NULL)
		sgMain::eventhandler->onInit(sgmain);
	
	[self startAnimation];
}

- (void)drawView:(id)sender
{
	[EAGLContext setCurrentContext:context];
	sgmain->drawView();
	if(sgmain->renderer->oglversion >= 2)
	{
		[context presentRenderbuffer:GL_RENDERBUFFER];
	}else
	{
		[context presentRenderbuffer:GL_RENDERBUFFER_OES];
	}
}

- (BOOL)resizeFromLayer:(CAEAGLLayer*)layer
{
	if(sgmain->renderer->oglversion == 1)
	{
		// Allocate color buffer backing based on the current layer size
		glBindRenderbufferOES(GL_RENDERBUFFER_OES, sgmain->renderer->colorRenderbuffer);
		[context renderbufferStorage:GL_RENDERBUFFER_OES fromDrawable:layer];
		glGetRenderbufferParameterivOES(GL_RENDERBUFFER_OES, GL_RENDERBUFFER_WIDTH_OES, &sgRenderer::backingWidth);
		glGetRenderbufferParameterivOES(GL_RENDERBUFFER_OES, GL_RENDERBUFFER_HEIGHT_OES, &sgRenderer::backingHeight);
	}
	
	if(sgmain->renderer->oglversion >= 2)
	{
		// Allocate color buffer backing based on the current layer size
		glBindRenderbuffer(GL_RENDERBUFFER, sgmain->renderer->colorRenderbuffer);
		[context renderbufferStorage:GL_RENDERBUFFER fromDrawable:layer];
		glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH, &sgRenderer::backingWidth);
		glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT, &sgRenderer::backingHeight);
	}
	
	return sgmain->renderer->resizeBuffers();
}

- (void)layoutSubviews
{
    [self resizeFromLayer:(CAEAGLLayer*)self.layer];
    [self drawView:nil];
}

- (NSInteger)animationFrameInterval
{
    return animationFrameInterval;
}

- (void)setAnimationFrameInterval:(NSInteger)frameInterval
{
	if(frameInterval >= 1)
	{
		animationFrameInterval = frameInterval;
	
		if(animating)
		{
			[self stopAnimation];
			[self startAnimation];
		}
	}
}

- (void)startAnimation
{
    if(!animating)
    {
		displayLink = [CADisplayLink displayLinkWithTarget:self selector:@selector(drawView:)];
		[displayLink setFrameInterval:animationFrameInterval];
		[displayLink addToRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
		
        animating = TRUE;
    }
}

- (void)stopAnimation
{
    if(animating)
    {
        [displayLink invalidate];
        displayLink = nil;
		
        animating = FALSE;
    }
}

- (void)dealloc
{
	delete sgmain;
	[appwind release];
	
	// Tear down context
	if([EAGLContext currentContext] == context)
		[EAGLContext setCurrentContext:nil];
	
	if(context != nil)
	{
		[context release];
		context = nil;
	}
	
	[super dealloc];
}


//App delegate implementation
- (void)applicationWillResignActive:(UIApplication *)application
{
		[self stopAnimation];
}

- (void)applicationDidBecomeActive:(UIApplication *)application
{
		[self startAnimation];
}

- (void)applicationWillTerminate:(UIApplication *)application
{
		[self stopAnimation];
}


//Handle player input
- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event
{
	for(int i = 0; i < [touches count]; i++)
	{
		UITouch *curr = [touches anyObject];
		CGPoint pos = [curr locationInView:self];
		
		switch(sgmain->orientation)
		{
			case 0:
				sgTouches::addTouch(sgVector2(sgRenderer::backingWidth-pos.x, pos.y), sgVector2(pos.x, pos.y));
				break;
		
			case 1:
				sgTouches::addTouch(sgVector2(pos.x, sgRenderer::backingHeight-pos.y), sgVector2(pos.x, pos.y));
				break;

			case 2:
				sgTouches::addTouch(sgVector2(sgRenderer::backingHeight-pos.y, sgRenderer::backingWidth-pos.x), sgVector2(pos.x, pos.y));
				break;
				
			case 3:
				sgTouches::addTouch(sgVector2(pos.y, pos.x), sgVector2(pos.x, pos.y));
				break;
				
			default:
				sgTouches::addTouch(sgVector2(sgRenderer::backingWidth-pos.x, pos.y), sgVector2(pos.x, pos.y));
				break;
		}
	}
}

- (void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event
{
	UITouch *curr;
	for(int i = 0; i < sgTouches::touches.size(); i++)
	{
		curr = [[[event allTouches] allObjects] objectAtIndex:i];
		CGPoint pos = [curr locationInView:self];
		CGPoint ppos = [curr previousLocationInView:self];
		
		switch(sgmain->orientation)
		{
			case 0:
				sgTouches::updateTouch(i, sgVector2(sgRenderer::backingWidth-pos.x, pos.y), sgVector2(sgRenderer::backingWidth-ppos.x, ppos.y), sgVector2(pos.x, pos.y));
				break;
				
			case 1:
				sgTouches::updateTouch(i, sgVector2(pos.x, sgRenderer::backingHeight-pos.y), sgVector2(ppos.x, sgRenderer::backingHeight-ppos.y), sgVector2(pos.x, pos.y));
				break;
				
			case 2:
				sgTouches::updateTouch(i, sgVector2(sgRenderer::backingHeight-pos.y, sgRenderer::backingWidth-pos.x), sgVector2(sgRenderer::backingHeight-ppos.y, sgRenderer::backingWidth-ppos.x), sgVector2(pos.x, pos.y));
				break;
				
			case 3:
				sgTouches::updateTouch(i, sgVector2(pos.y, pos.x), sgVector2(ppos.y, ppos.x), sgVector2(pos.x, pos.y));
				break;
				
			default:
				sgTouches::updateTouch(i, sgVector2(sgRenderer::backingWidth-pos.x, pos.y), sgVector2(sgRenderer::backingWidth-ppos.x, ppos.y), sgVector2(pos.x, pos.y));
				break;
		}
	}
}

- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event
{
	for(int i = 0; i < [touches count]; i++)
		sgTouches::removeTouch(sgTouches::touches.size()-1);
	
	UITouch *curr;
	for(int i = 0; i < sgTouches::touches.size(); i++)
	{
		curr = [[[event allTouches] allObjects] objectAtIndex:i];
		CGPoint pos = [curr locationInView:self];
		CGPoint ppos = [curr previousLocationInView:self];
		
		switch(sgmain->orientation)
		{
			case 0:
				sgTouches::updateTouch(i, sgVector2(sgRenderer::backingWidth-pos.x, pos.y), sgVector2(sgRenderer::backingWidth-ppos.x, ppos.y), sgVector2(pos.x, pos.y));
				break;
				
			case 1:
				sgTouches::updateTouch(i, sgVector2(pos.x, sgRenderer::backingHeight-pos.y), sgVector2(ppos.x, sgRenderer::backingHeight-ppos.y), sgVector2(pos.x, pos.y));
				break;
				
			case 2:
				sgTouches::updateTouch(i, sgVector2(sgRenderer::backingHeight-pos.y, sgRenderer::backingWidth-pos.x), sgVector2(sgRenderer::backingHeight-ppos.y, sgRenderer::backingWidth-ppos.x), sgVector2(pos.x, pos.y));
				break;
				
			case 3:
				sgTouches::updateTouch(i, sgVector2(pos.y, pos.x), sgVector2(ppos.y, ppos.x), sgVector2(pos.x, pos.y));
				break;
				
			default:
				sgTouches::updateTouch(i, sgVector2(sgRenderer::backingWidth-pos.x, pos.y), sgVector2(sgRenderer::backingWidth-ppos.x, ppos.y), sgVector2(pos.x, pos.y));
				break;
		}
		
	}
}

- (void)touchesCancelled:(NSSet *)touches withEvent:(UIEvent *)event
{
	for(int i = 0; i < [touches count]; i++)
		sgTouches::removeTouch(sgTouches::touches.size()-1);
	
	UITouch *curr;
	for(int i = 0; i < sgTouches::touches.size(); i++)
	{
		curr = [[[event allTouches] allObjects] objectAtIndex:i];
		CGPoint pos = [curr locationInView:self];
		CGPoint ppos = [curr previousLocationInView:self];
		
		switch(sgmain->orientation)
		{
			case 0:
				sgTouches::updateTouch(i, sgVector2(sgRenderer::backingWidth-pos.x, pos.y), sgVector2(sgRenderer::backingWidth-ppos.x, ppos.y), sgVector2(pos.x, pos.y));
				break;
				
			case 1:
				sgTouches::updateTouch(i, sgVector2(pos.x, sgRenderer::backingHeight-pos.y), sgVector2(ppos.x, sgRenderer::backingHeight-ppos.y), sgVector2(pos.x, pos.y));
				break;
				
			case 2:
				sgTouches::updateTouch(i, sgVector2(sgRenderer::backingHeight-pos.y, sgRenderer::backingWidth-pos.x), sgVector2(sgRenderer::backingHeight-ppos.y, sgRenderer::backingWidth-ppos.x), sgVector2(pos.x, pos.y));
				break;
				
			case 3:
				sgTouches::updateTouch(i, sgVector2(pos.y, pos.x), sgVector2(ppos.y, ppos.x), sgVector2(pos.x, pos.y));
				break;
				
			default:
				sgTouches::updateTouch(i, sgVector2(sgRenderer::backingWidth-pos.x, pos.y), sgVector2(sgRenderer::backingWidth-ppos.x, ppos.y), sgVector2(pos.x, pos.y));
				break;
		}
		
	}
}

- (void)accelerometer:(UIAccelerometer*)accelerometer didAccelerate:(UIAcceleration*)acceleration
{	
	sgVector3 temp;
	temp.x = acceleration.x;
	temp.y = acceleration.y;
	temp.z = acceleration.z;
	
	sgAccelerometer::curraccabs = temp;
	
	switch(sgmain->orientation)
	{
		case 0:
			sgAccelerometer::curracc.x = temp.x;
			sgAccelerometer::curracc.y = temp.y;
			sgAccelerometer::curracc.z = temp.z;
			break;
			
		case 1:
			sgAccelerometer::curracc.x = -temp.x;
			sgAccelerometer::curracc.y = -temp.y;
			sgAccelerometer::curracc.z = -temp.z;
			break;
			
		case 2:
			sgAccelerometer::curracc.x = temp.y;
			sgAccelerometer::curracc.y = -temp.x;
			sgAccelerometer::curracc.z = temp.z;
			break;
			
		case 3:
			sgAccelerometer::curracc.x = -temp.y;
			sgAccelerometer::curracc.y = temp.x;
			sgAccelerometer::curracc.z = temp.z;
			break;
			
		default:
			sgAccelerometer::curracc.x = temp.x;
			sgAccelerometer::curracc.y = temp.y;
			sgAccelerometer::curracc.z = temp.z;
			break;
	}
}

@end