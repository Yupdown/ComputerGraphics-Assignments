#include "pch.h"

using namespace ygl;

ObjectCamera* camera;

Shader* shader;
Shader* shaderPhong;

ObjectModel* obj;
Object* objLight;
Object* objLightAnchor;

Mesh* meshCube;
Mesh* meshCone;

float lightIntensity = 1.0f;
bool light = true;

void Update();

int main(int argc, char** argv)
{
	YGLFramework::Initialize(argc, argv);

	shader = new Shader("yglvert.glsl", "yglfrag.glsl");
	shaderPhong = new Shader("yglvertphong.glsl", "yglfragphong.glsl");

	meshCube = new Mesh();
	meshCube->LoadFromFile("cube.obj");
	meshCube->MakeGLObjects();

	meshCone = new Mesh();
	meshCone->LoadFromFile("cone.obj");
	meshCone->MakeGLObjects();

	camera = new ObjectCameraPerspective(glm::radians(90.0f), 0.1f, 100.0f);
	camera->SetPosition(Vector3(0.0f, 1.0f, 2.0f));
	camera->SetRotation(Quaternion(Vector3(glm::radians(-30.0f), 0.0f, 0.0f)));

	Scene* mainScene = new Scene(camera);
	YGLFramework::SwitchScene(mainScene);

	obj = new ObjectModel(meshCone, shaderPhong);

	objLight = new ObjectModel(meshCube, shader);
	objLightAnchor = new Object();

	objLightAnchor->AddChild(objLight);

	mainScene->AddChild(objLightAnchor);
	mainScene->AddChild(obj);

	objLight->SetPositionX(2.0f);
	objLight->SetScale(0.25f);
	objLightAnchor->SetRotation(Quaternion(Vector3(0.0f, -45.0f, 0.0f)));

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	YGLFramework::SetUpdateHandler(Update);
	YGLFramework::BeginUpdateLoop();

	delete shader;
	delete mainScene;
	delete meshCube;
	delete meshCone;
	delete camera;
}

void Update()
{
	float t = clock() / 1000.0f;
	static float lt = 0.0f;
	float dt = glm::min(t - lt, 0.02f);
	lt = t;

	obj->SetRotation(Quaternion(Vector3(t * 0.5f, t, 0.0f)));

	lightIntensity = lightIntensity + (light - lightIntensity) * dt * 16.0f;

	shaderPhong->SetUniformVector3("light_Direction", objLightAnchor->GetRotation() * Vector3(1.0f, 0.0f, 0.0f));
	shaderPhong->SetUniformVector3("view_Position", camera->GetPosition());
	shaderPhong->SetUniformVector3("ambient_Color", Vector3(0.2f, 0.2f, 0.2f));
	shaderPhong->SetUniformVector3("diffuse_Color", Vector3(0.8f, 0.8f, 0.8f) * lightIntensity);
	shaderPhong->SetUniformVector3("specular_Color", Vector3(1.0f, 1.0f, 1.0f) * lightIntensity);

	if (Input::GetKeyDown('N'))
		obj->SetMesh(obj->GetMesh() == meshCube ? meshCone : meshCube);
	if (Input::GetKeyDown('M'))
		light = !light;
	if (Input::GetKey('W'))
		objLight->SetPositionX(objLight->GetPosition().x - dt);
	if (Input::GetKey('A'))
		objLightAnchor->SetRotation(Quaternion(Vector3(0.0f, dt, 0.0f)) * objLightAnchor->GetRotation());
	if (Input::GetKey('S'))
		objLight->SetPositionX(objLight->GetPosition().x + dt);
	if (Input::GetKey('D'))
		objLightAnchor->SetRotation(Quaternion(Vector3(0.0f, -dt, 0.0f)) * objLightAnchor->GetRotation());
	if (Input::GetKeyDown('Q'))
		PostQuitMessage(0);
}