#include "pch.h"

std::vector<GLfloat> VERTEX_DATA
{
	-1.0f, -1.0f, 0.0f,
	-1.0f, 1.0f, 0.0f,
	1.0f, -1.0f, 0.0f,
	1.0f, 1.0f, 0.0f,
};

std::vector<GLfloat> COLOR_DATA
{
	1.0f, 1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,
};

std::vector<GLuint> INDEX_DATA
{
	0, 2, 1, 3, 1, 2
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
glm::vec3 positions[5];
bool line = false;
int count = 1;

time_t e_time = 0;

GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);
GLvoid Mouse(int button, int state, int x, int y);
GLvoid Keyboard(unsigned char key, int x, int y);
GLvoid Timer();

GLvoid LoadPolygon(const char* fileName);
GLvoid InitBuffer();

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

	GLuint vert = MakeVertexShaders("vert.glsl");
	GLuint frag = MakeFragmentShaders("frag.glsl");
	shaderProgramID = MakeShaderProgram(vert, frag);
	glUseProgram(shaderProgramID);
	InitBuffer();

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	for (int i = 1; i < 5; ++i)
		positions[i] = glm::vec3((float)(rand() % 2000 - 1000) / 1000, (float)(rand() % 2000 - 1000) / 1000, 0.0f);

	glutDisplayFunc(drawScene);
	glutReshapeFunc(Reshape);
	glutKeyboardFunc(Keyboard);
	glutMouseFunc(Mouse);
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
	return glm::vec2(xp, -yp);
}

GLvoid drawScene()
{
	glm::vec3 col = colors[count - 1] * 0.25f;
	glClearColor(col.r, col.g, col.b, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(shaderProgramID);

	for (int c = 0; c < count; ++c)
	{
		for (float l = -4.0f + (line ? 0.0f : e_time % 20 * 0.005f); l < 4.0f; l += (line ? 0.01f : 0.1f))
		{
			float x = 0.0f;
			float y = 0.0f;
			float xp = l;
			float yp = pow(l, 3.0f);

			for (float i = 0.0f; i < 24.0f; i += 1.0f)
			{
				x += xp / (i * 4.0f + 1.5f + cos(e_time * 0.005f) * 0.25f);
				y += yp / (i * 4.0f + 3.5f + sin(e_time * 0.005f) * 0.25f);

				xp *= -pow(l, 4.0f) / (i * 2.0f + 1.0f) / (i * 2.0f + 2.0f);
				yp *= -pow(l, 4.0f) / (i * 2.0f + 2.0f) / (i * 2.0f + 3.0f);
			}

			glm::mat4 t = glm::translate(glm::mat4(1.0f), positions[c] + glm::vec3(x, y, 0.0f) * 0.25f + vPos);
			glm::mat4 r = glm::rotate(glm::mat4(1.0f), 0.0f, glm::vec3(0.0f, 0.0f, 1.0f));
			glm::mat4 s = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f) * 0.0025f);
			glm::mat4 transform = mProj * t * r * s;

			unsigned int modelLocation = glGetUniformLocation(shaderProgramID, "model_Transform");
			glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(transform));

			int ip = c + 1;
			glm::vec3 col = glm::vec3(ip / 4 % 2, ip / 2 % 2, ip % 2);

			modelLocation = glGetUniformLocation(shaderProgramID, "model_Color");
			glUniform3f(modelLocation, col.r, col.b, col.g);

			glBindVertexArray(VAO);
			glDrawElements(GL_TRIANGLES, INDEX_DATA.size(), GL_UNSIGNED_INT, 0);
		}
	}

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
	glm::vec2 mouse_position = screen_to_world(x, y);
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
		count = std::min(count + 1, 5);
	glutPostRedisplay();
}

GLvoid Timer()
{
	e_time = clock();
	glutPostRedisplay();
}

GLvoid Keyboard(unsigned char key, int x, int y)
{
	switch (toupper(key))
	{
	case 'P':
		line = false;
		break;
	case 'L':
		line = true;
		break;
	case '1':
		count = 1;
		break;
	case '2':
		count = 2;
		break;
	case '3':
		count = 3;
		break;
	case '4':
		count = 4;
		break;
	case '5':
		count = 5;
		break;
	}
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