#include <vector>
#include <stdio.h>
#include <iostream>

#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>

#include "OBJ_Loader.h"

std::vector<GLfloat> VERTEX_DATA;
std::vector<GLfloat> COLOR_DATA;
std::vector<GLuint> INDEX_DATA;

std::vector<std::pair<glm::vec3, int>> shapeData;
int currentShape;

GLvoid LoadPolygon(const char* fileName);
GLvoid InitBuffer();

GLuint MakeVertexShaders(const char* fileName);
GLuint MakeFragmentShaders(const char* fileName);
GLuint MakeShaderProgram(GLuint vertShader, GLuint fragShader);

GLvoid drawScene(GLvoid);
GLvoid Keyboard(unsigned char key, int x, int y);
GLvoid Reshape(int w, int h);
GLvoid Mouse(int button, int state, int x, int y);
GLvoid Timer(int delta);

char* FileToBuffer(const char* file);

GLuint shaderProgramID;
GLuint VAO, VBO[2], EBO;
glm::mat4 mProj = glm::mat4(1.0f);
glm::vec3 vPos = glm::vec3(0.0f);

int main(int argc, char** argv)
{
	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(640, 480);

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

	GLuint vert = MakeVertexShaders("vert.glsl");
	GLuint frag = MakeFragmentShaders("frag.glsl");
	shaderProgramID = MakeShaderProgram(vert, frag);
	glUseProgram(shaderProgramID);
	LoadPolygon("teapot.obj");
	InitBuffer();

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	glutDisplayFunc(drawScene);
	glutKeyboardFunc(Keyboard);
	glutReshapeFunc(Reshape);
	glutMouseFunc(Mouse);
	glutTimerFunc(0, Timer, 0);

	glutMainLoop();

	return 0;
}

GLvoid drawScene()
{
	clock_t c = clock();

	glClearColor(0.125f, 0.125f, 0.125f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(shaderProgramID);

	int i = 0;
	for (const auto& shape : shapeData)
	{
		glm::mat4 t = glm::translate(glm::mat4(1.0f), shape.first + vPos);
		glm::mat4 r = glm::rotate(glm::mat4(1.0f), glm::radians(sin(c * 0.01f + i++ * 0.25f) * 10.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		glm::mat4 s = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f));
		glm::mat4 transform = mProj * t * r * s;

		unsigned int modelLocation = glGetUniformLocation(shaderProgramID, "model_Transform");
		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(transform));

		glm::vec3 col = glm::vec3(i / 4 % 2, i / 2 % 2, i % 2);

		modelLocation = glGetUniformLocation(shaderProgramID, "model_Color");
		glUniform3f(modelLocation, col.r, col.b, col.g);

		switch (shape.second)
		{
		case 0:
			glBegin(GL_POINTS);
			glVertex2i(0.0f, 0.0f);
			glEnd();
			break;
		case 1:
			glBegin(GL_LINES);
			glVertex2f(-0.1f, -0.1f);
			glVertex2f(0.1f, 0.1f);
			glEnd();
			break;
		case 2:
		{
			GLfloat p1[3] = { -0.1, -0.1, 1 };
			GLfloat p2[3] = { 0.1, -0.1, 1 };
			GLfloat p3[3] = { 0.0, 0.1, 1 };
			glBegin(GL_POLYGON);
			glVertex3fv(p1);
			glVertex3fv(p2);
			glVertex3fv(p3);
			glEnd();
			break;
		}
		case 3:
		{
			GLfloat p1[3] = { -0.1, -0.1, 1 };
			GLfloat p2[3] = { 0.1, -0.1, 1 };
			GLfloat p3[3] = { 0.1, 0.1, 1 };
			GLfloat p4[3] = { -0.1, 0.1, 1 };
			glBegin(GL_POLYGON);
			glVertex3fv(p1);
			glVertex3fv(p2);
			glVertex3fv(p3);
			glVertex3fv(p4);
			glEnd();
			break;
		}
		}
	}

	glutSwapBuffers();
}

GLvoid Timer(int delta)
{
	glutPostRedisplay();
	glutTimerFunc(0, Timer, 0);
}

GLvoid Reshape(int w, int h)
{
	GLfloat aspect = (GLfloat)w / h;
	mProj = glm::ortho(-aspect, aspect, -1.0f, 1.0f, -1.0f, 1.0f);

	glViewport(0, 0, w, h);
}

GLvoid InitBuffer()
{
	glGenVertexArrays(1, &VAO);
	glGenBuffers(2, VBO);
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * VERTEX_DATA.size(), VERTEX_DATA.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * COLOR_DATA.size(), COLOR_DATA.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
	glEnableVertexAttribArray(1);

	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * INDEX_DATA.size(), INDEX_DATA.data(), GL_STATIC_DRAW);
}

