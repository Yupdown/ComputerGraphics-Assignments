#include "pch.h"
#include "PerlinNoise.hpp"

using namespace ygl;

std::default_random_engine dre;
std::uniform_real_distribution<float> urd(-3.0f, 3.0f);
std::uniform_real_distribution<float> urds(0.5f, 1.0f);

ObjectCamera* camera;

Shader* shader;
Shader* shaderPhong;

Object* objLight;
Object* objPyramid;
Object* objPlanet[3];
Object* objPillar[5];

Mesh* mesh;
Mesh* mesh_pyramid;
Mesh* mesh_sphere;
Mesh* mesh_ring;

float lightIntensity = 1.0f;
float target_light = lightIntensity;
bool light = true;
bool flakes_on = false;

Vector3 lightColor;
Vector3 targetLight[3] =
{
	Vector3(1.0f, 1.0f, 0.0f),
	Vector3(0.0f, 1.0f, 1.0f),
	Vector3(1.0f, 0.0f, 1.0f)
};
int targetIndex = 0;
int light_position = 0;
float theta = 0.0f;
float radius = 2.0f;
float level = 0.0f;
float target_level = level;

class Flake : public ObjectModel
{
private:
	float timer;
	float speed;

public:
	Flake() : ObjectModel(mesh, shaderPhong)
	{
		timer = 1.0f;
		speed = urds(dre);
		SetPosition(Vector3(urd(dre), 3.0f, urd(dre)));
		SetScale(0.0f);
	}

	void Update(float dt)
	{
		Vector3 v = GetPosition();

		SetPositionY(v.y - speed * dt);
		SetScale(GetScale().x + (flakes_on * 0.02f - GetScale().x) * dt * 8.0f);

		if (GetPosition().y <= 0.0f)
		{
			SetPositionY(0.0f);
			timer -= dt;
		}
		else
		{
			SetPositionX(v.x + Perlin::Fbm(Vector3D(v.x, v.y, v.z), 2) * 2.0f * dt);
			SetPositionZ(v.z + Perlin::Fbm(Vector3D(-v.x, -v.y, -v.z), 2) * 2.0f * dt);
		}

		if (timer <= 0.0f)
		{
			SetPosition(Vector3(urd(dre), 3.0f, urd(dre)));
			timer = 1.0f;
		}
	}
};

constexpr int SIZEOF_FLAKES = 256;
Flake* flakes[SIZEOF_FLAKES];

void Update();

void MakePyramid(Object* parent, int n)
{
	if (n <= 0)
	{
		auto obj = new ObjectModel(mesh_pyramid, shaderPhong);
		parent->AddChild(obj);
		return;
	}
	for (int i = 0; i < 5; ++i)
	{
		auto obj = new Object();
		obj->SetScale(0.5f);
		switch (i)
		{
		case 0:
			obj->SetPosition(Vector3(0.0f, 0.5f, 0.0f));
			break;
		case 1:
			obj->SetPosition(Vector3(-0.25f, 0.0f, -0.25f));
			break;
		case 2:
			obj->SetPosition(Vector3(0.25f, 0.0f, -0.25f));
			break;
		case 3:
			obj->SetPosition(Vector3(-0.25f, 0.0f, 0.25f));
			break;
		case 4:
			obj->SetPosition(Vector3(0.25f, 0.0f, 0.25f));
			break;
		}
		parent->AddChild(obj);
		MakePyramid(obj, n - 1);
	}
}

