#include <GL\glew.h>
#include <GL\freeglut.h>
#include <iostream>
#include <string>
#include <algorithm>
#include <vector>
#include "StructuresAndHelperFuncs.h"

using namespace std;


class Camera {
private:
	mat mattb(mat &m){
		mat matrix;
		matrix.m[0][0] = m.m[0][0]; matrix.m[0][1] = m.m[1][0]; matrix.m[0][2] = m.m[2][0]; matrix.m[0][3] = 0.0f;
		matrix.m[1][0] = m.m[0][1]; matrix.m[1][1] = m.m[1][1]; matrix.m[1][2] = m.m[2][1]; matrix.m[1][3] = 0.0f;
		matrix.m[2][0] = m.m[0][2]; matrix.m[2][1] = m.m[1][2]; matrix.m[2][2] = m.m[2][2]; matrix.m[2][3] = 0.0f;
		matrix.m[3][0] = -(m.m[3][0] * matrix.m[0][0] + m.m[3][1] * matrix.m[1][0] + m.m[3][2] * matrix.m[2][0]);
		matrix.m[3][1] = -(m.m[3][0] * matrix.m[0][1] + m.m[3][1] * matrix.m[1][1] + m.m[3][2] * matrix.m[2][1]);
		matrix.m[3][2] = -(m.m[3][0] * matrix.m[0][2] + m.m[3][1] * matrix.m[1][2] + m.m[3][2] * matrix.m[2][2]);
		matrix.m[3][3] = 1.0f;
		return matrix;
	}

public:

	mat pointAt(point &camerapos, point &lookdir, point &vup, point &vtar) {

		point newForward = vecSub(vtar, camerapos);
		newForward = vecNorm(newForward);

		point u = vecMultnum(newForward, vecDot(vup, newForward));
		point newUp = vecSub(vup, u);
		newUp = vecNorm(newUp);

		point newRight = vecCross(newUp, newForward);

		mat matrix;
		matrix.m[0][0] = newRight.x;	matrix.m[0][1] = newRight.y;	matrix.m[0][2] = newRight.z;	matrix.m[0][3] = 0.0f;
		matrix.m[1][0] = newUp.x;		matrix.m[1][1] = newUp.y;		matrix.m[1][2] = newUp.z;		matrix.m[1][3] = 0.0f;
		matrix.m[2][0] = newForward.x;	matrix.m[2][1] = newForward.y;	matrix.m[2][2] = newForward.z;	matrix.m[2][3] = 0.0f;
		matrix.m[3][0] = camerapos.x;	matrix.m[3][1] = camerapos.y;	matrix.m[3][2] = camerapos.z;	matrix.m[3][3] = 1.0f;

		return mattb(matrix);
	}

};