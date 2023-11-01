#include "pch.h"

using namespace ygl;

void UpdateHandler();

namespace
{
	std::unique_ptr<Shader> shader;
	std::unique_ptr<Mesh> meshCube, meshCylinder;
	std::unique_ptr<Scene> scene;
	std::unique_ptr<ObjectModel> object;
	std::unique_ptr<ObjectCameraPerspective> camera;
	
	constexpr int TILE_SIZE = 8;
	float tileHeight[TILE_SIZE][TILE_SIZE];
	float targetTileHeight[TILE_SIZE][TILE_SIZE];

	float openTime = 0.0f;
	float elapsedTime = 0.0f;

	glm::vec3 velocity[4];
	float lastAngle[4];
	glm::vec3 moveDir = glm::vec3(0.0f);
	float speed = 4.0f;

	float camRadius = 8.0f;
	float camTheta = 0.0f;

	int playerCount = 1;
}

class ObjectPlayer : public ygl::Object
{
private:
	Object* parts[11];

public:
	ObjectPlayer() : Object()
	{
		for (int i = 0; i < 11; ++i)
			parts[i] = i > 0 && i < 7 ? new ObjectModel(meshCube.get(), shader.get()) : new Object();

		parts[0]->SetScale(glm::vec3(0.5f, 0.5f, 0.5f));
		parts[1]->SetPosition(glm::vec3(0.0f, 0.75f * 1.5f, 0.0f));
		parts[1]->SetScale(glm::vec3(0.5f, 0.75f, 0.25f));
		parts[2]->SetPosition(glm::vec3(0.0f, 1.75f, 0.0f));
		parts[2]->SetScale(glm::vec3(0.5f, 0.5f, 0.5f));
		parts[3]->SetPosition(glm::vec3(0.25f * -0.5f, 0.75f * -0.5f, 0.0f));
		parts[3]->SetScale(glm::vec3(0.25f, 0.75f, 0.25f));
		parts[4]->SetPosition(glm::vec3(0.25f * 0.5f, 0.75f * -0.5f, 0.0f));
		parts[4]->SetScale(glm::vec3(0.25f, 0.75f, 0.25f));
		parts[5]->SetPosition(glm::vec3(0.0f, 0.75f * -0.5f, 0.0f));
		parts[5]->SetScale(glm::vec3(0.25f, 0.75f, 0.25f));
		parts[6]->SetPosition(glm::vec3(0.0f, 0.75f * -0.5f, 0.0f));
		parts[6]->SetScale(glm::vec3(0.25f, 0.75f, 0.25f));
		parts[7]->SetPosition(glm::vec3(-0.25f, 1.5f, 0.0f));
		parts[8]->SetPosition(glm::vec3(0.25f, 1.5f, 0.0f));
		parts[9]->SetPosition(glm::vec3(-0.25f * 0.5f, 0.75f, 0.0f));
		parts[10]->SetPosition(glm::vec3(0.25f * 0.5f, 0.75f, 0.0f));

		parts[0]->AddChild(parts[1]);
		parts[0]->AddChild(parts[2]);
		parts[7]->AddChild(parts[3]);
		parts[8]->AddChild(parts[4]);
		parts[9]->AddChild(parts[5]);
		parts[10]->AddChild(parts[6]);
		parts[0]->AddChild(parts[7]);
		parts[0]->AddChild(parts[8]);
		parts[0]->AddChild(parts[9]);
		parts[0]->AddChild(parts[10]);
		AddChild(parts[0]);
	}

	~ObjectPlayer() override
	{
		for (int i = 0; i < 11; ++i)
			delete parts[i];
	}

	void UpdateAnimation(float factor, float speed)
	{
		parts[7]->SetRotation(Quaternion(Vector3(factor * -16.0f * speed, 0.0f, 0.0f)));
		parts[8]->SetRotation(Quaternion(Vector3(factor * 16.0f * speed, 0.0f, 0.0f)));
		parts[9]->SetRotation(Quaternion(Vector3(factor * 12.0f * speed, 0.0f, 0.0f)));
		parts[10]->SetRotation(Quaternion(Vector3(factor * -12.0f * speed, 0.0f, 0.0f)));
	}
};

