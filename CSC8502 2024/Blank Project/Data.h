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
            float yaw = 0.0f;   // ƫ����
            float pitch = -45.0f; // �����ǣ�Ĭ�ϸ���45��
        } rotation;

        float radius = 2000.0f; // Բ���˶��İ뾶
        float speed = -0.1f;    // �ٶȣ�������ת�ٶ�
        float angle = 0.0f;     // ��ǰ�Ƕ�
    } camera;

    // ������������
    struct EnvironmentData {
        float waterRotate = 0.0f;
        float waterCycle = 0.0f;
    } environment;

    // ��������
    struct AnimationData {
        float soldierFrameTime = 0.0f;
        int soldierCurrentFrame = 0;
        float rockMoveTime = 0.0f;
    } animation;

    // ��������
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

