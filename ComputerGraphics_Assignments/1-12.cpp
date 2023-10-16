#include "pch.h"
#include "Mesh.hpp"

Mesh meshes[6];
glm::vec2 shapePositions[15];
glm::vec2 shapeVelocity[15];
bool states[15];
int shapes[15];
int moveModes[15];
bool suspended[15];

time_t e_time = 0;
int drag_index = -1;

GLvoid LoadPolygon(const char* fileName);
GLvoid InitBuffer();

GLuint MakeVertexShaders(const char* fileName);
GLuint MakeFragmentShaders(const char* fileName);
GLuint MakeShaderProgram(GLuint vertShader, GLuint fragShader);

GLvoid drawScene(GLvoid);
GLvoid Keyboard(unsigned char key, int x, int y);
GLvoid Reshape(int w, int h);
GLvoid Mouse(int button, int state, int x, int y);
GLvoid MouseMotion(int x, int y);
GLvoid Timer(int delta);

char* FileToBuffer(const char* file);
clock_t last_time;

GLuint shaderProgramID;
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

	for (int i = 0; i < 15; ++i)
	{
		shapePositions[i] = glm::vec2((float)(rand() % 2000 - 1000) / 1000, (float)(rand() % 2000 - 1000) / 1000);
		shapes[i] = i % 5 + 1;
	}

	glutDisplayFunc(drawScene);
	glutKeyboardFunc(Keyboard);
	glutReshapeFunc(Reshape);
	glutMouseFunc(Mouse);
	glutMotionFunc(MouseMotion);
	glutTimerFunc(0, Timer, 0);

	glutMainLoop();

	return 0;
}

GLvoid drawPolygon(float radius, float edges)
{
	glBegin(edges > 2.0f ? GL_POLYGON : GL_LINES);

	if (edges > 5.0f)
	{
		radius *= 6.0f - edges;
		edges = 5.0f;
	}

	float offset = glm::radians(360.0f / edges);
	float start = -(glm::pi<float>() + offset) * 0.5f;
	for (int i = 0; i < ceil(edges); ++i)
	{
		float x = cos(start + offset * i) * radius;
		float y = sin(start + offset * i) * radius;
		glVertex3f(x, y, 0.0f);
	}

	glEnd();
}

GLvoid drawScene()
{
	clock_t c = clock();

	glClearColor(0.125f, 0.125f, 0.125f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(shaderProgramID);

	for (int i = 0; i < 15; ++i)
	{
		if (suspended[i])
			continue;

		glm::mat4 t = glm::translate(glm::mat4(1.0f), glm::vec3(shapePositions[i], 0.0f) + vPos);
		glm::mat4 r = glm::rotate(glm::mat4(1.0f), 0.0f, glm::vec3(0.0f, 0.0f, 1.0f));
		glm::mat4 s = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f) * 0.15f);
		glm::mat4 transform = mProj * t * r * s;

		unsigned int modelLocation = glGetUniformLocation(shaderProgramID, "model_Transform");
		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(transform));

		int ip = i + 1;
		glm::vec3 col = glm::vec3(ip / 4 % 2, ip / 2 % 2, ip % 2);

		modelLocation = glGetUniformLocation(shaderProgramID, "model_Color");
		glUniform3f(modelLocation, col.r, col.b, col.g);

		meshes[shapes[i] - 1].DrawPolygon();
	}

	glutSwapBuffers();
}

