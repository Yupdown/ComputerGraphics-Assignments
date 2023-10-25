#include "pch.h"
#include "Mesh.hpp"

Mesh meshCylinder;
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

void SimulatePlayer(float dt);

char* FileToBuffer(const char* file);

GLuint shaderProgramID;
GLfloat aspect = 1.0f;

int win_id;

float openTime = 0.0f;
float elapsedTime = 0.0f;

glm::vec3 camPos = glm::vec3(0.0f, 0.0f, 8.0f);

int sw, sh;

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

glm::mat4 GetMatrix(const Transform& transform)
{
	glm::mat4 t = glm::translate(glm::mat4(1.0f), transform.position);
	glm::mat4 r = glm::mat4(1.0f);
	r = glm::rotate(r, glm::radians(transform.rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
	r = glm::rotate(r, glm::radians(transform.rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
	r = glm::rotate(r, glm::radians(transform.rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
	glm::mat4 s = glm::scale(glm::mat4(1.0f), transform.scale);

	return t * r * s;
}

glm::vec3 velocity = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 moveDir = glm::vec3(0.0f);
float speed = 4.0f;

Transform trc;
Transform tr[11];
int parent[11] = { -1, 0, 0, 7, 8, 9, 10, 0, 0, 0, 0 };

Transform obstacles[3];
Transform trw[9];

float animFactor = 0.0f;

glm::mat4 projPerspective = glm::mat4(1.0f);
glm::mat4 projOrthographic = glm::mat4(1.0f);

void InitTransform()
{
	trc.position = glm::vec3(0.0f);
	moveDir = glm::normalize(glm::vec3(1.0f, 0.0f, 0.3f));

	obstacles[0].position = glm::vec3(1.0f, -4.0f, 2.0f);
	obstacles[1].position = glm::vec3(-3.0f, -4.0f, 1.0f);
	obstacles[2].position = glm::vec3(2.0f, -4.0f, -2.0f);
	obstacles[0].scale = glm::vec3(2.0f, 1.0f, 2.0f);
	obstacles[1].scale = glm::vec3(2.0f, 1.0f, 2.0f);
	obstacles[2].scale = glm::vec3(2.0f, 1.0f, 2.0f);

	tr[0].scale = glm::vec3(0.5f, 0.5f, 0.5f);
	tr[1].position = glm::vec3(0.0f, 0.75f * 1.5f, 0.0f);
	tr[1].scale = glm::vec3(0.5f, 0.75f, 0.25f);
	tr[2].position = glm::vec3(0.0f, 1.75f, 0.0f);
	tr[2].scale = glm::vec3(0.5f, 0.5f, 0.5f);
	tr[3].position = glm::vec3(0.25f * -0.5f, 0.75f * -0.5f, 0.0f);
	tr[3].scale = glm::vec3(0.25f, 0.75f, 0.25f);
	tr[4].position = glm::vec3(0.25f * 0.5f, 0.75f * -0.5f, 0.0f);
	tr[4].scale = glm::vec3(0.25f, 0.75f, 0.25f);
	tr[5].position = glm::vec3(0.0f, 0.75f * -0.5f, 0.0f);
	tr[5].scale = glm::vec3(0.25f, 0.75f, 0.25f);
	tr[6].position = glm::vec3(0.0f, 0.75f * -0.5f, 0.0f);
	tr[6].scale = glm::vec3(0.25f, 0.75f, 0.25f);
	tr[7].position = glm::vec3(-0.25f, 1.5f, 0.0f);
	tr[8].position = glm::vec3(0.25f, 1.5f, 0.0f);
	tr[9].position = glm::vec3(-0.25f * 0.5f, 0.75f, 0.0f);
	tr[10].position = glm::vec3(0.25f * 0.5f, 0.75f, 0.0f);

	trw[0].position = glm::vec3(-1.0f, 0.0f, 3.9f);
	trw[1].position = glm::vec3(1.0f, 0.0f, 3.9f);
	trw[2].position = glm::vec3(-3.0f, 0.0f, 4.0f);
	trw[3].position = glm::vec3(3.0f, 0.0f, 4.0f);

	trw[4].position = glm::vec3(0.0f, 0.0f, -4.0f);
	trw[5].position = glm::vec3(0.0f, 4.0f, 0.0f);
	trw[6].position = glm::vec3(0.0f, -4.0f, 0.0f);
	trw[7].position = glm::vec3(4.0f, 0.0f, 0.0f);
	trw[8].position = glm::vec3(-4.0f, 0.0f, 0.0f);

	trw[0].scale = glm::vec3(2.0f, 8.0f, 0.1f);
	trw[1].scale = glm::vec3(2.0f, 8.0f, 0.1f);
	trw[2].scale = glm::vec3(2.0f, 8.0f, 0.1f);
	trw[3].scale = glm::vec3(2.0f, 8.0f, 0.1f);

	trw[4].scale = glm::vec3(8.0f, 8.0f, 0.1f);
	trw[5].scale = glm::vec3(8.0f, 0.1f, 8.0f);
	trw[6].scale = glm::vec3(8.0f, 0.1f, 8.0f);
	trw[7].scale = glm::vec3(0.1f, 8.0f, 8.0f);
	trw[8].scale = glm::vec3(0.1f, 8.0f, 8.0f);
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(1280, 960);

	win_id = glutCreateWindow(windowTitle.c_str());
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

	InitTransform();
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

GLvoid drawViewport(int x, int y, int w, int h, int c)
{
	aspect = (GLfloat)w / h;

	projPerspective = glm::perspective(glm::radians(90.0f), aspect, 0.1f, 100.0f);
	projOrthographic = glm::ortho(-aspect, aspect, -1.0f, 1.0f, -2.0f, 2.0f);

	glViewport(x, y, w, h);
	glClear(GL_DEPTH_BUFFER_BIT);

	unsigned int modelLocation = glGetUniformLocation(shaderProgramID, "model_Transform");

	glm::mat4 viewMat = glm::mat4(1.0f);
	switch (c)
	{
	case 0:
		viewMat = glm::lookAt(camPos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		break;
	case 1:
		viewMat = glm::lookAt(glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		break;
	case 2:
		viewMat = glm::lookAt(glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		break;
	}

	glm::mat4 viewPersp = projPerspective * viewMat;
	glm::mat4 viewOrtho = projOrthographic * viewMat;

	for (int i = 1; i < 7; ++i)
	{
		glm::mat4 transform = glm::mat4(1.0f);

		for (int ti = i; ti >= 0; ti = parent[ti])
			transform = GetMatrix(tr[ti]) * transform;
		transform = GetMatrix(trc) * transform;

		transform = (c ? viewOrtho : viewPersp) * transform;

		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(transform));
		mesh.Draw(GL_TRIANGLES);
	}

	for (int i = 0; i < 9; ++i)
	{
		glm::mat4 transform = GetMatrix(trw[i]);

		transform = (c ? viewOrtho : viewPersp) * transform;

		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(transform));
		mesh.Draw(GL_TRIANGLES);
	}

	for (int i = 0; i < 3; ++i)
	{
		glm::mat4 transform = GetMatrix(obstacles[i]);

		transform = (c ? viewOrtho : viewPersp) * transform;

		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(transform));
		meshCylinder.Draw(GL_TRIANGLES);
	}

	glClear(GL_DEPTH_BUFFER_BIT);
	glUseProgram(shaderProgramID);

	glm::mat4 s = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f) * 1.0f);

	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(viewOrtho * s));
	meshGizmo.Draw(GL_LINES);
}

GLvoid drawScene()
{
	glClearColor(0.125f, 0.125f, 0.125f, 1.0f);
	glUseProgram(shaderProgramID);

	glClear(GL_COLOR_BUFFER_BIT);

	drawViewport(0, 0, sw, sh, 0);
	glutSwapBuffers();
}

GLvoid Timer()
{
	float lastTime = elapsedTime;
	clock_t t = clock();
	elapsedTime = static_cast<float>(t) * 0.001f;
	float dt = elapsedTime - lastTime;

	animFactor = glm::sin(elapsedTime * 10.0f);

	if (openTime > 0.0f)
	{
		float openFactor = (1.0f - glm::cos(glm::clamp(elapsedTime - openTime, 0.0f, 1.0f) * glm::pi<float>())) * 0.5f;
		trw[0].position.x = -1.0f - 4.0f * openFactor;
		trw[1].position.x = 1.0f + 4.0f * openFactor;
		trw[2].position.x = -3.0f - 2.0f * openFactor;
		trw[3].position.x = 3.0f + 2.0f * openFactor;
	}

	tr[7].rotation.x = animFactor * -16.0f * speed;
	tr[8].rotation.x = animFactor * 16.0f * speed;
	tr[9].rotation.x = animFactor * 12.0f * speed;
	tr[10].rotation.x = animFactor * -12.0f * speed;

	camPos = glm::rotate(glm::mat4(1.0f), 0.0f, glm::vec3(0.0f, 1.0f, 0.0f)) * glm::vec4(camPos, 1.0f);

	SimulatePlayer(dt);

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
	sw = w;
	sh = h;

	glViewport(0, 0, w, h);
}

GLvoid LoadPolygon(const char* fileName)
{
	meshCylinder.LoadFromFile("cylinder.obj");
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
	meshCylinder.MakeArrayBuffers();
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
	case 'w':
	case 'W':
		moveDir = glm::vec3(0.0f, 0.0f, -1.0f);
		break;
	case 'a':
	case 'A':
		moveDir = glm::vec3(-1.0f, 0.0f, 0.0f);
		break;
	case 's':
	case 'S':
		moveDir = glm::vec3(0.0f, 0.0f, 1.0f);
		break;
	case 'd':
	case 'D':
		moveDir = glm::vec3(1.0f, 0.0f, 0.0f);
		break;
	case '+':
		speed += 1.0f;
		break;
	case '-':
		speed = glm::max(speed - 1.0f, 1.0f);
		break;
	case 'j':
	case 'J':
		velocity.y = 10.0f;
		break;
	case 'i':
	case 'I':
		InitTransform();
		speed = 4.0f;
		openTime = 0.0f;
		camPos = glm::vec3(0.0f, 0.0f, 8.0f);
		break;
	case 'z':
	case 'Z':
		camPos.z += upper ? -0.2f : 0.2f;
		break;
	case 'x':
	case 'X':
		camPos.x += upper ? -0.2f : 0.2f;
		break;
	case 'y':
	case 'Y':
		camPos = glm::rotate(glm::mat4(1.0f), upper ? -0.1f : 0.1f, glm::vec3(0.0f, 1.0f, 0.0f)) * glm::vec4(camPos, 1.0f);
		break;
	case 'q':
	case 'Q':
		glutDestroyWindow(win_id);
		return;
	default:
		openTime = elapsedTime;
		break;
	}
}

float LerpAngle(float a, float b, float t)
{
	float da = fmod(b - a, 360.0f);
	return  a + (fmod(da * 2.0f, 360.0f) - da) * t;
}

void SimulatePlayer(float dt)
{
	static float lastAngle = 0.0f;
	velocity.x = moveDir.x * speed;
	velocity.y += -40.0f * dt;
	velocity.z = moveDir.z * speed;

	glm::vec3 p = trc.position + velocity * dt;
	
	if (p.x < -4.0f)
	{
		moveDir.x = -moveDir.x;
		p.x = -8.0f - p.x;
	}
	else if (p.x > 4.0f)
	{
		moveDir.x = -moveDir.x;
		p.x = 8.0f - p.x;
	}
	if (p.z < -4.0f)
	{
		moveDir.z = -moveDir.z;
		p.z = -8.0f - p.z;
	}
	else if (p.z > 4.0f)
	{
		moveDir.z = -moveDir.z;
		p.z = 8.0f - p.z;
	}
	if (p.y < -4.0f)
	{
		velocity.y = 0.0f;
		p.y = -4.0f;
	}

	for (int i = 0; i < 3; ++i)
	{
		glm::vec3 o = obstacles[i].position;
		float dist = glm::distance(glm::vec2(p.x, p.z), glm::vec2(o.x, o.z));
		if (dist < 1.0f)
		{
			if (trc.position.y >= -3.0f)
			{
				if (p.y < -3.0f)
				{
					velocity.y = 0.0f;
					p.y = -3.0f;
				}
			}
			else
			{
				glm::vec2 l = glm::vec2(-moveDir.x, -moveDir.z);
				glm::vec2 n = glm::normalize(glm::vec2(p.x, p.z) - glm::vec2(o.x, o.z));
				glm::vec2 r = n * 2.0f * glm::dot(n, l) - l;
				moveDir.x = r.x;
				moveDir.z = r.y;
			}
		}
	}

	trc.position = p;
	float targetAngle = -atan2f(moveDir.z, moveDir.x) * 180.0f / glm::pi<float>() + 90.0f;
	lastAngle = LerpAngle(lastAngle, targetAngle, dt * 10.0f);
	trc.rotation.y = lastAngle;
}