int main(int argc, char** argv)
{
	YGLFramework::Initialize(argc, argv);

	shader = new Shader("yglvert.glsl", "yglfragdepth.glsl");
	shaderPhong = new Shader("yglvertphong.glsl", "yglfragphong_1-31.glsl");

	mesh = new Mesh();
	mesh->LoadFromFile("cube.obj");
	mesh->MakeGLObjects();

	mesh_pyramid = new Mesh();
	mesh_pyramid->LoadFromFile("pyramid.obj");
	mesh_pyramid->MakeGLObjects();

	mesh_sphere = new Mesh();
	mesh_sphere->LoadFromFile("sphere.obj");
	mesh_sphere->MakeGLObjects();

	mesh_ring = new Mesh();
	for (int i = 0; i < 32; ++i)
	{
		float rad = i / 16.0f * glm::pi<float>();
		mesh_ring->AppendVertex(Vector3(glm::cos(rad), 0.0f, glm::sin(rad)));
		mesh_ring->AppendColor(Vector4(1.0f, 1.0f, 1.0f, 1.0f));
		mesh_ring->AppendNormal(Vector3(glm::cos(rad), 0.0f, glm::sin(rad)));
		mesh_ring->AppendUV(Vector2(0.0f, 0.0f));
		mesh_ring->AppendIndex(i);
	}
	mesh_ring->MakeGLObjects();

	camera = new ObjectCameraPerspective(glm::radians(90.0f), 0.1f, 100.0f);
	camera->SetPosition(Vector3(0.0f, 1.0f, 2.0f));
	camera->SetRotation(Quaternion(Vector3(glm::radians(-30.0f), 0.0f, 0.0f)));

	Scene* mainScene = new Scene(camera);
	YGLFramework::SwitchScene(mainScene);

	objLight = new ObjectModel(mesh, shader);
	objLight->SetScale(0.1f);
	mainScene->AddChild(objLight);

	objPyramid = new Object();
	MakePyramid(objPyramid, 4);
	mainScene->AddChild(objPyramid);

	auto objGround = new ObjectModel(mesh, shaderPhong);
	objGround->SetPositionY(-0.5f);
	objGround->SetScale(Vector3(10.0f, 1.0f, 10.0f));
	mainScene->AddChild(objGround);

	for (int i = 0; i < 3; ++i)
	{
		objPlanet[i] = new Object();
		objPlanet[i]->SetPositionY(0.5f);

		auto objRing = new ObjectModel(mesh_ring, shaderPhong);
		objRing->SetDrawMode(GL_LINE_LOOP);
		objPlanet[i]->AddChild(objRing);

		auto objSphere = new ObjectModel(mesh_sphere, shaderPhong);
		objSphere->SetPositionX(1.0f);
		objSphere->SetScale(0.1f);
		objPlanet[i]->AddChild(objSphere);

		mainScene->AddChild(objPlanet[i]);
	}

	for (int i = 0; i < 5; ++i)
	{
		objPillar[i] = new ObjectModelBlend(mesh, shaderPhong, nullptr, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		objPillar[i]->SetPosition(Vector3(i - 2.0f, 0.5f, 1.0f));
		objPillar[i]->SetScale(Vector3(0.5f, 1.0f, 0.5f));
		mainScene->AddChild(objPillar[i]);
	}

	for (int i = 0; i < SIZEOF_FLAKES; ++i)
	{
		flakes[i] = new Flake();
		mainScene->AddChild(flakes[i]);
	}

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	YGLFramework::SetUpdateHandler(Update);
	YGLFramework::BeginUpdateLoop();

	delete objPyramid;
	delete mainScene;
	delete mesh;
	delete mesh_pyramid;
	delete camera;
}

void Update()
{
	float t = clock() / 1000.0f;
	static float lt = 0.0f;
	float dt = glm::min(t - lt, 0.02f);
	lt = t;

	lightIntensity = lightIntensity + (target_light * light - lightIntensity) * dt * 16.0f;
	lightColor = lightColor + (targetLight[targetIndex] - lightColor) * dt * 8.0f;

	objLight->SetPosition(Vector3(sin(theta) * radius, 0.5f, cos(theta) * radius));

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
	if (Input::GetKeyDown('F'))
		flakes_on = !flakes_on;
	if (Input::GetKey('A'))
		theta -= dt * 3.0f;
	if (Input::GetKey('D'))
		theta += dt * 3.0f;
	if (Input::GetKey('W'))
		radius -= dt;
	if (Input::GetKey('S'))
		radius += dt;
	if (Input::GetKeyDown('P'))
	{
		light_position = (light_position + 1) % 4;
		theta = glm::radians(45.0f + light_position * 90.0f);
		radius = 4.0f;
	}
	if (Input::GetKeyDown('+'))
		target_level = glm::min(target_level + 1.0f, 4.0f);
	if (Input::GetKeyDown('-'))
		target_level = glm::max(target_level - 1.0f, 0.0f);
	if (Input::GetKeyDown('['))
		target_light -= 0.2f;
	if (Input::GetKeyDown(']'))
		target_light += 0.2f;
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

	level = level + (target_level - level) * dt * 16.0f;
	objPyramid->SetScale(pow(2.0f, level));
	objPyramid->SetPositionY(1.0f - pow(2.0f, level));

	for (int i = 0; i < 3; ++i)
		objPlanet[i]->SetRotation(Quaternion(Vector3(t, t + i, t - i)));

	for (int i = 0; i < SIZEOF_FLAKES; ++i)
		flakes[i]->Update(dt);

	camera->SetPositionX(glm::cos(a) * r);
	camera->SetPositionZ(glm::sin(a) * r);
	camera->SetRotation(Quaternion(Vector3(0.0f, -a + glm::radians(90.0f), 0.0f)));
}