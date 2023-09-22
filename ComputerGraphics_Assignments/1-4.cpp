#include <iostream>
#include <math.h>
#include <gl/glew.h>
#include <glm/glm.hpp>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>

int win_id;
glm::vec3 colors[5] =
{
	{ 1.0f, 1.0f, 0.0f },
	{ 0.0f, 1.0f, 0.0f },
	{ 0.0f, 1.0f, 1.0f },
	{ 0.0f, 0.0f, 1.0f },
	{ 1.0f, 0.0f, 1.0f }
};
glm::vec2 positions_origin[5];
glm::vec2 positions[5];
glm::vec2 directions[5];

int count = 1;
time_t e_time = 0;

int move_mode = 0;
bool toggle_scale = false;
bool toggle_color = false;

GLvoid drawScene(GLvoid);
GLvoid Keyboard(unsigned char key, int x, int y);
GLvoid Reshape(int w, int h);
GLvoid Mouse(int button, int state, int x, int y);
GLvoid MouseMotion(int x, int y);
GLvoid Timer(int delta);

void main(int argc, char** argv)
{
	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(600, 600);

	win_id = glutCreateWindow("OpenGLExperiment");
	// glutFullScreen();

	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
	{
		std::cerr << "Unable to initialize GLEW" << std::endl;
		exit(EXIT_FAILURE);
	}
	else
		std::cout << "GLEW Initialization" << std::endl;

	glutDisplayFunc(drawScene);
	glutKeyboardFunc(Keyboard);
	glutReshapeFunc(Reshape);
	glutMouseFunc(Mouse);
	glutMotionFunc(MouseMotion);
	glutTimerFunc(0, Timer, 0);
	glutMainLoop();
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
	glClearColor(0.15f, 0.15f, 0.15f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	for (int i = 0; i < count; ++i)
	{
		float r = 0.1f;
		if (toggle_scale)
			r = sin(i + e_time / 10.0f) * 0.05f + 0.1f;

		float x0 = positions[i].x - r;
		float x1 = positions[i].x + r;
		float y0 = positions[i].y + r;
		float y1 = positions[i].y - r;

		int ci = i;
		if (toggle_color)
			ci = (i + e_time / 10) % 5;

		glColor3f(colors[ci].r, colors[ci].g, colors[ci].b);
		glRectf(x0, y0, x1, y1);
	}

	glutSwapBuffers();
}

GLvoid Keyboard(unsigned char key, int x, int y)
{
	switch (toupper(key))
	{
	case 'A':
		if (move_mode == 1)
		{
			move_mode = 0;
			for (int i = 0; i < 5; ++i)
				directions[i] = glm::vec2(0.0f, 0.0f);
		}
		else
		{
			move_mode = 1;
			for (int i = 0; i < 5; ++i)
				directions[i] = glm::vec2(1.0f, 1.0f);
		}
		break;
	case 'I':
		if (move_mode == 2)
		{
			move_mode = 0;
			for (int i = 0; i < 5; ++i)
				directions[i] = glm::vec2(0.0f, 0.0f);
		}
		else
		{
			move_mode = 2;
			for (int i = 0; i < 5; ++i)
				directions[i] = glm::vec2(1.0f, 0.0f);
		}
		break;
	case 'C':
		toggle_scale = !toggle_scale;
		break;
	case 'O':
		toggle_color = !toggle_color;
		break;
	case 'S':
		move_mode = 0;
		toggle_scale = false;
		toggle_color = false;
		break;
	case 'M':
		for (int i = 0; i < count; ++i)
			positions[i] = positions_origin[i];
		break;
	case 'R':
		count = 1;
		break;
	case 'Q':
		glutDestroyWindow(win_id);
		return;
	}
	glutPostRedisplay();
}

GLvoid Reshape(int w, int h)
{
	glViewport(0, 0, w, h);
}

int drag_index = -1;

GLvoid Mouse(int button, int state, int x, int y)
{
	glm::vec2 pos = screen_to_world(x, y);

	if (state == GLUT_DOWN)
	{
		if (count < 5)
		{
			positions[count] = pos;
			positions_origin[count] = pos;
			count += 1;
		}
	}
	else if (state == GLUT_UP)
		drag_index = -1;

	glutPostRedisplay();
}

GLvoid MouseMotion(int x, int y)
{
	glm::vec2 pos = screen_to_world(x, y);

	if (drag_index >= 0)
		positions[drag_index] = pos;

	glutPostRedisplay();
}

GLvoid Timer(int delta)
{
	e_time += 1;

	for (int i = 0; i < count; ++i)
	{
		positions[i] += directions[i] * 0.01f;

		if (positions[i].x < -0.9f)
		{
			directions[i].x = -directions[i].x;
			positions[i].x = -1.8f - positions[i].x;
		}
		else if (positions[i].x > 0.9f)
		{
			directions[i].x = -directions[i].x;
			positions[i].x = 1.8f - positions[i].x;
		}
		if (positions[i].y < -0.9f)
		{
			directions[i].y = -directions[i].y;
			positions[i].y = -1.8f - positions[i].y;
		}
		else if (positions[i].y > 0.9f)
		{
			directions[i].y = -directions[i].y;
			positions[i].y = 1.8f - positions[i].y;
		}
	}

	glutPostRedisplay();
	glutTimerFunc(10, Timer, 0);
}