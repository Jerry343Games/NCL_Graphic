#pragma once
#include "../nclgl/SceneNode.h"

struct Particle {
	Vector3 speed;
	Vector3 position;
	bool isAlive;
};

class Rain : public SceneNode {
public:
	Rain();
	void Update(float dt) override;

protected:
	void InitializeParticles();
	void InitializeSceneNodes();
	Mesh* LoadMesh();
	GLuint LoadTexture();
	SceneNode* CreateRainDrop(const Particle& particle);
	void ResetRainDrop(int index);
	void MoveRainDrop(int index);

	float variant = 100;
	float base = 100;
	float numParticles = 600;
	vector<Particle> particle_list;

	Mesh* mesh;
	GLuint texture;
};



