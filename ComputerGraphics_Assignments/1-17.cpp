#include "pch.h"
#include "Mesh.hpp"

Mesh* selectedMesh[2];
Mesh meshes[4];
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

bool drawMode = false;
bool depthTest = true;
float rotateX = 0.0f;
float rotateY = 0.0f;
float rotateXGlobal = 0.0f;
float rotateYGlobal = 0.0f;
int moveMode = 0;

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
	meshes[0].LoadFromFile("cube.obj");
	meshes[1].LoadFromFile("cone.obj");
	meshes[2].LoadFromFile("teapot.obj");
	meshes[3].LoadFromFile("suzanne.obj");
	selectedMesh[0] = &meshes[1];
	selectedMesh[1] = &meshes[3];
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

	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(shaderProgramID);

	if (drawMode)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glm::mat4 rWorld = glm::rotate(glm::mat4(1.0f), glm::radians(c * rotateXGlobal + 30.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	glm::mat4 tWorld = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -1.0f));
	rWorld = glm::rotate(rWorld, glm::radians(c * rotateYGlobal), glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 proj = glm::perspective(glm::radians(60.0f), aspect, 0.1f, 100.0f);
	glm::mat4 projOrtho = glm::ortho(-aspect, aspect, -1.0f, 1.0f);

	unsigned int modelLocation = glGetUniformLocation(shaderProgramID, "model_Transform");

	for (int i = 0; i < 2; ++i)
	{
		float radius = i ? 0.5f : -0.5f;
		switch (moveMode)
		{
		case 0:
		case 1:
			radius *= (glm::sin(c * 0.001f) + 1.0f) * 0.5f;
			break;
		case 2:
			radius *= glm::sin(c * 0.001f);
			break;
		}

		float theta = c * 0.004f;
		glm::mat4 t = glm::mat4(1.0f);

		switch (moveMode)
		{
		case 0:
		case 3:
			t = glm::translate(glm::mat4(1.0f), glm::vec3(radius * cos(theta), 0.0f, radius * sin(theta)));
			break;
		case 1:
		case 2:
			t = glm::translate(glm::mat4(1.0f), glm::vec3(radius, 0.0f, 0.0f));
			break;
			break;
		case 4:
			t = glm::translate(glm::mat4(1.0f), glm::vec3(radius * cos(theta), radius * sin(theta), 0.0f));
			break;
		}
		glm::mat4 r = glm::rotate(glm::mat4(1.0f), glm::radians(c * rotateX + 30.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		r = glm::rotate(r, glm::radians(c * rotateY - 30.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		glm::mat4 s = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f) * 0.25f);
		glm::mat4 transform = proj * tWorld * rWorld * t * r * s;

		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(transform));
		selectedMesh[i]->Draw(GL_TRIANGLES);
	}

	glClear(GL_DEPTH_BUFFER_BIT);
	glUseProgram(shaderProgramID);

	glm::mat4 s = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f) * 1.0f);

	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(projOrtho * rWorld * s));
	meshGizmo.Draw(GL_LINES);

	if (moveMode == 0)
	{
		glBegin(GL_LINE_STRIP);
		for (int i = 0; i < 100; ++i)
		{
			float theta = i * glm::pi<float>() / 15.0f;
			float radius = i / 100.0f;

			glVertex3f(cos(theta) * radius, 0.0f, sin(theta) * radius);
		}
		glEnd();
	}

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
	for (auto& mesh : meshes)
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
	switch (toupper(key))
	{
	case 'R':
		moveMode = 0;
		break;
	case 'T':
		moveMode = 1;
		break;
	case '1':
		moveMode = 2;
		break;
	case '2':
		moveMode = 3;
		break;
	case '3':
		moveMode = 4;
		break;
	case '5':
		rotateXGlobal = 0.1f;
		break;
	case '6':
		rotateXGlobal = -0.1f;
		break;
	case '7':
		rotateYGlobal = 0.1f;
		break;
	case '8':
		rotateYGlobal = -0.1f;
		break;
	}
}
