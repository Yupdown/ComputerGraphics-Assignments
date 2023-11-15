#include "pch.h"

using namespace ygl;

ObjectCamera* camera;

Shader* shader;
Shader* shaderPhong;

ObjectModel* obj;

Mesh* meshCube;
Mesh* meshCone;

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
	meshCone->LoadFromFile("yup.obj");
	meshCone->MakeGLObjects();

	camera = new ObjectCameraPerspective(glm::radians(60.0f), 0.1f, 100.0f);
	camera->SetPosition(Vector3(0.0f, 1.0f, 2.0f));
	camera->SetRotation(Quaternion(Vector3(glm::radians(-30.0f), 0.0f, 0.0f)));

	Scene* mainScene = new Scene(camera);
	YGLFramework::SwitchScene(mainScene);

	obj = new ObjectModel(meshCone, shaderPhong);
	mainScene->AddChild(obj);

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

	obj->SetRotation(Quaternion(Vector3(0.0f, t, 0.0f)));

	shaderPhong->SetUniformVector3("light_Direction", glm::normalize(Vector3(0.0f, 1.0f, 0.0f)));
	shaderPhong->SetUniformVector3("view_Position", camera->GetPosition());

	if (Input::GetKeyDown('N'))
		obj->SetMesh(obj->GetMesh() == meshCube ? meshCone : meshCube);
	if (Input::GetKeyDown('M'))
		;
	if (Input::GetKeyDown('Y'))
		;
	if (Input::GetKeyDown('R'))
		;
	if (Input::GetKeyDown('Z'))
		;
	if (Input::GetKeyDown('Q'))
		PostQuitMessage(0);
}