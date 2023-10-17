#include "pch.h"
#include "Mesh.hpp"

Mesh meshSphere;
Mesh meshRing;
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

bool projFlag = false;
float projFactor = 0.0f;

struct Transform
{
	glm::vec3 position;
	glm::vec3 rotation;
	glm::vec3 scale;

	Transform()
	{
		position = glm::vec3(0.0f, 0.0f, 0.0f);
		rotation = glm::vec3(0.0f, 0.0f, 0.0f);
		scale = glm::vec3(1.0f, 1.0f, 1.0f);
	}
};

glm::mat4 GetRotationMatrix(const Transform& transform)
{
	glm::mat4 r = glm::mat4(1.0f);
	r = glm::rotate(r, glm::radians(transform.rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
	r = glm::rotate(r, glm::radians(transform.rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
	r = glm::rotate(r, glm::radians(transform.rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
	return r;
}

glm::mat4 GetMatrix(const Transform& transform)
{
	glm::mat4 t = glm::translate(glm::mat4(1.0f), transform.position);
	glm::mat4 r = GetRotationMatrix(transform);
	glm::mat4 s = glm::scale(glm::mat4(1.0f), transform.scale);

	return t * r * s;
}

Transform transformGlobal;
Transform transformStar[3];
Transform transformPlanet[3];
Transform transformMoon[3];

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

	GLuint vert = MakeVertexShaders("vert.glsl");
	GLuint frag = MakeFragmentShaders("frag.glsl");
	shaderProgramID = MakeShaderProgram(vert, frag);
	glUseProgram(shaderProgramID);

	meshSphere.LoadFromFile("sphere.obj");
	LoadPolygon("");
	InitBuffer();

	for (int i = 0; i < 3; ++i)
	{
		transformStar[i].position = glm::vec3(0.0f, 0.0f, 0.0f);
		transformStar[i].scale = glm::vec3(0.1f);
		transformPlanet[i].position = glm::vec3(5.0f, 0.0f, 0.0f);
		transformPlanet[i].scale = glm::vec3(0.5f);
		transformMoon[i].position = glm::vec3(3.0f, 0.0f, 0.0f);
		transformMoon[i].scale = glm::vec3(0.5f);
	}

	transformGlobal.position = glm::vec3(0.0f, 0.0f, -1.0f);
	transformGlobal.rotation = glm::vec3(45.0f, 45.0f, 0.0f);
	transformStar[0].rotation = glm::vec3(-60.0f, 0.0f, 0.0f);
	transformStar[1].rotation = glm::vec3(0.0f, 120.0f, 0.0f);
	transformStar[2].rotation = glm::vec3(60.0f, 240.0f, 0.0f);

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
	glUseProgram(shaderProgramID);

	if (drawMode)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glm::mat4 proj = projPerspective + (projOrthographic - projPerspective) * projFactor;

	unsigned int modelLocationWorld = glGetUniformLocation(shaderProgramID, "model_Transform");
	unsigned int modelLocationColor = glGetUniformLocation(shaderProgramID, "model_Color");

	for (int i = 0; i < 3; ++i)
	{
		glm::mat4 matrixStar = proj * GetMatrix(transformGlobal) * GetMatrix(transformStar[i]);
		if (i == 0)
		{
			glUniformMatrix4fv(modelLocationWorld, 1, GL_FALSE, glm::value_ptr(glm::mat4(matrixStar)));
			glUniform3fv(modelLocationColor, 1, glm::value_ptr(glm::vec3(1.0f, 0.0f, 0.0f)));
			meshSphere.Draw(GL_TRIANGLES);
		}

		glm::mat4 matrixPlanet = proj * GetMatrix(transformGlobal) * GetMatrix(transformStar[i]) * GetMatrix(transformPlanet[i]);
		glUniformMatrix4fv(modelLocationWorld, 1, GL_FALSE, glm::value_ptr(glm::mat4(matrixPlanet)));
		glUniform3fv(modelLocationColor, 1, glm::value_ptr(glm::vec3(0.0f, 1.0f, 0.0f)));
		meshSphere.Draw(GL_TRIANGLES);
		glUniformMatrix4fv(modelLocationWorld, 1, GL_FALSE, glm::value_ptr(glm::mat4(matrixStar) * glm::scale(glm::mat4(1.0f), glm::vec3(glm::length(transformPlanet[i].position)))));
		meshRing.Draw(GL_LINE_STRIP);

		glm::mat4 matrixMoon = proj * GetMatrix(transformGlobal) * GetMatrix(transformStar[i]) * GetMatrix(transformPlanet[i]) * GetMatrix(transformMoon[i]);

		glUniformMatrix4fv(modelLocationWorld, 1, GL_FALSE, glm::value_ptr(glm::mat4(matrixMoon)));
		glUniform3fv(modelLocationColor, 1, glm::value_ptr(glm::vec3(0.0f, 0.0f, 1.0f)));
		meshSphere.Draw(GL_TRIANGLES);
		glUniformMatrix4fv(modelLocationWorld, 1, GL_FALSE, glm::value_ptr(glm::mat4(matrixPlanet) * glm::scale(glm::mat4(1.0f), glm::vec3(glm::length(transformMoon[i].position)))));
		meshRing.Draw(GL_LINE_STRIP);
	}

	glClear(GL_DEPTH_BUFFER_BIT);
	glUseProgram(shaderProgramID);

	glm::mat4 s = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f) * 1.0f);

	glUniformMatrix4fv(modelLocationWorld, 1, GL_FALSE, glm::value_ptr(projOrthographic * GetMatrix(transformGlobal)));
	// meshGizmo.Draw(GL_LINES);

	glutSwapBuffers();
}

GLvoid Timer()
{
	float dt = 0.01f;
	projFactor = projFactor + ((projFlag ? 1.0f : 0.0f) - projFactor) * dt;

	for (int i = 0; i < 3; ++i)
	{
		transformStar[i].rotation.y += rotateX;
		transformPlanet[i].rotation.y += rotateX;
		transformMoon[i].rotation.y += rotateX;
		transformStar[i].rotation.x += rotateY;
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
	for (int i = 0; i <= 100; ++i)
	{
		float theta = static_cast<float>(i) / 50.0f * glm::pi<float>();
		meshRing.AppendVertex(glm::vec3(glm::cos(theta), 0.0f, glm::sin(theta)));
		meshRing.AppendColor(glm::vec3(1.0f, 1.0f, 1.0f));
		meshRing.AppendIndex(i);
	}

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
	meshSphere.MakeArrayBuffers();
	meshRing.MakeArrayBuffers();
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
	case 'M':
		drawMode = !drawMode;
		break;
	case 'W':
		transformGlobal.position.y += 0.1f;
		break;
	case 'A':
		transformGlobal.position.x -= 0.1f;
		break;
	case 'S':
		transformGlobal.position.y -= 0.1f;
		break;
	case 'D':
		transformGlobal.position.x += 0.1f;
		break;
	case '+':
		transformGlobal.position.z += 0.1f;
		break;
	case '-':
		transformGlobal.position.z -= 0.1f;
		break;
	case '1':
		rotateX = 0.1f;
		break;
	case '2':
		rotateX = -0.1f;
		break;
	case '3':
		rotateY = 0.05f;
		break;
	case '4':
		rotateY = -0.05f;
		break;
	case 'P':
		projFlag = !projFlag;
		break;
	}
}
