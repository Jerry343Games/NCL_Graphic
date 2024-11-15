#include "Rain.h"

Rain::Rain() {
	mesh = LoadMesh();
	texture = LoadTexture();
	InitializeParticles();
	InitializeSceneNodes();
}

Mesh* Rain::LoadMesh() {
	return Mesh::LoadFromMeshFile("Sphere.msh");
}

GLuint Rain::LoadTexture() {
	return SOIL_load_OGL_texture(TEXTUREDIR "raindrop.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
}

void Rain::InitializeParticles() {
	for (int i = 0; i < numParticles; i++) {
		Particle particle;
		particle.isAlive = true;
		//about terrain size
		particle.position = Vector3(rand() % 15000, 8000, rand() % 15000);
		float speed = (rand() % 15) + 8;
		particle.speed = Vector3(0, -speed, -3);
		particle_list.push_back(particle);
	}
}

void Rain::InitializeSceneNodes() {
	for (int i = 0; i < numParticles; i++) {
		SceneNode* rain_drop = CreateRainDrop(particle_list[i]);
		AddChild(rain_drop);
	}
}

SceneNode* Rain::CreateRainDrop(const Particle& particle) {
	SceneNode* rain_drop = new SceneNode();
	rain_drop->SetMesh(mesh);
	rain_drop->SetTexture(texture);
	rain_drop->SetTransform(Matrix4::Translation(particle.position));
	rain_drop->SetModelScale(Vector3(20, 20, 20));
	rain_drop->SetColour(Vector4(1.0f, 1.0f, 1.0f, 1.0f));
	rain_drop->SetBoundingRadius(5.0f);
	return rain_drop;
}

void Rain::Update(float dt) {
	for (int i = 0; i < numParticles; i++) {
		if (children[i]->GetTransform().GetPositionVector().y <= -1000) {
			ResetRainDrop(i);
		}
		MoveRainDrop(i);
	}
	SceneNode::Update(dt);
}

void Rain::ResetRainDrop(int index) {
	Vector3 newPosition(rand() % 15000, 8000, rand() % 15000);
	children[index]->SetTransform(Matrix4::Translation(newPosition));
	float speed = (rand() % 15) + 8;
	particle_list[index].speed = Vector3(0, -speed, 0);
}

void Rain::MoveRainDrop(int index) {
	children[index]->SetTransform(children[index]->GetTransform() * Matrix4::Translation(particle_list[index].speed));
}
