#include "pch.h"

using namespace ygl;

ObjectCamera* camera;

Shader* shader;
Shader* shader_screen;

Mesh* mesh_plane;
Mesh* mesh_cube;
Mesh* mesh_pyramid;

Texture* texture_test;
Texture* texture_screen;

Object* object_anchor;
Object* object_cube;
Object* object_pyramid;

bool mode;
bool rotate_x;
bool rotate_y;

void Update();

int main(int argc, char** argv)
{
	YGLFramework::Initialize(argc, argv);
	glDisable(GL_DEPTH_TEST);

	shader = new Shader("yglvert.glsl", "yglfragtexture.glsl");
	shader_screen = new Shader("yglvertscreen.glsl", "yglfragtexture.glsl");

	mesh_plane = new Mesh();
	mesh_plane->LoadFromFile("plane.obj");
	mesh_plane->MakeGLObjects();

	mesh_cube = new Mesh();
	mesh_cube->LoadFromFile("cube.obj");
	mesh_cube->MakeGLObjects();

	mesh_pyramid = new Mesh();
	mesh_pyramid->LoadFromFile("pyramid.obj");
	mesh_pyramid->MakeGLObjects();

	texture_screen = new Texture();
	texture_screen->LoadFromFile("F_dmw8lagAAWJes.jpeg");
	texture_screen->MakeGLObject();

	texture_test = new Texture();
	texture_test->LoadFromFile("UV_checker_Map_byValle.jpg");
	texture_test->MakeGLObject();

	camera = new ObjectCameraPerspective(glm::radians(90.0f), 0.1f, 100.0f);
	camera->SetPosition(Vector3(0.0f, 1.0f, 2.0f));
	camera->SetRotation(Quaternion(Vector3(glm::radians(-30.0f), 0.0f, 0.0f)));

	Scene* mainScene = new Scene(camera);
	YGLFramework::SwitchScene(mainScene);

	auto objGround = new ObjectModel(mesh_plane, shader_screen, texture_screen);
	mainScene->AddChild(objGround);

	object_anchor = new Object();
	mainScene->AddChild(object_anchor);

	object_cube = new ObjectModel(mesh_cube, shader, texture_test);
	object_anchor->AddChild(object_cube);

	object_pyramid = new ObjectModel(mesh_pyramid, shader, texture_test);
	object_anchor->AddChild(object_pyramid);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	YGLFramework::SetUpdateHandler(Update);
	YGLFramework::BeginUpdateLoop();
}

void Update()
{
	float t = clock() / 1000.0f;
	static float lt = 0.0f;
	float dt = glm::min(t - lt, 0.02f);
	lt = t;

	if (rotate_x)
		object_anchor->SetRotation(glm::rotate(object_anchor->GetRotation(), dt, Vector3(1.0f, 0.0f, 0.0f)));
	if (rotate_y)
		object_anchor->SetRotation(glm::rotate(object_anchor->GetRotation(), dt, Vector3(0.0f, 1.0f, 0.0f)));

	if (Input::GetKeyDown('1'))
		mode = false;
	if (Input::GetKeyDown('2'))
		mode = true;
	if (Input::GetKeyDown('X'))
		rotate_x = !rotate_x;
	if (Input::GetKeyDown('Y'))
		rotate_y = !rotate_y;
	if (Input::GetKeyDown('S'))
	{
		object_anchor->SetRotation(glm::identity<Quaternion>());
		rotate_x = false;
		rotate_y = false;
	}

	object_cube->SetScale(object_cube->GetScale().x + (!mode - object_cube->GetScale().x) * dt * 16.0f);
	object_pyramid->SetScale(object_pyramid->GetScale().x + (mode - object_pyramid->GetScale().x) * dt * 16.0f);
}