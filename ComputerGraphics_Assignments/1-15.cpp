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
GLvoid SpecialKeyboard(int key, int x, int y);
GLvoid Reshape(int w, int h);
GLvoid Timer();

char* FileToBuffer(const char* file);
void LoadIndexes(int shape);

GLuint shaderProgramID;
GLfloat aspect = 1.0f;

bool drawMode = false;
bool depthTest = true;
float rotateX = 0.0f;
float rotateY = 0.0f;
float offsetX = 0.0f;
float offsetY = 0.0f;

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
	glutSpecialFunc(SpecialKeyboard);

	glutMainLoop();

	return 0;
}

GLvoid drawScene()
{
	clock_t c = clock();

	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(NULL);

	glBegin(GL_LINES);
	glVertex3f(-1.0f, 0.0f, 0.0f);
	glVertex3f(1.0f, 0.0f, 0.0f);
	glVertex3f(0.0f, -1.0f, 0.0f);
	glVertex3f(0.0f, 1.0f, 0.0f);
	glColor3f(0.0f, 0.0f, 0.0f);
	glColor3f(0.0f, 0.0f, 0.0f);
	glColor3f(0.0f, 0.0f, 0.0f);
	glColor3f(0.0f, 0.0f, 0.0f);
	glEnd();

	glClear(GL_DEPTH_BUFFER_BIT);
	glUseProgram(shaderProgramID);

	if (drawMode)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glm::mat4 t = glm::translate(glm::mat4(1.0f), glm::vec3(offsetX, offsetY, -1.0f));
	glm::mat4 r = glm::rotate(glm::mat4(1.0f), glm::radians(c * rotateX + 30.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	r = glm::rotate(r, glm::radians(c * rotateY - 30.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 s = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f) * 0.5f);
	glm::mat4 proj = glm::perspective(glm::radians(60.0f), aspect, 0.1f, 100.0f);
	glm::mat4 transform = proj * t * r * s;

	unsigned int modelLocation = glGetUniformLocation(shaderProgramID, "model_Transform");
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(transform));

	mesh.Draw(GL_TRIANGLES);

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
	mesh.AppendVertex(glm::vec3(0.0f, 0.5f, 0.0f));
	mesh.AppendColor(glm::vec3(0.5f, 1.0f, 0.5f));

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

void LoadIndexes(int shape)
{
	mesh.ClearIndexes();
	switch (shape)
	{
	case 0:
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
		break;
	case 1:
		mesh.AppendIndex(0, 1, 4);
		mesh.AppendIndex(5, 4, 1);
		mesh.AppendIndex(0, 8, 1);
		mesh.AppendIndex(1, 8, 5);
		mesh.AppendIndex(5, 8, 4);
		mesh.AppendIndex(4, 8, 0);
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
	case 'C':
		LoadIndexes(0);
		break;
	case 'P':
		LoadIndexes(1);
		break;
	case 'H':
		depthTest = !depthTest;
		(depthTest ? glEnable : glDisable)(GL_DEPTH_TEST);
		break;
	case 'W':
		drawMode = true;
		break;
	case 'S':
		drawMode = false;
		break;
	case '1':
		rotateX = 0.1f;
		break;
	case '2':
		rotateX = -0.1f;
		break;
	case '3':
		rotateY = 0.1f;
		break;
	case '4':
		rotateY = -0.1f;
		break;
	case 'R':
		drawMode = false;
		depthTest = true;
		glEnable(GL_DEPTH_TEST);
		rotateX = 0.0f;
		rotateY = 0.0f;
		offsetX = 0.0f;
		offsetY = 0.0f;
		break;
	}
}

GLvoid SpecialKeyboard(int key, int x, int y)
{
	switch (key)
	{
	case GLUT_KEY_UP:
		offsetY += 0.01f;
		break;
	case GLUT_KEY_DOWN:
		offsetY -= 0.01f;
		break;
	case GLUT_KEY_RIGHT:
		offsetX += 0.01f;
		break;
	case GLUT_KEY_LEFT:
		offsetX -= 0.01f;
		break;
	}
}
