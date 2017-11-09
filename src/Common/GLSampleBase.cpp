#include "GLSampleBase.h"
#include <iostream>
#include<assert.h>

GLSampleBase::GLSampleBase()
{
	assert(_instance == nullptr);
	_instance = this;
}

GLSampleBase* GLSampleBase::_instance = nullptr;

GLSampleBase* GLSampleBase::GetInstance()
{
	return _instance;
}

bool GLSampleBase::Init()
{
	glfwSetErrorCallback(ErrorCallback);

	if (!glfwInit())
	{
		return false;
	}

	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_FALSE);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

	_window = glfwCreateWindow(1024, 768, "Hello GLFW", NULL, NULL);

	if (!_window)
	{
		glfwTerminate();
		return false;
	}

	glfwSetKeyCallback(_window, KeyCallback);

	glfwMakeContextCurrent(_window);
	//glfwSwapInterval(1);

#ifndef EMSCRIPTEN
	glewExperimental = true; // Needed for core profile

	GLenum err = glewInit();

	if (err != 0)
		return false;
#endif // !EMSCRIPTEN

	std::cout << glGetString(GL_VERSION) << std::endl;

	return true;
}

void GLSampleBase::MainLoop()
{
	_instance->Render();

	glfwSwapBuffers(_instance->_window);
	glfwPollEvents();
}

void GLSampleBase::Run()
{
	//glGenVertexArrays(1, &_vertexArrayObject);
	//glBindVertexArray(_vertexArrayObject);

	StartUp();

#ifdef EMSCRIPTEN
	emscripten_set_main_loop(&GLSampleBase::MainLoop, 0, 1);
#else
	while (!glfwWindowShouldClose(_window))
	{
		MainLoop();
	}
#endif // EMSCRIPTEN

	std::cout << "Run End " << std::endl;

	EndUp();
	//glDeleteVertexArrays(1, &_vertexArrayObject);
	glfwTerminate();
}

GLuint GLSampleBase::LoadShader(GLenum type, String shaderSrc)
{
	// Create the shader object
	GLuint shader = glCreateShader(type);

	if (shader == 0)
		return 0;

	#ifdef EMSCRIPTEN
		shaderSrc = "#version 100\n" + shaderSrc;
	#else
		shaderSrc = "#version 110\n" + shaderSrc;
	#endif // EMSCRIPTEN

	const char* cstr = shaderSrc.c_str();

	// Load the shader source
	glShaderSource(shader, 1, &cstr, NULL);

	// Compile the shader
	glCompileShader(shader);

	//Check the compile status
	GLint compileResult = GL_TRUE;

	glGetShaderiv(shader, GL_COMPILE_STATUS, &compileResult);

	if (compileResult == GL_FALSE)
	{
		GLint infoLen = 0;

		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);

		if (infoLen > 1)
		{
			char* infoLog = (char*)malloc(sizeof(char) * infoLen);

			glGetShaderInfoLog(shader, infoLen, NULL, infoLog);

			std::cout << "compile shader error: " << infoLog << std::endl;

			free(infoLog);
		}
		return 0;
	}

	return shader;
}

GLuint GLSampleBase::CompileShaders(String vertextShader, String fragmentShader)
{
	GLuint vertex_shader = LoadShader(GL_VERTEX_SHADER, vertextShader);

	GLuint fragment_shader = LoadShader(GL_FRAGMENT_SHADER, fragmentShader);

	if ((!vertex_shader) || (!fragment_shader))
		return 0;

	GLuint program = glCreateProgram();

	glAttachShader(program, vertex_shader);

	glAttachShader(program, fragment_shader);

	glLinkProgram(program);

	GLint linkResult = 0;

	glGetProgramiv(program,GL_LINK_STATUS,&linkResult);

	if (linkResult == GL_FALSE)
	{
		GLint infoLen = 0;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLen);
		if (infoLen > 1)
		{
			char *infoLog = (char*)malloc(sizeof(char) * infoLen);
			glGetProgramInfoLog(program, infoLen, NULL, infoLog);
			std::cout << "link program error: " << infoLog << std::endl;
			free(infoLog);
		}
		glDeleteProgram(program);
		return 0;
	}

	glDeleteShader(vertex_shader);

	glDeleteShader(fragment_shader);

	return program;
}

void GLSampleBase::ErrorCallback(int error, const char* description)
{
	std::cout << "err" << description << std::endl;
}

void GLSampleBase::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, 1);
}