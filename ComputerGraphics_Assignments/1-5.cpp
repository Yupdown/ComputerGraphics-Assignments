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
glm::vec2 positions[40];
bool active[40];

glm::vec2 mouse_position;
bool mouse_active = false;
float mouse_size = 0.05f;
int mouse_color = -1;

time_t e_time = 0;

GLvoid drawScene(GLvoid);
GLvoid Keyboard(unsigned char key, int x, int y);
GLvoid Reshape(int w, int h);
GLvoid Mouse(int button, int state, int x, int y);
GLvoid MouseMotion(int x, int y);
GLvoid Timer(int delta);

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
	glutKeyboardFunc(Keyboard);
	glutReshapeFunc(Reshape);
	glutMouseFunc(Mouse);
	glutMotionFunc(MouseMotion);
	glutTimerFunc(0, Timer, 0);
	glutMainLoop();
}

void Initialize()
{
	memset(active, true, sizeof(active));
	for (int i = 0; i < 40; ++i)
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

GLvoid drawScene()
{
	glClearColor(0.22f, 0.22f, 0.25f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	for (int i = 0; i < 40; ++i)
	{
		if (active[i])
			DrawSquare(positions[i], 0.05f, colors[i % 5]);
	}
	if (mouse_active)
		DrawSquare(mouse_position, mouse_size, mouse_color < 0 ? glm::vec3(0.0f, 0.0f, 0.0f) : colors[mouse_color]);

	glutSwapBuffers();
}

GLvoid Keyboard(unsigned char key, int x, int y)
{
	switch (toupper(key))
	{
	case 'R':
		Initialize();
		break;
	}
	glutPostRedisplay();
}

GLvoid Reshape(int w, int h)
{
	glViewport(0, 0, w, h);
} 

GLvoid Mouse(int button, int state, int x, int y)
{
	mouse_position = screen_to_world(x, y);
	if (button == GLUT_LEFT_BUTTON)
	{
		if (state == GLUT_DOWN)
		{
			mouse_active = true;
			mouse_size = 0.05f;
			mouse_color = -1;
		}
		else if (state == GLUT_UP)
			mouse_active = false;
	}
	glutPostRedisplay();
}

GLvoid MouseMotion(int x, int y)
{
	if (mouse_active)
		mouse_position = screen_to_world(x, y);

	glutPostRedisplay();
}

GLvoid Timer(int delta)
{
	e_time += 1;

	for (int i = 0; i < 40; ++i)
	{
		if (!active[i])
			continue;

		float x = positions[i].x;
		float y = positions[i].y;

		float d0 = x + 1.0f;
		float d1 = y + 1.0f;
		float d2 = x - 1.0f;
		float d3 = y - 1.0f;
		float force = 0.0001f;

		positions[i] += glm::vec2(1.0f, 0.0f) / (d0 * d0) * force;
		positions[i] += glm::vec2(0.0f, 1.0f) / (d1 * d1) * force;
		positions[i] += glm::vec2(-1.0f, 0.0f) / (d2 * d2) * force;
		positions[i] += glm::vec2(0.0f, -1.0f) / (d3 * d3) * force;

		for (int j = i + 1; j < 40; ++j)
		{
			if (!active[i])
				continue;

			glm::vec2 dv = positions[i] - positions[j];
			float sqrmag = dv.x * dv.x + dv.y * dv.y;
			glm::vec2 dvn = dv / sqrt(sqrmag);

			positions[j] -= dvn / sqrmag * force;
			positions[i] += dvn / sqrmag * force;
		}

		if (mouse_active)
		{
			glm::vec2 dv = positions[i] - mouse_position;
			if (abs(dv.x) < 0.05f + mouse_size && abs(dv.y) < 0.05f + mouse_size)
			{
				mouse_color = i % 5;
				active[i] = false;
				mouse_size += 0.01f;
			}
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
