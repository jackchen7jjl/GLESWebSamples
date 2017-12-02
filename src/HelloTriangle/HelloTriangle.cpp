#include "GLSampleBase.h"

#ifdef EMSCRIPTEN

#include<iostream>
#include <pthread.h>
#include <emscripten.h>
#include <emscripten/threading.h>

int testFlag = 0;

pthread_t thread_ptr;

void* ThreadFunc(void*param)
{
	testFlag += 1;
	return(0);
}

bool StartThread()
{
	if (thread_ptr)
	{
		std::cerr << "Thread::Start() error,thread_ptr!=nullptr." << std::endl;
	}

	pthread_attr_t attr;

	pthread_attr_init(&attr);

	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

	if (pthread_create(&thread_ptr, &attr, (void* (*)(void *))ThreadFunc,NULL))						//返回0表示正常
	{
		thread_ptr = 0;
		pthread_attr_destroy(&attr);
		std::cerr << "Create Thread (pthread_create) failed.errno:" << errno << std::endl;
		return(false);
	}

	pthread_attr_destroy(&attr);
	return(true);
}

#endif // EMSCRIPTEN

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
#ifdef EMSCRIPTEN
		StartThread();
		std::cout << testFlag << std::endl;
#endif
	}

	void Render() override
	{
#ifdef EMSCRIPTEN
		if(_frameCnt%100 == 0)
			std::cout << testFlag << std::endl;
#endif
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

