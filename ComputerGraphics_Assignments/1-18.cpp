#include "pch.h"
#include "Mesh.hpp"

Mesh polygon[2];
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

bool shape = false;

float rotateX = 0.0f;
float rotateY = 0.0f;
float rotateXGlobal = 0.0f;
float rotateYGlobal = 0.0f;

int openFlags = 0;
float upFactor = 0.0f;
float frontFactor = 0.0f;
float sideFactor = 0.0f;
float backFactor = 0.0f;
float projFactor = 0.0f;
float coneFactor = 0.0f;

struct Transform
{
	glm::vec3 position;
	glm::vec3 rotation;
	glm::vec3 scale;
};

Transform tr[6];
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

	GLuint vert = MakeVertexShaders("vert_1-18.glsl");
	GLuint frag = MakeFragmentShaders("frag.glsl");
	shaderProgramID = MakeShaderProgram(vert, frag);
	glUseProgram(shaderProgramID);
	polygon[0].AppendVertex(glm::vec3(-0.5f, -0.5f, 0.0f));
	polygon[0].AppendVertex(glm::vec3(-0.5f, 0.5f, 0.0f));
	polygon[0].AppendVertex(glm::vec3(0.5f, -0.5f, 0.0f));
	polygon[0].AppendVertex(glm::vec3(0.5f, 0.5f, 0.0f));
	polygon[0].AppendColor(glm::vec3(1.0f, 1.0f, 1.0f));
	polygon[0].AppendColor(glm::vec3(1.0f, 1.0f, 1.0f));
	polygon[0].AppendColor(glm::vec3(1.0f, 1.0f, 1.0f));
	polygon[0].AppendColor(glm::vec3(1.0f, 1.0f, 1.0f));
	polygon[0].AppendIndex(0, 2, 1);
	polygon[0].AppendIndex(3, 1, 2);

	polygon[1].AppendVertex(glm::vec3(-0.5f, 0.0f, 0.0f));
	polygon[1].AppendVertex(glm::vec3(0.5f, 0.0f, 0.0f));
	polygon[1].AppendVertex(glm::vec3(0.0f, 1.0f, 0.0f));
	polygon[1].AppendColor(glm::vec3(1.0f, 1.0f, 1.0f));
	polygon[1].AppendColor(glm::vec3(1.0f, 1.0f, 1.0f));
	polygon[1].AppendColor(glm::vec3(1.0f, 1.0f, 1.0f));
	polygon[1].AppendIndex(0, 1, 2);

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

	glm::mat4 rWorld = glm::rotate(glm::mat4(1.0f), glm::radians(c * rotateXGlobal + 30.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	glm::mat4 tWorld = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -1.0f));
	rWorld = glm::rotate(rWorld, glm::radians(c * rotateYGlobal + 45.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	unsigned int modelLocationWorld = glGetUniformLocation(shaderProgramID, "world_Transform");
	unsigned int modelLocationProj = glGetUniformLocation(shaderProgramID, "model_Transform");

	for (int i = 0; i < (shape ? 5 : 6); ++i)
	{
		glm::mat4 t = glm::translate(glm::mat4(1.0f), tr[i].position);
		glm::mat4 r = glm::mat4(1.0f);
		r = glm::rotate(r, glm::radians(tr[i].rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
		r = glm::rotate(r, glm::radians(tr[i].rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
		r = glm::rotate(r, glm::radians(tr[i].rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
		glm::mat4 s = glm::scale(glm::mat4(1.0f), tr[i].scale);

		glm::mat4 transformWorld = s * t * r;
		glm::mat4 transform = (projPerspective + (projOrthographic - projPerspective) * projFactor) * tWorld * rWorld;

		glUniformMatrix4fv(modelLocationWorld, 1, GL_FALSE, glm::value_ptr(transformWorld));
		glUniformMatrix4fv(modelLocationProj, 1, GL_FALSE, glm::value_ptr(transform));
		polygon[shape && i > 0].Draw(GL_TRIANGLES);
	}

	glClear(GL_DEPTH_BUFFER_BIT);
	glUseProgram(shaderProgramID);

	glm::mat4 s = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f) * 1.0f);

	glUniformMatrix4fv(modelLocationWorld, 1, GL_FALSE, glm::value_ptr(s));
	glUniformMatrix4fv(modelLocationProj, 1, GL_FALSE, glm::value_ptr(projOrthographic * rWorld));
	meshGizmo.Draw(GL_LINES);

	glutSwapBuffers();
}

GLvoid Timer()
{
	float dt = 0.01f;
	auto modifier = [](float s, float t) {return glm::clamp(t * 4.0f - s, 0.0f, 1.0f) * 0.5f; };
	bool flag = openFlags & 0b1000000;

	upFactor = upFactor + ((openFlags & 0b0001 ? 1.0f : 0.0f) - upFactor) * dt;
	frontFactor = frontFactor + ((openFlags & 0b0010 ? 1.0f : 0.0f) - frontFactor) * dt;
	sideFactor = sideFactor + ((openFlags & 0b0100 ? 1.0f : 0.0f) - sideFactor) * dt;
	backFactor = backFactor + ((openFlags & 0b1000 ? 1.0f : 0.0f) - backFactor) * dt;
	projFactor = projFactor + ((openFlags & 0b10000 ? 1.0f : 0.0f) - projFactor) * dt;
	coneFactor = coneFactor + ((openFlags & 0b100000 ? 1.0f : 0.0f) - coneFactor) * (flag ? dt * 0.125f : dt);

	if (shape)
	{
		tr[0].position = glm::vec3(0.0f, 0.0f, 0.0f);
		tr[1].position = glm::vec3(-0.5f, 0.0f, 0.0f);
		tr[2].position = glm::vec3(0.5f, 0.0f, 0.0f);
		tr[3].position = glm::vec3(0.0f, 0.0f, -0.5f);
		tr[4].position = glm::vec3(0.0f, 0.0f, 0.5f);

		tr[0].rotation = glm::vec3(90.0f, 0.0f, 0.0f);
		tr[1].rotation = glm::vec3(-30.0f + (flag ? modifier(0.0f, coneFactor) : coneFactor) * 240.0f, -90.0f, 0.0f);
		tr[2].rotation = glm::vec3(-30.0f + (flag ? modifier(1.0f, coneFactor) : coneFactor) * 240.0f, 90.0f, 0.0f);
		tr[3].rotation = glm::vec3(-30.0f + (flag ? modifier(2.0f, coneFactor) : coneFactor) * 240.0f, 180.0f, 0.0f);
		tr[4].rotation = glm::vec3(-30.0f + (flag ? modifier(3.0f, coneFactor) : coneFactor) * 240.0f, 0.0f, 0.0f);

		tr[0].scale = glm::vec3(0.5f);
		tr[1].scale = glm::vec3(0.5f);
		tr[2].scale = glm::vec3(0.5f);
		tr[3].scale = glm::vec3(0.5f);
		tr[4].scale = glm::vec3(0.5f);
	}
	else
	{
		tr[0].position = glm::vec3(-0.5f, sideFactor, 0.0f);
		tr[1].position = glm::vec3(0.5f, sideFactor, 0.0f);
		tr[2].position = glm::vec3(0.0f, -0.5f, 0.0f);
		tr[3].position = glm::vec3(0.0f, 0.5f, 0.0f);
		tr[4].position = glm::vec3(0.0f, 0.0f, -0.5f);
		tr[5].position = glm::vec3(0.0f, sinf((1.0f - frontFactor) * 0.5f * glm::pi<float>()) * 0.5f - 0.5f, 0.5f + cosf((1.0f - frontFactor) * 0.5f * glm::pi<float>()) * 0.5f);

		tr[0].rotation = glm::vec3(0.0f, 90.0f, 0.0f);
		tr[1].rotation = glm::vec3(0.0f, 90.0f, 0.0f);
		tr[2].rotation = glm::vec3(90.0f, 0.0f, 0.0f);
		tr[3].rotation = glm::vec3((upFactor - 1.0f) * 90.0f, 0.0f, 0.0f);
		tr[4].rotation = glm::vec3(0.0f, 0.0f, 0.0f);
		tr[5].rotation = glm::vec3(frontFactor * 90.0f, 0.0f, 0.0f);

		tr[0].scale = glm::vec3(0.5f);
		tr[1].scale = glm::vec3(0.5f);
		tr[2].scale = glm::vec3(0.5f);
		tr[3].scale = glm::vec3(0.5f);
		tr[4].scale = glm::vec3((1.0f - backFactor) * 0.5f);
		tr[5].scale = glm::vec3(0.5f);
	}

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

	projPerspective = glm::perspective(glm::radians(60.0f), aspect, 0.1f, 100.0f);
	projOrthographic = glm::ortho(-aspect, aspect, -1.0f, 1.0f, -2.0f, 2.0f);

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
	for (auto& mesh : polygon)
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
	break;
	case 'H':
		depthTest = !depthTest;
		(depthTest ? glEnable : glDisable)(GL_DEPTH_TEST);
		break;
	case 'W':
		drawMode = !drawMode;
		break;
	case '1':
		rotateX = 0.2f;
		break;
	case '2':
		rotateX = -0.2f;
		break;
	case '3':
		rotateY = 0.2f;
		break;
	case '4':
		rotateY = -0.2f;
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
	case 'T':
		shape = false;
		openFlags ^= 0b0001;
		break;
	case 'F':
		shape = false;
		openFlags ^= 0b0010;
		break;
	case 'S':
		shape = false;
		openFlags ^= 0b0100;
		break;
	case 'B':
		shape = false;
		openFlags ^= 0b1000;
		break;
	case 'P':
		openFlags ^= 0b10000;
		break;
	case 'O':
		shape = true;
		openFlags ^= 0b100000;
		openFlags &= ~0b1000000;
		break;
	case 'R':
		shape = true;
		openFlags ^= 0b100000;
		openFlags |= 0b1000000;
		break;
	}
}
