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

	GLFWmonitor *monitor = glfwGetPrimaryMonitor();
 	const GLFWvidmode *vidmode = glfwGetVideoMode(monitor);
	
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_FALSE);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	//glfwWindowHint(GLFW_AUTO_ICONIFY,false);

	//glfwWindowHint(GLFW_RED_BITS, vidmode->redBits);
	//glfwWindowHint(GLFW_GREEN_BITS, vidmode->greenBits);
	//glfwWindowHint(GLFW_BLUE_BITS, vidmode->blueBits);
	//glfwWindowHint(GLFW_REFRESH_RATE, vidmode->refreshRate);

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

	//======================================check gl features===============================
	GLint maxVertexAttribs, maxVertexUniforms, maxVarying;
	GLint maxVertexTextureUnits, maxCombinedTextureUnits;
	GLint maxTexureImageUnits,numCompressedTextureFormat;
	GLint maxFragmentUniformVectors;

	glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &maxVertexAttribs);
	glGetIntegerv(GL_MAX_VERTEX_UNIFORM_VECTORS, &maxVertexUniforms);
	glGetIntegerv(GL_MAX_VARYING_VECTORS, &maxVarying);
	glGetIntegerv(GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS, &maxVertexTextureUnits);
	glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &maxCombinedTextureUnits);
	glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &maxTexureImageUnits);
	glGetIntegerv(GL_NUM_COMPRESSED_TEXTURE_FORMATS, &numCompressedTextureFormat);
	glGetIntegerv(GL_MAX_FRAGMENT_UNIFORM_VECTORS, &maxFragmentUniformVectors);

	GLint *textureFormats = new GLint[numCompressedTextureFormat];
	glGetIntegerv(GL_COMPRESSED_TEXTURE_FORMATS, textureFormats);

	//std::cout << "GLFW VERSION: " << GLFW_VERSION_MAJOR << "." << GLFW_VERSION_MINOR << "." << GLFW_VERSION_REVISION << std::endl;
	std::cout << "GLFW VERSION: " << glfwGetVersionString() << std::endl;
	std::cout << "GL_VERSION: " << glGetString(GL_VERSION) << std::endl;
	
	std::cout << "GL_MAX_VERTEX_ATTRIBS(8): " << maxVertexAttribs << std::endl;
	std::cout << "GL_MAX_VERTEX_UNIFORM_VECTORS(128): " << maxVertexUniforms << std::endl;
	std::cout << "GL_MAX_VARYING_VECTORS(8): " << maxVarying << std::endl;

	std::cout << "GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS(0): " << maxVertexTextureUnits << std::endl;
	std::cout << "GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS(8): " << maxCombinedTextureUnits << std::endl;

	std::cout << "GL_MAX_TEXTURE_IMAGE_UNITS(8): " << maxTexureImageUnits << std::endl;
	std::cout << "GL_NUM_COMPRESSED_TEXTURE_FORMATS: " << numCompressedTextureFormat << std::endl;

	std::cout << "GL_MAX_FRAGMENT_UNIFORM_VECTORS: " << maxFragmentUniformVectors << std::endl;

	
	int count = 0;
	const GLFWvidmode *vidmodes = glfwGetVideoModes(monitor, &count);
	std::cout << "vidmodes: " << count << std::endl;

	//for (int i = 0; i < count; i++)
	//{		
	//	std::cout 
	//		<< "redBits="<< vidmodes[i].redBits 
	//		<< ", greenBits=" << vidmodes[i].greenBits 
	//		<< ", blueBits=" << vidmodes[i].blueBits 
	//		<< ", width=" << vidmodes[i].width 
	//		<< ", height=" << vidmodes[i].height 
	//		<< ", refreshRate=" << vidmodes[i].refreshRate << std::endl;
	//	
	//}

	/*int monitorCnt;
	GLFWmonitor **monitors = glfwGetMonitors(&monitorCnt);
	for (int i = 0; i < monitorCnt; i++)
	{
		GLFWmonitor *moni = monitors[i];		
		std::cout << "monitor" << i << " ---------:"  << std::endl;
		int pw, ph;
		glfwGetMonitorPhysicalSize(moni, &pw, &ph);

		int posX, posY;
		glfwGetMonitorPos(moni,&posX,&posY);

		const GLFWvidmode *vidmode = glfwGetVideoMode(moni);
		std::cout
			<< "name=" << glfwGetMonitorName(moni)
			<< ",PhysicalWidth=" << pw
			<< ",PhysicalHeight=" << ph
			<< ",MonitorPosX=" << posX
			<< ",MonitorPosY=" << posY
			<< ",redBits=" << vidmode->redBits
			<< ", greenBits=" << vidmode->greenBits
			<< ", blueBits=" << vidmode->blueBits
			<< ", width=" << vidmode->width
			<< ", height=" << vidmode->height
			<< ", refreshRate=" << vidmode->refreshRate << std::endl;
	}*/
	 
	return true;
}

void GLSampleBase::MainLoop()
{
	_instance->_frameCnt++;
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
		shaderSrc = "#version 120\n" + shaderSrc;
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
	_instance->KeyHandler(window,key,scancode,action,mods);
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, 1);
	else if (key == GLFW_KEY_SPACE)
		glfwSetWindowMonitor(window, glfwGetPrimaryMonitor(), 0, 0, 640, 480, 60);
	else if (key == GLFW_KEY_1)
		glfwSetWindowSize(window, 1600, 800);
}