#pragma once
#include "Matrix4.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Mesh.h"
#include <vector>
#include "MeshAnimation.h"

enum RenderType
{
	TYPE_NONE,
	TYPE_ANIMATION,
	TYPE_BUMP,
	TYPE_NORMAL
};


class SceneNode
{
public:
	SceneNode(Mesh* m = NULL, Vector4 colour = Vector4(1, 1, 1, 1));
	~SceneNode(void);

	float GetBoundingRadius() const { return boundingRadius; }
	void SetBoundingRadius(float f) { boundingRadius = f; }

	float GetCameraDistance() const { return distanceFromCamera; }
	void SetCameraDistance(float f) { distanceFromCamera = f; }

	void SetTexture(GLuint tex) { texture = tex; }
	GLuint GetTexture() const { return texture; }

	void SetBumpTexture(GLuint tex) { bumpTex = tex; }
	GLuint GetBumpTexture() const { return bumpTex; }

	void SetTextures(vector<GLuint> tex) { matTextures = tex; }
	vector<GLuint> GetTextures() const { return matTextures; }

	static bool CompareByCameraDistance(SceneNode* a, SceneNode* b)
	{
		return (a->distanceFromCamera < b->distanceFromCamera) ? true : false;
	}

	void SetTransform(const Matrix4& matrix) { transform = matrix; }
	const Matrix4& GetTransform() const { return transform; }
	Matrix4 GetWorldTransform() const { return worldTransform; }

	int GetCurrentFrame() const { return currentFrame; }
	void SetCurrentFrame(int c) { currentFrame = c; }

	float GetframTime() const { return frameTime; }
	void SetframeTime(float c) { frameTime = c; }

	Vector4 GetColour() const { return colour; }
	void SetColour(Vector4 c) { colour = c; }

	RenderType GetType() const { return type; }
	void SetType(RenderType r) { type = r; }

	Vector3 GetModelScale() const { return modelScale; }
	void SetModelScale(Vector3 s) { modelScale = s; }

	Mesh* GetMesh() const { return mesh; }
	void SetMesh(Mesh* m) { mesh = m; }

	MeshAnimation* GetAnimation() const { return Anim; }
	void SetAnimation(MeshAnimation* anim) { Anim = anim; }

	void AddChild(SceneNode* s);
	virtual void Update(float dt);
	virtual void Draw(const OGLRenderer& r);

	std::vector<SceneNode*>::const_iterator GetChildIteratorStart()
	{
		return children.begin();
	}
	std::vector<SceneNode*>::const_iterator GetChildIteratorEnd()
	{
		return children.end();
	}

protected:
	SceneNode* parent;
	Mesh* mesh;
	MeshAnimation* Anim;
	Matrix4 worldTransform;
	Matrix4 transform;
	Vector3 modelScale;
	Vector4 colour;
	std::vector<SceneNode*> children;
	float distanceFromCamera;
	float boundingRadius;
	GLuint texture;
	GLuint bumpTex;
	vector<GLuint> matTextures;
	RenderType type;

	int currentFrame;
	float frameTime;
};