#include "Renderer.h"
#include "Data.h"
#include <algorithm>
using namespace std;
const int POST_PASSES = 10;
#define SHADOWSIZE 2048


Renderer::Renderer(Window& parent) : OGLRenderer(parent) {

#pragma region Load Mesh and Ani
	
	skyboxQuad = Mesh::GenerateQuad();

	sphere = Mesh::LoadFromMeshFile("Sphere.msh");

	cactus_12 = Mesh::LoadFromMeshFile("Desert_Plant_12_Small_0_1.msh");

	cactus_09 = Mesh::LoadFromMeshFile("Desert_Plant_09_Small_0_1.msh");

	rock = Mesh::LoadFromMeshFile("Rock_Desert_01_Small_0_1.msh");
	
	bone =Mesh:: LoadFromMeshFile("bone.msh");

	soldier = Mesh::LoadFromMeshFile("Role_T.msh");
	
	soldierAnim = new MeshAnimation("Role_T.anm");
	
	soldierMat = new MeshMaterial("Role_T.mat");
#pragma endregion
	
#pragma region Load Texture and Skybox
	cubeMapSunset = SOIL_load_OGL_cubemap(
	TEXTUREDIR"rusted_west.jpg", TEXTUREDIR"rusted_east.jpg",
	TEXTUREDIR"rusted_up.jpg", TEXTUREDIR"rusted_down.jpg",
	TEXTUREDIR"rusted_south.jpg", TEXTUREDIR"rusted_north.jpg",
	SOIL_LOAD_RGB, SOIL_CREATE_NEW_ID, 0);

	cubeMapNight = SOIL_load_OGL_cubemap(
	TEXTUREDIR"vz_gray_left.png", TEXTUREDIR"vz_gray_left.png",
	TEXTUREDIR"vz_gray_up.png", TEXTUREDIR"vz_gray_down.png",
	TEXTUREDIR"vz_gray_left.png", TEXTUREDIR"vz_gray_left.png",
			SOIL_LOAD_RGB, SOIL_CREATE_NEW_ID, 0);
	if (!cubeMapNight||!cubeMapSunset)
	{
		return;
	}
	//default skybox
	currentSkybox=cubeMapSunset;
	
	heightMap = new HeightMap(TEXTUREDIR"noise.png");
	
	cactusTex = SOIL_load_OGL_texture(TEXTUREDIR"Gradients_02_flip.png",
		SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);


	earthTex = SOIL_load_OGL_texture(
		TEXTUREDIR"Ground035_1K-JPG_Color.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	
	earthBump = SOIL_load_OGL_texture(
		TEXTUREDIR"Dirt_1_Normal.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);


	waterTex = SOIL_load_OGL_texture(
		TEXTUREDIR"water.tga", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);


	waterBump = SOIL_load_OGL_texture(
		TEXTUREDIR"waterbump.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	
	glassTex = SOIL_load_OGL_texture(
		TEXTUREDIR"stainedglass.tga", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, 0);

	for (int i = 0; i < soldier->GetSubMeshCount(); ++i)
	{
		const MeshMaterialEntry* matEntry =
			soldierMat->GetMaterialForLayer(i);

		const string* filename = nullptr;
		matEntry->GetEntry("Diffuse", &filename);
		string path = TEXTUREDIR + *filename;
		GLuint texID = SOIL_load_OGL_texture(path.c_str(), SOIL_LOAD_AUTO,
			SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y);
		if (!texID)
		{
			return;
		}
		soldiermatTextures.emplace_back(texID);
	}
	
	SetTextureRepeating(earthTex, true);
	SetTextureRepeating(waterBump, true);
	SetTextureRepeating(earthBump, true);
	SetTextureRepeating(waterTex, true);
	SetTextureRepeating(cactusTex, true);
	SetTextureRepeating(rockTex, true);

#pragma endregion

#pragma region Load Shader
	skyboxShader = new Shader(
		"skyboxVertex.glsl", "skyboxFragment.glsl"
	);

	blurShader = new Shader(
		"TexturedVertex.glsl", "TexturedFragment.glsl"
	);

	emitterShader = new Shader(
		"vertex.glsl", "fragment.glsl", "geometry.glsl"
	);

	processShader = new Shader(
		"TexturedVertex.glsl", "processfrag.glsl"
	);
	lightShader = new Shader(
		"BumpVertex.glsl", "BumpFragment.glsl"
	);

	waterShader = new Shader(
		"waterVertex.glsl", "waterFragment.glsl"
	);


	scenegraphShader = new Shader(
		"PerPixelVertex.glsl", "PerPixelFragment.glsl"
	);


	soldierShader = new Shader(
		"SkinningVertex.glsl", "PerPixelFragment.glsl"
	);

	postShader = new Shader(
		"TexturedVertex.glsl", "processfrag.glsl"
	);
	
	heightmapNolightShader = new Shader(
		"BumpVertex.glsl", "bufferFragment.glsl"
	);

	pointlightShader = new Shader(
		"pointlightvert.glsl", "pointlightfrag.glsl"
	);

	combineShader = new Shader(
		"combinevert.glsl", "combinefrag.glsl"
	);

	shadowShader = new Shader(
		"shadowVertex.glsl", "shadowFragment.glsl"
	);

	watershadowShader = new Shader(
		"reflectShadowVertex.glsl", "reflectShadowFragment.glsl"
	);

	bumpshadowShader = new Shader(
		"BumpShadowVertex.glsl", "BumpShadowFragment.glsl"
	);

	PerPixelshaodwShader = new Shader(
		"PerPixelShadowVertex.glsl", "PerPixelShadowFragment.glsl"
	);

	soldiershadowShader = new Shader(
		"SkinningshadowVertex.glsl", "shadowFragment.glsl"
	);

	animationshadowShader = new Shader(
		"SkinningVertex01.glsl", "PerPixelShadowFragment.glsl"
	);
#pragma endregion

#pragma region Creat Scenegraph with random generation
	
	Vector3 heightmapSize = heightMap->GetHeightmapSize();

	root = new SceneNode();
	rain = new Rain();
	root->AddChild(rain);

	srand(static_cast<unsigned>(time(nullptr)));

	//get random counts of meshes
	int numCactus12 = 5 + rand() % 4;
	int numCactus09 = 5 + rand() % 4; 
	int numRock = 5 + rand() % 4;     

	//add
	int totalModels = numCactus12 + numCactus09 + numRock;

	//generate random position for each mesh
	for (int i = 0; i < totalModels; ++i) 
	{
		SceneNode* s = new SceneNode();
		s->SetColour(Vector4(1.0f, 1.0f, 1.0f, 1.0f));
		
		float randomX = static_cast<float>(rand()) / RAND_MAX;
		float randomZ = static_cast<float>(rand()) / RAND_MAX;
    
		
		if (i < numCactus12) {
			s->SetTransform(Matrix4::Translation(heightmapSize * Vector3(randomX, 0.3f, randomZ)));
			s->SetMesh(cactus_12);
			s->SetTexture(cactusTex);
		}
		else if (i < numCactus12 + numCactus09) {
			s->SetTransform(Matrix4::Translation(heightmapSize * Vector3(randomX, 0.5f, randomZ)));
			s->SetMesh(cactus_09);
			s->SetTexture(cactusTex);
		}
		else {
			s->SetTransform(Matrix4::Translation(heightmapSize * Vector3(randomX, 0.5f, randomZ)));
			s->SetMesh(rock);
			s->SetTexture(cactusTex);
		}

		s->SetModelScale(Vector3(100.0f, 100.0f, 100.0f));
		s->SetType(TYPE_NORMAL);
		root->AddChild(s);
	}

	SceneNode* boneNode = new SceneNode();
	boneNode->SetColour(Vector4(1.0f, 1.0f, 1.0f, 1.0f));
	boneNode->SetTransform(Matrix4::Translation(heightmapSize * Vector3(0.5f, 0.5f, 0.5f))); 
	boneNode->SetModelScale(Vector3(100.0f, 100.0f, 100.0f));
	boneNode->SetMesh(bone); // 使用 bone 模型
	boneNode->SetTexture(cactusTex);
	boneNode->SetType(TYPE_NORMAL);
	root->AddChild(boneNode);
	
	soldierNode = new SceneNode();
	soldierNode->SetColour(Vector4(1.0f, 1.0f, 1.0f, 1.0f));
	soldierNode->SetTransform(Matrix4::Translation(heightmapSize * Vector3(0.7f, 0.3f, 0.8f)));
	soldierNode->SetModelScale(Vector3(50.0f, 50.0f, 50.0f));
	soldierNode->SetMesh(soldier);
	soldierNode->SetTextures(soldiermatTextures);
	soldierNode->SetAnimation(soldierAnim);
	soldierNode->SetType(TYPE_ANIMATION);
	root->AddChild(soldierNode);

#pragma endregion

#pragma region Creat camera and generate random light

	//create camera and light
	camera = new Camera(-15.0f, 50.0f,heightmapSize * Vector3(1.3f, 3.0f, 1.3f));
	generalCamera = new Camera(-90.0f, 0.0f,heightmapSize * Vector3(0.5f, 10.0f, 0.5f));
	
	light = new Light(heightmapSize * Vector3(0.5f, 1.5f, 1.0f),Vector4(1, 1, 1, 1), heightmapSize.x);
	pointLights = new Light[5];

	for (int i = 0; i < 5; ++i) 
	{
		Light& l = pointLights[i];
		l.SetPosition(Vector3(rand() % (int)heightmapSize.x, 400.0f, rand() % (int)heightmapSize.z));
		
		l.SetColour(Vector4(1.0f, 1.0f, 0.8f, 1.0f));

		l.SetRadius(1000.0f + (rand() % 250));
	}

	//set up projMatrix
	defaultprojMatrix = Matrix4::Perspective(1.0f, 15000.0f,
		(float)width / (float)height, 45.0f);
	projMatrix = defaultprojMatrix;
#pragma endregion

	InitializeFramebuffers();

	//water
	flowRotate = 0.0f;
	flowSpeed = 0.0f;
	
	//animation
	soldiercurrentFrame = 0;
	soldierframeTime = 0.0f;
}

#pragma region Initialize FBO and Texture buffer
void Renderer::GenerateFramebuffer(GLuint& fbo, GLuint depthTex, GLuint* colorTex, int colorCount) {
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    if (depthTex) {
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTex, 0);
    }

    GLenum* buffers = new GLenum[colorCount];
    for (int i = 0; i < colorCount; ++i) {
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, colorTex[i], 0);
        buffers[i] = GL_COLOR_ATTACHMENT0 + i;
    }
    glDrawBuffers(colorCount, buffers);
    delete[] buffers;

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "Framebuffer creation failed!" << std::endl;
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        return;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::InitializeFramebuffers() {
    GenerateScreenTexture(bufferDepthTex, true);
    for (int i = 0; i < 2; ++i) {
        GenerateScreenTexture(bufferColourTex[i]);
    }
    GenerateScreenTexture(bufferNormalTex);
    GenerateScreenTexture(lightDiffuseTex);
    GenerateScreenTexture(lightSpecularTex);

    GenerateFramebuffer(bufferFBO, bufferDepthTex, bufferColourTex, 2);
    GenerateFramebuffer(pointLightFBO, 0, &lightDiffuseTex, 2);

    glGenTextures(1, &shadowTex);
    glBindTexture(GL_TEXTURE_2D, shadowTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOWSIZE, SHADOWSIZE, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glBindTexture(GL_TEXTURE_2D, 0);

    glGenTextures(1, &blurbufferDepthTex);
    glBindTexture(GL_TEXTURE_2D, blurbufferDepthTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, width, height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);
    glBindTexture(GL_TEXTURE_2D, 0);

    for (int i = 0; i < 2; ++i) {
        glGenTextures(1, &blurbufferColourTex[i]);
        glBindTexture(GL_TEXTURE_2D, blurbufferColourTex[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    GenerateFramebuffer(blurbufferFBO, blurbufferDepthTex, blurbufferColourTex, 1);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    init = true;
}
#pragma endregion

#pragma region Destructor

Renderer::~Renderer(void) {
	//delete meshes
	delete skyboxQuad;
	delete soldier;
	delete cactus_09;
	delete cactus_12;
	delete rock;
	delete sphere;

	//delete shaders
	delete lightShader;
	delete skyboxShader;
	delete scenegraphShader;
	delete waterShader;
	delete emitterShader;
	delete blurShader;
	delete processShader;
	delete soldierShader;
	delete postShader;
	delete heightmapNolightShader;
	delete pointlightShader;
	delete combineShader;
	delete shadowShader;
	delete watershadowShader;
	delete bumpshadowShader;
	delete PerPixelshaodwShader;
	delete soldiershadowShader;
	delete animationshadowShader;

	//delete FBOs and attachments
	glDeleteTextures(2, bufferColourTex);
	glDeleteTextures(2, blurbufferColourTex);
	glDeleteTextures(1, &bufferNormalTex);
	glDeleteTextures(1, &bufferDepthTex);
	glDeleteTextures(1, &blurbufferDepthTex);
	glDeleteTextures(1, &lightDiffuseTex);
	glDeleteTextures(1, &lightSpecularTex);
	glDeleteTextures(1, &shadowTex);

	glDeleteFramebuffers(1, &bufferFBO);
	glDeleteFramebuffers(1, &processFBO);
	glDeleteFramebuffers(1, &pointLightFBO);
	glDeleteFramebuffers(1, &blurbufferFBO);
	glDeleteFramebuffers(1, &shadowFBO);
	
	//delete others
	delete camera;
	delete light;
	delete root;
	delete[] pointLights;
}

#pragma endregion

#pragma region Orbit camera
void Renderer::UpdateScene(float dt) {
	
	
	camera->UpdateCamera(dt);
	viewMatrix = camera->BuildViewMatrix();
	
	while (soldierframeTime < 0.0f)
	{
		soldiercurrentFrame = (soldiercurrentFrame + 1) % soldierAnim->GetFrameCount();
		soldierframeTime += 1.0f / soldierAnim->GetFrameRate();
	}
	soldierNode->SetCurrentFrame(soldiercurrentFrame);
	
	Vector3 hSize = heightMap->GetHeightmapSize();

	flowRotate += dt * 0.15f;
	flowSpeed += dt * 0.15f;

	soldierframeTime -= dt;
	
	root->Update(dt);
}


void Renderer::OrbitCamera(float dt) {
	
	Vector3 heightmapsize = heightMap->GetHeightmapSize();
	
	Vector3 center = heightmapsize * Vector3(0.5f, 5.0f, 0.5f);
	
	float radius = 2000.0f; 
	float speed = -0.1f;     
	
	static float angle = 0.0f;
	angle += speed * dt; 
	
	float x = center.x + radius * cos(angle);
	float z = center.z + radius * sin(angle);
	float y = center.y + 5.0f; 


	Vector3 cameraPos(x, y, z);
	camera->SetPosition(cameraPos);
	
	Vector3 directionToCenter = (cameraPos - center).Normalised();
	
	float yaw = atan2(directionToCenter.x, directionToCenter.z) * (180.0f / 3.1415926f);

	camera->SetYaw(yaw);
	camera->SetPitch(-35.0f);
	
	viewMatrix = camera->BuildViewMatrix();
	
	flowRotate += dt * 0.5f;
	flowSpeed += dt * 0.15f;
	
	soldierframeTime -= dt;
	while (soldierframeTime < 0.0f) {
		soldiercurrentFrame = (soldiercurrentFrame + 1) % soldierAnim->GetFrameCount();
		soldierframeTime += 1.0f / soldierAnim->GetFrameRate();
	}
	soldierNode->SetCurrentFrame(soldiercurrentFrame);
	
	root->Update(dt);
}
#pragma endregion

#pragma region Scene Graph

void Renderer::BuildNodeLists(SceneNode* from) {
    Vector3 dir = from->GetWorldTransform().GetPositionVector() - camera->GetPosition();
    from->SetCameraDistance(Vector3::Dot(dir, dir));

    if (from->GetColour().w < 1.0f) {
        transparentNodeList.push_back(from);
    } else {
        nodeList.push_back(from);
    }

    for (auto i = from->GetChildIteratorStart(); i != from->GetChildIteratorEnd(); ++i) {
        BuildNodeLists(*i);
    }
}

void Renderer::SortNodeLists() {
    std::sort(transparentNodeList.rbegin(), transparentNodeList.rend(), SceneNode::CompareByCameraDistance);
    std::sort(nodeList.begin(), nodeList.end(), SceneNode::CompareByCameraDistance);
}

void Renderer::DrawNodes(Camera* camera, bool SW, bool shadowSW) {
    for (auto node : nodeList) {
        DrawNode(camera, node, shadowSW);
    }
    for (auto node : transparentNodeList) {
        DrawNode(camera, node, shadowSW);
    }
}

void Renderer::DrawNode(Camera* camera, SceneNode* node, bool shadowSW) {
    if (!node->GetMesh()) {
        return;
    }

    // 设置适合的着色器
    if (shadowSW) {
        BindShader(shadowShader);
        viewMatrix = camera->BuildViewMatrix();
        projMatrix = defaultprojMatrix;
        glUniform1i(glGetUniformLocation(GetCurrentShader()->GetProgram(), "shadowTex"), 2);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, shadowTex);
    } else {
        BindShader(node->GetType() == TYPE_ANIMATION ? soldierShader : scenegraphShader);
        SetShaderLight(*light);
        viewMatrix = camera->BuildViewMatrix();
        projMatrix = defaultprojMatrix;
    }

    if (node == soldierNode) {
        DrawSoldierNode(camera, node);
    } else {
        DrawDefaultNode(camera, node);
    }
}

void Renderer::DrawSoldierNode(Camera* camera, SceneNode* node) {
    glUniform1i(glGetUniformLocation(GetCurrentShader()->GetProgram(), "diffuseTex"), 0);
    UpdateShaderMatrices();

    Matrix4 modelMatrix = node->GetWorldTransform() * Matrix4::Scale(node->GetModelScale());
    glUniformMatrix4fv(glGetUniformLocation(GetCurrentShader()->GetProgram(), "modelMatrix"), 1, false, modelMatrix.values);

    glUniform3fv(glGetUniformLocation(GetCurrentShader()->GetProgram(), "cameraPos"), 1, (float*)&camera->GetPosition());
    nodeTexture = soldiermatTextures[0];
    glUniform1i(glGetUniformLocation(GetCurrentShader()->GetProgram(), "useTexture"), nodeTexture);

    vector<Matrix4> frameMatrices;
    const Matrix4* invBindPose = node->GetMesh()->GetInverseBindPose();
    const Matrix4* frameData = node->GetAnimation()->GetJointData(node->GetCurrentFrame());

    for (unsigned int i = 0; i < node->GetMesh()->GetJointCount(); ++i) {
        frameMatrices.emplace_back(frameData[i] * invBindPose[i]);
    }

    glUniformMatrix4fv(glGetUniformLocation(GetCurrentShader()->GetProgram(), "joints"),
                       frameMatrices.size(), false, (float*)frameMatrices.data());

    for (int i = 0; i < node->GetMesh()->GetSubMeshCount(); ++i) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, node->GetTextures()[i]);
        node->GetMesh()->DrawSubMesh(i);
    }
}

void Renderer::DrawDefaultNode(Camera* camera, SceneNode* node) {
    UpdateShaderMatrices();

    glUniform1i(glGetUniformLocation(GetCurrentShader()->GetProgram(), "diffuseTex"), 0);

    Matrix4 modelMatrix = node->GetWorldTransform() * Matrix4::Scale(node->GetModelScale());
    glUniformMatrix4fv(glGetUniformLocation(GetCurrentShader()->GetProgram(), "modelMatrix"), 1, false, modelMatrix.values);
    glUniform4fv(glGetUniformLocation(GetCurrentShader()->GetProgram(), "nodeColour"), 1, (float*)&node->GetColour());

    glUniform3fv(glGetUniformLocation(GetCurrentShader()->GetProgram(), "cameraPos"), 1, (float*)&camera->GetPosition());

    nodeTexture = node->GetTexture();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, nodeTexture);

    glUniform1i(glGetUniformLocation(GetCurrentShader()->GetProgram(), "useTexture"), nodeTexture);
    node->Draw(*this);
}

#pragma endregion

void Renderer::DrawSkybox(GLuint skybox)
{
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	glDepthMask(GL_FALSE);

	BindShader(skyboxShader);
	viewMatrix = camera->BuildViewMatrix();
	projMatrix = defaultprojMatrix;
	modelMatrix.ToIdentity();

	UpdateShaderMatrices();
	
	//use current skybox
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skybox);
	glUniform1i(glGetUniformLocation(skyboxShader->GetProgram(), "cubeTex"), 0);

	skyboxQuad->Draw();

	glDisable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	glDepthMask(GL_TRUE);
}

#pragma region Draw Terrain with heightmap
void Renderer::BindAndSetShader(Shader* shader, Camera* camera, bool shadowSW) {
	BindShader(shader);
	if (!shadowSW) {
		SetShaderLight(*light);
	}
	viewMatrix = camera->BuildViewMatrix();
	projMatrix = defaultprojMatrix;
	UpdateShaderMatrices();
}

void Renderer::SetTexture(GLuint texture, int textureUnit, const std::string& name) {
	glUniform1i(glGetUniformLocation(GetCurrentShader()->GetProgram(), name.c_str()), textureUnit);
	glActiveTexture(GL_TEXTURE0 + textureUnit);
	glBindTexture(GL_TEXTURE_2D, texture);
}

void Renderer::SetCubeMap(GLuint cubeMap, int textureUnit, const std::string& name) {
	glUniform1i(glGetUniformLocation(GetCurrentShader()->GetProgram(), name.c_str()), textureUnit);
	glActiveTexture(GL_TEXTURE0 + textureUnit);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap);
}


