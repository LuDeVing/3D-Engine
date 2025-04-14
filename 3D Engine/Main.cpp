#include <Windows.h>
#include <GL\glew.h>
#include <GL\freeglut.h>
#include <iostream>
#include <string>
#include <algorithm>
#include <vector>
#include <fstream>
#include <strstream>
#include <chrono>
#include <stdlib.h>
#include <openservice.h>
#include <cstdlib>
#include "lighting.h"
#include "translateAndNormalise.h"
#include "cimg.h"

// using cimg opengl glut glew windows.h

using namespace std;
using namespace std::chrono;
using namespace cimg_library;

# define N_PI 3.14159265358979323846f

point lookdir = { 0.0f, 0.0f, 1.0f }, vup = { 0.0f, 1.0f, 0.0f }, vtar = { 0, 0, 1 };
point camera = { 0, 0, 0 }, light_direction = { 0.0f, 1.0f, 0.0f };

GLfloat a, f, q, liglen, x1r, y1r, x2r, y2r, x3r, y3r,
dp = 0.0f, addx = 0, addy, addz, cameraspeed = 0.5f, turn = 0.0f, frametime = 0.0f,
rotation = 0.0f, fTheta = 0, movementSpeed = 15.0f, turnSpeed = 1.5f,
deltaTime = 1.0f / 62.0f, screenStepX, screenStepY, v1, v2, v3, u1, u2, u3, w1, w2, w3;

GLfloat deltaAddy = 0, deltaAddz = 0;

color lightColor = { 0.225f, 0.225f, 0.225f },
ambient = { 0.55, 0.55, 0.45 }, cl = { 1,1,1 },
colprev = { 0,0,0 };

constexpr GLfloat screenWidth = 640, screenHeight = 360;

GLuint textureID;
DisplayImage displayImage;
unsigned int upscale = 2;

bool objectHasTextures = false;
UINT32 frame = 0;
int page = 0;

bool AddWaterFilter = false;

vector <bool> textobjs;
CImg<float> texture;

constexpr int threadSize = 16;

object obj;
mesh m;

constexpr int swtshcnstexpr = screenWidth * screenHeight;
GLfloat DeftBuffer[swtshcnstexpr];

GLfloat* texCols = nullptr;

void render(void);
void timer_callback(int);

//helper functions

point rotateAroundY(point p, GLfloat angle) {

	point ans = p;

	ans.z = p.z * cosf(angle) - p.x * sinf(angle);
	ans.x = p.z * sinf(angle) + p.x * cosf(angle);

	return ans;

}

point rotateAroundZ(point p, GLfloat angle) {
	point ans = p;

	ans.x = p.x * cosf(angle) - p.y * sinf(angle);
	ans.y = p.x * sinf(angle) + p.y * cosf(angle);
	
	return ans;
}

point Matrix_MultiplyVector(mat m, point i) {

	point v;

	v.x = i.x * m.m[0][0] + i.y * m.m[1][0] + i.z * m.m[2][0];
	v.y = i.x * m.m[0][1] + i.y * m.m[1][1] + i.z * m.m[2][1];
	v.z = i.x * m.m[0][2] + i.y * m.m[1][2] + i.z * m.m[2][2];

	return v;

}

