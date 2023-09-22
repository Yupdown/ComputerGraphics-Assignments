#include <iostream>
#include <math.h>
#include <gl/glew.h>
#include <glm/glm.hpp>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>

glm::vec3 colors[5] =
{
	{ 1.0f, 1.0f, 0.0f },
	{ 0.0f, 1.0f, 0.0f },
	{ 0.0f, 1.0f, 1.0f },
	{ 0.0f, 0.0f, 1.0f },
	{ 1.0f, 0.0f, 1.0f }
};
glm::vec2 positions[5];
int active_time[5];

time_t e_time = 0;

GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);
GLvoid Mouse(int button, int state, int x, int y);
GLvoid Timer(int delta);

void DrawFragments(glm::vec2 pos, int time, glm::vec3 color, int mode);
void DrawSquare(glm::vec2 pos, float radius, glm::vec3 color);
void Initialize();

void main(int argc, char** argv)
{
	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(600, 600);

	glutCreateWindow("OpenGLExperiment");
	// glutFullScreen();

	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
	{
		std::cerr << "Unable to initialize GLEW" << std::endl;
		exit(EXIT_FAILURE);
	}
	else
		std::cout << "GLEW Initialization" << std::endl;

	Initialize();

	glutDisplayFunc(drawScene);
	glutReshapeFunc(Reshape);
	glutMouseFunc(Mouse);
	glutTimerFunc(0, Timer, 0);
	glutMainLoop();
}

void Initialize()
{
	memset(active_time, -1, sizeof(active_time));
	for (int i = 0; i < 5; ++i)
		positions[i] = glm::vec2((float)(rand() % 2000 - 1000) / 1000, (float)(rand() % 2000 - 1000) / 1000);
}

glm::vec2 screen_to_world(int x, int y)
{
	int ww, wh;
	ww = glutGet(GLUT_WINDOW_WIDTH);
	wh = glutGet(GLUT_WINDOW_HEIGHT);
	float xp = static_cast<float>(x * 2) / ww - 1.0f;
	float yp = static_cast<float>(y * 2) / wh - 1.0f;
	return glm::vec2(xp, -yp);
}

void DrawFragments(glm::vec2 pos, int time, glm::vec3 color, int mode)
{
	float radius = std::max(0.1f - time * 0.001f, 0.0f);
	float dist = sqrt(time) * 0.1f;

	if (mode & 1)
	{
		DrawSquare(pos + glm::vec2(-dist, -dist), radius, color);
		DrawSquare(pos + glm::vec2(-dist, dist), radius, color);
		DrawSquare(pos + glm::vec2(dist, -dist), radius, color);
		DrawSquare(pos + glm::vec2(dist, dist), radius, color);
	}
	if (mode & 2)
	{
		DrawSquare(pos + glm::vec2(-dist, 0.0f), radius, color);
		DrawSquare(pos + glm::vec2(dist, 0.0f), radius, color);
		DrawSquare(pos + glm::vec2(0.0f, -dist), radius, color);
		DrawSquare(pos + glm::vec2(0.0f, dist), radius, color);
	}
}

GLvoid drawScene()
{
	glClearColor(0.22f, 0.22f, 0.25f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	for (int i = 0; i < 5; ++i)
	{
		if (active_time[i] < 0)
			DrawSquare(positions[i], 0.1f, colors[i % 5]);
		else
			DrawFragments(positions[i], e_time - active_time[i], colors[i % 5], i % 3 + 1);
	}

	glutSwapBuffers();
}

GLvoid Reshape(int w, int h)
{
	glViewport(0, 0, w, h);
}

GLvoid Mouse(int button, int state, int x, int y)
{
	glm::vec2 mouse_position = screen_to_world(x, y);
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		for (int i = 0; i < 5; ++i)
		{
			glm::vec2 dv = mouse_position - positions[i];
			if (abs(dv.x) < 0.1f && abs(dv.y) < 0.1f)
				active_time[i] = e_time;
		}
	}
	glutPostRedisplay();
}

GLvoid Timer(int delta)
{
	e_time += 1;

	for (int i = 0; i < 5; ++i)
	{
		if (active_time[i] >= 0)
			continue;

		float x = positions[i].x;
		float y = positions[i].y;

		float d0 = x + 1.0f;
		float d1 = y + 1.0f;
		float d2 = x - 1.0f;
		float d3 = y - 1.0f;
		float force = 0.001f;

		positions[i] += glm::vec2(1.0f, 0.0f) / (d0 * d0) * force;
		positions[i] += glm::vec2(0.0f, 1.0f) / (d1 * d1) * force;
		positions[i] += glm::vec2(-1.0f, 0.0f) / (d2 * d2) * force;
		positions[i] += glm::vec2(0.0f, -1.0f) / (d3 * d3) * force;

		for (int j = i + 1; j < 5; ++j)
		{
			if (active_time[i] >= 0)
				continue;

			glm::vec2 dv = positions[i] - positions[j];
			float sqrmag = dv.x * dv.x + dv.y * dv.y;
			glm::vec2 dvn = dv / sqrt(sqrmag);

			positions[j] -= dvn / sqrmag * force;
			positions[i] += dvn / sqrmag * force;
		}
	}

	glutPostRedisplay();
	glutTimerFunc(10, Timer, 0);
}

void DrawSquare(glm::vec2 pos, float radius, glm::vec3 color)
{
	float x0 = pos.x - radius;
	float x1 = pos.x + radius;
	float y0 = pos.y + radius;
	float y1 = pos.y - radius;

	glColor3f(color.r, color.g, color.b);
	glRectf(x0, y0, x1, y1);
}