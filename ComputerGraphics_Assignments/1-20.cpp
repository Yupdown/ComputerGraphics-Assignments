#include "pch.h"
#include "Mesh.hpp"

Mesh mesh;
Mesh meshGizmo;

std::string windowTitle = "OpenGLExperiment";

GLvoid LoadPolygon(const char* fileName);
GLvoid InitBuffer();

GLuint MakeVertexShaders(const char* fileName);
GLuint MakeFragmentShaders(const char* fileName);
GLuint MakeShaderProgram(GLuint vertShader, GLuint fragShader);

GLvoid drawScene(GLvoid);
GLvoid Keyboard(unsigned char key, int x, int y);
GLvoid Reshape(int w, int h);
GLvoid Timer();

char* FileToBuffer(const char* file);

GLuint shaderProgramID;
GLfloat aspect = 1.0f;

int win_id;

bool drawMode = false;
bool depthTest = true;

float oTranslatonXSpeed = 0.0f;
float oRotationYSpeed = 0.0f;

struct Transform
{
	glm::vec3 position;
	glm::vec3 rotation;
	glm::vec3 scale;
};

Transform tr[9];
int parent[9] = {-1, 7, 7, 7, 7, 7, 7, 8, -1};

glm::mat4 projPerspective = glm::mat4(1.0f);
glm::mat4 projOrthographic = glm::mat4(1.0f);

int main(int argc, char** argv)
{
	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(640, 480);

	glutCreateWindow(windowTitle.c_str());
	// glutFullScreen();

	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
	{
		std::cerr << "Unable to initialize GLEW" << std::endl;
		exit(EXIT_FAILURE);
	}
	else
		std::cout << "GLEW Initialization" << std::endl;

	GLuint vert = MakeVertexShaders("vert_1-14.glsl");
	GLuint frag = MakeFragmentShaders("frag.glsl");
	shaderProgramID = MakeShaderProgram(vert, frag);
	glUseProgram(shaderProgramID);

	tr[0].scale = glm::vec3(10.0f, 0.1f, 10.0f);
	tr[1].position = glm::vec3(0.0f, 0.15f, 0.0f);
	tr[1].scale = glm::vec3(0.5f, 0.3f, 0.5f);
	tr[2].position = glm::vec3(0.0f, 0.3f, 0.0f);
	tr[2].scale = glm::vec3(0.3f, 0.3f, 0.3f);
	tr[3].position = glm::vec3(-0.15f, 0.5f, 0.0f);
	tr[3].scale = glm::vec3(0.1f, 0.3f, 0.1f);
	tr[4].position = glm::vec3(0.15f, 0.5f, 0.0f);
	tr[4].scale = glm::vec3(0.1f, 0.3f, 0.1f);
	tr[5].position = glm::vec3(-0.25f, 0.1f, 0.3f);
	tr[5].scale = glm::vec3(0.1f, 0.1f, 0.3f);
	tr[6].position = glm::vec3(0.25f, 0.1f, 0.3f);
	tr[6].scale = glm::vec3(0.1f, 0.1f, 0.3f);

	LoadPolygon("");
	InitBuffer();

	glEnable(GL_DEPTH_TEST);

	glutDisplayFunc(drawScene);
	glutReshapeFunc(Reshape);
	glutIdleFunc(Timer);
	glutKeyboardFunc(Keyboard);

	glutMainLoop();

	return 0;
}

