#pragma once
#include <iostream>
#include <string>
#include <cmath>
class Data
{
public:
    // 相机数据
    struct CameraData {
        struct Position {
            float x = 0.0f;
            float y = 0.0f;
            float z = 0.0f;
        } position;

        struct Rotation {
            float yaw = 0.0f;   // 偏航角
            float pitch = -45.0f; // 俯仰角，默认俯视45度
        } rotation;

        float radius = 2000.0f; // 圆周运动的半径
        float speed = -0.1f;    // 速度，控制旋转速度
        float angle = 0.0f;     // 当前角度
    } camera;

    // 环境控制数据
    struct EnvironmentData {
        float waterRotate = 0.0f;
        float waterCycle = 0.0f;
    } environment;

    // 动画数据
    struct AnimationData {
        float soldierFrameTime = 0.0f;
        int soldierCurrentFrame = 0;
        float rockMoveTime = 0.0f;
    } animation;

    // 场景数据
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

