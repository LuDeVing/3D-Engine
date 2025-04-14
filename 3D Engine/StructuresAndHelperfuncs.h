#ifndef STRUCTURESANDHELPERFUNCTIONS_CLASS
#define STRUCTURESANDHELPERFUNCTIONS_CLASS

#define _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS
#define _SILENCE_CXX17_STRSTREAM_DEPRECATION_WARNING 

#include <Windows.h>
#include <GL\glew.h>
#include <GL\freeglut.h>
#include <iostream>
#include <string>
#include <algorithm>
#include <vector>
#include <fstream>
#include <strstream>

using namespace std;
typedef long double ld;

struct tri2d {
	GLfloat x1, y1, x2, y2, x3, y3;
};

struct point2D {
	GLfloat u = 0.0f, v = 0.0f, w = 0.0f;
};

struct point {
	GLfloat x = 0.0f, y = 0.0f, z = 0.0f;
};

struct triangle {

	point p[3];
	point2D t[3];
	point normal = { 0, 0, 1 };
	color color = { 1, 1, 1 };
	bool visible = false;

};

class Pixel {

public:

	unsigned int r, g, b, a;

	Pixel(unsigned int r = 0, unsigned int g = 0, unsigned int b = 0, unsigned int a = 255) {

		this->r = r;
		this->g = g;
		this->b = b;
		this->a = a;

	}

};

class DisplayImage {

public:

	unsigned char* imageData;
	unsigned int width, height;

	DisplayImage() = default;

	DisplayImage(unsigned int width, unsigned int height) {

		this->width = width;
		this->height = height;
		this->imageData = new unsigned char[width * height * 4];

	}

	void Draw(int x, int y, Pixel color) {

		int idx = (width * y + x) * 4;

		if (idx > this->width * this->height * 4 - 4)
			return;

		imageData[idx] = color.r;
		imageData[idx + 1] = color.g;
		imageData[idx + 2] = color.b;
		imageData[idx + 3] = color.a;

	}

	Pixel getPixel(int x, int y) {

		int idx = (width * y + x) * 4;

		if (idx > this->width * this->height * 4 - 4)
			return Pixel();

		return Pixel(
			imageData[idx],
			imageData[idx + 1],
			imageData[idx + 2],
			imageData[idx + 3]
		);

	}

};

struct mesh
{
	vector<triangle> triangles;

	bool LoadFromObjectFile(string sFilename, bool bHasTexture = false, bool ps1SpyroMap = false)
	{
		ifstream f(sFilename);
		if (!f.is_open())
			return false;

		// Local cache of verts
		vector<point> verts;
		vector<point2D> texs;

		while (!f.eof())
		{
			char line[128];
			f.getline(line, 128);

			strstream s;
			s << line;

			char junk;

			if (line[0] == 'v')
			{
				if (line[1] == 't')
				{
					point2D v;
					s >> junk >> junk >> v.u >> v.v;

					if (ps1SpyroMap) v.v = 1.0f - v.v;

					texs.push_back(v);
				}
				else
				{
					point v;
					s >> junk >> v.x >> v.y >> v.z;
					verts.push_back(v);
				}
			}

			if (!bHasTexture)
			{
				if (line[0] == 'f')
				{
					int f[3];
					s >> junk >> f[0] >> f[1] >> f[2];
					triangles.push_back({ verts[f[0] - 1], verts[f[1] - 1], verts[f[2] - 1] });
				}
			}
			else
			{
				if (line[0] == 'f')
				{
					s >> junk;

					string tokens[6];
					int nTokenCount = -1;

					while (!s.eof())
					{
						char c = s.get();
						if (c == ' ' || c == '/')
							nTokenCount++;
						else
							tokens[nTokenCount].append(1, c);
					}

					tokens[nTokenCount].pop_back();

					triangles.push_back({ verts[stoi(tokens[0]) - 1], verts[stoi(tokens[2]) - 1], verts[stoi(tokens[4]) - 1],
						texs[stoi(tokens[1]) - 1], texs[stoi(tokens[3]) - 1], texs[stoi(tokens[5]) - 1] });

				}

			}
		}

		return true;
	}