GLvoid LoadPolygon(const char* fileName)
{
	using namespace objl;

	Loader loader;
	if (!loader.LoadFile(fileName))
		return;

	for (int i = 0; i < loader.LoadedMeshes.size(); ++i)
	{
		Mesh m = loader.LoadedMeshes[i];

		for (int j = 0; j < m.Vertices.size(); ++j)
		{
			Vector3 p = m.Vertices[j].Position;

			VERTEX_DATA.push_back(p.X);
			VERTEX_DATA.push_back(p.Y);
			VERTEX_DATA.push_back(p.Z);
			COLOR_DATA.push_back(1.0f);
			COLOR_DATA.push_back(1.0f);
			COLOR_DATA.push_back(1.0f);
		}

		for (int j = 0; j < m.Indices.size(); ++j)
		{
			GLuint u = m.Indices[j];
			INDEX_DATA.push_back(u);
		}
	}
}

GLuint MakeVertexShaders(const char* fileName)
{
	GLchar* vertexSrc = FileToBuffer(fileName);
	GLuint vertShader = glCreateShader(GL_VERTEX_SHADER);

	glShaderSource(vertShader, 1, &vertexSrc, NULL);
	glCompileShader(vertShader);

	GLint result;
	GLchar errorLog[512];

	glGetShaderiv(vertShader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(vertShader, 512, NULL, errorLog);
		std::cerr << "ERROR: vertex shader compile FAILED\n" << errorLog << std::endl;
	}
	return vertShader;
}

GLuint MakeFragmentShaders(const char* fileName)
{
	GLchar* fragmentSrc = FileToBuffer(fileName);
	GLuint fragShader = glCreateShader(GL_FRAGMENT_SHADER);

	glShaderSource(fragShader, 1, &fragmentSrc, NULL);
	glCompileShader(fragShader);

	GLint result;
	GLchar errorLog[512];

	glGetShaderiv(fragShader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(fragShader, 512, NULL, errorLog);
		std::cerr << "ERROR: fragment shader compile FAILED\n" << errorLog << std::endl;
	}
	return fragShader;
}

GLuint MakeShaderProgram(GLuint vertShader, GLuint fragShader)
{
	GLuint shaderID;
	shaderID = glCreateProgram();

	glAttachShader(shaderID, vertShader);
	glAttachShader(shaderID, fragShader);

	glLinkProgram(shaderID);

	glDeleteShader(vertShader);
	glDeleteShader(fragShader);

	GLint result;
	GLchar errorLog[512];

	glGetProgramiv(shaderID, GL_LINK_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(shaderID, 512, NULL, errorLog);
		std::cerr << "ERROR: shader program link FAILED\n" << errorLog << std::endl;
	}
	
	return shaderID;
}

char* FileToBuffer(const char* file)
{
	FILE* fptr;
	long length;
	char* buf;

	fopen_s(&fptr, file, "rb");			// Open file for reading
	if (!fptr)							// Return NULL on failure
		return NULL;

	fseek(fptr, 0, SEEK_END);			// Seek to the end of the file
	length = ftell(fptr);				// Find out how many bytes into the file we are
	buf = (char*)malloc(length + 1);	// Allocate a buffer for the entire length of the file and a null terminator
	fseek(fptr, 0, SEEK_SET);			// Go back to the beginning of the file
	fread(buf, length, 1, fptr);		// Read the contents of the file in to the buffer

	fclose(fptr);						// Close the file
	buf[length] = 0;					// Null terminator

	return buf;							// Return the buffer
}

GLvoid Keyboard(unsigned char key, int x, int y)
{
	switch (toupper(key))
	{
	case 'P':
		currentShape = 0;
		break;
	case 'L':
		currentShape = 1;
		break;
	case 'T':
		currentShape = 2;
		break;
	case 'R':
		currentShape = 3;
		break;
	case 'W':
		vPos += glm::vec3(0.0f, 0.1f, 0.0f);
		break;
	case 'A':
		vPos += glm::vec3(-0.1f, 0.0f, 0.0f);
		break;
	case 'S':
		vPos += glm::vec3(0.0f, -0.1f, 0.0f);
		break;
	case 'D':
		vPos += glm::vec3(0.1f, 0.0f, 0.0f);
		break;
	case 'C':
		shapeData.clear();
		break;
	}
}

glm::vec2 ScreenToWorld(int x, int y)
{
	int ww = glutGet(GLUT_WINDOW_WIDTH);
	int wh = glutGet(GLUT_WINDOW_HEIGHT);

	float xp = static_cast<float>(x * 2) / ww - 1.0f;
	float yp = static_cast<float>(y * 2) / wh - 1.0f;

	glm::mat4 inv = glm::inverse(mProj);
	return (glm::vec3)(inv * glm::vec4(xp, -yp, 0.0f, 1.0f)) - vPos;
}

GLvoid Mouse(int button, int state, int x, int y)
{
	if (state == GLUT_DOWN && button == GLUT_LEFT_BUTTON)
	{
		glm::vec3 p = glm::vec3(ScreenToWorld(x, y), 0.0f);
		shapeData.push_back(std::make_pair(p, currentShape));
	}
}