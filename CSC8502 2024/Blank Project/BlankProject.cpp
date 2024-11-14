#include "../NCLGL/window.h"
#include "Renderer.h"

//PRESS 1 : FREECAMERA
//PRESS 2 : AUTOCAMERA position forward
//PRESS 3 : DEFAULT CAMERA POSITION
//PRESS 4 : AUTOCAMERA up
//PRESS 5 : AUTOCAMERA left
//PRESS 6 : AUTOCAMERA down
//PRESS 7 : AUTOCAMERA right
// 
//PRESS Z : NORMAL RENDERING
//PRESS X : DEFERRED RENDERING 
//PRESS C : BLUR RENDERING
//PRESS V : SHADOW RENDERING






int main() {
	Window w("Make your own project!", 1280, 720, false);

	if (!w.HasInitialised()) {
		return -1;
	}

	Renderer renderer(w);
	if (!renderer.HasInitialised()) {
		return -1;
	}

	w.LockMouseToWindow(true);
	w.ShowOSPointer(false);

	int CAMERASW = 2;
	int SCENESW = 1;

	while (w.UpdateWindow() && !Window::GetKeyboard()->KeyDown(KEYBOARD_ESCAPE)) {

		if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_1)) {
			CAMERASW = 1;
		}
		if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_2)) {
			CAMERASW = 2;
		}
		if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_3)) {
			CAMERASW = 3;
		}
		if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_4)) {
			CAMERASW = 4;
		}
		if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_5)) {
			CAMERASW = 5;
		}
		if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_6)) {
			CAMERASW = 6;
		}
		if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_7)) {
			CAMERASW = 7;
		}

		if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_Z)) {
			SCENESW = 1;
		}
		if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_X)) {
			SCENESW = 2;
		}
		if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_C)) {
			SCENESW = 3;
		}
		if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_V)) {
			SCENESW = 4;
		}

		//camera settings
		if (CAMERASW == 1)
		{
			renderer.UpdateScene(w.GetTimer()->GetTimeDeltaSeconds());
		}
		else if (CAMERASW == 2)
		{
			renderer.AutoUpdateCamera(w.GetTimer()->GetTimeDeltaSeconds());

		}
		else if (CAMERASW == 3)
		{
			renderer.SetDefultCamera(w.GetTimer()->GetTimeDeltaSeconds());

		}
		else if (CAMERASW == 4 || CAMERASW == 5 || CAMERASW == 6 || CAMERASW == 7)
		{
			renderer.AutoUpdateCamera2(w.GetTimer()->GetTimeDeltaSeconds(), CAMERASW);

		}

		//scenesettings
		if (SCENESW == 1)
		{
			renderer.RenderScene();
			renderer.SwapBuffers();
			if (Window::GetKeyboard()->KeyDown(KEYBOARD_F5)) {
				Shader::ReloadAllShaders();
			}
		}
		else if (SCENESW == 2)
		{
			renderer.RenderSceneNight();
			renderer.SwapBuffers();
			if (Window::GetKeyboard()->KeyDown(KEYBOARD_F5)) {
				Shader::ReloadAllShaders();
			}
		}
		else if (SCENESW == 3)
		{
			renderer.RenderSceneBlur();
			renderer.SwapBuffers();
			if (Window::GetKeyboard()->KeyDown(KEYBOARD_F5)) {
				Shader::ReloadAllShaders();
			}
		}
		else if (SCENESW == 4)
		{
			renderer.RenderSceneShadow();
			renderer.SwapBuffers();
			if (Window::GetKeyboard()->KeyDown(KEYBOARD_F5)) {
				Shader::ReloadAllShaders();
			}
		}
	}
	return 0;
}