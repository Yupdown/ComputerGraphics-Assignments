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

std::vector<GLfloat> VERTEX_DATA
{
	-0.5f, -0.5f, 0.0f,
	-0.5f, 0.5f, 0.0f,
	0.5f, -0.5f, 0.0f,
	0.5f, 0.5f, 0.0f,
};

std::vector<GLfloat> COLOR_DATA
{
	1.0f, 0.0f, 0.0f,
	0.0f, 1.0f, 0.0f,
	0.0f, 0.0f, 1.0f,
	1.0f, 1.0f, 1.0f,
};

std::vector<GLuint> INDEX_DATA
{
	0, 1, 3, 2, 0
};

glm::vec3 colors[6] =
{
	{ 0.5f, 0.5f, 0.5f },
	{ 1.0f, 1.0f, 0.0f },
	{ 0.0f, 1.0f, 0.0f },
	{ 0.0f, 1.0f, 1.0f },
	{ 0.0f, 0.0f, 1.0f },
	{ 1.0f, 0.0f, 1.0f }
};

time_t e_time = 0;
int drag_index = -1;

GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);
GLvoid Mouse(int button, int state, int x, int y);
GLvoid Keyboard(unsigned char key, int x, int y);
GLvoid Timer();

GLvoid LoadPolygon(const char* fileName);
GLvoid InitBuffer();
GLvoid ValidateVBO();
GLvoid MouseMotion(int x, int y);

GLuint MakeVertexShaders(const char* fileName);
GLuint MakeFragmentShaders(const char* fileName);
GLuint MakeShaderProgram(GLuint vertShader, GLuint fragShader);

char* FileToBuffer(const char* file);

GLuint shaderProgramID;
GLuint VAO, VBO[2], EBO;
glm::mat4 mProj = glm::mat4(1.0f);
glm::vec3 vPos = glm::vec3(0.0f);

void main(int argc, char** argv)
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

	GLuint vert = MakeVertexShaders("vert_1-13.glsl");
	GLuint frag = MakeFragmentShaders("frag.glsl");
	shaderProgramID = MakeShaderProgram(vert, frag);
	glUseProgram(shaderProgramID);
	InitBuffer();

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	glutDisplayFunc(drawScene);
	glutReshapeFunc(Reshape);
	glutKeyboardFunc(Keyboard);
	glutMouseFunc(Mouse);
	glutMotionFunc(MouseMotion);
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
	return glm::inverse(mProj) * glm::vec4(xp, -yp, 0.0f, 1.0f);
}

GLvoid drawScene()
{
	glm::vec3 col = colors[0] * 0.25f;
	glClearColor(col.r, col.g, col.b, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(shaderProgramID);

	glm::mat4 t = glm::translate(glm::mat4(1.0f), vPos);
	glm::mat4 r = glm::rotate(glm::mat4(1.0f), 0.0f, glm::vec3(0.0f, 0.0f, 1.0f));
	glm::mat4 s = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f));
	glm::mat4 transform = mProj * t * r * s;

	unsigned int modelLocation = glGetUniformLocation(shaderProgramID, "model_Transform");
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(transform));

	glBindVertexArray(VAO);
	glDrawElements(GL_LINE_STRIP, INDEX_DATA.size(), GL_UNSIGNED_INT, 0);

	glutSwapBuffers();
}

GLvoid Reshape(int w, int h)
{
	GLfloat aspect = (GLfloat)w / h;
	mProj = glm::ortho(-aspect, aspect, -1.0f, 1.0f, -1.0f, 1.0f);

	glViewport(0, 0, w, h);
}

GLvoid Mouse(int button, int state, int x, int y)
{
	glm::vec2 pos = screen_to_world(x, y);
	if (button == GLUT_LEFT_BUTTON)
	{
		if (state == GLUT_DOWN)
		{
			for (int i = 0; i < 4; ++i)
			{
				glm::vec2 dv = pos - glm::vec2(VERTEX_DATA[i * 3], VERTEX_DATA[i * 3 + 1]);
				if (dv.x * dv.x + dv.y * dv.y <= 0.025f)
				{
					drag_index = i;
					MouseMotion(x, y);
					break;
				}
			}
		}
		else if (state == GLUT_UP)
			drag_index = -1;
	}
	glutPostRedisplay();
}

GLvoid Timer()
{
	e_time = clock();
	glutPostRedisplay();
}

GLvoid Keyboard(unsigned char key, int x, int y)
{
	glutPostRedisplay();
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

GLvoid ValidateVBO()
{
	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(GLfloat) * VERTEX_DATA.size(), VERTEX_DATA.data());
}

GLvoid MouseMotion(int x, int y)
{
	glm::vec2 pos = screen_to_world(x, y);

	if (drag_index >= 0)
	{
		VERTEX_DATA[drag_index * 3] = pos.x;
		VERTEX_DATA[drag_index * 3 + 1] = pos.y;
		ValidateVBO();
	}

	glutPostRedisplay();
}