//
//	sgShader.cpp
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

#include "sgShader.h"

#include <string>
#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/glext.h>
#include "sgResourceManager.h"
#include "sgDebug.h"

sgShader::sgShader()
{
	program = 0;
}

sgShader::sgShader(const char *vsfilename, const char *fsfilename)
{
	program = 0;
	create(vsfilename, fsfilename);
	maxlights = 2;
}

sgShader::~sgShader()
{
	
}

sgShader *sgShader::getShader(const char *vsfilename, const char *fsfilename)
{
	std::string str = std::string("shader")+std::string(vsfilename)+std::string(fsfilename);
	
	sgShader *shader = (sgShader*)sgResourceManager::getResource(str.c_str());
	if(shader != NULL)
		return shader;
	
	shader = new sgShader(vsfilename, fsfilename);
	sgResourceManager::addResource(str.c_str(), shader);
	
	return shader;
}

sgShader *sgShader::getShader(unsigned int shad)
{
	sgShader *shader = NULL;
	if(shad == 0)
	{
		shader = (sgShader*)sgResourceManager::getResource("shaderdefault");
		if(shader == NULL)
		{
			shader = new sgShader("default", "default");
			sgResourceManager::addResource("shaderdefault", shader);
		}
	}
	if(shad == 1)
	{
		shader = (sgShader*)sgResourceManager::getResource("shaderdefaultlight");
		if(shader == NULL)
		{
			shader = new sgShader("defaultlight", "defaultlight");
			sgResourceManager::addResource("shaderdefaultlight", shader);
		}
	}
	if(shad == 2)
	{
		shader = (sgShader*)sgResourceManager::getResource("shadershadowvolume");
		if(shader == NULL)
		{
			shader = new sgShader("shadowvolume", "shadowvolume");
			sgResourceManager::addResource("shadershadowvolume", shader);
		}
	}
	if(shad == 3)
	{
		shader = (sgShader*)sgResourceManager::getResource("shadershadowquad");
		if(shader == NULL)
		{
			shader = new sgShader("shadowquad", "shadowquad");
			sgResourceManager::addResource("shadershadowquad", shader);
		}
	}
	return shader;
}

bool sgShader::compileShader(GLuint *shader, GLenum type, const char *filepath)
{
	GLint status;
	const GLchar *source;
	
	source = (GLchar *)sgResourceManager::getFileAsString(filepath);
	if(!source)
	{
		sgLog("Failed to load shader");
		return false;
	}
	
	*shader = glCreateShader(type);
	glShaderSource(*shader, 1, &source, NULL);
	glCompileShader(*shader);
	
	delete[] source;
	
	GLint logLength;
	glGetShaderiv(*shader, GL_INFO_LOG_LENGTH, &logLength);
	if(logLength > 0)
	{
		GLchar *log = (GLchar *)malloc(logLength);
		glGetShaderInfoLog(*shader, logLength, &logLength, log);
		sgLog("Shader compile log:\n %s", log);
		free(log);
	}
	
	glGetShaderiv(*shader, GL_COMPILE_STATUS, &status);
	if(status == 0)
	{
		glDeleteShader(*shader);
		return false;
	}
	
	return true;
}

bool sgShader::linkProgram(GLuint prog)
{
	GLint status;
	
	glLinkProgram(prog);
	
	GLint logLength;
	glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &logLength);
	if(logLength > 0)
	{
		GLchar *log = (GLchar *)malloc(logLength);
		glGetProgramInfoLog(prog, logLength, &logLength, log);
		sgLog("Program link log:\n &s", log);
		free(log);
	}
	
	glGetProgramiv(prog, GL_LINK_STATUS, &status);
	if(status == 0)
		return false;
	
	return true;
}

bool sgShader::validateProgram()
{
	GLint logLength, status;
	
	glValidateProgram(program);
	glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);
	if(logLength > 0)
	{
		GLchar *log = (GLchar *)malloc(logLength);
		glGetProgramInfoLog(program, logLength, &logLength, log);
//		sgDebug::writeString2("Program validate log:\n", log);
		free(log);
	}
	
	glGetProgramiv(program, GL_VALIDATE_STATUS, &status);
	if(status == 0)
		return false;
	
	return true;
}

void sgShader::getEngineUniforms()
{
	matproj = glGetUniformLocation(program, "matProj");
	matview = glGetUniformLocation(program, "matView");
	matmodel = glGetUniformLocation(program, "matModel");
	matnormal = glGetUniformLocation(program, "matNormal");
	mattex = glGetUniformLocation(program, "matTex");
	
	position = glGetAttribLocation(program, "vertPos");
	normal = glGetAttribLocation(program, "vertNormal");
	texcoord0 = glGetAttribLocation(program, "vertTexcoord0");
	color = glGetAttribLocation(program, "vertColor");
	
	mambientloc = glGetUniformLocation(program, "mAmbient");
	mdiffuseloc = glGetUniformLocation(program, "mDiffuse");
	mspecularloc = glGetUniformLocation(program, "mSpecular");
	mshininessloc = glGetUniformLocation(program, "mShininess");
	memissiveloc = glGetUniformLocation(program, "mEmissive");
	malphatestvalue = glGetUniformLocation(program, "mAlphaTest");
	
	lambientloc = glGetUniformLocation(program, "lAmbient");
	ldiffuseloc = glGetUniformLocation(program, "lDiffuse");
	lspecularloc = glGetUniformLocation(program, "lSpecular");
	lpositionloc = glGetUniformLocation(program, "lPosition");
	lattenuationloc = glGetUniformLocation(program, "lAttenuation");
}

bool sgShader::create(const char *vsfilename, const char *fsfilename)
{
	if(program != 0)
		return false;
	
	GLuint vertShader, fragShader;
	const char *vertShaderPathname, *fragShaderPathname;
	
	// Create shader program
	program = glCreateProgram();
	
	// Create and compile vertex shader
	vertShaderPathname = sgResourceManager::getPath(vsfilename, "vsh");
	if(!compileShader(&vertShader, GL_VERTEX_SHADER, vertShaderPathname))
	{
		delete[] vertShaderPathname;
//		sgDebug::writeString("Failed to compile vertex shader");
		return false;
	}
	delete[] vertShaderPathname;
	
	// Create and compile fragment shader
	fragShaderPathname = sgResourceManager::getPath(fsfilename, "fsh");
	if(!compileShader(&fragShader, GL_FRAGMENT_SHADER, fragShaderPathname))
	{
		delete[] fragShaderPathname;
//		sgDebug::writeString("Failed to compile fragment shader");
		return false;
	}
	delete[] fragShaderPathname;
	
	// Attach vertex shader to program
	glAttachShader(program, vertShader);
	
	// Attach fragment shader to program
	glAttachShader(program, fragShader);
	
	// Link program
	if(!linkProgram(program))
	{
//		sgDebug::writeStringFloat("Failed to link program: ", program);
		
		if(vertShader)
		{
			glDeleteShader(vertShader);
			vertShader = 0;
		}
		if(fragShader)
		{
			glDeleteShader(fragShader);
			fragShader = 0;
		}
		if(program)
		{
			glDeleteProgram(program);
			program = 0;
		}
		
		return false;
	}
	
	if(vertShader)
		glDeleteShader(vertShader);
	
	if(fragShader)
		glDeleteShader(fragShader);
	
	getEngineUniforms();
				
	return true;
}

void sgShader::destroy()
{
	if(program)
	{
		glDeleteProgram(program);
		program = 0;
	}
	
	sgResourceManager::removeResource(this);
	delete this;
}