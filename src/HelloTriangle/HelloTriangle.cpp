#include "GLSampleBase.h"

static GLuint vertexBuffer;

class HelloTriangle :public GLSampleBase
{
protected:
	void StartUp() override
	{
		_vertexShaderSource =
			"attribute vec4 a_position;    \n"
			"void main()                  \n"
			"{                            \n"
			"   gl_Position = a_position;\n"
			"}                            \n";

		_fragmentShaderSource =
			"precision mediump float;							\n"
			"void main()										\n"
			"{													\n"
			"  gl_FragColor = vec4(1.0,0.0,0.0,1.0);\n"
			"}\n";

		_program = CompileShaders(_vertexShaderSource, _fragmentShaderSource);

		glBindAttribLocation(_program,0,"a_position");

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

DECLARE_MAIN(HelloTriangle)

