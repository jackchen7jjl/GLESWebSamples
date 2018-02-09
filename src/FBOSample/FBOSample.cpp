#include "GLSampleBase.h"
#include <math.h>
#include <iostream>
#include <fstream>
using namespace std;

static GLuint vertexBuffer;
static GLuint indexBuferr;

static GLint mvpLoc;
static float mvp[16] = { 0 };
static float rotZ;

static GLint  positionLoc;

static GLint colorLoc;

GLuint framebuffer;
GLuint depthRenderbuffer;
GLuint texture;
GLuint depthTexture;

GLint texWidth = 1024, texHeight = 768;
GLint maxRenderbufferSize;
GLenum status;

String _vertexShaderSource2;
String _fragmentShaderSource2;
GLuint _program2;

static GLint  positionLoc2;
static GLint  texCoordLoc;
static GLint samplerLoc;
static GLuint vertexBuffer2, indexBuffer2;
static GLuint textureId;

static const GLfloat vVertices2[] =
{
	-0.5,  0.5, 0.3, 1.0, // Position 0
	0.0,  0.0,       // TexCoord 0
	-0.5, -0.5, 0.5, 1.0,  // Position 1
	0.0,  1.0,       // TexCoord 1
	0.5, -0.5, 0.8, 1.0,  // Position 2
	1.0,  1.0,       // TexCoord 2
	0.5,  0.5, -0.5, 1.0,  // Position 3
	1.0,  0.0,        // TexCoord 3
};

static const GLushort indices2[] = { 0, 1, 2, 0, 2, 3 };

char* readRGBFile(const char* filename)
{
	ifstream vsIutput(filename, ios::in | ios::binary);
	if (vsIutput.is_open())
	{
		vsIutput.seekg(0, std::ios_base::end);
		unsigned int len = vsIutput.tellg();
		vsIutput.seekg(0, std::ios_base::beg);
		char* bytes = (char*)malloc(len);
		vsIutput.read(bytes, len);
		vsIutput.close();
		return bytes;
	}
	return nullptr;
}

GLuint CreateSimpleTexture2D()
{
	// Texture object handle
	GLuint textureId;

	GLubyte *pixels = (GLubyte *)readRGBFile("HelloGL/grass.bmp");

	// Use tightly packed data
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

	// Generate a texture object
	glGenTextures(1, &textureId);

	// Bind the texture object
	glBindTexture(GL_TEXTURE_2D, textureId);

	// Load the texture
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 512, 512, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels);
	glGenerateMipmap(GL_TEXTURE_2D);

	// Set the filtering mode
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	return textureId;

}

class VertexShaderSample :public GLSampleBase
{
protected:
	void StartUp() override
	{
		glGetIntegerv(GL_MAX_RENDERBUFFER_SIZE, &maxRenderbufferSize);

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

		glEnable(GL_DEPTH);
		glEnable(GL_CULL_FACE);//glDisable（GL_CULL_FACE）、默认是disabled
		glCullFace(GL_BACK);//GL_FRONT, GL_BACK, and GL_FRONT_AND_BACK，默认是GL_BACK
		glFrontFace(GL_CCW);//GL_CW,GL_CCW,默认是GL_CCW

		//---------------------------22222222222222222----------------------------------
		_vertexShaderSource2 =
			"attribute vec4 a_position;    \n"
			"attribute vec2 a_texCoord;   \n"
			"varying vec2 v_texCoord;     \n"
			"void main()                  \n"
			"{                            \n"
			"   gl_Position = a_position;\n"
			"   v_texCoord = a_texCoord;  \n"
			"}                            \n";

		_fragmentShaderSource2 =
			"precision mediump float;							\n"
			"varying vec2 v_texCoord;							\n"
			"uniform sampler2D s_texture;						\n"
			"void main()										\n"
			"{													\n"
			"  gl_FragColor = texture2D(s_texture, v_texCoord );\n"
			"}\n";

		_program2 = CompileShaders(_vertexShaderSource2, _fragmentShaderSource2);

		positionLoc2 = glGetAttribLocation(_program2, "a_position");
		texCoordLoc = glGetAttribLocation(_program2, "a_texCoord");
		samplerLoc = glGetUniformLocation(_program2, "s_texture");
		
		glGenBuffers(1, &vertexBuffer2);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer2);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vVertices2), vVertices2, GL_STATIC_DRAW);

		glGenBuffers(1, &indexBuffer2);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer2);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices2), indices2, GL_STATIC_DRAW);


		textureId = CreateSimpleTexture2D();
	}

	void Render() override
	{		
		glGenFramebuffers(1, &framebuffer);
		glGenRenderbuffers(1, &depthRenderbuffer);
		
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texWidth, texHeight,
			0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

		glGenTextures(1, &depthTexture);
		glBindTexture(GL_TEXTURE_2D, depthTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, texWidth, texHeight,
			0, GL_DEPTH_COMPONENT, GL_UNSIGNED_SHORT, NULL);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

		//glBindRenderbuffer(GL_RENDERBUFFER, depthRenderbuffer);
		//glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16,
		//	texWidth, texHeight);

		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture, 0);
		//glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRenderbuffer);
		status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

		if (status == GL_FRAMEBUFFER_COMPLETE)
		{
			glEnable(GL_DEPTH_TEST);
			glClearColor(1.0, 0.6, 0, 1.0);
			glClearDepthf(0.9);
			glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
			glDepthFunc(GL_LEQUAL);

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



			glBindFramebuffer(GL_FRAMEBUFFER, 0);

			glClearColor(0.0, 0.0, 0.0, 1.0);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glUseProgram(_program2);

			glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer2);

			glVertexAttribPointer(positionLoc2, 4, GL_FLOAT, GL_FALSE, 24, (void*)0);
			glVertexAttribPointer(texCoordLoc, 2, GL_FLOAT, GL_FALSE, 24, (void*)16);

			glEnableVertexAttribArray(positionLoc2);
			glEnableVertexAttribArray(texCoordLoc);

			// Bind the texture
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, depthTexture);
			glUniform1i(samplerLoc, 0);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer2);
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);
		}

		glDeleteRenderbuffers(1, &depthRenderbuffer);
		glDeleteFramebuffers(1, &framebuffer);
		glDeleteTextures(1, &texture);
		glDeleteTextures(1, &depthTexture);
	}

	void EndUp() override
	{
		glDeleteProgram(_program);
	}
};

DECLARE_MAIN(VertexShaderSample)