void Renderer::DrawHeightmap(Camera* camera, Shader* shader, bool shadowSW) {
	BindAndSetShader(shader, camera, shadowSW);

	SetTexture(earthTex, 0, "diffuseTex");
	SetTexture(earthBump, 1, "bumpTex");

	if (shadowSW) {
		SetTexture(shadowTex, 2, "shadowTex");
	}

	modelMatrix.ToIdentity();
	UpdateShaderMatrices();
	heightMap->Draw();
}

void Renderer::DrawHeightmapDaylight(Camera* camera, bool shadowSW) {
	DrawHeightmap(camera, shadowSW ? heightmapshadowShader : lightShader, shadowSW);
}

void Renderer::DrawHeightmapNight(Camera* camera) {
	DrawHeightmap(camera, heightmapNolightShader, false);
}


//switch Scene
void Renderer::RenderSceneDaylight() {
	currentSkybox = cubeMapSunset;
	PrepareScene();

	DrawSkybox(currentSkybox);
	DrawHeightmapDaylight(camera, false);
	DrawNodes(camera, true, false);
	DrawWater(camera, false);

	RenderMiniMap();
	ClearNodeLists();
}

void Renderer::RenderSceneNight() {
	currentSkybox = cubeMapNight;
	PrepareScene();

	DrawScene();
	DrawPointLights();
	CombineBuffers();

	RenderMiniMap();
	ClearNodeLists();
}

