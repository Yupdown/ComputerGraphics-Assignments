#include "pch.h"

using namespace ygl;

ObjectCamera* camera;

Shader* shaderPhong;

ObjectModel* obj[3];

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

int main(int argc, char** argv)
{
	YGLFramework::Initialize(argc, argv);

	shaderPhong = new Shader("yglvertphong.glsl", "yglfragphong_1-26.glsl");

	mesh = new Mesh();
	mesh->LoadFromFile("sphere.obj");
	mesh->MakeGLObjects();

	camera = new ObjectCameraPerspective(glm::radians(90.0f), 0.1f, 100.0f);
	camera->SetPosition(Vector3(0.0f, 1.0f, 2.0f));
	camera->SetRotation(Quaternion(Vector3(glm::radians(-30.0f), glm::radians(-30.0f), 0.0f)));

	Scene* mainScene = new Scene(camera);
	YGLFramework::SwitchScene(mainScene);

	for (int i = 0; i < 3; ++i)
	{
		obj[i] = new ObjectModel(mesh, shaderPhong);
		obj[i]->SetScale(0.25f);
		obj[i]->SetPositionX(i);
		mainScene->AddChild(obj[i]);
	}

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

	shaderPhong->SetUniformVector3("light_Direction", Vector3(sin(theta), 0.0f, cos(theta)));
	shaderPhong->SetUniformVector3("view_Position", camera->GetPosition());
	shaderPhong->SetUniformVector3("ambient_Color", Vector3(0.2f, 0.2f, 0.2f));
	shaderPhong->SetUniformVector3("diffuse_Color", lightColor * lightIntensity);
	shaderPhong->SetUniformVector3("specular_Color", Vector3(1.0f, 1.0f, 1.0f) * lightIntensity);

	for (int i = 0; i < 3; ++i)
	{
		obj[i]->SetPositionX(sin(t * i) * i);
		obj[i]->SetPositionZ(cos(t * i) * i);
	}

	if (Input::GetKeyDown('C'))
		targetIndex = (targetIndex + 1) % 3;
	if (Input::GetKey('A'))
		theta -= dt * 3.0f;
	if (Input::GetKey('D'))
		theta += dt * 3.0f;
	if (Input::GetKeyDown('Q'))
		PostQuitMessage(0);
}