ObjectPlayer* players[4];
ObjectModel* walls[8];
ObjectModel* cylinderObstacles[4];
ObjectModel* cylinderGate[2];
ObjectModel* floors[TILE_SIZE][TILE_SIZE];

float LerpAngle(float a, float b, float t)
{
	float da = fmod(b - a, 360.0f);
	return  a + (fmod(da * 2.0f, 360.0f) - da) * t;
}

void SimulatePlayer(float dt)
{
	velocity[0].x = moveDir.x * speed;
	velocity[0].y += -40.0f * dt;
	velocity[0].z = moveDir.z * speed;

	for (int i = 1; i < 4; ++i)
	{
		Vector3 delta = players[i - 1]->GetPosition() - players[i]->GetPosition();
		Vector2 delta2D = Vector2(delta.x, delta.z);
		Vector2 dir = glm::length(delta2D) > 0.0f ? glm::normalize(delta2D) : Vector2(0.0f, 0.0f);
		float s = speed * glm::pow(0.8f, i);
		velocity[i] = Vector3(dir.x * s, velocity[i].y - 40.0f * dt, dir.y * s);
	}

	for (int i = 0; i < 4; ++i)
	{
		glm::vec3 p = players[i]->GetPosition() + velocity[i] * dt;

		if (i == 0)
		{
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

			for (int i = 0; i < 4; ++i)
			{
				glm::vec3 o = cylinderObstacles[i]->GetPosition();
				float dist = glm::distance(glm::vec2(p.x, p.z), glm::vec2(o.x, o.z));
				if (dist < 0.5f)
				{
					glm::vec2 l = glm::vec2(-moveDir.x, -moveDir.z);
					glm::vec2 n = glm::normalize(glm::vec2(p.x, p.z) - glm::vec2(o.x, o.z));
					glm::vec2 r = n * 2.0f * glm::dot(n, l) - l;
					moveDir.x = r.x;
					moveDir.z = r.y;
				}
			}

			for (int i = 0; i < 2; ++i)
			{
				glm::vec3 o = cylinderGate[i]->GetPosition();
				float dist = glm::distance(glm::vec2(p.x, p.z), glm::vec2(o.x, o.z));
				if (p.y < 0.0f && dist < 0.5f)
				{
					glm::vec2 l = glm::vec2(-moveDir.x, -moveDir.z);
					glm::vec2 n = glm::normalize(glm::vec2(p.x, p.z) - glm::vec2(o.x, o.z));
					glm::vec2 r = n * 2.0f * glm::dot(n, l) - l;
					moveDir.x = r.x;
					moveDir.z = r.y;
				}
			}
		}

		Vector2Int tp = Vector2Int(
			glm::clamp<int>(floor(p.x + TILE_SIZE * 0.5f), 0, TILE_SIZE - 1),
			glm::clamp<int>(floor(p.z + TILE_SIZE * 0.5f), 0, TILE_SIZE - 1)
		);
		float ground = -4.0f + tileHeight[tp.x][tp.y];

		if (p.y < ground)
		{
			velocity[i].y = 0.0f;
			p.y = ground;
			tileHeight[tp.x][tp.y] = tileHeight[tp.x][tp.y] + (targetTileHeight[tp.x][tp.y] - tileHeight[tp.x][tp.y]) * dt * 10.0f;
		}

		float targetAngle = -atan2f(velocity[i].z, velocity[i].x) * 180.0f / glm::pi<float>() + 90.0f;
		lastAngle[i] = LerpAngle(lastAngle[i], targetAngle, dt * 10.0f);

		players[i]->SetPosition(p);
		players[i]->SetRotation(Quaternion(Vector3(0.0f, glm::radians(lastAngle[i]), 0.0f)));
	}
}

