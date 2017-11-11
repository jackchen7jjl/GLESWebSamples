#include "GLSampleBase.h"
#include <iostream>

static GLuint vertexBuffer;

static GLint mvpLoc;
static float mvp[16] = { 0 };
static float rotZ;

static GLint  positionLoc;

static GLint colorLoc;

class UniformsSample :public GLSampleBase
{
protected:
	void StartUp() override
	{
		GLint maxVertexAtrribs;
		glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &maxVertexAtrribs);
		std::cout << maxVertexAtrribs << std::endl;

		_vertexShaderSource =
			"attribute vec4 vaPosition;    \n"
			"uniform mat4 vcMvp;		  \n"
			"void main()                  \n"
			"{                            \n"
			"   gl_Position = vcMvp * vaPosition;\n"
			"}                            \n";

		_fragmentShaderSource =
			"precision mediump float;							\n"
			"uniform vec4 fcColor;						\n"
			"void main()										\n"
			"{													\n"
			"  gl_FragColor = fcColor;\n"
			"}\n";

		_program = CompileShaders(_vertexShaderSource, _fragmentShaderSource);

		positionLoc = glGetAttribLocation(_program, "vaPosition");

		mvpLoc = glGetUniformLocation(_program, "vcMvp");
		mvp[15] = mvp[10] = 1;

		colorLoc = glGetUniformLocation(_program, "fcColor");

		static GLfloat vVertices[] = { 0.0f, 0.5f, 0.0f,
			-0.5f, -0.5f, 0.0f,
			0.5f, -0.5f, 0.0f };

		glGenBuffers(1, &vertexBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vVertices), vVertices, GL_STATIC_DRAW);
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
		glUniform4f(colorLoc, mvp[0], mvp[1], mvp[2], 1.0);

		glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
		glEnableVertexAttribArray(0);

		glDrawArrays(GL_TRIANGLES,0,3);
	}

	void EndUp() override
	{
		glDeleteProgram(_program);
	}
};

DECLARE_MAIN(UniformsSample)

