/*
 *  DynamicBatch.cpp
 */

#include "DynamicBatch.h"
#include "RaptorGame.h"
#include "Shader.h"

#include <cstring>


DynamicBatch::DynamicBatch( void )
{
	VAO = 0;
	VBO = 0;
	GPUReady = false;
	Mode = GL_TRIANGLES;
	DrawMode = GL_TRIANGLES;
	Active = false;

	CurS = CurT = 0.f;
	CurNX = CurNY = 0.f;
	CurNZ = 1.f;
	CurR = CurG = CurB = CurA = 1.f;
	CurTX = CurTY = CurTZ = 0.f;
	CurBX = CurBY = CurBZ = 0.f;

	QuadCount = 0;

	Vertices.reserve( 1024 );
}


DynamicBatch::~DynamicBatch()
{
	FreeGPU();
}


void DynamicBatch::InitGPU( void )
{
	if( GPUReady )
		return;

	glGenVertexArrays( 1, &VAO );
	glGenBuffers( 1, &VBO );

	GPUReady = true;
}


void DynamicBatch::FreeGPU( void )
{
	if( VBO )
	{
		glDeleteBuffers( 1, &VBO );
		VBO = 0;
	}
	if( VAO )
	{
		glDeleteVertexArrays( 1, &VAO );
		VAO = 0;
	}
	GPUReady = false;
}


void DynamicBatch::Begin( GLenum mode )
{
	Active = true;
	Mode = mode;
	Vertices.clear();
	QuadCount = 0;

	if( mode == GL_QUADS )
		DrawMode = GL_TRIANGLES;
	else
		DrawMode = mode;

	CurR = CurG = CurB = CurA = 1.f;
	CurS = CurT = 0.f;
	CurNX = CurNY = 0.f;
	CurNZ = 1.f;
	CurTX = CurTY = CurTZ = 0.f;
	CurBX = CurBY = CurBZ = 0.f;
}


void DynamicBatch::End( void )
{
	if( ! Active )
		return;
	Active = false;

	if( Vertices.empty() )
		return;

	if( ! GPUReady )
		InitGPU();

	size_t count = Vertices.size();
	if( count > MAX_VERTICES )
		count = MAX_VERTICES;

	size_t stride = sizeof(BatchVertex);

	// Activate batch shader if no other shader is active.
	Shader *batch_shader = Raptor::Game->Gfx.BatchShader;
	GLint prev_program = 0;
	bool activated_batch = false;
	glGetIntegerv( GL_CURRENT_PROGRAM, &prev_program );

	if( batch_shader && batch_shader->ProgramHandle && ! Raptor::Game->ShaderMgr.Active() )
	{
		glUseProgram( batch_shader->ProgramHandle );
		activated_batch = true;
	}

	Raptor::Game->Gfx.UploadMatrices();

	// Set texture enabled uniform.
	GLint program = 0;
	glGetIntegerv( GL_CURRENT_PROGRAM, &program );
	if( program )
	{
		GLint loc = glGetUniformLocation( program, "uTextureEnabled" );
		if( loc >= 0 )
			glUniform1i( loc, Raptor::Game->Gfx.TextureEnabled ? 1 : 0 );
		loc = glGetUniformLocation( program, "uTexture" );
		if( loc >= 0 )
			glUniform1i( loc, 0 );
	}

	glBindVertexArray( VAO );
	glBindBuffer( GL_ARRAY_BUFFER, VBO );
	glBufferData( GL_ARRAY_BUFFER, count * sizeof(BatchVertex), Vertices.data(), GL_STREAM_DRAW );

	glEnableVertexAttribArray( 0 );
	glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, stride, (void*) offsetof(BatchVertex, x) );

	glEnableVertexAttribArray( 1 );
	glVertexAttribPointer( 1, 2, GL_FLOAT, GL_FALSE, stride, (void*) offsetof(BatchVertex, s) );

	glEnableVertexAttribArray( 2 );
	glVertexAttribPointer( 2, 3, GL_FLOAT, GL_FALSE, stride, (void*) offsetof(BatchVertex, nx) );

	glEnableVertexAttribArray( 3 );
	glVertexAttribPointer( 3, 4, GL_FLOAT, GL_FALSE, stride, (void*) offsetof(BatchVertex, r) );

	glEnableVertexAttribArray( 4 );
	glVertexAttribPointer( 4, 3, GL_FLOAT, GL_FALSE, stride, (void*) offsetof(BatchVertex, tx) );

	glEnableVertexAttribArray( 5 );
	glVertexAttribPointer( 5, 3, GL_FLOAT, GL_FALSE, stride, (void*) offsetof(BatchVertex, bx) );

	glDrawArrays( DrawMode, 0, count );

	glDisableVertexAttribArray( 0 );
	glDisableVertexAttribArray( 1 );
	glDisableVertexAttribArray( 2 );
	glDisableVertexAttribArray( 3 );
	glDisableVertexAttribArray( 4 );
	glDisableVertexAttribArray( 5 );

	glBindBuffer( GL_ARRAY_BUFFER, 0 );
	glBindVertexArray( 0 );

	if( activated_batch )
		glUseProgram( prev_program );
}


