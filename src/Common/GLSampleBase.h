#pragma once
#if defined(EMSCRIPTEN)
#include "GLES2\gl2.h"
#include <emscripten\emscripten.h>
#else
#include "glew.h"
#endif
#include "GLFW\glfw3.h"

#include <string>

using String = std::string;

class GLSampleBase
{
public:
	virtual ~GLSampleBase() = default;
	GLSampleBase();

	bool Init();
	void Run();

	GLuint LoadShader(GLenum type, String shaderSrc);
	GLuint CompileShaders(String vertextShader, String fragmentShader);

protected:
	virtual void StartUp() = 0;
	virtual void Render() = 0;
	virtual void EndUp() = 0;

	virtual void KeyHandler(GLFWwindow* window, int key, int scancode, int action, int mods) {};

	GLFWwindow *_window;
	String _vertexShaderSource;
	String _fragmentShaderSource;

	GLuint _program;
	GLuint _vertexArrayObject;

	GLuint _vertexBuffer;
	GLuint _indexBuffer;

	int _frameCnt = 0;

private:
	static void ErrorCallback(int error, const char* description);
	static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void MainLoop();
	static GLSampleBase* GetInstance();

	static GLSampleBase* _instance;
};

#define DECLARE_MAIN(GLSample)\
int main()\
{\
	GLSampleBase *sample = new GLSample(); \
	if(!(sample->Init()))\
		return 1;\
	sample->Run();\
	delete sample;\
	return 0;\
}