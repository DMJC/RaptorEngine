/*
 *  Mat4.h
 */

#pragma once

#include <cmath>
#include <cstring>
#include "Vec.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif


class Mat4
{
public:
	double M[ 16 ]; // column-major

	Mat4( void )
	{
		memset( M, 0, sizeof(M) );
	}

	static Mat4 Identity( void )
	{
		Mat4 m;
		m.M[0] = 1.; m.M[5] = 1.; m.M[10] = 1.; m.M[15] = 1.;
		return m;
	}

	static Mat4 FromColumnMajor( const double *src )
	{
		Mat4 m;
		memcpy( m.M, src, 16 * sizeof(double) );
		return m;
	}

	static Mat4 Ortho( double left, double right, double bottom, double top, double znear, double zfar )
	{
		Mat4 m;
		m.M[0]  =  2. / (right - left);
		m.M[5]  =  2. / (top - bottom);
		m.M[10] = -2. / (zfar - znear);
		m.M[12] = -(right + left) / (right - left);
		m.M[13] = -(top + bottom) / (top - bottom);
		m.M[14] = -(zfar + znear) / (zfar - znear);
		m.M[15] = 1.;
		return m;
	}

	static Mat4 Perspective( double fov_y_deg, double aspect, double znear, double zfar )
	{
		double f = 1. / tan( fov_y_deg * M_PI / 360. );
		Mat4 m;
		m.M[0]  = f / aspect;
		m.M[5]  = f;
		m.M[10] = (zfar + znear) / (znear - zfar);
		m.M[11] = -1.;
		m.M[14] = (2. * zfar * znear) / (znear - zfar);
		return m;
	}

	static Mat4 LookAt( double eyeX, double eyeY, double eyeZ,
	                     double centerX, double centerY, double centerZ,
	                     double upX, double upY, double upZ )
	{
		double fx = centerX - eyeX, fy = centerY - eyeY, fz = centerZ - eyeZ;
		double len = sqrt( fx*fx + fy*fy + fz*fz );
		if( len > 0. ) { fx /= len; fy /= len; fz /= len; }

		double sx = fy * upZ - fz * upY;
		double sy = fz * upX - fx * upZ;
		double sz = fx * upY - fy * upX;
		len = sqrt( sx*sx + sy*sy + sz*sz );
		if( len > 0. ) { sx /= len; sy /= len; sz /= len; }

		double ux = sy * fz - sz * fy;
		double uy = sz * fx - sx * fz;
		double uz = sx * fy - sy * fx;

		Mat4 m;
		m.M[0] = sx;  m.M[4] = sy;  m.M[8]  = sz;
		m.M[1] = ux;  m.M[5] = uy;  m.M[9]  = uz;
		m.M[2] = -fx; m.M[6] = -fy; m.M[10] = -fz;
		m.M[12] = -(sx * eyeX + sy * eyeY + sz * eyeZ);
		m.M[13] = -(ux * eyeX + uy * eyeY + uz * eyeZ);
		m.M[14] =  (fx * eyeX + fy * eyeY + fz * eyeZ);
		m.M[15] = 1.;
		return m;
	}

	static Mat4 Translate( double x, double y, double z )
	{
		Mat4 m = Identity();
		m.M[12] = x; m.M[13] = y; m.M[14] = z;
		return m;
	}

	static Mat4 Scale( double x, double y, double z )
	{
		Mat4 m;
		m.M[0] = x; m.M[5] = y; m.M[10] = z; m.M[15] = 1.;
		return m;
	}

	static Mat4 RotateAxis( double degrees, double ax, double ay, double az )
	{
		double rad = degrees * M_PI / 180.;
		double c = cos( rad ), s = sin( rad ), t = 1. - c;
		double len = sqrt( ax*ax + ay*ay + az*az );
		if( len > 0. ) { ax /= len; ay /= len; az /= len; }

		Mat4 m;
		m.M[0]  = t*ax*ax + c;     m.M[4]  = t*ax*ay - s*az;  m.M[8]  = t*ax*az + s*ay;
		m.M[1]  = t*ax*ay + s*az;  m.M[5]  = t*ay*ay + c;     m.M[9]  = t*ay*az - s*ax;
		m.M[2]  = t*ax*az - s*ay;  m.M[6]  = t*ay*az + s*ax;  m.M[10] = t*az*az + c;
		m.M[15] = 1.;
		return m;
	}

	Mat4 operator*( const Mat4 &b ) const
	{
		Mat4 r;
		for( int col = 0; col < 4; col ++ )
			for( int row = 0; row < 4; row ++ )
				r.M[ col*4 + row ] = M[row] * b.M[col*4] + M[4+row] * b.M[col*4+1] + M[8+row] * b.M[col*4+2] + M[12+row] * b.M[col*4+3];
		return r;
	}

	void ToFloat( float out[16] ) const
	{
		for( int i = 0; i < 16; i ++ )
			out[ i ] = (float) M[ i ];
	}

	const double *Ptr( void ) const { return M; }
};
