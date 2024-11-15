#include "../NCLGL/window.h"
#include "Renderer.h"

//PRESS 1 : Free Camera
//PRESS 2 : Orbit Camera
//PRESS 3 : Daylight Scene
//PRESS 4 : Night Scene
//PRESS 5 : Blur Post Processing


int main() {
	Window w("Make your own project!", 1080, 720, false);

	if (!w.HasInitialised()) {
		return -1;
	}

	Renderer renderer(w);
	if (!renderer.HasInitialised()) {
		return -1;
	}

	w.LockMouseToWindow(true);
	w.ShowOSPointer(false);
	
	int CamEventIndex = 2;
	int SceneEventIndex = 1;

	while (w.UpdateWindow() && !Window::GetKeyboard()->KeyDown(KEYBOARD_ESCAPE)) {

		if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_1)) {
			CamEventIndex = 1;
		}
		if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_2)) {
			CamEventIndex = 2;
		}
		
		if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_3)) {
			SceneEventIndex = 1;
		}
		if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_4)) {
			SceneEventIndex = 2;
		}
		if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_5)) {
			SceneEventIndex = 3;
		}

		//camera settings
		if (CamEventIndex == 1)
		{
			renderer.UpdateScene(w.GetTimer()->GetTimeDeltaSeconds());
		}
		else if (CamEventIndex == 2)
		{
			renderer.OrbitCamera(w.GetTimer()->GetTimeDeltaSeconds());

		}

		//scenesettings
		if (SceneEventIndex == 1)
		{
			renderer.RenderSceneDaylight();
			renderer.SwapBuffers();
			if (Window::GetKeyboard()->KeyDown(KEYBOARD_F5)) {
				Shader::ReloadAllShaders();
			}
		}
		else if (SceneEventIndex == 2)
		{
			renderer.RenderSceneNight();
			renderer.SwapBuffers();
			if (Window::GetKeyboard()->KeyDown(KEYBOARD_F5)) {
				Shader::ReloadAllShaders();
			}
		}
		else if (SceneEventIndex == 3)
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