int main(int argc, char* argv)
{
	YGLFramework::Initialize(argc, &argv);

	shader = std::make_unique<Shader>("yglvert.glsl", "yglfragdepth.glsl");
	camera = std::make_unique<ObjectCameraPerspective>(glm::radians(90.0f), 0.1f, 100.0f);
	scene = std::make_unique<Scene>(camera.get());
	object = std::make_unique<ObjectModel>(meshCylinder.get(), shader.get());

	meshCube = std::make_unique<Mesh>();
	meshCylinder = std::make_unique<Mesh>();
	meshCube->LoadFromFile("cube.obj");
	meshCylinder->LoadFromFile("cylinder.obj");
	meshCube->MakeGLObjects();
	meshCylinder->MakeGLObjects();

	for (int i = 0; i < 4; ++i)
	{
		players[i] = new ObjectPlayer();
		players[i]->SetScale(Vector3(1.0f, 1.0f, 1.0f) * (float)glm::pow(0.8f, i));
	}
	scene->AddObject(players[0]);

	for (int i = 0; i < 8; ++i)
	{
		walls[i] = new ObjectModel(meshCube.get(), shader.get());
		scene->AddObject(walls[i]);
	}
	moveDir = glm::normalize(glm::vec3(1.0f, 0.0f, 0.3f));

	walls[0]->SetPosition(glm::vec3(-1.0f, 0.0f, 3.9f));
	walls[1]->SetPosition(glm::vec3(1.0f, 0.0f, 3.9f));
	walls[2]->SetPosition(glm::vec3(-3.0f, 0.0f, 4.0f));
	walls[3]->SetPosition(glm::vec3(3.0f, 0.0f, 4.0f));

	walls[4]->SetPosition(glm::vec3(0.0f, 0.0f, -4.0f));
	walls[5]->SetPosition(glm::vec3(0.0f, 4.0f, 0.0f));
	walls[6]->SetPosition(glm::vec3(4.0f, 0.0f, 0.0f));
	walls[7]->SetPosition(glm::vec3(-4.0f, 0.0f, 0.0f));

	walls[0]->SetScale(glm::vec3(2.0f, 8.0f, 0.1f));
	walls[1]->SetScale(glm::vec3(2.0f, 8.0f, 0.1f));
	walls[2]->SetScale(glm::vec3(2.0f, 8.0f, 0.1f));
	walls[3]->SetScale(glm::vec3(2.0f, 8.0f, 0.1f));

	walls[4]->SetScale(glm::vec3(8.0f, 8.0f, 0.1f));
	walls[5]->SetScale(glm::vec3(8.0f, 0.1f, 8.0f));
	walls[6]->SetScale(glm::vec3(0.1f, 8.0f, 8.0f));
	walls[7]->SetScale(glm::vec3(0.1f, 8.0f, 8.0f));

	for (int i = 0; i < 4; ++i)
	{
		cylinderObstacles[i] = new ObjectModel(meshCylinder.get(), shader.get());
		cylinderObstacles[i]->SetScale(Vector3(1.0f, 8.0f, 1.0f));
		scene->AddObject(cylinderObstacles[i]);
	}

	cylinderObstacles[0]->SetPosition(Vector3(-3.0f, -4.0f, -3.0f));
	cylinderObstacles[1]->SetPosition(Vector3(-3.0f, -4.0f, 3.0f));
	cylinderObstacles[2]->SetPosition(Vector3(3.0f, -4.0f, -3.0f));
	cylinderObstacles[3]->SetPosition(Vector3(3.0f, -4.0f, 3.0f));

	for (int i = 0; i < 2; ++i)
	{
		cylinderGate[i] = new ObjectModel(meshCylinder.get(), shader.get());
		cylinderGate[i]->SetScale(Vector3(1.0f, 4.0f, 1.0f));
		scene->AddObject(cylinderGate[i]);
	}

	cylinderGate[0]->SetPosition(Vector3(-1.5f, -4.0f, 0.0f));
	cylinderGate[1]->SetPosition(Vector3(1.5f, -4.0f, 0.0f));

	ObjectModel* gateCeil = new ObjectModel(meshCube.get(), shader.get());
	gateCeil->SetScale(Vector3(4.0f, 0.5f, 1.0f));
	scene->AddObject(gateCeil);

	for (int i = 0; i < TILE_SIZE; ++i)
	{
		for (int j = 0; j < TILE_SIZE; ++j)
		{
			floors[i][j] = new ObjectModel(meshCube.get(), shader.get());
			floors[i][j]->SetPosition(Vector3(i - (TILE_SIZE - 1) * 0.5f, -4.0f - 0.5f, j - (TILE_SIZE - 1) * 0.5f));
			floors[i][j]->SetRotation(Vector3((i + j) % 2 ? 0.0f : glm::pi<float>(), 0.0f, 0.0f));
			scene->AddObject(floors[i][j]);
		}
	}

	tileHeight[2][1] = 1.0f;
	tileHeight[6][4] = 1.0f;
	tileHeight[3][5] = 1.0f;
	targetTileHeight[6][4] = -0.5f;
	targetTileHeight[3][5] = -0.5f;

	//scene->AddObject(object.get());
	YGLFramework::SwitchScene(scene.get());

	YGLFramework::SetUpdateHandler(UpdateHandler);
	YGLFramework::BeginUpdateLoop();

	return 0;
}