	void RotatePoint(point& p, char axis, float cosA, float sinA)
	{
		float tmp;
		switch (axis)
		{
		case 'X':
			tmp = p.y;
			p.y = p.y * cosA - p.z * sinA;
			p.z = tmp * sinA + p.z * cosA;
			break;
		case 'Y':
			tmp = p.x;
			p.x = p.x * cosA + p.z * sinA;
			p.z = -tmp * sinA + p.z * cosA;
			break;
		case 'Z':
			tmp = p.x;
			p.x = p.x * cosA - p.y * sinA;
			p.y = tmp * sinA + p.y * cosA;
			break;
		default:
			break;
		}
	}

	void RotateMesh(char axis, float angle)
	{
		float cosA = cos(angle);
		float sinA = sin(angle);

		for (auto& tri : triangles)
		{
			// Rotate each point of the triangle.
			RotatePoint(tri.p[0], axis, cosA, sinA);
			RotatePoint(tri.p[1], axis, cosA, sinA);
			RotatePoint(tri.p[2], axis, cosA, sinA);
		}
	}

	void TranslateMesh(float dx, float dy, float dz)
	{
		for (auto& tri : triangles)
		{
			tri.p[0].x += dx;
			tri.p[0].y += dy;
			tri.p[0].z += dz;

			tri.p[1].x += dx;
			tri.p[1].y += dy;
			tri.p[1].z += dz;

			tri.p[2].x += dx;
			tri.p[2].y += dy;
			tri.p[2].z += dz;
		}
	}

};

struct mat {
	GLfloat m[4][4] = { 0 };
};

GLfloat maxGLfloat(GLfloat a, GLfloat b) {

	if (a < b) {
		return b;
	}

	return a;

}

GLfloat minGLfloat(GLfloat a, GLfloat b) {

	if (a > b) {
		return b;
	}

	return a;

}

int maxInt(int a, int b) {

	if (a < b) {
		return b;
	}

	return a;

}

int minInt(int a, int b) {

	if (a > b) {
		return b;
	}

	return a;

}

struct line {
	ld x1, y1, x2, y2;
};

long double cross(long double x1, long double y1, long double x2, long double y2, long double x3, long double y3) {
	return (((x2 - x1) * (y3 - y1)) - ((x3 - x1) * (y2 - y1)));
}


