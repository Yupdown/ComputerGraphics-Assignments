#include <iostream>
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>

struct ColorRGB
{
	float r, g, b;
};

ColorRGB colors[4][2];
float offsets[4];

GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);
GLvoid Mouse(int button, int state, int x, int y);

void main(int argc, char** argv)
{
	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(640, 480);

	glutCreateWindow("OpenGLExperiment");

	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
	{
		std::cerr << "Unable to initialize GLEW" << std::endl;
		exit(EXIT_FAILURE);
	}
	else
		std::cout << "GLEW Initialization" << std::endl;

	for (int i = 0; i < 4; ++i)
	{
		colors[i][0].r = rand() % 1000 / 1000.0f;
		colors[i][0].g = rand() % 1000 / 1000.0f;
		colors[i][0].b = rand() % 1000 / 1000.0f;
		colors[i][1].r = rand() % 1000 / 1000.0f;
		colors[i][1].g = rand() % 1000 / 1000.0f;
		colors[i][1].b = rand() % 1000 / 1000.0f;
		offsets[i] = 0.0f;
	}

	glutDisplayFunc(drawScene);
	glutReshapeFunc(Reshape);
	glutMouseFunc(Mouse);
	glutMainLoop();
}

GLvoid drawScene()
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	for (int i = 0; i < 4; ++i)
	{
		float x0 = i % 2 - 1;
		float x1 = i % 2;
		float y0 = i / 2 - 1;
		float y1 = i / 2;

		glColor3f(colors[i][0].r, colors[i][0].g, colors[i][0].b);
		glRectf(x0, y0, x1, y1);
		glColor3f(colors[i][1].r, colors[i][1].g, colors[i][1].b);
		glRectf(x0 + offsets[i], y0 + offsets[i], x1 - offsets[i], y1 - offsets[i]);
	}

	glutSwapBuffers();
}

GLvoid Reshape(int w, int h)
{
	glViewport(0, 0, w, h);
}

GLvoid Mouse(int button, int state, int x, int y)
{
	if (state == GLUT_DOWN)
	{
		int ww, wh;
		ww = glutGet(GLUT_WINDOW_WIDTH);
		wh = glutGet(GLUT_WINDOW_HEIGHT);
		float xp = static_cast<float>(x) / ww * 2.0f - 1.0f;
		float yp = static_cast<float>(y) / wh * 2.0f - 1.0f;
		int index = (xp > 0) + (yp < 0) * 2;

		if (abs(xp) > offsets[index] && abs(xp) < 1.0f - offsets[index] && abs(yp) > offsets[index] && abs(yp) < 1.0f - offsets[index])
		{
			if (button == GLUT_LEFT_BUTTON)
			{
				colors[index][1].r = rand() % 1000 / 1000.0f;
				colors[index][1].g = rand() % 1000 / 1000.0f;
				colors[index][1].b = rand() % 1000 / 1000.0f;
			}
			else if (button == GLUT_RIGHT_BUTTON)
				offsets[index] += 0.1f;
		}
		else
		{
			if (button == GLUT_LEFT_BUTTON)
			{
				colors[index][0].r = rand() % 1000 / 1000.0f;
				colors[index][0].g = rand() % 1000 / 1000.0f;
				colors[index][0].b = rand() % 1000 / 1000.0f;
			}
			else if (button == GLUT_RIGHT_BUTTON)
				offsets[index] -= 0.1f;
		}
	}
}