void UpdateHandler()
{
	float lastTime = elapsedTime;
	clock_t t = clock();
	elapsedTime = static_cast<float>(t) * 0.001f;
	float dt = elapsedTime - lastTime;

	object->SetPosition(Vector3(0.0f, 0.0f, -2.0f));
	object->SetRotation(Quaternion(Vector3(0.0f, elapsedTime * 10.0f, 0.0f)));
	

	if (Input::GetKeyDown('W'))
		moveDir = glm::vec3(0.0f, 0.0f, -1.0f);
	if (Input::GetKeyDown('A'))
		moveDir = glm::vec3(-1.0f, 0.0f, 0.0f);
	if (Input::GetKeyDown('S'))
		moveDir = glm::vec3(0.0f, 0.0f, 1.0f);
	if (Input::GetKeyDown('D'))
		moveDir = glm::vec3(1.0f, 0.0f, 0.0f);
	if (Input::GetKeyDown('J'))
		velocity[0].y = 10.0f;
	if (Input::GetKey(GLUT_KEY_UP))
		camRadius -= 2.0f * dt;
	if (Input::GetKey(GLUT_KEY_DOWN))
		camRadius += 2.0f * dt;
	if (Input::GetKey(GLUT_KEY_LEFT))
		camTheta -= dt;
	if (Input::GetKey(GLUT_KEY_RIGHT))
		camTheta += dt;
	if (Input::GetKeyDown('T'))
	{
		if (playerCount < 4)
			scene->AddObject(players[playerCount++]);
	}
	if (Input::GetKeyDown('F'))
	{
		velocity[1].y = 10.0f;
		velocity[2].y = 10.0f;
		velocity[3].y = 10.0f;
	}

	camera->SetPosition(Vector3(glm::sin(camTheta), 0.0f, glm::cos(camTheta)) * camRadius);
	camera->SetRotation(Vector3(0.0f, camTheta, 0.0f));

	if (Input::GetKeyDown('O'))
		openTime = elapsedTime;

	if (openTime > 0.0f)
	{
		float openFactor = (1.0f - glm::cos(glm::clamp(elapsedTime - openTime, 0.0f, 1.0f) * glm::pi<float>())) * 0.5f;
		walls[0]->SetPosition(Vector3(-1.0f - 4.0f * openFactor, 0.0f, 3.9f));
		walls[1]->SetPosition(Vector3(1.0f + 4.0f * openFactor, 0.0f, 3.9f));
		walls[2]->SetPosition(Vector3(-3.0f - 2.0f * openFactor, 0.0f, 4.0f));
		walls[3]->SetPosition(Vector3(3.0f + 2.0f * openFactor, 0.0f, 4.0f));
	}

	for (int i = 0; i < TILE_SIZE; ++i)
	{
		for (int j = 0; j < TILE_SIZE; ++j)
		{
			// tileHeight[i][j] = sin(i + j + elapsedTime * 4.0f) + 1.0f;
			floors[i][j]->SetScale(Vector3(1.0f, tileHeight[i][j] * 2.0f + 1.0f, 1.0f));
		}
	}

	SimulatePlayer(dt);
	for (int i = 0; i < 4; ++i)
		players[i]->UpdateAnimation(sin((elapsedTime + i) * 5.0f) * 1.0f, 0.1f);
}