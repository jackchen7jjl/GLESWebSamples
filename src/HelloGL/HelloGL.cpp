#include "GLSampleBase.h"
#include <math.h>
#include <iostream>
#include <fstream>
using namespace std;

// Texture handle
static GLuint textureId;

// Attribute locations Sampler location mvp location
static GLint  positionLoc;
static GLint  texCoordLoc;
static GLint samplerLoc;
static GLint mvpLoc;

static GLuint vertexBuffer, indexBuffer;

static float mvp[16] = { 0 };
static float rotZ;

static const GLfloat vVertices[] =
{
	-0.5,  0.5, -1.0, 1.0, // Position 0
	0.0,  0.0,       // TexCoord 0
	-0.5, -0.5, -1.0, 1.0,  // Position 1
	0.0,  1.0,       // TexCoord 1
	0.5, -0.5, -1.0, 1.0,  // Position 2
	1.0,  1.0,       // TexCoord 2
	0.5,  0.5, -1.0, 1.0,  // Position 3
	1.0,  0.0,        // TexCoord 3
};

static const GLushort indices[] = { 0, 1, 2, 0, 2, 3 };

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

class HelloGL :public GLSampleBase
{
protected:
	void StartUp() override
	{
		_vertexShaderSource =
			"attribute vec4 a_position;    \n"
			"attribute vec2 a_texCoord;   \n"
			"varying vec2 v_texCoord;     \n"
			"uniform mat4 c_mvp;		  \n"
			"void main()                  \n"
			"{                            \n"
			"   gl_Position = c_mvp * a_position;\n"
			"   v_texCoord = a_texCoord;  \n"
			"}                            \n";

		_fragmentShaderSource =
			"precision mediump float;							\n"
			"varying vec2 v_texCoord;							\n"
			"uniform sampler2D s_texture;						\n"
			"void main()										\n"
			"{													\n"
			"  gl_FragColor = texture2D(s_texture, v_texCoord );\n"
			"}\n";

		_program = CompileShaders(_vertexShaderSource, _fragmentShaderSource);

		positionLoc = glGetAttribLocation(_program, "a_position");
		texCoordLoc = glGetAttribLocation(_program, "a_texCoord");

		samplerLoc = glGetUniformLocation(_program, "s_texture");

		mvpLoc = glGetUniformLocation(_program, "c_mvp");
		mvp[15] = mvp[10] = 1;

		textureId = CreateSimpleTexture2D();

		glGenBuffers(1, &vertexBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vVertices), vVertices, GL_STATIC_DRAW);

		glGenBuffers(1, &indexBuffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
		
	}

	void Render() override
	{
		glClearColor(1.0, 0.6, 0, 1.0);
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(_program);

		glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);

		glVertexAttribPointer(positionLoc, 4, GL_FLOAT, GL_FALSE, 24, (void*)0);
		glVertexAttribPointer(texCoordLoc, 2, GL_FLOAT, GL_FALSE, 24, (void*)16);

		glEnableVertexAttribArray(positionLoc);
		glEnableVertexAttribArray(texCoordLoc);

		// Bind the texture
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureId);

		glUniform1i(samplerLoc, 0);


		rotZ += 0.01;

		mvp[0] = cos(rotZ);
		mvp[1] = sin(rotZ);
		mvp[4] = -sin(rotZ);
		mvp[5] = cos(rotZ);

		glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, (const GLfloat*)mvp);

		glDepthRangef(-1.0, 1.0);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);

	}

	void EndUp() override
	{
		glDeleteProgram(_program);
	}
};

DECLARE_MAIN(HelloGL)