void Renderer::RenderMiniMap() {
	glClear(GL_DEPTH_BUFFER_BIT);
	glViewport(0 * width, 0.8 * height, (width / height) * width / 5, (width / height) * height / 5);
	DrawHeightmapDaylight(generalCamera, false);
	DrawNodes(generalCamera, true, false);
}

void Renderer::PrepareScene() {
	BuildNodeLists(root);
	SortNodeLists();
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	glViewport(0, 0, width, height);
}

#pragma endregion

#pragma region Update Water
void Renderer::DrawWater(Camera* camera, bool shadowSW)
{

		BindShader(waterShader);
		SetShaderLight(*light);
		Vector3 hSize = heightMap->GetHeightmapSize();

		modelMatrix = Matrix4::Translation(hSize * 0.5f) *
			Matrix4::Scale(hSize) *
			Matrix4::Rotation(90, Vector3(1, 0, 0));

		textureMatrix = Matrix4::Translation(Vector3(flowSpeed, 0.0f, flowSpeed)) *
			Matrix4::Scale(Vector3(5, 5, 5)) *
			Matrix4::Rotation(flowRotate, Vector3(0, 0, 1));

		projMatrix = defaultprojMatrix;
		viewMatrix = camera->BuildViewMatrix();
		UpdateShaderMatrices();

	glUniform3fv(glGetUniformLocation(GetCurrentShader()->GetProgram(),
		"cameraPos"), 1, (float*)&camera->GetPosition());

	glUniform1i(glGetUniformLocation(GetCurrentShader()->GetProgram(),
		"diffuseTex"), 0);

	glUniform1i(glGetUniformLocation(GetCurrentShader()->GetProgram(),
		"bumpTex"), 1);

	glUniform1i(glGetUniformLocation(GetCurrentShader()->GetProgram(),
		"cubeTex"), 2);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, waterBump);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, waterTex);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapSunset);
	
	modelMatrix = Matrix4::Translation(Vector3(hSize.x-2000, hSize.y-200.0f, hSize.z-2000)) *
		Matrix4::Scale(hSize*0.5) *
		Matrix4::Rotation(90, Vector3(1, 0, 0));
	UpdateShaderMatrices();

	skyboxQuad->Draw();
}
#pragma endregion

