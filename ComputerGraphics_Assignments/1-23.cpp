#include "pch.h"

using namespace ygl;

ObjectCamera* camera;
Object* origin;

World phyWorld(Vec2(0.0f, -20.0f), 10);

Body objectBody;
Body groundBody;

void Update();

int main(int argc, char** argv)
{
	YGLFramework::Initialize(argc, argv);

	Shader* shader = new Shader("yglvert.glsl", "yglfrag.glsl");
	Mesh* mesh = new Mesh();
	mesh->LoadFromFile("cube.obj");
	mesh->MakeGLObjects();

	camera = new ObjectCameraPerspective(glm::radians(60.0f), 0.1f, 100.0f);
	camera->SetPosition(Vector3(0.0f, 2.0f, 10.0f));
	camera->SetRotation(Quaternion(Vector3(glm::radians(0.0f), 0.0f, 0.0f)));

	Scene* mainScene = new Scene(camera);
	YGLFramework::SwitchScene(mainScene);

	origin = new ObjectModel(mesh, shader);
	origin->SetPosition(Vector3(0.0f, 0.0f, 0.0f));
	mainScene->AddObject(origin);

	groundBody.Set(Vec2(100.0f, 20.0f), FLT_MAX);
	groundBody.position.Set(0.0f, -0.5f * groundBody.width.y);
	groundBody.friction = 1.0f;

	objectBody.Set(Vec2(1.0f, 1.0f), 200.0f);
	objectBody.position.Set(0.0f, 30.0f);
	objectBody.friction = 0.1f;
	objectBody.rotation = 60.0f;

	phyWorld.Add(&objectBody);
	phyWorld.Add(&groundBody);

	YGLFramework::SetUpdateHandler(Update);
	YGLFramework::BeginUpdateLoop();

	delete origin;
	delete shader;
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

	phyWorld.Step(dt);
	origin->SetPosition(Vector3(objectBody.position.x, objectBody.position.y, 0.0f));
	origin->SetRotation(Quaternion(Vector3(0.0f, 0.0f, objectBody.rotation)));

	if (Input::GetMouseDown(GLUT_LEFT_BUTTON))
	{
		objectBody.velocity.Set(-5.0f, 10.0f);
		objectBody.angularVelocity += 10.0f;
	}
	if (Input::GetMouseDown(GLUT_RIGHT_BUTTON))
	{
		objectBody.velocity.Set(5.0f, 10.0f);
		objectBody.angularVelocity -= 10.0f;
	}
}