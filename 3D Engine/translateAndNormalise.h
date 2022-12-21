#include <Windows.h>
#include <GL\glew.h>
#include <GL\freeglut.h>
#include <iostream>
#include <string>
#include <algorithm>
#include <vector>
#include <fstream>
#include <strstream>
#include <list>
#include <set>
#include "Camera.h"

class object {
private:
	mesh cubeproj, ogm;
	vector <triangle> sorted;

	/*FUNCTIONS-------------------------------------------*/

	void tricout(triangle& tri) {
		veccout(tri.p[0]);
		veccout(tri.p[1]);
		veccout(tri.p[2]);
	}

	void normalise(mesh& m, mesh& cubeproj, mat& proj, point& vfor, mat& matview) {
		for (unsigned int k = 0; k < m.triangles.size(); k++) {

			matMultsv(matview, m.triangles[k]);

			point normal, l1, l2;

			l1 = vecSub(m.triangles[k].p[1], m.triangles[k].p[0]);
			l2 = vecSub(m.triangles[k].p[2], m.triangles[k].p[0]);
			normal = vecCross(l1, l2);
			normal = vecNorm(normal);

			point ray = vecSub(m.triangles[k].p[0], vfor);

			m.triangles[k].normal = normal;

			if (vecDot(normal, ray) < 0.0f) {

				m.triangles[k].visible = true;

				int clippedTrianglesCnt = 0;
				triangle clipped[2];
				point cr = { 0, 0, 0.1f }, forwrd = { 0,0,1 };
				clippedTrianglesCnt = triangleClipping(cr, forwrd, m.triangles[k], clipped[0], clipped[1]);

				int w = cubeproj.triangles.size() - 1, GTT = 0;

				if (clippedTrianglesCnt == 1 || clippedTrianglesCnt == 3) {
					cubeproj.triangles.push_back(clipped[0]);
					GTT = 1;
				}
				if (clippedTrianglesCnt == 2) {
					cubeproj.triangles.push_back(clipped[0]);
					cubeproj.triangles.push_back(clipped[1]);
					GTT = 2;
				}

				while (GTT--) {

					w++;

					cubeproj.triangles[w].normal = m.triangles[k].normal;
					cubeproj.triangles[w].visible = true;

					for (int i = 0; i < 3; i++) {

						GLfloat td = cubeproj.triangles[w].p[i].x * proj.m[0][3] + cubeproj.triangles[w].p[i].y * proj.m[1][3] + 
							cubeproj.triangles[w].p[i].z * proj.m[2][3] + proj.m[3][3];

						cubeproj.triangles[w].t[i].u /= td;
						cubeproj.triangles[w].t[i].v /= td;
						cubeproj.triangles[w].t[i].w = 1.0f / td;

						cubeproj.triangles[w].p[i].x *= proj.m[0][0];
						cubeproj.triangles[w].p[i].x /= cubeproj.triangles[w].p[i].z;
						cubeproj.triangles[w].p[i].y *= proj.m[1][1];
						cubeproj.triangles[w].p[i].y /= cubeproj.triangles[w].p[i].z;
						cubeproj.triangles[w].p[i].z = cubeproj.triangles[w].p[i].z * proj.m[2][2] + proj.m[2][3];
					}

					triangle push = {   cubeproj.triangles[w].p[0].x, cubeproj.triangles[w].p[0].y, cubeproj.triangles[w].p[0].z,
										cubeproj.triangles[w].p[1].x, cubeproj.triangles[w].p[1].y, cubeproj.triangles[w].p[1].z,
										cubeproj.triangles[w].p[2].x, cubeproj.triangles[w].p[2].y, cubeproj.triangles[w].p[2].z,
										cubeproj.triangles[w].t[0].u, cubeproj.triangles[w].t[0].v, cubeproj.triangles[w].t[0].w,
										cubeproj.triangles[w].t[1].u, cubeproj.triangles[w].t[1].v, cubeproj.triangles[w].t[1].w,
										cubeproj.triangles[w].t[2].u, cubeproj.triangles[w].t[2].v, cubeproj.triangles[w].t[2].w,
										cubeproj.triangles[w].normal, cubeproj.triangles[w].color, cubeproj.triangles[w].visible };

					sorted.push_back(push);

				}
			}
		}
	}

	void matMult(mesh& m, mat& mt) {
		for (unsigned int k = 0; k < m.triangles.size(); k++) {
			for (int i = 0; i < 3; i++) {
				point cd = m.triangles[k].p[i];

				m.triangles[k].p[i].x = cd.x * mt.m[0][0] + cd.y * mt.m[1][0] + cd.z * mt.m[2][0] + mt.m[3][0];
				m.triangles[k].p[i].y = cd.x * mt.m[0][1] + cd.y * mt.m[1][1] + cd.z * mt.m[2][1] + mt.m[3][1];
				m.triangles[k].p[i].z = cd.x * mt.m[0][2] + cd.y * mt.m[1][2] + cd.z * mt.m[2][2] + mt.m[3][2];
				GLfloat num = cd.x * mt.m[0][3] + cd.y * mt.m[1][3] + cd.z * mt.m[2][3] + mt.m[3][3];

				if (num != 0.0f) {
					m.triangles[k].p[i].x /= num;
					m.triangles[k].p[i].y /= num;
					m.triangles[k].p[i].z /= num;
				}
			}
		}
	}

