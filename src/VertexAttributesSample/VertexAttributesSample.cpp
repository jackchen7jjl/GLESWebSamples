#include "GLSampleBase.h"
#include <math.h>
#include <iostream>

static GLuint vertexBuffer;
static GLuint indexBuferr;

static GLint mvpLoc;
static float mvp[16] = { 0 };
static float rotZ;

static GLint  positionLoc;

static GLint colorLoc;

class VertexAttributesSample :public GLSampleBase
{
protected:
	void StartUp() override
	{
		GLint maxVertexAtrribs;
		glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &maxVertexAtrribs);
		std::cout << maxVertexAtrribs << std::endl;

		//const GLubyte *extentions = glGetString(GL_EXTENSIONS);
		//std::cout << extentions << std::endl;

		_vertexShaderSource =
			"attribute vec4 vaPosition;    \n"
			"attribute vec4 vaColor;    \n"
			"uniform mat4 vcMvp;		  \n"
			"varying vec4 vColor;		  \n"
			"void main()                  \n"
			"{                            \n"
			"   gl_Position = vcMvp * vaPosition;\n"
			"   vColor = vaColor;\n"
			"}                            \n";

		_fragmentShaderSource =
			"precision mediump float;							\n"
			"varying vec4 vColor;		  \n"
			"void main()										\n"
			"{													\n"
			"  gl_FragColor = vColor;\n"
			"}\n";

		_program = CompileShaders(_vertexShaderSource, _fragmentShaderSource);

		positionLoc = glGetAttribLocation(_program, "vaPosition");

		mvpLoc = glGetUniformLocation(_program, "vcMvp");
		mvp[15] = mvp[10] = 1;

		colorLoc = glGetAttribLocation(_program, "vaColor");

		static GLfloat vVertices[] = { 0.0f, 0.5f, 0.0f,1.0f,0.0f,0.0f,1.0f,
			-0.5f, -0.5f, 0.0f,0.0f,1.0f,0.0f,1.0f,
			0.5f, -0.5f, 0.0f,0.0f,0.0f,1.0f,1.0f };

		static GLushort indices[] = {0,1,2};

		glGenBuffers(1, &vertexBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vVertices), vVertices, GL_STATIC_DRAW);

		glGenBuffers(1, &indexBuferr);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuferr);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	}

	void Render() override
	{
		glClearColor(1.0, 0.6, 0, 1.0);
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(_program);

		rotZ += 0.01;
		mvp[0] = cos(rotZ);
		mvp[1] = sin(rotZ);
		mvp[4] = -sin(rotZ);
		mvp[5] = cos(rotZ);
		
		glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, (const GLfloat*)mvp);
		
		glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuferr);

		glEnableVertexAttribArray(positionLoc);
		glVertexAttribPointer(positionLoc, 3, GL_FLOAT, GL_FALSE, 28, (void*)0);

		glEnableVertexAttribArray(colorLoc);
		glVertexAttribPointer(colorLoc, 4, GL_FLOAT, GL_FALSE, 28, (void*)12);

		glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_SHORT, 0);
	}

	void EndUp() override
	{
		glDeleteProgram(_program);
	}
};

DECLARE_MAIN(VertexAttributesSample)

