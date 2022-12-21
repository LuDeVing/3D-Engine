#include <Windows.h>
#include <GL\glew.h>
#include <GL\freeglut.h>
#include <iostream>
#include <algorithm>
#include <vector>
using namespace std;

struct color {
	GLfloat r = 0.0f, g = 0.0f, b = 0.0f;
};

void lightValue(color& lightColor, GLfloat& r, GLfloat& g, GLfloat& b, color& ambient, GLfloat& dp, GLfloat lightIntensity = 1.0f) {

	color ans = { 0.0f,0.0f,0.0f };
	
	ans.r += r * ambient.r;
	ans.g += g * ambient.g;
	ans.b += b * ambient.b;

	ans.r += r * lightColor.r * dp;
	ans.g += g * lightColor.g * dp;
	ans.b += b * lightColor.b * dp;

	ans.r += r * lightColor.r * dp;
	ans.g += g * lightColor.g * dp;
	ans.b += b * lightColor.b * dp;

	ans.r *= lightIntensity;
	ans.g *= lightIntensity;
	ans.b *= lightIntensity;

	r = ans.r;
	g = ans.g;
	b = ans.b;

}