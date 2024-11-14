#pragma once
#include "../NCLGL/OGLRenderer.h"
#include "../nclgl/SceneNode.h"
#include "../nclgl/Frustum.h"
#include "../nclgl/Camera.h"
#include "../nclgl/Light.h"
#include "../nclgl/HeightMap.h"
#include "../nclgl/MeshAnimation.h"
#include "../nclgl/MeshMaterial.h"
#include "Rain.h"

class Renderer : public OGLRenderer {
public:
	Renderer(Window& parent);
	~Renderer(void);

	//different scenes switcher
	void RenderScene()				override;
	void RenderSceneNight();
	void RenderSceneBlur();
	void RenderSceneShadow();

	void UpdateScene(float msec)	override;
	void AutoUpdateCamera(float msc);
	void AutoUpdateCamera2(float msc, int SW);
	void SetDefultCamera(float msc);
	void SetShaderParticleSize(float f);
protected:
	//scenegraph functions
	void BuildNodeLists(SceneNode* from);
	void SortNodeLists();
	void ClearNodeLists();
	void DrawNodes(Camera* camera, bool SW, bool shadowSW);
	void DrawNode(Camera* camera, SceneNode* n, bool SW, bool shadowSW);

	//skybox,heightmap,water functions
	void DrawSkybox();
	void DrawHeightmap(Camera* camera, bool SW, bool shadowSW);
	void DrawWater(Camera* camera, bool SW, bool shadowSW);
	void DrawHeightmapNoLight();


	//post processing
	void DrawBlurScene();
	void DrawPostProcess();
	void PresentScene();


	//deferred shading
	void DrawScene();
	void DrawPointLights();
	void CombineBuffers();
	void GenerateScreenTexture(GLuint& into, bool depth = false);

	//shadow mapping
	void DrawNodesShadow();
	void DrawNodeShadow(SceneNode* n);
	void DrawShadowMapping();
	void DrawCombinedScene();


	SceneNode* root;
	SceneNode* soldierNode;


	HeightMap* heightMap;
	Light* light;
	Light* light2;
	Light* pointLights;
	Camera* camera;
	Camera* skycamera;

	Mesh* skyboxQuad;
	Mesh* PresentQuad;
	Mesh* cube;
	Mesh* sphere;
	Mesh* rocksphere;
	Mesh* palmtree;
	Mesh* fritree;
	Mesh* soldier;
	Mesh* rock;

	MeshAnimation* soldierAnim;
	MeshMaterial* soldierMat;
	MeshAnimation* femaleAnim;
	MeshMaterial* femaleMat;


	Shader* skyboxShader;
	Shader* scenegraphShader;
	Shader* lightShader;
	Shader* heightmapNolightShader;
	Shader* waterShader;
	Shader* soldierShader;
	Shader* processShader;
	Shader* postShader;
	Shader* emitterShader;
	Shader* pointlightShader;
	Shader* combineShader;
	Shader* blurShader;

	Shader* shadowShader;
	Shader* heightmapshadowShader;
	Shader* watershadowShader;
	Shader* soldiershadowShader;
	Shader* bumpshadowShader;
	Shader* PerPixelshaodwShader;
	Shader* animationshadowShader;

	//FBO
	GLuint bufferFBO;
	GLuint bufferDepthTex;
	GLuint bufferNormalTex;
	GLuint bufferColourTex[2];

	GLuint pointLightFBO;
	GLuint lightDiffuseTex;
	GLuint lightSpecularTex;

	GLuint processFBO;

	GLuint blurbufferFBO;
	GLuint blurbufferColourTex[2];
	GLuint blurbufferDepthTex;

	GLuint shadowFBO;
	GLuint shadowTex;


	//textures
	GLuint cubeMap;
	GLuint earthBump;
	GLuint earthTex;
	GLuint nodeTexture;
	GLuint nodeBumpTexture;
	GLuint waterTex;
	GLuint treeTex;
	GLuint waterBump;
	GLuint rockTex;
	GLuint rockBump;
	GLuint glassTex;
	vector<GLuint> soldiermatTextures;
	vector<GLuint> femalematTextures;


	vector<SceneNode*> transparentNodeList;
	vector<SceneNode*> nodeList;

	//water properties
	float waterRotate;
	float waterCycle;

	//animation properties
	int soldiercurrentFrame;
	float soldierframeTime;
	float rockmovTime;


	//default projMatrix and rockMovMatrix
	Matrix4 defaultprojMatrix;
	Matrix4 rockmodelMatrix;

	//rain
	Rain* rain;

};

