#include "pch.h"

using namespace ygl;

ObjectCamera* camera;

Shader* shader;
Shader* shaderPhong;

Object* obj[13];
Object* objLight;

Mesh* mesh;

float lightIntensity = 1.0f;
bool light = true;

Vector3 lightColor;
Vector3 targetLight[3] =
{
	Vector3(1.0f, 1.0f, 0.0f),
	Vector3(0.0f, 1.0f, 1.0f),
	Vector3(1.0f, 0.0f, 1.0f)
};
int targetIndex = 0;
float theta = 0.0f;

void Update();
void InitTransform();

int main(int argc, char** argv)
{
	YGLFramework::Initialize(argc, argv);

	shader = new Shader("yglvert.glsl", "yglfragdepth.glsl");
	shaderPhong = new Shader("yglvertphong.glsl", "yglfragphong_1-27.glsl");

	mesh = new Mesh();
	mesh->LoadFromFile("cube.obj");
	mesh->MakeGLObjects();

	camera = new ObjectCameraPerspective(glm::radians(90.0f), 0.1f, 100.0f);
	camera->SetPosition(Vector3(0.0f, 1.0f, 2.0f));
	camera->SetRotation(Quaternion(Vector3(glm::radians(-30.0f), 0.0f, 0.0f)));

	Scene* mainScene = new Scene(camera);
	YGLFramework::SwitchScene(mainScene);

	for (int i = 0; i < 13; ++i)
	{
		if (i < 7)
			obj[i] = new ObjectModel(mesh, shaderPhong);
		else
			obj[i] = new Object();
	}

	objLight = new ObjectModel(mesh, shader);
	objLight->SetScale(0.1f);
	mainScene->AddChild(objLight);

	mainScene->AddChild(obj[0]);
	obj[8]->AddChild(obj[1]);
	obj[7]->AddChild(obj[2]);
	obj[9]->AddChild(obj[3]);
	obj[10]->AddChild(obj[4]);
	obj[11]->AddChild(obj[5]);
	obj[12]->AddChild(obj[6]);
	obj[8]->AddChild(obj[7]);
	mainScene->AddChild(obj[8]);
	obj[7]->AddChild(obj[9]);
	obj[7]->AddChild(obj[10]);
	obj[8]->AddChild(obj[11]);
	obj[8]->AddChild(obj[12]);

	InitTransform();

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	YGLFramework::SetUpdateHandler(Update);
	YGLFramework::BeginUpdateLoop();

	delete mainScene;
	delete mesh;
	delete camera;
}

void Update()
{
	float t = clock() / 1000.0f;
	static float lt = 0.0f;
	float dt = glm::min(t - lt, 0.02f);
	lt = t;

	lightIntensity = lightIntensity + (light - lightIntensity) * dt * 16.0f;
	lightColor = lightColor + (targetLight[targetIndex] - lightColor) * dt * 8.0f;

	objLight->SetPosition(Vector3(sin(theta) * 2.0f, 0.5f, cos(theta) * 2.0f));

	shaderPhong->SetUniformVector3("light_Position", objLight->GetPosition());
	shaderPhong->SetUniformVector3("view_Position", camera->GetPosition());
	shaderPhong->SetUniformVector3("ambient_Color", Vector3(0.2f, 0.2f, 0.2f));
	shaderPhong->SetUniformVector3("diffuse_Color", lightColor * lightIntensity);
	shaderPhong->SetUniformVector3("specular_Color", Vector3(1.0f, 1.0f, 1.0f) * lightIntensity);

	Vector3 v3 = camera->GetPosition();
	float r = glm::length(Vector2(v3.x, v3.z));
	float a = glm::atan(v3.z, v3.x);

	if (Input::GetKeyDown('C'))
		targetIndex = (targetIndex + 1) % 3;
	if (Input::GetKeyDown('M'))
		light = !light;
	if (Input::GetKey('A'))
		theta -= dt * 3.0f;
	if (Input::GetKey('D'))
		theta += dt * 3.0f;
	if (Input::GetKey(GLUT_KEY_LEFT))
		a += dt;
	if (Input::GetKey(GLUT_KEY_RIGHT))
		a -= dt;
	if (Input::GetKey(GLUT_KEY_DOWN))
		r += dt;
	if (Input::GetKey(GLUT_KEY_UP))
		r -= dt;
	if (Input::GetKeyDown('Q'))
		PostQuitMessage(0);

	camera->SetPositionX(glm::cos(a) * r);
	camera->SetPositionZ(glm::sin(a) * r);
	camera->SetRotation(Quaternion(Vector3(0.0f, -a + glm::radians(90.0f), 0.0f)));
}

void InitTransform()
{
	obj[0]->SetScale(glm::vec3(10.0f, 0.1f, 10.0f));
	obj[1]->SetPosition(glm::vec3(0.0f, 0.15f, 0.0f));
	obj[1]->SetScale(glm::vec3(0.5f, 0.3f, 0.5f));
	obj[2]->SetPosition(glm::vec3(0.0f, 0.3f, 0.0f));
	obj[2]->SetScale(glm::vec3(0.3f, 0.3f, 0.3f));

	obj[3]->SetPosition(glm::vec3(0.0f, 0.15f, 0.0f));
	obj[3]->SetScale(glm::vec3(0.1f, 0.3f, 0.1f));
	obj[4]->SetPosition(glm::vec3(0.0f, 0.15f, 0.0f));
	obj[4]->SetScale(glm::vec3(0.1f, 0.3f, 0.1f));

	obj[5]->SetPosition(glm::vec3(0.0f, 0.0f, 0.15f));
	obj[5]->SetScale(glm::vec3(0.1f, 0.1f, 0.3f));
	obj[6]->SetPosition(glm::vec3(0.0f, 0.0f, 0.15f));
	obj[6]->SetScale(glm::vec3(0.1f, 0.1f, 0.3f));

	obj[11]->SetPosition(glm::vec3(-0.25f, 0.1f, 0.15f));
	obj[12]->SetPosition(glm::vec3(0.25f, 0.1f, 0.15f));

	obj[7]->SetRotation(Quaternion());
	obj[8]->SetPositionX(0.0f);
}