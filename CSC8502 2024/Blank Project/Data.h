#pragma once
#include <iostream>
#include <string>
#include <cmath>
class Data
{
public:
    // �������
    struct CameraData {
        struct Position {
            float x = 0.0f;
            float y = 0.0f;
            float z = 0.0f;
        } position;

        struct Rotation {
            float yaw = 0.0f;   
            float pitch = -45.0f; 
        } rotation;

        float radius = 2000.0f; 
        float speed = -0.1f;   
        float angle = 0.0f;     
    } camera;


    struct EnvironmentData {
        float waterRotate = 0.0f;
        float waterCycle = 0.0f;
    } environment;
    
    struct AnimationData {
        float soldierFrameTime = 0.0f;
        int soldierCurrentFrame = 0;
        float rockMoveTime = 0.0f;
    } animation;


    struct SceneData {
        struct HeightMapSize {
            float x = 0.0f;
            float y = 0.0f;
            float z = 0.0f;
        } heightMapSize;

        struct Center {
            float x = 0.0f;
            float y = 5.0f;
            float z = 0.0f;
        } center;
    } scene;
};

