#include <iostream>
#include <gl/glew.h>
#include <glm/glm.hpp>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>

glm::uvec3 colors[5] =
{
	{ 1.0f, 1.0f, 0.0f },
	{ 0.0f, 1.0f, 0.0f },
	{ 0.0f, 1.0f, 1.0f },
	{ 0.0f, 0.0f, 1.0f },
	{ 1.0f, 0.0f, 1.0f }
};
glm::vec2 positions[5];
int count = 1;

GLvoid drawScene(GLvoid);
GLvoid Keyboard(unsigned char key, int x, int y);
GLvoid Reshape(int w, int h);
GLvoid Mouse(int button, int state, int x, int y);
GLvoid MouseMotion(int x, int y);

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

	glutDisplayFunc(drawScene);
	glutKeyboardFunc(Keyboard);
	glutReshapeFunc(Reshape);
	glutMouseFunc(Mouse);
	glutMotionFunc(MouseMotion);
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
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	for (int i = 0; i < count; ++i)
	{
		float x0 = positions[i].x - 0.1f;
		float x1 = positions[i].x + 0.1f;
		float y0 = positions[i].y + 0.1f;
		float y1 = positions[i].y - 0.1f;

		glColor3f(colors[i].r, colors[i].g, colors[i].b);
		glRectf(x0, y0, x1, y1);
	}

	glutSwapBuffers();
}

GLvoid Keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 'a':
		if (count < 5)
		{
			positions[count].x = (rand() % 1000 / 500.0f) - 1.0f;
			positions[count].y = (rand() % 1000 / 500.0f) - 1.0f;
			count += 1;
		}
		glutPostRedisplay();
		break;
	}
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
		for (int i = count - 1; i >= 0; --i)
		{
			if (pos.x > positions[i].x - 0.1f && pos.x < positions[i].x + 0.1f && pos.y > positions[i].y - 0.1f && pos.y < positions[i].y + 0.1f)
			{
				drag_index = i;
				break;
			}
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