//draw frame
void drawMesh(vector <triangle>& sorted, GLfloat ScreenWidth, GLfloat ScreenHeight, bool textures = false) {

	int norp = 0, losttri = 0, triNum = 0;
	GLfloat drawTime = 0.0f;

	for (auto& tritoclip : sorted) {

		triangle clipped[2];
		list <triangle> listtri;

		listtri.push_back(tritoclip);
		int newtri = 1;

		for (unsigned int t = 0; t < 4; t++) {

			int tristoadd = 0;

			while (newtri > 0) {

				triangle curnt = listtri.front();
				listtri.pop_front();
				newtri--;

				switch (t) {
				case 0:	tristoadd = triangleClipping({ 0.0f, -1.0f, 0.0f }, { 0.0f, 1.0f, 0.0f }, curnt, clipped[0], clipped[1]); break;
				case 1: tristoadd = triangleClipping({ 0.0f, 1.0f, 0.0f }, { 0.0f, -1.0f, 0.0f }, curnt, clipped[0], clipped[1]); break;
				case 2:	tristoadd = triangleClipping({ -1.0f, 0.0f, 0.0f }, { 1.0f, 0.0f, 0.0f }, curnt, clipped[0], clipped[1]); break;
				case 3:	tristoadd = triangleClipping({ 1.0f, 0.0f, 0.0f }, { -1.0f, 0.0f, 0.0f }, curnt, clipped[0], clipped[1]); break;
				}

				if (tristoadd == 0) {
					losttri++;
				}
				if (tristoadd == 1) {
					listtri.push_back(clipped[0]);
					//cout << clipped[0].t[0].u << " " << clipped[0].t[1].u << " " << clipped[0].t[2].u << endl << endl;
				}
				else if (tristoadd == 2) {
					listtri.push_back(clipped[0]);
					listtri.push_back(clipped[1]);
				}
			}

			newtri = listtri.size();
		}

		triNum += listtri.size();

		auto start = high_resolution_clock::now();

		for (auto& i : listtri) {

			dp = i.normal.x * light_direction.x + i.normal.y * light_direction.y + i.normal.z * light_direction.z;

			norp++;

			objectHasTextures = textures;
			x1r = i.p[0].x;
			y1r = i.p[0].y;
			x2r = i.p[1].x;
			y2r = i.p[1].y;
			x3r = i.p[2].x;
			y3r = i.p[2].y;
			u1 = i.t[0].u;
			v1 = i.t[0].v;
			w1 = i.t[0].w;
			u2 = i.t[1].u;
			v2 = i.t[1].v;
			w2 = i.t[1].w;
			u3 = i.t[2].u;
			v3 = i.t[2].v;
			w3 = i.t[2].w;
			cl = i.color;

			render();
		}

		auto stop = high_resolution_clock::now();

		auto duration = duration_cast<milliseconds>(stop - start);

		drawTime += duration.count();

	}

}

void DrawImage() {

	glClear(GL_COLOR_BUFFER_BIT);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, displayImage.width, displayImage.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, displayImage.imageData);

	GLuint fboId = 0;
	glGenFramebuffers(1, &fboId);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, fboId);
	glFramebufferTexture2D(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
		GL_TEXTURE_2D, textureID, 0);

	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	glBlitFramebuffer(0, 0, screenWidth, screenHeight, 0, 0,
		screenWidth * upscale, screenHeight * upscale, GL_COLOR_BUFFER_BIT, GL_NEAREST);

	glFlush();

}

//fix points

void getInputs() {

	deltaAddz = 0;
	deltaAddy = 0;

	if (GetAsyncKeyState(0x57)) {
		addz += movementSpeed * deltaTime;
		deltaAddz = movementSpeed * deltaTime;
	}
	if (GetAsyncKeyState(0x53)) {
		addz -= movementSpeed * deltaTime;
		deltaAddz = -movementSpeed * deltaTime;
	}
	if (GetAsyncKeyState(VK_SPACE)) {
		addy += movementSpeed * deltaTime;
		deltaAddy = movementSpeed * deltaTime;
	}
	if (GetAsyncKeyState(0x43)) {
		addy -= movementSpeed * deltaTime;
		deltaAddy = -movementSpeed * deltaTime;
	}
	
	if (GetAsyncKeyState(0x41)) {
		turn -= turnSpeed * deltaTime;
	}
	if (GetAsyncKeyState(0x44)) {
		turn += turnSpeed * deltaTime;
	}

	if (GetAsyncKeyState(VK_ADD)) {
		movementSpeed += 0.5f;
	}
	if (GetAsyncKeyState(VK_SUBTRACT)) {
		movementSpeed -= 0.5f;
	}
	if (GetAsyncKeyState(VK_F1)) {
		movementSpeed *= 1.2f;
	}
	if (GetAsyncKeyState(VK_F2)) {
		movementSpeed /= 1.2f;
	}
	if (GetAsyncKeyState(VK_F3)) {
		turnSpeed *= 1.2f;
	}
	if (GetAsyncKeyState(VK_F4)) {
		turnSpeed /= 1.2f;
	}

	movementSpeed = max(movementSpeed, 0);

}

void fixRotationAndPosition() {

	point nb = { 0, 0, deltaAddz };

	lookdir = { 0.0f, 0.0f, 1.0f };
	vup = { 0.0f, 1.0f, 0.0f };
	vtar = { 0, 0, 1 };
	light_direction = { 0.0f, 1.0f, 0.0f };

	camera = vecAdd(camera, rotateAroundY(nb, turn));
	camera.y += deltaAddy;

	lookdir = rotateAroundY(vtar, turn);
	
	vtar = vecAdd(lookdir, camera);

	light_direction = rotateAroundY(light_direction, -turn);
	// light_direction = rotateAroundZ(light_direction, Zturn);

	//std::cout << camera.x << ' ' << camera.z << '\n';

	//addx = 0; addy = 0; addz = 0; turn = 0;

}

