#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#include "GLSampleBase.h"
#include <math.h>
#include <iostream>

using uchar = unsigned char;
using uint = unsigned int;

static GLuint vertexBuffer;
static GLuint indexBuferr;

static GLint mvpLoc;
static float mvp[16] = { 0 };
static float rotZ;

static GLint  positionLoc;

static GLint vTexCoordLoc;

static GLuint textureId = 0;

GLubyte pixels[4 * 4] =
{
	255, 0, 0,255, // Red
	0, 255, 0,255, // Green
	0, 0, 255,255, // Blue
	255, 255, 0,255 // Yellow
};

static GLint textureSampleLoc;
static GLint offsetLoc;

void CreateTexture()
{
	int textureWidth = 2;
	int textureHeight = 2;

	glPixelStorei(GL_PACK_ALIGNMENT,1);

	glGenTextures(1, &textureId);
	glBindTexture(GL_TEXTURE_2D, textureId);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, textureWidth, textureHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
	glGenerateMipmap(GL_TEXTURE_2D);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

class TextureSample :public GLSampleBase
{
protected:
	void StartUp() override
	{
		const static char _vertexShaderSource[] = R"(
		attribute vec4 vaPosition;
		attribute vec2 vaUV;
		uniform mat4 vcMvp;
		varying vec2 vUV;
		void main()
		{
			gl_Position = vcMvp * vaPosition;
			vUV = vaUV;
		}
												)";


		const static char _fragmentShaderSource[] = R"(
			precision mediump float;
			uniform sampler2D sDiffuse;
			varying vec2 vUV;
			void main()
			{
			  gl_FragColor = texture2D(sDiffuse,vUV);
			}
													)";

		_program = CompileShaders(_vertexShaderSource, _fragmentShaderSource);


		positionLoc = glGetAttribLocation(_program, "vaPosition");
		
		textureSampleLoc = glGetUniformLocation(_program,"sDiffuse");

		mvpLoc = glGetUniformLocation(_program, "vcMvp");
		mvp[15] = mvp[10] = 1;

		vTexCoordLoc = glGetAttribLocation(_program, "vaUV");

		static GLfloat vVertices[] = { -0.5f, 0.5f, 0.0f,0.0f,0.0f,
			-0.5f, -0.5f, 0.0f,0.0f,1.0f,
			0.5f, -0.5f, 0.0f,1.0f,1.0f,
			0.5f, 0.5f, 0.0f,1.0f,0.0f};

		static GLushort indices[] = { 0,1,2,0,2,3};

		glGenBuffers(1, &vertexBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vVertices), vVertices, GL_STATIC_DRAW);

		glGenBuffers(1, &indexBuferr);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuferr);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

		glEnable(GL_CULL_FACE);//glDisable（GL_CULL_FACE）、默认是disabled
		glCullFace(GL_BACK);//GL_FRONT, GL_BACK, and GL_FRONT_AND_BACK，默认是GL_BACK
		glFrontFace(GL_CCW);//GL_CW,GL_CCW,默认是GL_CCW

		CreateTexture();
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
		glBindTexture(GL_TEXTURE_2D, textureId);
		glUniform1i(textureSampleLoc,0);

		glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, (const GLfloat*)mvp);

		glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuferr);

		glEnableVertexAttribArray(positionLoc);
		glVertexAttribPointer(positionLoc, 3, GL_FLOAT, GL_FALSE, 20, (void*)0);

		glEnableVertexAttribArray(vTexCoordLoc);
		glVertexAttribPointer(vTexCoordLoc, 2, GL_FLOAT, GL_FALSE, 20, (void*)12);

		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);
	}

	void EndUp() override
	{
		glDeleteProgram(_program);
	}
};

DECLARE_MAIN(TextureSample)

