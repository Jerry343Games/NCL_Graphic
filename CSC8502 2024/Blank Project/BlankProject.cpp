#include "../NCLGL/window.h"
#include "Renderer.h"

//PRESS 1 : FREECAMERA
//PRESS 2 : AUTOCAMERA position forward
//PRESS Z : NORMAL RENDERING
//PRESS X : DEFERRED RENDERING 
//PRESS C : BLUR RENDERING






int main() {
	Window w("Make your own project!", 1980, 1080, false);

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
		if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_Z)) {
			SCENESW = 1;
		}
		if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_X)) {
			SCENESW = 2;
		}
		if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_C)) {
			SCENESW = 3;
		}

		//camera settings
		if (CAMERASW == 1)
		{
			renderer.UpdateScene(w.GetTimer()->GetTimeDeltaSeconds());
		}
		else if (CAMERASW == 2)
		{
			renderer.OrbitCamera(w.GetTimer()->GetTimeDeltaSeconds());

		}

		//scenesettings
		if (SCENESW == 1)
		{
			renderer.RenderSceneDaylight();
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
	}
	return 0;
}