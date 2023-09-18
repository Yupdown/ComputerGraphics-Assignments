#include <iostream>
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>

int win_id;
GLclampf rgb[3] = { 1.0f, 1.0f, 1.0f };
int crgb = 0;
int timer = 0;

GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);
GLvoid Keyboard(unsigned char key, int x, int y);
GLvoid Timer(int delta);

void main(int argc, char** argv)
{
	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(800, 600);

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
	glutReshapeFunc(Reshape);
	glutKeyboardFunc(Keyboard);
	glutTimerFunc(0, Timer, 0);
	glutMainLoop();
}

GLvoid drawScene()
{
	glClearColor(rgb[0], rgb[1], rgb[2], 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glutSwapBuffers();
}

GLvoid Reshape(int w, int h)
{
	glViewport(0, 0, w, h);
}

void SelectColor(int index)
{
	crgb = index;
	switch (index)
	{
	case 0:
		rgb[0] = 1.0f;
		rgb[1] = 1.0f;
		rgb[2] = 1.0f;
		break;
	case 1:
		rgb[0] = 0.0f;
		rgb[1] = 0.0f;
		rgb[2] = 0.0f;
		break;
	case 2:
		rgb[0] = 0.0f;
		rgb[1] = 1.0f;
		rgb[2] = 1.0f;
		break;
	case 3:
		rgb[0] = 1.0f;
		rgb[1] = 0.0f;
		rgb[2] = 1.0f;
		break;
	case 4:
		rgb[0] = 1.0f;
		rgb[1] = 1.0f;
		rgb[2] = 0.0f;
		break;
	}
}

GLvoid Keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 'c':
		timer = 0;
		SelectColor(2);
		break;
	case 'm':
		timer = 0;
		SelectColor(3);
		break;
	case 'y':
		timer = 0;
		SelectColor(4);
		break;
	case 'a':
		timer = 0;
		SelectColor((crgb + rand()) % 5);
		break;
	case 'w':
		timer = 0;
		SelectColor(0);
		break;
	case 'k':
		timer = 0;
		SelectColor(1);
		break;
	case 't':
		timer = 1;
		break;
	case 's':
		timer = 0;
		break;
	case 'q':
		glutDestroyWindow(win_id);
		return;
	}
	glutPostRedisplay();
}

GLvoid Timer(int delta)
{
	if (timer > 0)
		SelectColor((crgb + 1) % 5);
	glutTimerFunc(1000 / 5, Timer, 0);
	glutPostRedisplay();
}