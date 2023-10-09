#include "pch.h"
#include "OBJ_Loader.h"
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
void LoadIndexes(int shape, int mask);

GLuint shaderProgramID;
GLfloat aspect = 1.0f;

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
	LoadPolygon("suzanne.obj");
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

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(NULL);

	glBegin(GL_LINES);
	glVertex3f(-1.0f, 0.0f, 0.0f);
	glVertex3f(1.0f, 0.0f, 0.0f);
	glVertex3f(0.0f, -1.0f, 0.0f);
	glVertex3f(0.0f, 1.0f, 0.0f);
	glEnd();

	glClear(GL_DEPTH_BUFFER_BIT);
	glUseProgram(shaderProgramID);

	glm::mat4 t = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -1.0f));
	glm::mat4 r = glm::rotate(glm::mat4(1.0f), glm::radians(c * 0.1f), glm::vec3(0.0f, 1.0f, 0.0f));
	r = glm::rotate(r, glm::radians(c * 0.05f), glm::vec3(0.0f, 0.0f, 1.0f));
	glm::mat4 s = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f) * 0.5f);
	glm::mat4 proj = glm::perspective(glm::radians(60.0f), aspect, 0.1f, 100.0f);
	glm::mat4 transform = proj * t * r * s;

	unsigned int modelLocation = glGetUniformLocation(shaderProgramID, "model_Transform");
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(transform));

	mesh.DrawElements();

	glUseProgram(shaderProgramID);

	t = glm::translate(glm::mat4(1.0f), glm::vec3(0.8f, 0.8f, 0.0f));
	s = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f) * 0.1f);
	proj = glm::ortho(-aspect, aspect, -1.0f, 1.0f);
	transform = proj * t * r * s;
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(transform));

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
	glViewport(0, 0, w, h);
}

GLvoid LoadPolygon(const char* fileName)
{
	for (int i = 0; i < 8; ++i)
	{
		mesh.AppendVertex(glm::vec3(i % 2 - 0.5f, i / 2 % 2 - 0.5f, i / 4 % 2 - 0.5f));
		mesh.AppendColor(glm::vec3(i % 2, i / 2 % 2, i / 4 % 2));
	}

	meshGizmo.AppendVertex(glm::vec3(0.0f, 0.0f, 0.0f));
	meshGizmo.AppendVertex(glm::vec3(1.0f, 0.0f, 0.0f));
	meshGizmo.AppendVertex(glm::vec3(0.0f, 0.0f, 0.0f));
	meshGizmo.AppendVertex(glm::vec3(0.0f, 1.0f, 0.0f));
	meshGizmo.AppendVertex(glm::vec3(0.0f, 0.0f, 0.0f));
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

void LoadIndexes(int shape, int mask)
{
	mesh.ClearIndexes();
	switch (shape)
	{
	case 0:
		if (mask & 0b000001)
		{
			mesh.AppendIndex(0, 1, 4);
			mesh.AppendIndex(5, 4, 1);
		}
		if (mask & 0b000010)
		{
			mesh.AppendIndex(2, 6, 3);
			mesh.AppendIndex(7, 3, 6);
		}
		if (mask & 0b000100)
		{
			mesh.AppendIndex(0, 2, 1);
			mesh.AppendIndex(3, 1, 2);
		}
		if (mask & 0b001000)
		{
			mesh.AppendIndex(5, 7, 4);
			mesh.AppendIndex(6, 4, 7);
		}
		if (mask & 0b010000)
		{
			mesh.AppendIndex(4, 6, 0);
			mesh.AppendIndex(2, 0, 6);
		}
		if (mask & 0b100000)
		{
			mesh.AppendIndex(1, 3, 5);
			mesh.AppendIndex(7, 5, 3);
		}
		break;
	case 1:
		if (mask & 0b000001)
		{
			mesh.AppendIndex(0, 2, 1);
		}
		if (mask & 0b000010)
		{
			mesh.AppendIndex(0, 4, 2);
		}
		if (mask & 0b000100)
		{
			mesh.AppendIndex(0, 1, 4);
		}
		if (mask & 0b001000)
		{
			mesh.AppendIndex(1, 2, 4);
		}
		break;
	}
	mesh.ValidateEBO();
}

GLvoid InitBuffer()
{
	meshGizmo.MakeArrayBuffers();
	mesh.MakeArrayBuffers();
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
	case '1':
		LoadIndexes(0, 0b000001);
		break;
	case '2':
		LoadIndexes(0, 0b000010);
		break;
	case '3':
		LoadIndexes(0, 0b000100);
		break;
	case '4':
		LoadIndexes(0, 0b001000);
		break;
	case '5':
		LoadIndexes(0, 0b010000);
		break;
	case '6':
		LoadIndexes(0, 0b100000);
		break;
	case '7':
		LoadIndexes(1, 0b000001);
		break;
	case '8':
		LoadIndexes(1, 0b000010);
		break;
	case '9':
		LoadIndexes(1, 0b000100);
		break;
	case '0':
		LoadIndexes(1, 0b001000);
		break;
	case 'C':
		LoadIndexes(0, 0b000011 << (rand() % 3 * 2));
		break;
	case 'T':
		LoadIndexes(1, 0b000011 << (rand() % 3));
		break;
	}
}