GLvoid Timer(int delta)
{
	time_t e_time_new = clock();
	float dt = std::min((e_time_new - e_time) / 1000.0f, 0.1f);
	e_time = e_time_new;

	for (int i = 0; i < 15; ++i)
	{
		if (i == drag_index || suspended[i])
			continue;

		float x = shapePositions[i].x;
		float y = shapePositions[i].y;

		float d0 = x + 1.0f;
		float d1 = y + 1.0f;
		float d2 = x - 1.0f;
		float d3 = y - 1.0f;
		float force = dt * 0.05f;

		if (moveModes[i] == 0)
		{
			shapePositions[i] += glm::vec2(1.0f, 0.0f) / (d0 * d0) * force;
			shapePositions[i] += glm::vec2(0.0f, 1.0f) / (d1 * d1) * force;
			shapePositions[i] += glm::vec2(-1.0f, 0.0f) / (d2 * d2) * force;
			shapePositions[i] += glm::vec2(0.0f, -1.0f) / (d3 * d3) * force;
		}

		for (int j = i + 1; j < 15; ++j)
		{
			if (i == drag_index || suspended[i])
				continue;

			glm::vec2 dv = shapePositions[i] - shapePositions[j];
			float sqrmag = dv.x * dv.x + dv.y * dv.y;
			glm::vec2 dvn = dv / sqrt(sqrmag);

			shapePositions[j] -= dvn / sqrmag * force;
			shapePositions[i] += dvn / sqrmag * force;
		}

		if (moveModes[i] == 0)
			continue;

		shapePositions[i] += shapeVelocity[i] * dt * 2.0f;

		if (shapePositions[i].x < -0.9f)
		{
			shapeVelocity[i].x = -shapeVelocity[i].x;
			shapePositions[i].x = -1.8f - shapePositions[i].x;
			if (moveModes[i] == 2)
				shapePositions[i].y += states[i] ? 0.2f : -0.2f;
		}
		else if (shapePositions[i].x > 0.9f)
		{
			shapeVelocity[i].x = -shapeVelocity[i].x;
			shapePositions[i].x = 1.8f - shapePositions[i].x;
			if (moveModes[i] == 2)
				shapePositions[i].y += states[i] ? 0.2f : -0.2f;
		}
		if (shapePositions[i].y < -0.9f)
		{
			shapeVelocity[i].y = -shapeVelocity[i].y;
			shapePositions[i].y = -1.8f - shapePositions[i].y;
			if (moveModes[i] == 2)
				states[i] = !states[i];
		}
		else if (shapePositions[i].y > 0.9f)
		{
			shapeVelocity[i].y = -shapeVelocity[i].y;
			shapePositions[i].y = 1.8f - shapePositions[i].y;
			if (moveModes[i] == 2)
				states[i] = !states[i];
		}
	}

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
	for (auto& mesh : meshes)
		mesh.MakeArrayBuffers();
}

GLvoid LoadPolygon(const char* fileName)
{
	for (int i = 0; i < 6; ++i)
	{
		int edges = i + 1;
		if (i == 0)
			edges = 4;
		float radius = 1.0f;

		float offset = glm::radians(360.0f / edges);
		float start = -(glm::pi<float>() + offset) * 0.5f;
		for (int j = 0; j < edges; ++j)
		{
			float x = cos(start + offset * j) * radius;
			float y = sin(start + offset * j) * radius;

			if (i == 0)
			{
				x *= 0.1f;
				y *= 0.1f;
			}

			meshes[i].AppendVertex(glm::vec3(x, y, 0.0f));
			meshes[i].AppendColor(glm::vec3(1.0f, 1.0f, 1.0f));
			meshes[i].AppendIndex(j);
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
	glm::vec2 pos = ScreenToWorld(x, y);

	if (state == GLUT_DOWN)
	{
		for (int i = 0; i < 15; ++i)
		{
			if (suspended[i])
				continue;

			if (pos.x > shapePositions[i].x - 0.2f && pos.x < shapePositions[i].x + 0.2f && pos.y > shapePositions[i].y - 0.2f && pos.y < shapePositions[i].y + 0.2f)
			{
				drag_index = i;
				break;
			}
		}
	}
	else if (state == GLUT_UP && drag_index >= 0)
	{
		for (int i = 0; i < 15; ++i)
		{
			if (suspended[i])
				continue;
			if (i == drag_index)
				continue;

			glm::vec2 dv = shapePositions[i] - pos;
			if (dv.x * dv.x + dv.y * dv.y < 0.1f)
			{
				shapes[drag_index] = std::min(shapes[drag_index] + shapes[i], 6);
				moveModes[drag_index] = rand() % 2 + 1;
				switch (moveModes[drag_index])
				{
				case 1:
					shapeVelocity[drag_index] = glm::vec3(-1.0f, 1.0f, 0.0f);
					break;
				case 2:
					shapeVelocity[drag_index] = glm::vec3(1.0f, 0.0f, 0.0f);
					break;
				}
				suspended[i] = true;
				break;
			}
		}

		drag_index = -1;
	}
}

GLvoid MouseMotion(int x, int y)
{
	glm::vec2 pos = ScreenToWorld(x, y);

	if (drag_index >= 0)
		shapePositions[drag_index] = pos;

	glutPostRedisplay();
}