GLvoid drawScene()
{
	clock_t c = clock();

	glClearColor(0.125f, 0.125f, 0.125f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(shaderProgramID);

	if (drawMode)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	unsigned int modelLocation = glGetUniformLocation(shaderProgramID, "model_Transform");

	for (int i = 0; i < 7; ++i)
	{
		glm::mat4 transform = glm::mat4(1.0f);

		for (int ti = i; ti >= 0; ti = parent[ti])
		{
			glm::mat4 t = glm::translate(glm::mat4(1.0f), tr[ti].position);
			glm::mat4 r = glm::mat4(1.0f);
			r = glm::rotate(r, glm::radians(tr[ti].rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
			r = glm::rotate(r, glm::radians(tr[ti].rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
			r = glm::rotate(r, glm::radians(tr[ti].rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
			glm::mat4 s = glm::scale(glm::mat4(1.0f), tr[ti].scale);
			transform = t * r * s * transform;
		}

		transform = projPerspective * glm::lookAt(glm::vec3(0.0f, 1.0f, 1.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f)) * transform;

		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(transform));
		mesh.Draw(GL_TRIANGLES);
	}

	glClear(GL_DEPTH_BUFFER_BIT);
	glUseProgram(shaderProgramID);

	glm::mat4 s = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f) * 1.0f);
	meshGizmo.Draw(GL_LINES);

	glutSwapBuffers();
}

GLvoid Timer()
{


#ifdef _DEBUG
	constexpr int REFRESH_RATE = 20;
	static int frameCount = 0;
	static std::chrono::steady_clock timer;
	static std::chrono::steady_clock::time_point lc;
	std::chrono::steady_clock::time_point c = timer.now();
	double delta = std::chrono::duration_cast<std::chrono::duration<double>>(c - lc).count();
	if (delta * REFRESH_RATE > 1.0)
	{
		int fps = (int)round(1.0 / delta * frameCount);
		glutSetWindowTitle(("OpenGL Application" + std::format(" @ {} FPS", fps)).c_str());
		frameCount = 0;
		lc = c;
	}
	frameCount += 1;
#endif

	glutPostRedisplay();
}

GLvoid Reshape(int w, int h)
{
	aspect = (GLfloat)w / h;

	projPerspective = glm::perspective(glm::radians(90.0f), aspect, 0.1f, 100.0f);
	projOrthographic = glm::ortho(-aspect, aspect, -1.0f, 1.0f, -2.0f, 2.0f);

	glViewport(0, 0, w, h);
}

GLvoid LoadPolygon(const char* fileName)
{
	for (int i = 0; i < 8; ++i)
	{
		mesh.AppendVertex(glm::vec3(i % 2 - 0.5f, i / 2 % 2 - 0.5f, i / 4 % 2 - 0.5f));
		mesh.AppendColor(glm::vec3(i % 2, i / 2 % 2, i / 4 % 2));
	}
	mesh.AppendIndex(0, 1, 4);
	mesh.AppendIndex(5, 4, 1);
	mesh.AppendIndex(2, 6, 3);
	mesh.AppendIndex(7, 3, 6);
	mesh.AppendIndex(0, 2, 1);
	mesh.AppendIndex(3, 1, 2);
	mesh.AppendIndex(5, 7, 4);
	mesh.AppendIndex(6, 4, 7);
	mesh.AppendIndex(4, 6, 0);
	mesh.AppendIndex(2, 0, 6);
	mesh.AppendIndex(1, 3, 5);
	mesh.AppendIndex(7, 5, 3);

	meshGizmo.AppendVertex(glm::vec3(-1.0f, 0.0f, 0.0f));
	meshGizmo.AppendVertex(glm::vec3(1.0f, 0.0f, 0.0f));
	meshGizmo.AppendVertex(glm::vec3(0.0f, -1.0f, 0.0f));
	meshGizmo.AppendVertex(glm::vec3(0.0f, 1.0f, 0.0f));
	meshGizmo.AppendVertex(glm::vec3(0.0f, 0.0f, -1.0f));
	meshGizmo.AppendVertex(glm::vec3(0.0f, 0.0f, 1.0f));

	meshGizmo.AppendColor(glm::vec3(1.0f, 0.0f, 0.0f));
	meshGizmo.AppendColor(glm::vec3(1.0f, 0.0f, 0.0f));
	meshGizmo.AppendColor(glm::vec3(0.0f, 1.0f, 0.0f));
	meshGizmo.AppendColor(glm::vec3(0.0f, 1.0f, 0.0f));
	meshGizmo.AppendColor(glm::vec3(0.0f, 0.0f, 1.0f));
	meshGizmo.AppendColor(glm::vec3(0.0f, 0.0f, 1.0f));

	meshGizmo.AppendIndex(0);
	meshGizmo.AppendIndex(1);
	meshGizmo.AppendIndex(2);
	meshGizmo.AppendIndex(3);
	meshGizmo.AppendIndex(4);
	meshGizmo.AppendIndex(5);
}

GLvoid InitBuffer()
{
	mesh.MakeArrayBuffers();
	meshGizmo.MakeArrayBuffers();
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
	bool upper = isupper(key);
	switch (key)
	{
		break;
	case 'h':
		depthTest = !depthTest;
		(depthTest ? glEnable : glDisable)(GL_DEPTH_TEST);
		break;
	case 'w':
		drawMode = !drawMode;
		break;
	case 'b':
	case 'B':
		break;
	case 'm':
	case 'M':
		break;
	case 'f':
	case 'F':
		break;
	case 'e':
	case 'E':
		break;
	case 't':
	case 'T':
		break;
	case 'z':
	case 'Z':
		break;
	case 'x':
	case 'X':
		break;
	case 'y':
	case 'Y':
		break;
	case 'r':
	case 'R':
		break;
	case 'a':
	case 'A':
		break;
	case 's':
	case 'S':
		break;
	case 'c':
	case 'C':
		break;
	case 'q':
	case 'Q':
		glutDestroyWindow(win_id);
		break;
	}
}
