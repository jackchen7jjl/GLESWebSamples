#include "GLSampleBase.h"
#include <math.h>
#include <iostream>

static GLuint vertexBuffer;

static GLuint indexBuffers[5];
static GLuint indexBuffer;

static GLint mvpLoc;
static float mvp[16] = { 0 };
static float rotZ;

static GLint  positionLoc;

static GLint colorLoc;

class StencilTestSample :public GLSampleBase
{
protected:
	void StartUp() override
	{
		GLint maxVertexAtrribs;
		glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &maxVertexAtrribs);
		std::cout << maxVertexAtrribs << std::endl;

		_vertexShaderSource =
			"attribute vec4 vaPosition;    \n"
			"void main()                  \n"
			"{                            \n"
			"   gl_Position = vaPosition;\n"
			"}                            \n";

		_fragmentShaderSource =
			"precision mediump float;							\n"
			"void main()										\n"
			"{													\n"
			"  gl_FragColor = vec4(1.0,0.0,0.0,1.0);\n"
			"}\n";

		_program = CompileShaders(_vertexShaderSource, _fragmentShaderSource);

		positionLoc = glGetAttribLocation(_program, "vaPosition");

		static GLfloat vVertices[] =
		{
			-0.75f, 0.25f, 0.5f,1.0f, // Quad #0
			-0.25f, 0.25f, 0.5f,1.0f,
			-0.25f, 0.75f, 0.5f,1.0f,
			-0.75f, 0.75f, 0.5f,1.0f,
			0.25f, 0.25f, 0.90f,1.0f, // Quad #1
			0.75f, 0.25f, 0.90f,1.0f,
			0.75f, 0.75f, 0.90f,1.0f,
			0.25f, 0.75f, 0.90f,1.0f,
			-0.75f, -0.75f, 0.50f,1.0f, // Quad #2
			-0.25f, -0.75f, 0.50f,1.0f,
			-0.25f, -0.25f, 0.50f,1.0f,
			-0.75f, -0.25f, 0.50f,1.0f,
			0.25f, -0.75f, 0.50f, 1.0f,// Quad #3
			0.75f, -0.75f, 0.50f,1.0f,
			0.75f, -0.25f, 0.50f,1.0f,
			0.25f, -0.25f, 0.50f,1.0f,
			-1.00f, -1.00f, 0.00f,1.0f, // Big Quad
			1.00f, -1.00f, 0.00f,1.0f,
			1.00f, 1.00f, 0.00f,1.0f,
			-1.00f, 1.00f, 0.00f,1.0f
		};

		static GLushort indicess[][6] =
		{
			{ 0, 1, 2, 0, 2, 3 }, // Quad #0
			{ 4, 5, 6, 4, 6, 7 }, // Quad #1
			{ 8, 9, 10, 8, 10, 11 }, // Quad #2
			{ 12, 13, 14, 12, 14, 15 }, // Quad #3
			{ 16, 17, 18, 16, 18, 19 } // Big Quad
		};

		static GLushort indices[] = { 0,1,2 };

		glGenBuffers(1, &vertexBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vVertices), vVertices, GL_STATIC_DRAW);

		std::cout <<"sizeof(indicess[0])"<< sizeof(indicess[0]) << std::endl;
		std::cout << "sizeof(indices)" << sizeof(indices) << std::endl;

		for (int i = 0; i < 5; ++i)
		{
			glGenBuffers(1, &indexBuffers[i]);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffers[i]);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indicess[i]), &indicess[i], GL_STATIC_DRAW);
		}

		glEnable(GL_STENCIL_TEST);
		glEnable(GL_DEPTH_TEST);
	}

	void Render() override
	{
		glClearColor(1.0, 0.6, 0, 1.0);
		glClearStencil(1);		
		glClearDepth(0.8);
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);

		glStencilMask(255);

		glDepthMask(GL_TRUE);		

		glDepthFunc(GL_LESS);

		glUseProgram(_program);

		glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);

		glEnableVertexAttribArray(positionLoc);
		glVertexAttribPointer(positionLoc, 4, GL_FLOAT, GL_FALSE, 16, (void*)0);

		glStencilFunc(GL_LESS, 0x7, 0x3);//stencil test fail,stencil buffer value = 7
		glStencilOp(GL_REPLACE, GL_DECR, GL_DECR);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffers[0]);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);

		glStencilFunc(GL_GREATER, 0x3, 0x3);//stencil test pass,but depth test fail,sbv = 0
		glStencilOp(GL_KEEP, GL_DECR, GL_KEEP);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffers[1]);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);

		glStencilFunc(GL_EQUAL, 0x1, 0x3);//all pass,sbv = 2
		glStencilOp(GL_KEEP, GL_INCR, GL_INCR);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffers[2]);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);

		glStencilFunc(GL_EQUAL, 0x2, 0x1);//stencil test fail,sbv = 254
		glStencilOp(GL_INVERT, GL_KEEP, GL_KEEP);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffers[3]);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);

		int numStencilBits = 0;
		glGetIntegerv(GL_STENCIL_BITS, &numStencilBits);

		//std::cout << "NumStencilBits: " << numStencilBits << std::endl;
		glStencilMask(0);//donot write sbv

		glStencilFunc(GL_EQUAL, 0x3, 0x7);//fail sbv = 7
		glStencilOp(GL_INCR, GL_DECR, GL_DECR);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffers[0]);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);

		glStencilFunc(GL_EQUAL, 0x8, 0x3);//fail sbv = 7
		glStencilOp(GL_INCR, GL_DECR, GL_DECR);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffers[0]);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);
	}

	void EndUp() override
	{
		glDeleteProgram(_program);
	}
};

DECLARE_MAIN(StencilTestSample)