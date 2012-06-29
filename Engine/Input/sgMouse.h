//
//	sgMouse.h
//	Engine
//
//	Created by Nils Daumann on 17.06.12.
//	Copyright (c) 2012 Nils Daumann

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

#ifndef __SGMOUSE_H__
#define __SGMOUSE_H__

#include "sgVector2.h"

/**
 * Accelerometer namespace. Handles the accelerometer.
 */
namespace sgMouse
{
	/**
	 *	Current absolute acceleration.
	 *	The current absolute values provided by the accelerometer, without device rotation.
	 */
	extern sgVector2 currpos;

	/**
	 *	Current acceleration.
	 *	The current values provided by the accelerometer.
	 */
	extern sgVector2 currdir;
}

#endif