#pragma region Post Processing
void Renderer::ClearBuffers(const Vector4& color) {
	glClearColor(color.x, color.y, color.z, color.w);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
}

void Renderer::SetupFramebuffer(GLuint framebuffer, GLuint texture) {
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);
}

void Renderer::DrawQuadWithShader(Shader* shader, GLuint texture, const std::string& uniformName) {
	BindShader(shader);
	modelMatrix.ToIdentity();
	viewMatrix.ToIdentity();
	projMatrix.ToIdentity();
	UpdateShaderMatrices();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	glUniform1i(glGetUniformLocation(shader->GetProgram(), uniformName.c_str()), 0);
	skyboxQuad->Draw();
}
void Renderer::RenderSceneBlur() {
	PrepareScene();
	DrawBlurScene();
	PostProcessBlur();
	RenderMiniMap();
	ClearNodeLists();
}
void Renderer::DrawBlurScene() {
	SetupFramebuffer(blurbufferFBO, blurbufferColourTex[0]);
	ClearBuffers(Vector4(0, 0, 0, 1));

	DrawSkybox(currentSkybox);
	DrawHeightmapDaylight(camera, false);
	DrawNodes(camera, true, false);
	DrawWater(camera, false);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::PostProcessBlur() {
	for (int i = 0; i < POST_PASSES; ++i) {
		SetupFramebuffer(processFBO, blurbufferColourTex[1]);
		DrawQuadWithShader(processShader, blurbufferColourTex[0], "sceneTex");
		glUniform1i(glGetUniformLocation(processShader->GetProgram(), "isVertical"), 0);

		SetupFramebuffer(processFBO, blurbufferColourTex[0]);
		DrawQuadWithShader(processShader, blurbufferColourTex[1], "sceneTex");
		glUniform1i(glGetUniformLocation(processShader->GetProgram(), "isVertical"), 1);
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


#pragma endregion

#pragma region Deferred rendering and Point light
void Renderer::DrawScene()
{
	glBindFramebuffer(GL_FRAMEBUFFER, bufferFBO);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	projMatrix = Matrix4::Perspective(1.0f, 15000.0f,
		(float)width / (float)height, 45.0f);

	DrawSkybox(currentSkybox);
	DrawHeightmapDaylight(camera,false);
	DrawNodes(camera, true, false);
	if (currentSkybox==cubeMapSunset)
	{
		DrawWater(camera, false);
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::DrawPointLights() {
	PreparePointLightFramebuffer();
	
	BindShader(pointlightShader);
	SetPointLightShaderUniforms();
	
	DrawAllPointLights();
	RestoreDefaultRenderState();
}

void Renderer::PreparePointLightFramebuffer() {
	glBindFramebuffer(GL_FRAMEBUFFER, pointLightFBO);
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT);
	glBlendFunc(GL_ONE, GL_ONE);
	glCullFace(GL_FRONT);
	glDepthFunc(GL_ALWAYS);
	glDepthMask(GL_FALSE);
}

void Renderer::SetPointLightShaderUniforms() {
	glUniform1i(glGetUniformLocation(pointlightShader->GetProgram(), "depthTex"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, bufferDepthTex);

	glUniform1i(glGetUniformLocation(pointlightShader->GetProgram(), "normTex"), 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, bufferNormalTex);

	glUniform3fv(glGetUniformLocation(pointlightShader->GetProgram(), "cameraPos"), 1, (float*)&camera->GetPosition());
	glUniform2f(glGetUniformLocation(pointlightShader->GetProgram(), "pixelSize"), 1.0f / width, 1.0f / height);

	Matrix4 invViewProj = (projMatrix * viewMatrix).Inverse();
	glUniformMatrix4fv(glGetUniformLocation(pointlightShader->GetProgram(), "inverseProjView"), 1, false, invViewProj.values);
	UpdateShaderMatrices();
}

void Renderer::DrawAllPointLights() {
	for (int i = 0; i < 5; ++i) {
		SetShaderLight(pointLights[i]);
		sphere->Draw();
	}
	sphere->Draw();
}

void Renderer::RestoreDefaultRenderState() {
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glCullFace(GL_BACK);
	glDepthFunc(GL_LEQUAL);
	glDepthMask(GL_TRUE);

	glClearColor(0.2f, 0.2f, 0.2f, 1);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::CombineBuffers()
{
	BindShader(combineShader);
	modelMatrix.ToIdentity();
	viewMatrix.ToIdentity();
	projMatrix.ToIdentity();
	UpdateShaderMatrices();

	glUniform1i(glGetUniformLocation(combineShader->GetProgram(), "diffuseTex"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, bufferColourTex[0]);

	glUniform1i(glGetUniformLocation(combineShader->GetProgram(), "diffuseLight"), 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, lightDiffuseTex);

	glUniform1i(glGetUniformLocation(combineShader->GetProgram(), "specularLight"), 2);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, lightSpecularTex);

	skyboxQuad->Draw();
}

#pragma endregion


void Renderer::ClearNodeLists()
{
	transparentNodeList.clear();
	nodeList.clear();
}
void Renderer::SetShaderParticleSize(float f)
{
	glUniform1f(glGetUniformLocation(emitterShader->GetProgram(), "particleSize"), f);
}
void Renderer::GenerateScreenTexture(GLuint& into, bool depth)
{
	glGenTextures(1, &into);
	glBindTexture(GL_TEXTURE_2D, into);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	GLuint format = depth ? GL_DEPTH_COMPONENT24 : GL_RGBA8;
	GLuint type = depth ? GL_DEPTH_COMPONENT : GL_RGBA;

	glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, type, GL_UNSIGNED_BYTE, NULL);
	glBindTexture(GL_TEXTURE_2D, 0);
}