	void matMultsv(mat& mt, triangle& t) {
		for (int i = 0; i < 3; i++) {
			point cd = { t.p[i].x, t.p[i].y, t.p[i].z };

			t.p[i].x = cd.x * mt.m[0][0] + cd.y * mt.m[1][0] + cd.z * mt.m[2][0] + mt.m[3][0];
			t.p[i].y = cd.x * mt.m[0][1] + cd.y * mt.m[1][1] + cd.z * mt.m[2][1] + mt.m[3][1];
			t.p[i].z = cd.x * mt.m[0][2] + cd.y * mt.m[1][2] + cd.z * mt.m[2][2] + mt.m[3][2];
			GLfloat num = cd.x * mt.m[0][3] + cd.y * mt.m[1][3] + cd.z * mt.m[2][3] + mt.m[3][3];

			if (num != 0.0f) {
				t.p[i].x /= num;
				t.p[i].y /= num;
				t.p[i].z /= num;
			}
		}
	}

	void stp(mesh& m, GLfloat x, GLfloat y, GLfloat z) {
		for (unsigned int k = 0; k < m.triangles.size(); k++) {
			for (int i = 0; i < 3; i++) {
				m.triangles[k].p[i].x += x;
				m.triangles[k].p[i].y += y;
				m.triangles[k].p[i].z += z;
			}
		}
	}

	void str(mesh& m, GLfloat rx, GLfloat ry, GLfloat rz) {
		for (unsigned int k = 0; k < m.triangles.size(); k++) {
			for (int i = 0; i < 3; i++) {
				m.triangles[k].p[i] = rotateAroundX(m.triangles[k].p[i], rx);
				m.triangles[k].p[i] = rotateAroundY(m.triangles[k].p[i], ry);
				m.triangles[k].p[i] = rotateAroundZ(m.triangles[k].p[i], rz);

			}
		}
	}

	point rotateAroundX(point p, GLfloat angle) {
		point ans = p;

		ans.y = p.y * cosf(angle) - p.z * sinf(angle);
		ans.z = p.y * sinf(angle) + p.z * cosf(angle);

		return ans;
	}

	point rotateAroundY(point p, GLfloat angle) {
		point ans = p;

		ans.x = p.x * cosf(angle) - p.z * sinf(angle);
		ans.z = p.x * sinf(angle) + p.z * cosf(angle);

		return ans;
	}

	point rotateAroundZ(point p, GLfloat angle) {
		point ans = p;

		ans.x = p.x * cosf(angle) - p.y * sinf(angle);
		ans.y = p.x * sinf(angle) + p.y * cosf(angle);

		return ans;
	}


public:
	pair <mesh, vector <triangle> > mn(int framenum, mesh& msh, GLfloat ScreenWidth, GLfloat ScreenHeight,
		point& cameradel, point& lookdir, point& vup, point& vtar, point& lookdirgl, point startingPoint = { 0.0f, 0.0f, 0.0f },
		point startingRotation = { 0.0f, 0.0f, 0.0f }, point transformation = { 0.0f, 0.0f, 0.0f }, bool rotation = false) {

		sorted.clear();

		GLfloat a, f, q, screenHeight, screenWidth;

		screenHeight = ScreenHeight;
		screenWidth = ScreenWidth;

		GLfloat fov = 60.0f, fnear = 0.1f, ffar = 1000.0f;

		a = screenHeight / screenWidth;
		f = 1.0f / (tan(fov * 0.5f / 180.0f * 3.14159f));

		mat proj;

		proj.m[0][0] = a * f;
		proj.m[1][1] = f;
		proj.m[2][2] = (ffar) / (ffar - fnear);
		proj.m[3][2] = -(ffar * fnear) / (ffar - fnear);
		proj.m[2][3] = 1.0f;

		mat matView;

		int page = 0, frame = 0;
		float srn = 0; GLfloat fTheta = 0;

		bool spch = (startingPoint.x != 0.0f || startingPoint.y != 0.0f || startingPoint.z != 0.0f);
		if (spch && framenum == 0) stp(msh, startingPoint.x, startingPoint.y, startingPoint.z);
		
		bool isRotAllowed = startingRotation.x != 0.0f || startingRotation.y != 0.0f || startingRotation.z != 0.0f;


		if (isRotAllowed && framenum == 0) {
			str(msh, startingRotation.x, startingRotation.y, startingRotation.z);
		}

		fTheta = 0.05f;

		Camera cm;
		matView = cm.pointAt(cameradel, lookdir, vup, vtar);

		point rp = { 0,0,1 };

		normalise(msh, cubeproj, proj, cameradel, matView);
		cubeproj.triangles.clear();

		/*
		sort(sorted.begin(), sorted.end(), [](triangle& t1, triangle& t2) {
			GLfloat z1 = (t1.p[0].z + t1.p[1].z + t1.p[2].z) / 3.0f;
			GLfloat z2 = (t2.p[0].z + t2.p[1].z + t2.p[2].z) / 3.0f;
			return z1 > z2;
			});
		*/

		return { msh, sorted };
	}

};
