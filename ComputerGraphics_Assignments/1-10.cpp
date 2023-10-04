#include <vector>
#include <stdio.h>
#include <iostream>

#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>

glm::vec3 colors[6] =
{
	{ 0.5f, 0.5f, 0.5f },
	{ 1.0f, 1.0f, 0.0f },
	{ 0.0f, 1.0f, 0.0f },
	{ 0.0f, 1.0f, 1.0f },
	{ 0.0f, 0.0f, 1.0f },
	{ 1.0f, 0.0f, 1.0f }
};
glm::vec2 positions[5];
bool line = false;
int count = 1;

time_t e_time = 0;

GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);
GLvoid Mouse(int button, int state, int x, int y);
GLvoid Keyboard(unsigned char key, int x, int y);
GLvoid Timer();

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

	for (int i = 1; i < 5; ++i)
		positions[i] = glm::vec2((float)(rand() % 2000 - 1000) / 1000, (float)(rand() % 2000 - 1000) / 1000);

	glutDisplayFunc(drawScene);
	glutReshapeFunc(Reshape);
	glutKeyboardFunc(Keyboard);
	glutMouseFunc(Mouse);
	glutIdleFunc(Timer);
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
	glm::vec3 col = colors[count - 1] * 0.25f;
	glClearColor(col.r, col.g, col.b, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	
	for (int c = 0; c < count; ++c)
	{
		glBegin(line ? GL_LINES : GL_POINTS);
		bool initial = true;
		for (float l = -4.0f + e_time % 20 * 0.005f; l < 4.0f; l += 0.1f)
		{
			float x = 0.0f;
			float y = 0.0f;
			float xp = l;
			float yp = pow(l, 3.0f);

			for (float i = 0.0f; i < 24.0f; i += 1.0f)
			{
				x += xp / (i * 4.0f + 1.5f + cos(e_time * 0.005f) * 0.25f);
				y += yp / (i * 4.0f + 3.5f + sin(e_time * 0.005f) * 0.25f);

				xp *= -pow(l, 4.0f) / (i * 2.0f + 1.0f) / (i * 2.0f + 2.0f);
				yp *= -pow(l, 4.0f) / (i * 2.0f + 2.0f) / (i * 2.0f + 3.0f);
			}

			for (int i = 0; i < (initial ? 1 : 2); ++i)
				glVertex2f(positions[c].x + x * 0.25f, positions[c].y + y * 0.25f);
			initial = false;
		}
		glEnd();
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
		count = std::min(count + 1, 5);
	glutPostRedisplay();
}

GLvoid Timer()
{
	e_time = clock();
	glutPostRedisplay();
}

GLvoid Keyboard(unsigned char key, int x, int y)
{
	switch (toupper(key))
	{
	case 'P':
		line = false;
		break;
	case 'L':
		line = true;
		break;
	case '1':
		count = 1;
		break;
	case '2':
		count = 2;
		break;
	case '3':
		count = 3;
		break;
	case '4':
		count = 4;
		break;
	case '5':
		count = 5;
		break;
	}
	glutPostRedisplay();
}