//fix points
void makeFrame() {

	auto start = high_resolution_clock::now();

	frame++;

	getInputs();
	fixRotationAndPosition();

	for (int k = 0; k < screenWidth * screenHeight; k++)
		DeftBuffer[k] = 0.0f;

	for (int k = 0; k < displayImage.height; k++)
		for (int j = 0; j < displayImage.width; j++)
			displayImage.Draw(j, k, Pixel(8, 36, 90));


	vector <triangle> tries = obj.mn(frame, m, screenWidth, screenHeight,
		camera, lookdir, vup, vtar, lookdir);

	drawMesh(tries, screenWidth, screenHeight, true);

	DrawImage();
	glutSwapBuffers();

	//	glClearColor(0.03f, 0.139f, 0.350f, 1.0f);
	//	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	auto stop = high_resolution_clock::now();

	auto duration = duration_cast<milliseconds>(stop - start);
	frametime = (GLfloat)(duration.count());
	int framerate = min(62, round(1000.0 / frametime));

	deltaTime = 1.0f / min(62.0f, framerate);

	char windowName[] = "Legendary 3D Engine     fps";
	char fps[3];
	if (framerate / 100 > 0) {
		fps[0] = framerate / 100 + '0';
	}
	else fps[0] = ' ';
	if (framerate / 10 % 10 != 0 || framerate / 100 != 0) {
		fps[1] = (framerate / 10 % 10) + '0';
	}
	else fps[1] = ' ';

	fps[2] = (framerate % 10) + '0';

	windowName[21] = fps[0];
	windowName[22] = fps[1];
	windowName[23] = fps[2];

	glutSetWindowTitle(windowName);

}

void createOpenGLTexture() {

	glGenTextures(1, &textureID);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

}

void hideConsole() {
	HWND Stealth;
	AllocConsole();
	Stealth = FindWindowA("ConsoleWindowClass", NULL);
	ShowWindow(Stealth, 0);
}

int main(int argc, char* argv[]) {

	short int mapNum;

	cout << "You can push + to speed up, or - to slow down the movement speed.\nf1 is to increase speed by a lot and f2, to slow down a lot.";
	cout << "\nf3(add) and f4(subtruct) are for sensitivity speed.\nThose are the instructions, start exploring!\n";
	cout << "Choose map 1, 2 or 3: ";
	cml:cin >> mapNum;

	GLfloat num = 0.0f;

	screenStepX = 2.0f / screenWidth;
	screenStepY = 2.0f / screenHeight;

	if (mapNum == 1) {

		m.LoadFromObjectFile("meshes/Artisans Hub.txt", true, true);
		textobjs.push_back(true);

		string filePath = "textures/Artisans Hub.bmp";
		texture.load(filePath.c_str());

	}
	else if (mapNum == 2) {

		m.LoadFromObjectFile("meshes/Autumn Plains.txt", true, true);
		textobjs.push_back(true);

		m.RotateMesh('X', -90.0f * (N_PI / 180.0f));
		m.TranslateMesh(-5468, -2531, 10070);

		string filePath = "textures/Autumn Plains.bmp";
		texture.load(filePath.c_str());

	}
	else if (mapNum == 3) {

		m.LoadFromObjectFile("meshes/summer forest.txt", true, true);
		textobjs.push_back(true);

		m.RotateMesh('X', -90.0f * (N_PI / 180.0f));
		m.TranslateMesh(-2576.28, -2515.68, 6204.92);

		string filePath = "textures/summer forest.bmp";
		texture.load(filePath.c_str());

	}
	else {
		cout << "choose a valid number: ";
		goto cml;
	}

	texCols = new GLfloat[texture.width() * texture.height() * 3];

	for (int y = 0; y < texture.height(); y++) {
		for (int x = 0; x < texture.width(); x++) {

			int colCord = (texture.width() * y + x) * 3;

			texCols[colCord + 0] = texture(x, y, 0, 0);
			texCols[colCord + 1] = texture(x, y, 0, 1);
			texCols[colCord + 2] = texture(x, y, 0, 2);

		}
	}

	// hideConsole();

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	int scw = (int)(screenWidth), sch = (int)(screenHeight);
	glutInitWindowSize(scw * upscale, sch * upscale);
	glutCreateWindow("Legendary 3D Engine");

	glewInit();

	createOpenGLTexture();
	displayImage = DisplayImage(screenWidth, screenHeight);

	glutDisplayFunc(render);
	glutTimerFunc(5, timer_callback, 0);
	GLenum err = glewInit();

	if (GLEW_OK != err) {
		fprintf(stderr, "GLEW error");
		return 1;
	}

	glutMainLoop();

	return 0;
}