void DynamicBatch::EmitVertex( double x, double y, double z )
{
	BatchVertex v;
	v.x = (float)( x - Raptor::Game->Gfx.CamX );
	v.y = (float)( y - Raptor::Game->Gfx.CamY );
	v.z = (float)( z - Raptor::Game->Gfx.CamZ );
	v.s = CurS;  v.t = CurT;
	v.nx = CurNX;  v.ny = CurNY;  v.nz = CurNZ;
	v.r = CurR;  v.g = CurG;  v.b = CurB;  v.a = CurA;
	v.tx = CurTX;  v.ty = CurTY;  v.tz = CurTZ;
	v.bx = CurBX;  v.by = CurBY;  v.bz = CurBZ;

	if( Mode == GL_QUADS )
	{
		QuadBuf[ QuadCount ] = v;
		QuadCount ++;
		if( QuadCount == 4 )
		{
			Vertices.push_back( QuadBuf[ 0 ] );
			Vertices.push_back( QuadBuf[ 1 ] );
			Vertices.push_back( QuadBuf[ 2 ] );
			Vertices.push_back( QuadBuf[ 0 ] );
			Vertices.push_back( QuadBuf[ 2 ] );
			Vertices.push_back( QuadBuf[ 3 ] );
			QuadCount = 0;
		}
	}
	else
		Vertices.push_back( v );
}


void DynamicBatch::Vertex2i( int x, int y )       { EmitVertex( (double) x, (double) y, 0. ); }
void DynamicBatch::Vertex2f( float x, float y )   { EmitVertex( (double) x, (double) y, 0. ); }
void DynamicBatch::Vertex2d( double x, double y ) { EmitVertex( x, y, 0. ); }
void DynamicBatch::Vertex3f( float x, float y, float z )   { EmitVertex( (double) x, (double) y, (double) z ); }
void DynamicBatch::Vertex3d( double x, double y, double z ) { EmitVertex( x, y, z ); }

void DynamicBatch::TexCoord2i( int s, int t )       { CurS = (float) s;  CurT = (float) t; }
void DynamicBatch::TexCoord2f( float s, float t )   { CurS = s;  CurT = t; }
void DynamicBatch::TexCoord2d( double s, double t ) { CurS = (float) s;  CurT = (float) t; }

void DynamicBatch::Normal3f( float nx, float ny, float nz )   { CurNX = nx;  CurNY = ny;  CurNZ = nz; }
void DynamicBatch::Normal3d( double nx, double ny, double nz ) { CurNX = (float) nx;  CurNY = (float) ny;  CurNZ = (float) nz; }

void DynamicBatch::Color3f( float r, float g, float b )              { CurR = r;  CurG = g;  CurB = b;  CurA = 1.f; }
void DynamicBatch::Color4f( float r, float g, float b, float a )     { CurR = r;  CurG = g;  CurB = b;  CurA = a; }

void DynamicBatch::Attrib3f( int index, float x, float y, float z )
{
	if( index == 0 )      { CurTX = x;  CurTY = y;  CurTZ = z; }
	else if( index == 1 ) { CurBX = x;  CurBY = y;  CurBZ = z; }
}
