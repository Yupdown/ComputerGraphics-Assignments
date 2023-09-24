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

GLvoid LoadPolygon(const char* fileName);
GLvoid InitBuffer();

GLuint MakeVertexShaders(const char* fileName);
GLuint MakeFragmentShaders(const char* fileName);
GLuint MakeShaderProgram(GLuint vertShader, GLuint fragShader);

GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);
GLvoid Timer(int delta);

char* FileToBuffer(const char* file);

GLuint shaderProgramID;
GLuint VAO, VBO[2], EBO;
GLfloat aspect = 1.0f;

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
	LoadPolygon("nabin_blender_02.obj");
	InitBuffer();

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	glutDisplayFunc(drawScene);
	glutReshapeFunc(Reshape);
	glutTimerFunc(0, Timer, 0);

	glutMainLoop();

	return 0;
}

GLvoid drawScene()
{
	clock_t c = clock();

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(shaderProgramID);

	glm::mat4 t = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -0.2f, -0.5f));
	glm::mat4 r = glm::rotate(glm::mat4(1.0f), glm::radians(c * 0.1f), glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 s = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f) * 0.2f);
	glm::mat4 proj = glm::perspective(glm::radians(sin(c * 0.002f) * 5.0f + 60.0f), aspect, 0.1f, 100.0f);
	glm::mat4 transform = proj * t * r * s;

	unsigned int modelLocation = glGetUniformLocation(shaderProgramID, "model_Transform");
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(transform));

	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, INDEX_DATA.size(), GL_UNSIGNED_INT, 0);

	glutSwapBuffers();
}

GLvoid Timer(int delta)
{
	glutPostRedisplay();
	glutTimerFunc(0, Timer, 0);
}

GLvoid Reshape(int w, int h)
{
	aspect = (GLfloat)w / h;
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
			Vector3 n = m.Vertices[j].Normal;

			VERTEX_DATA.push_back(p.X);
			VERTEX_DATA.push_back(p.Y);
			VERTEX_DATA.push_back(p.Z);
			COLOR_DATA.push_back(n.Z * n.Z);
			COLOR_DATA.push_back(n.Z * n.Z);
			COLOR_DATA.push_back(n.Z * n.Z);
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