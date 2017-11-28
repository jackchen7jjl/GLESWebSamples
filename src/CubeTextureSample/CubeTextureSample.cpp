#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#include "GLSampleBase.h"
#include <math.h>
#include <iostream>
#include "esUtil.h"

using uchar = unsigned char;
using uint = unsigned int;

static GLuint vertexBuffer;
static GLuint normalBuffer;
static GLuint indexBuferr;

static GLint mvpLoc;
static float mvp[16] = { 0 };
static float rotZ;

static GLint  positionLoc;
static GLint  normalLoc;

static GLint vTexCoordLoc;

static GLuint textureId = 0;

static uint numIndices;

GLubyte pixels[4 * 4] =
{
	255, 0, 0,255, // Red
	0, 255, 0,255, // Green
	0, 0, 255,255, // Blue
	255, 255, 0,255 // Yellow
};

static GLint textureSampleLoc;
static GLint offsetLoc;

GLuint CreateSimpleTextureCubemap()
{
	// Six 1x1 RGB faces
	GLubyte cubePixels[6][3] =
	{
		// Face 0 - Red
		255, 0, 0,
		// Face 1 - Green,
		0, 255, 0,
		// Face 3 - Blue
		0, 0, 255,
		// Face 4 - Yellow
		255, 255, 0,
		// Face 5 - Purple
		255, 0, 255,
		// Face 6 - White
		255, 255, 255
	};

	// Generate a texture object
	glGenTextures(1, &textureId);

	// Bind the texture object
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureId);

	// Load the cube face - Positive X
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGB, 1, 1, 0,
		GL_RGB, GL_UNSIGNED_BYTE, &cubePixels[0]);

	// Load the cube face - Negative X
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGB, 1, 1, 0,
		GL_RGB, GL_UNSIGNED_BYTE, &cubePixels[1]);

	// Load the cube face - Positive Y
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGB, 1, 1, 0,
		GL_RGB, GL_UNSIGNED_BYTE, &cubePixels[2]);

	// Load the cube face - Negative Y
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGB, 1, 1, 0,
		GL_RGB, GL_UNSIGNED_BYTE, &cubePixels[3]);

	// Load the cube face - Positive Z
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGB, 1, 1, 0,
		GL_RGB, GL_UNSIGNED_BYTE, &cubePixels[4]);

	// Load the cube face - Negative Z
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGB, 1, 1, 0,
		GL_RGB, GL_UNSIGNED_BYTE, &cubePixels[5]);

	// Set the filtering mode
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	return textureId;

}

class TextureSample :public GLSampleBase
{
protected:
	void StartUp() override
	{
		const static char _vertexShaderSource[] =
      "attribute vec4 vaPosition;   \n"
      "attribute vec3 vaNormal;     \n"
      "varying vec3 vNormal;       \n"
      "void main()                  \n"
      "{                            \n"
      "   gl_Position = vaPosition; \n"
      "   vNormal = vaPosition.xyz;      \n"
      "}                            \n";


		const static char _fragmentShaderSource[] =
      "precision mediump float;                            \n"
      "varying vec3 vNormal;                              \n"
      "uniform samplerCube sTexture;                      \n"
      "void main()                                         \n"
      "{                                                   \n"
      "  gl_FragColor = textureCube( sTexture, vNormal );\n"
      "}                                                   \n";     

		_program = CompileShaders(_vertexShaderSource, _fragmentShaderSource);


		positionLoc = glGetAttribLocation(_program, "vaPosition");
		normalLoc = glGetAttribLocation(_program, "vaNormal");

		textureSampleLoc = glGetUniformLocation(_program,"sTexture");

		//mvpLoc = glGetUniformLocation(_program, "vcMvp");
		//mvp[15] = mvp[10] = 1;

		//vTexCoordLoc = glGetAttribLocation(_program, "vaUV");

		static GLfloat *vVertices = nullptr;
		static GLfloat *vNormals = nullptr;

		static GLushort *indices;

		int numSlices = 20;
		int numParallels = numSlices / 2;
		int numVertices = (numParallels + 1) * (numSlices + 1);
		
		numIndices = esGenSphere(numSlices, 0.75f, &vVertices, &vNormals,NULL, &indices);

		glGenBuffers(1, &vertexBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
		glBufferData(GL_ARRAY_BUFFER, numVertices * 3 * 4, vVertices, GL_STATIC_DRAW);

		glGenBuffers(1, &normalBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
		glBufferData(GL_ARRAY_BUFFER, numVertices * 3 * 4, vNormals, GL_STATIC_DRAW);

		glGenBuffers(1, &indexBuferr);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuferr);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, numIndices * 2, indices, GL_STATIC_DRAW);

		glEnable(GL_CULL_FACE);//glDisable（GL_CULL_FACE）、默认是disabled
		glCullFace(GL_BACK);//GL_FRONT, GL_BACK, and GL_FRONT_AND_BACK，默认是GL_BACK
		glFrontFace(GL_CCW);//GL_CW,GL_CCW,默认是GL_CCW

		CreateSimpleTextureCubemap();
	}

	void Render() override
	{
		glClearColor(1.0, 0.6, 0, 1.0);
		glClear(GL_COLOR_BUFFER_BIT);

		if (textureId == 0)
			return;

		glUseProgram(_program);

		rotZ = 0.01;
		mvp[0] = cos(rotZ);
		mvp[1] = sin(rotZ);
		mvp[4] = -sin(rotZ);
		mvp[5] = cos(rotZ);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, textureId);
		glUniform1i(textureSampleLoc,0);

		//glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, (const GLfloat*)mvp);

		glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);		
		glEnableVertexAttribArray(positionLoc);
		glVertexAttribPointer(positionLoc, 3, GL_FLOAT, GL_FALSE, 12, (void*)0);

		glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
		glEnableVertexAttribArray(normalLoc);
		glVertexAttribPointer(normalLoc, 3, GL_FLOAT, GL_FALSE, 12, (void*)0);

		//glEnableVertexAttribArray(vTexCoordLoc);
		//glVertexAttribPointer(vTexCoordLoc, 2, GL_FLOAT, GL_FALSE, 20, (void*)12);
		
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuferr);
		glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_SHORT, 0);
	}

	void EndUp() override
	{
		glDeleteProgram(_program);
	}
};

DECLARE_MAIN(TextureSample)

