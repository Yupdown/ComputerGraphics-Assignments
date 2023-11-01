#include "pch.h"

using namespace ygl;

ObjectCamera* camera;
Object* axis;
Object* box;
Object* boxes[5];
vector<ObjectModel*> sphere;
vector<Vector3> velocity;
Body objectBody[5];
Shader* shader;
Mesh* sphereMesh;

World phyWorld(Vec2(0.0f, -40.0f), 10);

Body groundBody[4];

void Update();

void AddSphere(Mesh* mesh, Shader* shader)
{
	static std::default_random_engine dre;
	static std::uniform_real_distribution urd(-1.0f, 1.0f);

	ObjectModel* ref = new ObjectModel(mesh, shader);
	sphere.push_back(ref);
	ref->SetPosition(Vector3(urd(dre), urd(dre), urd(dre)) * 4.0f);
	axis->AddChild(ref);
	velocity.push_back(Vector3(urd(dre), urd(dre), urd(dre)) * 4.0f);
}

void UpdateGroundBodies()
{
	for (int i = 0; i < 4; ++i)
	{
		float theta = i * 0.5f * glm::pi<float>() + glm::eulerAngles(axis->GetRotation()).z;
		float radius = 5.0f + 0.5f;
		groundBody[i].position = Vec2(glm::cos(theta) * radius, glm::sin(theta) * radius);
		groundBody[i].rotation = theta + glm::pi<float>() * 0.5f;
	}
}

int main(int argc, char** argv)
{
	YGLFramework::Initialize(argc, argv);

	shader = new Shader("yglvert.glsl", "yglfrag.glsl");
	Mesh* mesh = new Mesh();
	mesh->LoadFromFile("cube.obj");
	mesh->MakeGLObjects();

	Mesh* meshBack = new Mesh();
	meshBack->LoadFromFile("cube_cullback.obj");
	meshBack->MakeGLObjects();

	sphereMesh = new Mesh();
	sphereMesh->LoadFromFile("sphere.obj");
	sphereMesh->MakeGLObjects();

	camera = new ObjectCameraPerspective(glm::radians(60.0f), 0.1f, 100.0f);
	camera->SetPosition(Vector3(0.0f, 0.0f, 16.0f));
	camera->SetRotation(Quaternion(Vector3(glm::radians(0.0f), 0.0f, 0.0f)));

	Scene* mainScene = new Scene(camera);
	YGLFramework::SwitchScene(mainScene);

	for (int i = 0; i < 5; ++i)
	{
		boxes[i] = new ObjectModel(mesh, shader);
		boxes[i]->SetScale(Vector3(1.0, 1.0f, 1.0f) * static_cast<float>(i + 1) * 0.5f);
		mainScene->AddObject(boxes[i]);
	}

	axis = new Object();
	mainScene->AddObject(axis);

	box = new ObjectModel(meshBack, shader);
	box->SetPosition(Vector3(0.0f, 0.0f, 0.0f));
	box->SetScale(Vector3(1.0f, 1.0f, 1.0f) * 10.0f);
	axis->AddChild(box);

	for (int i = 0; i < 5; ++i)
		AddSphere(sphereMesh, shader);

	for (int i = 0; i < 4; ++i)
	{
		groundBody[i].Set(Vec2(10.0f, 1.0f), FLT_MAX);
		groundBody[i].friction = 10.0f;
		phyWorld.Add(&groundBody[i]);
	}
	UpdateGroundBodies();

	for (int i = 0; i < 5; ++i)
	{
		float scale = (i + 1) * 0.5f;
		objectBody[i].Set(Vec2(scale, scale), glm::pow(scale, 3.0f));
		objectBody[i].position.Set(0.0f, 1.0f);
		objectBody[i].friction = 0.1f;
		objectBody[i].rotation = 60.0f;
		phyWorld.Add(&objectBody[i]);
	}

	YGLFramework::SetUpdateHandler(Update);
	YGLFramework::BeginUpdateLoop();

	delete boxes;
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

	UpdateGroundBodies();
	phyWorld.Step(dt);

	for (int i = 0; i < 5; ++i)
	{
		boxes[i]->SetPosition(Vector3(objectBody[i].position.x, objectBody[i].position.y, 0.0f));
		boxes[i]->SetRotation(Quaternion(Vector3(0.0f, 0.0f, objectBody[i].rotation)));
	}

	if (Input::GetKey('X'))
		camera->SetPosition(camera->GetPosition() + Vector3(0.0f, 0.0f, 1.0f) * 10.0f * dt);
	if (Input::GetKey('Z'))
		camera->SetPosition(camera->GetPosition() + Vector3(0.0f, 0.0f, -1.0f) * 10.0f * dt);
	if (Input::GetKey('A'))
		axis->SetRotation(glm::rotate(axis->GetRotation(), 4.0f * dt, Vector3(0.0f, 0.0f, 1.0f)));
	if (Input::GetKey('D'))
		axis->SetRotation(glm::rotate(axis->GetRotation(), 4.0f * dt, Vector3(0.0f, 0.0f, -1.0f)));
	if (Input::GetKeyDown('B'))
		AddSphere(sphereMesh, shader);

	if (Input::GetMouseDown(GLUT_LEFT_BUTTON))
	{
		for (int i = 0; i < 5; ++i)
		{
			objectBody[i].velocity.Set(-5.0f, 10.0f);
			objectBody[i].angularVelocity += 10.0f;
		}
	}
	if (Input::GetMouseDown(GLUT_RIGHT_BUTTON))
	{
		for (int i = 0; i < 5; ++i)
		{
			objectBody[i].velocity.Set(5.0f, 10.0f);
			objectBody[i].angularVelocity -= 10.0f;
		}
	}
	if (Input::GetMouse(GLUT_LEFT_BUTTON))
		axis->SetRotation(Quaternion(Vector3(0.0f, 0.0f, Input::MousePosition().x * 0.004f)));

	for (int i = 0; i < sphere.size(); ++i)
	{
		Vector3 nv = sphere[i]->GetPosition() + velocity[i] * dt;
		Vector3 ns = velocity[i];

		if (nv.x < -4.0f || nv.x > 4.0f)
			ns.x = -ns.x;
		if (nv.y < -4.0f || nv.y > 4.0f)
			ns.y = -ns.y;
		if (nv.z < -4.0f || nv.z > 4.0f)
			ns.z = -ns.z;

		sphere[i]->SetPosition(nv);
		velocity[i] = ns;
	}
}