void rasterize(
	int yVal, int x1, int y1, int x2, int y2,
	GLfloat d12, GLfloat d13,
	GLfloat v12, GLfloat v13,
	GLfloat u12, GLfloat u13,
	GLfloat w12, GLfloat w13,
	GLfloat d23, GLfloat u23,
	GLfloat v23, GLfloat w23,
	int swgt, int shgt,
	bool rev = false) {

	int sw = swgt;
	int sh = shgt;

	int sw2 = sw / 2;
	int sh2 = sh / 2;

	int tw1 = texture.width() - 1;
	int th1 = texture.height() - 1;

	int lcnst = (sh2 + yVal) * sw + sw2;
	int swtsh1 = sw * sh - 1;
	int ysh2 = yVal + sh2;

	int twidth = texture.width();
	int theight = texture.height();

	if (!rev) {

		int bx, ex;
		GLfloat bu, bv, eu, ev, bw, ew;

		bx = (int)(x1 + (yVal - y1) * d12);
		ex = (int)(x1 + (yVal - y1) * d13);

		bu = u1 + (yVal - y1) * u12;
		eu = u1 + (yVal - y1) * u13;

		bv = v1 + (yVal - y1) * v12;
		ev = v1 + (yVal - y1) * v13;

		bw = w1 + (yVal - y1) * w12;
		ew = w1 + (yVal - y1) * w13;

		int twidth = texture.width();
		int theight = texture.height();

		if (bx > ex) {
			swap(bx, ex);
			swap(bv, ev);
			swap(bu, eu);
			swap(bw, ew);
		}

		GLfloat t = 0.0f, tex_u = bu, tex_v = bv, tex_w = bw;
		GLfloat tstep = 1.0f / ((GLfloat)(ex - bx));

		GLfloat red;
		GLfloat green;
		GLfloat blue;

		for (int xVal = bx; xVal <= ex; xVal++) {

			tex_u = (1.0f - t) * bu + t * eu;
			tex_v = (1.0f - t) * bv + t * ev;
			tex_w = (1.0f - t) * bw + t * ew;

			int uval = tex_u / tex_w * twidth;
			int vval = tex_v / tex_w * theight;

			int colCord = (maxGLfloat(0, minGLfloat(vval + 1.0f, th1)) * twidth + maxGLfloat(0, minGLfloat(uval + 1.0f, tw1))) * 3.0f;

			red = texCols[colCord + 0];
			green = texCols[colCord + 1];
			blue = texCols[colCord + 2];

			lightValue(lightColor, red, green, blue, ambient, dp);

			int deftp = maxInt(0, minInt((lcnst + xVal), swtsh1));

			if (tex_w > DeftBuffer[deftp]) {

				displayImage.Draw(xVal - sw2, ysh2, Pixel(red, green, blue));
				DeftBuffer[deftp] = tex_w;

			}

			t += tstep;

		}

		return;

	}

	int bx, ex;
	GLfloat bu, bv, eu, ev, bw, ew;

	bx = (int)(x2 + (yVal - y2) * d23);
	ex = (int)(x1 + (yVal - y1) * d13);

	bu = u2 + (yVal - y2) * u23;
	eu = u1 + (yVal - y1) * u13;

	bv = v2 + (yVal - y2) * v23;
	ev = v1 + (yVal - y1) * v13;

	bw = w2 + (yVal - y2) * w23;
	ew = w1 + (yVal - y1) * w13;

	if (bx > ex) {
		swap(bx, ex);
		swap(bv, ev);
		swap(bu, eu);
		swap(bw, ew);
	}

	GLfloat t = 0.0f, tex_u = bu, tex_v = bv, tex_w = bw;
	GLfloat tstep = 1.0f / ((GLfloat)(ex - bx));

	GLfloat red;
	GLfloat green;
	GLfloat blue;

	for (int xVal = bx; xVal <= ex; xVal++) {

		tex_u = (1.0f - t) * bu + t * eu;
		tex_v = (1.0f - t) * bv + t * ev;
		tex_w = (1.0f - t) * bw + t * ew;

		int uval = tex_u / tex_w * twidth;
		int vval = tex_v / tex_w * theight;

		int colCord = (maxGLfloat(0, minGLfloat(vval + 1.0f, th1)) * twidth + maxGLfloat(0, minGLfloat(uval + 1.0f, tw1))) * 3.0f;

		red = texCols[colCord + 0];
		green = texCols[colCord + 1];
		blue = texCols[colCord + 2];

		lightValue(lightColor, red, green, blue, ambient, dp);

		int deftp = maxInt(0, minInt((lcnst + xVal), swtsh1));

		if (tex_w > DeftBuffer[deftp]) {

			displayImage.Draw(xVal - sw2, ysh2, Pixel(red, green, blue));
			DeftBuffer[deftp] = tex_w;

		}

		t += tstep;
	}

}