GLfloat vecDot(point& v1, point& v2) {
	return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

GLfloat vecLen(point v) {
	return sqrt(vecDot(v, v));
}

point vecSub(point& v1, point& v2) {
	return { v1.x - v2.x, v1.y - v2.y, v1.z - v2.z };
}

point vecDiv(point& v1, GLfloat div) {
	return { v1.x / div, v1.y / div, v1.z / div };
}

point vecMultnum(point& v1, GLfloat mult) {
	return { v1.x * mult, v1.y * mult, v1.z * mult };
}

point vecsMult(point& v1, point& v2) {
	return { v1.x * v2.x, v1.y * v2.y, v1.z * v2.z };
}

point vecNorm(point& v) {
	GLfloat len = vecLen(v);
	return vecDiv(v, len);
}

point vecAdd(point v1, point v2) {
	return { v1.x + v2.x, v1.y + v2.y, v1.z + v2.z };
}


void veccout(point v) {
	cout << v.x << " " << v.y << " " << v.z << endl;
}

point vecCross(point& v1, point& v2) {
	point v;
	v.x = v1.y * v2.z - v1.z * v2.y;
	v.y = v1.z * v2.x - v1.x * v2.z;
	v.z = v1.x * v2.y - v1.y * v2.x;
	return v;
}

point plane_lineIntersection(point& plane_p, point& plane_n, point& lineStart, point& lineEnd, GLfloat& t)
{
	plane_n = vecNorm(plane_n);
	GLfloat plane_d = -vecDot(plane_n, plane_p);
	GLfloat ad = vecDot(lineStart, plane_n);
	GLfloat bd = vecDot(lineEnd, plane_n);
	t = (-plane_d - ad) / (bd - ad);
	point lineStartToEnd = vecSub(lineEnd, lineStart);
	point lineToIntersect = vecMultnum(lineStartToEnd, t);
	return vecAdd(lineStart, lineToIntersect);
}

int triangleClipping(point plane_p, point plane_n, triangle& intri, triangle& outtri, triangle& outtri2) {

	plane_n = vecNorm(plane_n);

	auto dist = [&](point& p) {
		point n = vecNorm(p);
		return (plane_n.x * p.x + plane_n.y * p.y + plane_n.z * p.z - vecDot(plane_n, plane_p));
	};

	point* insidepts[3]; int insidecnt = 0;
	point* outsidepts[3]; int outsidecnt = 0;
	point2D* insidet[3]; int insidetn = 0;
	point2D* outsidet[3]; int outsidetn = 0;

	float d0 = dist(intri.p[0]);
	float d1 = dist(intri.p[1]);
	float d2 = dist(intri.p[2]);

	//cout << d0 << " " << d1 << " " << d2 << endl;
	//cout << intri.p[0].x << " " << intri.p[0].y << " " << intri.p[0].z << endl;

	if (d0 >= 0) {
		insidepts[insidecnt++] = &intri.p[0];
		insidet[insidetn++] = &intri.t[0];
	}
	else {
		outsidepts[outsidecnt++] = &intri.p[0];
		outsidet[outsidetn++] = &intri.t[0];
	}
	if (d1 >= 0) {
		insidepts[insidecnt++] = &intri.p[1];
		insidet[insidetn++] = &intri.t[1];
	}
	else {
		outsidepts[outsidecnt++] = &intri.p[1];
		outsidet[outsidetn++] = &intri.t[1];
	}
	if (d2 >= 0) {
		insidepts[insidecnt++] = &intri.p[2];
		insidet[insidetn++] = &intri.t[2];
	}
	else {
		outsidepts[outsidecnt++] = &intri.p[2];
		outsidet[outsidetn++] = &intri.t[2];
	}

	if (insidecnt == 0) {
		return 0;
	}
	if (insidecnt == 3) {
		outtri = intri;
		return 1;
	}
	if (insidecnt == 1 && outsidecnt == 2) {

		outtri.color = intri.color;
		//outtri.color = { 1, 0, 0 };
		outtri.normal = intri.normal;
		outtri.visible = intri.visible;

		GLfloat t;

		outtri.p[0] = *insidepts[0];
		outtri.t[0] = *insidet[0];

		outtri.p[1] = plane_lineIntersection(plane_p, plane_n, *insidepts[0], *outsidepts[0], t);
		outtri.t[1].u = t * (outsidet[0]->u - insidet[0]->u) + insidet[0]->u;
		outtri.t[1].v = t * (outsidet[0]->v - insidet[0]->v) + insidet[0]->v;
		outtri.t[1].w = t * (outsidet[0]->w - insidet[0]->w) + insidet[0]->w;

		outtri.p[2] = plane_lineIntersection(plane_p, plane_n, *insidepts[0], *outsidepts[1], t);
		outtri.t[2].u = t * (outsidet[1]->u - insidet[0]->u) + insidet[0]->u;
		outtri.t[2].v = t * (outsidet[1]->v - insidet[0]->v) + insidet[0]->v;
		outtri.t[2].w = t * (outsidet[1]->w - insidet[0]->w) + insidet[0]->w;

		return 1;
	}
	if (insidecnt == 2 && outsidecnt == 1) {

		outtri.color = intri.color;
		//outtri.color = { 0, 1, 0 };
		outtri.normal = intri.normal;
		outtri.visible = intri.visible;

		outtri2.color = intri.color;
		//outtri2.color = { 0, 0, 1 };
		outtri2.normal = intri.normal;
		outtri2.visible = intri.visible;

		GLfloat t;

		outtri.p[0] = *insidepts[0];
		outtri.p[1] = *insidepts[1];
		outtri.t[0] = *insidet[0];
		outtri.t[1] = *insidet[1];

		outtri.p[2] = plane_lineIntersection(plane_p, plane_n, *insidepts[0], *outsidepts[0], t);

		outtri.t[2].u = t * (outsidet[0]->u - insidet[0]->u) + insidet[0]->u;
		outtri.t[2].v = t * (outsidet[0]->v - insidet[0]->v) + insidet[0]->v;
		outtri.t[2].w = t * (outsidet[0]->w - insidet[0]->w) + insidet[0]->w;

		outtri2.p[0] = *insidepts[1];
		outtri2.p[1] = outtri.p[2];
		outtri2.t[0] = *insidet[1];
		outtri2.t[1] = outtri.t[2];

		outtri2.p[2] = plane_lineIntersection(plane_p, plane_n, *insidepts[1], *outsidepts[0], t);

		outtri2.t[2].u = t * (outsidet[0]->u - insidet[1]->u) + insidet[1]->u;
		outtri2.t[2].v = t * (outsidet[0]->v - insidet[1]->v) + insidet[1]->v;
		outtri2.t[2].w = t * (outsidet[0]->w - insidet[1]->w) + insidet[1]->w;

		return 2;
	}
}

#endif