#include "GLSampleBase.h"
#include <math.h>
#include <iostream>
#include "FastByteArray.h"

static GLuint vertexBuffer;

static GLint mvpLoc;
static float mvp[16] = { 0 };
static float rotZ;

static GLint  positionLoc;

static GLint colorLoc;
static GLint colorIndexLoc;

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
			"attribute vec4 vaColorIndex;    \n"			
			"uniform mat4 vcMvp;		  \n"
			"uniform vec4 fcColors[10];						\n"
			"varying vec4 vColor;		  \n"
			"void main()                  \n"
			"{                            \n"
			"   vColor = fcColors[int(vaColorIndex.x)];\n"
			"   gl_Position = vcMvp * vaPosition;\n"
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
		colorIndexLoc = glGetAttribLocation(_program, "vaColorIndex");

		mvpLoc = glGetUniformLocation(_program, "vcMvp");
		mvp[15] = mvp[10] = 1;

		//colorIndexLoc = glGetUniformLocation(_program, "fcColorIndex");
		colorLoc = glGetUniformLocation(_program, "fcColors");

		static FastByteArray vVertices(3 * 4 * 4);

		vVertices.WriteFloat(0.0f);
		vVertices.WriteFloat(0.5f);
		vVertices.WriteFloat(0.0f);
		vVertices.WriteByte(1);
		vVertices.WriteByte(2);
		vVertices.WriteByte(1);
		vVertices.WriteByte(2);

		vVertices.WriteFloat(-0.5f);
		vVertices.WriteFloat(-0.5f);
		vVertices.WriteFloat(0.0f);
		vVertices.WriteByte(3);
		vVertices.WriteByte(4);
		vVertices.WriteByte(3);
		vVertices.WriteByte(4);

		vVertices.WriteFloat(0.5f);
		vVertices.WriteFloat(-0.5f);
		vVertices.WriteFloat(0.0f);
		vVertices.WriteByte(0);
		vVertices.WriteByte(2);
		vVertices.WriteByte(1);
		vVertices.WriteByte(2);


		glGenBuffers(1, &vertexBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
		glBufferData(GL_ARRAY_BUFFER, vVertices.GetDataLen(), vVertices.GetData(), GL_STATIC_DRAW);
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
		
		static GLfloat colors[40];
		for (int i = 0; i < 10; i++)
		{
			if (i == 1)
			{
				colors[i * 4] = 0.0;
				colors[i * 4 + 1] = 1.0;
				colors[i * 4 + 2] = 0.0;
				colors[i * 4 + 3] = 1.0;
			}
			else if (i == 2)
			{
				colors[i * 4] = 0.0;
				colors[i * 4 + 1] = 0.0;
				colors[i * 4 + 2] = 1.0;
				colors[i * 4 + 3] = 1.0;
			}
			else if (i == 3)
			{
				colors[i * 4] = 1.0;
				colors[i * 4 + 1] = 1.0;
				colors[i * 4 + 2] = 0.0;
				colors[i * 4 + 3] = 1.0;
			}
			else
			{
				colors[i * 4] = 1.0;
				colors[i * 4 + 1] = 0.0;
				colors[i * 4 + 2] = 0.0;
				colors[i * 4 + 3] = 1.0;
			}
		}

		glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, (const GLfloat*)mvp);
		
		glUniform4fv(colorLoc,10, colors);

		//glUniform4f(colorIndexLoc, 10,10,10,10);

		glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);

		glVertexAttribPointer(positionLoc, 3, GL_FLOAT, GL_FALSE, 16, (void*)0);
		glVertexAttribPointer(colorIndexLoc, 4, GL_UNSIGNED_BYTE, GL_FALSE, 16, (void*)12);

		glEnableVertexAttribArray(positionLoc);
		glEnableVertexAttribArray(colorIndexLoc);

		glDrawArrays(GL_TRIANGLES,0,3);
	}

	void EndUp() override
	{
		glDeleteProgram(_program);
	}
};

DECLARE_MAIN(UniformsSample)