void render() {

	color clr = cl;
	lightValue(lightColor, clr.r, clr.g, clr.b, ambient, dp);

	if (objectHasTextures) {

		if (y2r < y1r) {
			swap(y2r, y1r);
			swap(x2r, x1r);
			swap(u2, u1);
			swap(v2, v1);
			swap(w2, w1);
		}

		if (y3r < y1r) {
			swap(y3r, y1r);
			swap(x3r, x1r);
			swap(u3, u1);
			swap(v3, v1);
			swap(w3, w1);
		}

		if (y3r < y2r) {
			swap(y2r, y3r);
			swap(x2r, x3r);
			swap(u2, u3);
			swap(v2, v3);
			swap(w2, w3);
		}

		int x1, x2, x3, y1, y2, y3;

		x1 = (int)floor(x1r / screenStepX);
		x2 = (int)floor(x2r / screenStepX);
		x3 = (int)floor(x3r / screenStepX);

		y1 = (int)floor(y1r / screenStepY);
		y2 = (int)floor(y2r / screenStepY);
		y3 = (int)floor(y3r / screenStepY);

		GLfloat d12, d13, d23, u12, v12, w12, u13, v13, w13, u23, v23, w23;

		if (y2 - y1 != 0) {
			d12 = (GLfloat)(x2 - x1) / (GLfloat)(y2 - y1);
			u12 = (GLfloat)(u2 - u1) / (GLfloat)(y2 - y1);
			v12 = (GLfloat)(v2 - v1) / (GLfloat)(y2 - y1);
			w12 = (GLfloat)(w2 - w1) / (GLfloat)(y2 - y1);
		}
		else d12 = u12 = v12 = w12 = 0.0f;
		if (y3 - y1 != 0) {
			d13 = (GLfloat)(x3 - x1) / (GLfloat)(y3 - y1);
			u13 = (GLfloat)(u3 - u1) / (GLfloat)(y3 - y1);
			v13 = (GLfloat)(v3 - v1) / (GLfloat)(y3 - y1);
			w13 = (GLfloat)(w3 - w1) / (GLfloat)(y3 - y1);
		}
		else d13 = u13 = v13 = w13 = 0.0f;
		if (y3 - y2 != 0) {
			d23 = (GLfloat)(x3 - x2) / (GLfloat)(y3 - y2);
			u23 = (GLfloat)(u3 - u2) / (GLfloat)(y3 - y2);
			v23 = (GLfloat)(v3 - v2) / (GLfloat)(y3 - y2);
			w23 = (GLfloat)(w3 - w2) / (GLfloat)(y3 - y2);
		}
		else d23 = u23 = v23 = w23 = 0.0f;

		for (int yVal = y1; yVal <= y2; yVal++) {
			rasterize(yVal, x1, y1, x2, y2,
				d12, d13,
				v12, v13,
				u12, u13,
				w12, w13,
				d23, u23,
				v23, w23,
				screenWidth, screenHeight);
		}

		for (int yVal = y2 + 1; yVal <= y3; yVal++) {
			rasterize(yVal, x1, y1, x2, y2,
				d12, d13,
				v12, v13,
				u12, u13,
				w12, w13,
				d23, u23,
				v23, w23,
				screenWidth, screenHeight,
				true);
		}

	}
	else {
		glBegin(GL_TRIANGLES);
		glColor3f(clr.r, clr.g, clr.b); glVertex2f(x1r, y1r);
		glColor3f(clr.r, clr.g, clr.b); glVertex2f(x2r, y2r);
		glColor3f(clr.r, clr.g, clr.b); glVertex2f(x3r, y3r);
		glEnd();
	}

}

void timer_callback(int) {
	makeFrame();
	glutTimerFunc(max(0, 16 - frametime), timer_callback, 0);
}
