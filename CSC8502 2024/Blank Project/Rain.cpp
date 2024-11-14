#include "Rain.h"


Rain::Rain() {
	mesh = Mesh::LoadFromMeshFile("Sphere.msh");
	texture = SOIL_load_OGL_texture(TEXTUREDIR"raindrop.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);

	for (int i = 0; i < numParticles; i++) {
		Particle particle;
		particle.isAlive = true;
		particle.position = Vector3(rand() % 16384, 8000, rand() % 16384);
		float speed = (rand() % 15) + 8;
		particle.speed = Vector3(0, -speed, -3);
		particle_list.push_back(particle);
		SceneNode* rain_drop = new SceneNode();
		rain_drop->SetMesh(this->mesh);
		rain_drop->SetTexture(this->texture);
		rain_drop->SetTransform(Matrix4::Translation(particle.position));
		rain_drop->SetModelScale(Vector3(20, 20, 20));
		rain_drop->SetColour(Vector4(1.0f, 1.0f, 1.0f, 1.0f));
		rain_drop->SetBoundingRadius(5.0f);
		AddChild(rain_drop);
	}
}

void Rain::Update(float dt) {
	for (int i = 0; i < numParticles; i++) {
		if (children[i]->GetTransform().GetPositionVector().y <= -2000) {
			children[i]->SetTransform(Matrix4::Translation(Vector3(rand() % 16384, 8000, rand() % 16384)));
			float speed = (rand() % 15) + 8;
			particle_list[i].speed = Vector3(0, -speed, 0);
		}
		children[i]->SetTransform(children[i]->GetTransform() * Matrix4::Translation(particle_list[i].speed));
	}
	SceneNode::Update(dt);
}