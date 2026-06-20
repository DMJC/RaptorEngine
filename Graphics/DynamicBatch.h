/*
 *  DynamicBatch.h
 */

#pragma once

#include "RaptorGL.h"

#include <vector>

struct BatchVertex
{
	GLfloat x, y, z;
	GLfloat s, t;
	GLfloat nx, ny, nz;
	GLfloat r, g, b, a;
	GLfloat tx, ty, tz;
	GLfloat bx, by, bz;
};


class DynamicBatch
{
public:
	DynamicBatch( void );
	~DynamicBatch();

	void Begin( GLenum mode );
	void End( void );

	void Vertex2i( int x, int y );
	void Vertex2f( float x, float y );
	void Vertex2d( double x, double y );
	void Vertex3f( float x, float y, float z );
	void Vertex3d( double x, double y, double z );

	void TexCoord2i( int s, int t );
	void TexCoord2f( float s, float t );
	void TexCoord2d( double s, double t );

	void Normal3f( float nx, float ny, float nz );
	void Normal3d( double nx, double ny, double nz );

	void Color3f( float r, float g, float b );
	void Color4f( float r, float g, float b, float a );

	void Attrib3f( int index, float x, float y, float z );

	void FreeGPU( void );

private:
	void InitGPU( void );
	void EmitVertex( double x, double y, double z );

	GLuint VAO, VBO;
	bool GPUReady;

	GLenum Mode;
	GLenum DrawMode;
	bool Active;

	float CurS, CurT;
	float CurNX, CurNY, CurNZ;
	float CurR, CurG, CurB, CurA;
	float CurTX, CurTY, CurTZ;
	float CurBX, CurBY, CurBZ;

	std::vector<BatchVertex> Vertices;

	int QuadCount;
	BatchVertex QuadBuf[ 4 ];

	static const size_t MAX_VERTICES = 65536;
};
