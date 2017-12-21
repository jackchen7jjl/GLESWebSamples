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

class Matrix
{
public:
	double rawData[16];

	double& Get(int row, int column) { return rawData[row + (column * 4)]; }
	const double& Get(int row, int column)const { return rawData[row + (column * 4)]; }

	void Identity()
	{
		Get(0, 0) = 1.0;	Get(0, 1) = 0.0;	Get(0, 2) = 0.0;	Get(0, 3) = 0.0;
		Get(1, 0) = 0.0;	Get(1, 1) = 1.0;	Get(1, 2) = 0.0;	Get(1, 3) = 0.0;
		Get(2, 0) = 0.0;	Get(2, 1) = 0.0;	Get(2, 2) = 1.0;	Get(2, 3) = 0.0;
		Get(3, 0) = 0.0;	Get(3, 1) = 0.0;	Get(3, 2) = 0.0;	Get(3, 3) = 1.0;
	}
};

class VertexShaderSample :public GLSampleBase
{
protected:
	void StartUp() override
	{
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

		static GLushort indices[] = { 0,1,2 };

		glGenBuffers(1, &vertexBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vVertices), vVertices, GL_STATIC_DRAW);

		glGenBuffers(1, &indexBuferr);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuferr);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

		glEnable(GL_CULL_FACE);//glDisable（GL_CULL_FACE）、默认是disabled
		glCullFace(GL_BACK);//GL_FRONT, GL_BACK, and GL_FRONT_AND_BACK，默认是GL_BACK
		glFrontFace(GL_CCW);//GL_CW,GL_CCW,默认是GL_CCW

	}

	void Render() override
	{
		glClearColor(1.0, 0.6, 0, 1.0);
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(_program);

		rotZ += 0.01;

		//int cnt = 100000;
		//for (int i = 0; i < cnt; i++)
		//{
		//	Matrix m;
		//	m.rawData[0] = 0;
		//	rotZ += m.Get(0, 0);
		//}

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

DECLARE_MAIN(VertexShaderSample)

