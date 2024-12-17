#include <iostream>
#include <windows.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <glut.h> 
#include <math.h>
#include <stdio.h>
using namespace std;

#define PI 3.141592

// ���� ����
float shoulderAngle = 0.0f;  // �� ����
float legAngle = 0.0f;       // �ٸ� ����
bool armDirection = true;    // �� ������ ����
bool legDirection = true;    // �ٸ� ������ ����
bool movingRight = true;     // ĳ���� �̵� ����
float characterSpeed = 0.0015f; // ĳ���� �̵� �ӵ�
bool isWalking = false;      // �ȴ� �ִϸ��̼� Ȱ��ȭ ����
float characterRotation = 0.0f;  // ĳ���� ���� ȸ�� ����
float characterPosX = 0.0f;  // ĳ������ ���� X ��ǥ
float jumpHeight = 0.0f;
bool isJumping = false;
float jumpSpeed = 0.002f;
bool jumpUp = true;

// ī�޶� ���� ����
int cameraState = 0; // 0: ����, 1: ���, 2: ����
float cameraDistance = 10.0f; // ī�޶�� ĳ���� ������ �Ÿ�(�� ��� ���)
const float minCamDistance = 2.0f;
const float maxCamDistance = 200.0f;

bool isNameZoomAnimationActive = false; // "��" ����/�ܾƿ� �ִϸ��̼� Ȱ��ȭ ����
int nameZoomStage = 0; // "��" ���� �ܰ� (����, �밢��, �� ��)
float nameZoomTimer = 0.0f; // ����/�ܾƿ� Ÿ�̸�
const float nameZoomDuration = 3.0f; // �� �ܰ��� ���� �ð�
float finalZoomDistance = 2.0f; // ������ "��" ���� �Ÿ�

// "��"�� ��ġ
const float nameX = 45.0f;
const float nameY = -1.7f;
const float nameZ = 0.0f;

// Up-vector �ʱⰪ ����
float upVectorX = 0.0f;
float upVectorY = 1.0f;
float upVectorZ = 0.0f;

// === ī�޶� �ִϸ��̼� ����ü �� ���� ===
struct CameraStep {
    float camX, camY, camZ;       // ī�޶� ��ġ
    float lookAtX, lookAtY, lookAtZ; // �ٶ󺸴� ����
};

// �� ������ ������ ��
const int totalZoomSteps = 4; // "��", ��, ����, ĳ���ͷ� ���ƿ���

CameraStep cameraSteps[4] = {
    // Step 0: ù ��° ������ "��"���� ����
    {16.5f, 0.0f, 3.0f, 16.5f, -1.2f, 0.0f},

    // Step 1: �� ��° ������ ���� ����
    {28.0f, 0.0f, 3.0f, 28.0f, -1.2f, 0.0f},

    // Step 2: �� ��° ������ �������� ����
    {38.0f, 0.0f, 3.0f, 38.0f, -1.2f, 0.0f},

    // Step 3: ĳ���ͷ� ���ƿ��� (�� �ܰ�� �������� ����)
    {0.0f, 3.0f, 10.0f, 0.0f, 0.0f, 0.0f} // ���߿� ĳ���� ��ġ�� ������Ʈ
};

const int totalNameZoomStages = 7; // �� �� �ܰ� (�� ���� ����)

// �ִϸ��̼� ���� ����
bool isCameraAnimationActive = false;      // ī�޶� �ִϸ��̼� Ȱ��ȭ ����
int currentZoomStep = 0;                    // ���� ���� �ܰ� �ε���
float animationTimer = 0.0f;                // �ִϸ��̼� Ÿ�̸�
const float animationDuration = 2.0f;       // �� �ܰ��� �ִϸ��̼� ���� �ð� (��)

// �ִϸ��̼� �� ī�޶� ��ġ �� �ٶ󺸴� ����
float animCamX = 0.0f, animCamY = 0.0f, animCamZ = 0.0f;
float animLookAtX = 0.0f, animLookAtY = 0.0f, animLookAtZ = 0.0f;

// ���� ī�޶� ��ġ (�ִϸ��̼� ���� ����)
float startCamX = 0.0f, startCamY = 0.0f, startCamZ = 0.0f;
float startLookAtX = 0.0f, startLookAtY = 0.0f, startLookAtZ = 0.0f;

// �߱� ���� �ִϸ��̼� ���� ����
float emissionPhase = 0.0f; // ���� ��ȭ ����
const float emissionPhaseSpeed = 0.02f; // ���� ��ȭ �ӵ� (���� ����)

// ���� ��ġ
float lightPosX = 0.0f;
float lightPosY = 1.5f; // Y�� ��ġ ���� ����

// ȸ�� ����
float coinRotationAngle = 0.0f; // ���� ȸ�� ����
// Global variables for rotation
float nameItemRotationAngle = 0.0f;



// Look-at ����Ʈ ����
float lookAtX = 0.0f;
float lookAtY = 0.0f;
float lookAtZ = 0.0f;

// ��ũ��� ���� ����
bool isDarkMode = false; // ���� ������ �� ���� �����ϰ� false�� �ʱ�ȭ
// ��ֹ��� �ٶ󺸴��� ���θ� ����
bool lookingAtObstacles = false;


// �ؽ�ó ID
GLuint texID1, texID2, texID3;

// Light type variable
int currentLightType = 1; // 1: Directional (GL_LIGHT1), 2: Point (GL_LIGHT2), 3: Spotlight (GL_LIGHT3)

// �߰��� ����: Spotlight angle and attenuation controls
float spotlightAngle = 45.0f; // �ʱ� Spotlight ����
float spotlightExponent = 10.0f; // �ʱ� Spotlight ���ߵ�
float spotlightAttenuation = 1.0f; // �ʱ� Attenuation

// �߱� �ִϸ��̼� ����
bool emissionIncreasing = true;

// ���� �Ӽ� ���� �߰�
float ambientIntensity = 0.7f;
float diffuseIntensity = 0.8f;
float specularIntensity = 0.5f;
float shininessValue = 50.0f; // �ʱ� ���̶���Ʈ ����
float emissionIntensity = 0.0f; // �ʱ� �߱� ����


// === �̸� ������ ���� ���� ===
int nameItemsRemoved = 0; // 'i' Ű�� ���� ���ŵ� �̸� �������� �� (0~3)

// ���� ���� �Լ�
void setupLighting() {
    glEnable(GL_LIGHTING); // ���� Ȱ��ȭ

    if (!isDarkMode) {
        // �Ϲ� ��� ����
        glEnable(GL_LIGHT0);
        glDisable(GL_LIGHT1);
        glDisable(GL_LIGHT2);
        glDisable(GL_LIGHT3);

        GLfloat lightPos[] = { lightPosX, lightPosY + jumpHeight, -0.02f, 0.0f }; // ���⼺ ����
        GLfloat lightAmb[] = { ambientIntensity, ambientIntensity, ambientIntensity, 1.0f };
        GLfloat lightDiff[] = { diffuseIntensity, diffuseIntensity, diffuseIntensity, 1.0f };
        GLfloat lightSpec[] = { specularIntensity, specularIntensity, specularIntensity, 1.0f };

        glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
        glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmb);
        glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiff);
        glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpec);

        glEnable(GL_COLOR_MATERIAL);
        glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

        GLfloat defaultAmbient[] = { 0.2f, 0.2f, 0.2f, 1.0f };
        glLightModelfv(GL_LIGHT_MODEL_AMBIENT, defaultAmbient);
    }
    else {
        // ��ũ��� ����
        GLfloat ambientLight[] = { 0.05f, 0.05f, 0.05f, 1.0f };
        glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientLight);

        // ���� ���� Ÿ�Կ� ���� ���� ����
        switch (currentLightType) {
        case 1: // ���⼺ ����
            glEnable(GL_LIGHT1);
            glDisable(GL_LIGHT2);
            glDisable(GL_LIGHT3);
            {
                GLfloat lightDir[] = { 1.0f, -1.0f, 0.0f, 0.0f };
                GLfloat lightAmb[] = { ambientIntensity, ambientIntensity, ambientIntensity, 1.0f };
                GLfloat lightDiff[] = { diffuseIntensity, diffuseIntensity, diffuseIntensity, 1.0f };
                GLfloat lightSpec[] = { specularIntensity, specularIntensity, specularIntensity, 1.0f };

                glLightfv(GL_LIGHT1, GL_POSITION, lightDir);
                glLightfv(GL_LIGHT1, GL_AMBIENT, lightAmb);
                glLightfv(GL_LIGHT1, GL_DIFFUSE, lightDiff);
                glLightfv(GL_LIGHT1, GL_SPECULAR, lightSpec);
            }
            break;
        case 2: // ����Ʈ ����
            glEnable(GL_LIGHT2);
            glDisable(GL_LIGHT1);
            glDisable(GL_LIGHT3);
            {
                GLfloat lightPos[] = { characterPosX, lightPosY + jumpHeight + 1.0f, 2.0f, 1.0f };
                GLfloat lightAmb[] = { ambientIntensity, ambientIntensity, ambientIntensity, 1.0f };
                GLfloat lightDiff[] = { diffuseIntensity, diffuseIntensity, diffuseIntensity, 1.0f };
                GLfloat lightSpec[] = { specularIntensity, specularIntensity, specularIntensity, 1.0f };

                glLightfv(GL_LIGHT2, GL_POSITION, lightPos);
                glLightfv(GL_LIGHT2, GL_AMBIENT, lightAmb);
                glLightfv(GL_LIGHT2, GL_DIFFUSE, lightDiff);
                glLightfv(GL_LIGHT2, GL_SPECULAR, lightSpec);

                // ���� ����
                glLightf(GL_LIGHT2, GL_CONSTANT_ATTENUATION, 1.0f);
                glLightf(GL_LIGHT2, GL_LINEAR_ATTENUATION, 0.05f);
                glLightf(GL_LIGHT2, GL_QUADRATIC_ATTENUATION, 0.001f);
            }
            break;
        case 3: // ����Ʈ����Ʈ
            glEnable(GL_LIGHT3);
            glDisable(GL_LIGHT1);
            glDisable(GL_LIGHT2);
            {
                GLfloat spot_position[] = { characterPosX, lightPosY + jumpHeight + 1.0f, 0.0f, 1.0f };
                GLfloat spot_direction[] = { 0.0f, 0.0f, -1.0f };
                GLfloat spot_diffuse[] = { diffuseIntensity, diffuseIntensity, diffuseIntensity, 1.0f };
                GLfloat spot_specular[] = { specularIntensity, specularIntensity, specularIntensity, 1.0f };
                GLfloat spot_ambient[] = { ambientIntensity, ambientIntensity, ambientIntensity, 1.0f };
                GLfloat spot_cutoff = spotlightAngle;
                GLfloat spot_exp = spotlightExponent;

                glLightfv(GL_LIGHT3, GL_POSITION, spot_position);
                glLightfv(GL_LIGHT3, GL_SPOT_DIRECTION, spot_direction);
                glLightfv(GL_LIGHT3, GL_DIFFUSE, spot_diffuse);
                glLightfv(GL_LIGHT3, GL_SPECULAR, spot_specular);
                glLightfv(GL_LIGHT3, GL_AMBIENT, spot_ambient);
                glLightf(GL_LIGHT3, GL_SPOT_CUTOFF, spot_cutoff);
                glLightf(GL_LIGHT3, GL_SPOT_EXPONENT, spot_exp);

                // ���� ����
                glLightf(GL_LIGHT3, GL_CONSTANT_ATTENUATION, spotlightAttenuation);
                glLightf(GL_LIGHT3, GL_LINEAR_ATTENUATION, 0.05f);
                glLightf(GL_LIGHT3, GL_QUADRATIC_ATTENUATION, 0.001f);
            }
            break;
        default:
            break;
        }
    }
}

// Flashlight �׸��� �Լ� ����
void drawFlashlight() {
    if (!isDarkMode || currentLightType == 0) return; // Only in dark mode with flashlight on

    glPushMatrix();
    // ĳ������ ��ġ�� �������� ������ ��ġ ����
    glTranslatef(characterPosX + 0.5f, jumpHeight + 1.5f, 0.5f);
    glRotatef(characterRotation, 0.0f, 1.0f, 0.0f); // ĳ���� ȸ���� ���� ������ ȸ��
    glScalef(0.1f, 0.05f, 0.3f); // �������� ũ�� ����

    // ������ ���� ����
    GLfloat mat_flashlight_ambient[] = { 0.2f, 0.2f, 0.2f, 1.0f };
    GLfloat mat_flashlight_diffuse[] = { 1.0f, 0.5f, 0.8f, 1.0f };
    GLfloat mat_flashlight_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    GLfloat mat_flashlight_shininess[] = { 50.0f };

    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_flashlight_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_flashlight_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_flashlight_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_flashlight_shininess);

    glColor3f(0.8f, 0.8f, 0.8f); // ���� ȸ�� ����
    glutSolidCube(1.0f); // ������ ���¸� ť��� �׸���
    glPopMatrix();
}

// Handle Camera Controls
void handleCameraControls() {
    float camX = 0.0f;
    float camY = 0.0f;
    float camZ = 0.0f;

    // �켱����: "��" ���� �ִϸ��̼� �Ϸ� ���� Ȯ�� -> "��" �ִϸ��̼� ���� �� Ȯ�� -> p �ִϸ��̼� Ȯ�� -> �⺻ ī�޶� ����
    // "��" ���� �ִϸ��̼� ���� �� ����
    if (nameZoomStage == 6) {
        // "��" ���� �ִϸ��̼� �Ϸ� �� ������ ī�޶� ��
        gluLookAt(45.0f, 0.5f, 3.0f,
            45.0f, -1.2f, 0.0f,
            0.0f, 1.0f, 0.0f);
        return;
    }
    else if (isNameZoomAnimationActive) {
        // "��" ���� �ִϸ��̼� ���� ���� ��
        gluLookAt(animCamX, animCamY, animCamZ,
            animLookAtX, animLookAtY, animLookAtZ,
            upVectorX, upVectorY, upVectorZ);
        return;
    }
    else if (isCameraAnimationActive) {
        // p�� �����ϴ� ī�޶� �ִϸ��̼�(��ֹ� ����) ���� ���� ��
        gluLookAt(animCamX, animCamY, animCamZ,
            animLookAtX, animLookAtY, animLookAtZ,
            upVectorX, upVectorY, upVectorZ);
        return;
    }

    // ������� ���� � �ִϸ��̼ǵ� ���� ������ �����Ƿ� �⺻ ī�޶� ����
    if (lookingAtObstacles) {
        // ��ֹ��� �ٶ󺸵��� ����
        camX = characterPosX;
        camY = 3.0f;
        camZ = cameraDistance;
        gluLookAt(camX, camY, camZ,
            lookAtX, lookAtY, lookAtZ,
            0.0f, 1.0f, 0.0f);
    }
    else {
        // cameraState�� ���� ���� ī�޶� ��ġ ����
        if (cameraState == 0) { // ���� ��
            camX = characterPosX;
            camY = 3.0f;
            camZ = cameraDistance;
            gluLookAt(camX, camY, camZ,
                characterPosX, 0.0f, 0.0f,
                upVectorX, upVectorY, upVectorZ);
        }
        else if (cameraState == 1) { // ��� ��
            camX = characterPosX;
            camY = cameraDistance;
            camZ = 0.0f;
            gluLookAt(camX, camY, camZ,
                characterPosX, 0.0f, 0.0f,
                upVectorX, upVectorY, -1.0f); // ������ �����ٺ��Ƿ� up ���� ����
        }
        else { // ���� ��
            camX = characterPosX - cameraDistance;
            camY = 3.0f;
            camZ = 0.0f;
            gluLookAt(camX, camY, camZ,
                characterPosX, 0.0f, 0.0f,
                upVectorX, upVectorY, upVectorZ);
        }
    }
}


// �� �׸���
void drawCircle(float x, float y, float z, float radius) {
    int numSegments = 100;
    glBegin(GL_TRIANGLE_FAN);
    glVertex3f(x, y, z);
    for (int i = 0; i <= numSegments; i++) {
        float angle = 2.0f * PI * i / numSegments;
        float dx = cos(angle) * radius;
        float dy = sin(angle) * radius;
        glVertex3f(x + dx, y + dy, z);
    }
    glEnd();
}

// ĳ���Ϳ� ����
void setCharacterMaterial() {
    GLfloat mat_a[] = { 0.2f, 0.8f, 0.8f, 1.0f };
    GLfloat mat_d[] = { 0.4f, 0.85f, 0.8f, 1.0f };
    GLfloat mat_s[] = { specularIntensity, specularIntensity, specularIntensity, 1.0f };
    GLfloat mat_sh[] = { shininessValue };
    GLfloat mat_emission[] = { emissionIntensity, emissionIntensity, emissionIntensity, 1.0f };

    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_a);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_d);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_s);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_sh);
    glMaterialfv(GL_FRONT, GL_EMISSION, mat_emission);

    if (isDarkMode) {
        // �߱��� ������� �ʰ�, spotlight�� ĳ���͸� ���߹Ƿ� emission�� 0���� ����
        GLfloat noEmission[] = { 0.0f, 0.0f, 0.0f, 1.0f };
        glMaterialfv(GL_FRONT, GL_EMISSION, noEmission);
    }
    else {
        // �߱� ����
        GLfloat noEmission[] = { 0.0f, 0.0f, 0.0f, 1.0f };
        glMaterialfv(GL_FRONT, GL_EMISSION, noEmission);
    }
}

void setNameMaterial(bool darkMode) {
    GLfloat mat_ambient[] = { 0.2f, 0.2f, 0.2f, 1.0f };
    GLfloat mat_diffuse[] = { 0.0f, 0.5f, 0.9f, 1.0f }; // �⺻ ����: ���� ����
    GLfloat mat_specular[] = { 1.0f, 0.0f, 0.0f, 1.0f }; // ���� �ݻ籤
    GLfloat mat_shininess[] = { shininessValue };       // �������� ������ ��¦�� ����
    GLfloat mat_emission[4] = { 0.0f, 0.0f, 0.0f, 1.0f }; // �߱� �Ӽ� �ʱ�ȭ

    if (darkMode) {
        // ��ũ��忡�� �߱� ȿ�� ����
        float r = (sin(emissionPhase) + 1.0f) / 2.0f; // 0 ~ 1
        float g = (sin(emissionPhase + 2.094f) + 1.0f) / 2.0f; // 120�� ����
        float b = (sin(emissionPhase + 4.188f) + 1.0f) / 2.0f; // 240�� ����

        // �߱� ���� ����
        mat_emission[0] = r * emissionIntensity; // R
        mat_emission[1] = g * emissionIntensity; // G
        mat_emission[2] = b * emissionIntensity; // B
        mat_emission[3] = 1.0f;                  // ���� ��
    }

    // ���� ����
    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
    glMaterialfv(GL_FRONT, GL_EMISSION, mat_emission);
}

// ��� �׸��� �Լ�
void drawBackground() {
    glDisable(GL_LIGHTING); // ���� ��Ȱ��ȭ
    glEnable(GL_TEXTURE_2D); // �ؽ�ó Ȱ��ȭ
    glBindTexture(GL_TEXTURE_2D, texID3); // ��� �ؽ�ó ���ε�
    glColor3f(1.0f, 1.0f, 1.0f); // �ؽ�ó ���� ����

    glBegin(GL_QUADS);
    // �ؽ�ó ��ǥ�� ���� ��ǥ�� ����
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-10.0f, -30.0f, -1.0f); // ���� �Ʒ� ��
    glTexCoord2f(5.0f, 0.0f); glVertex3f(60.0f, -30.0f, -1.0f);  // ������ �Ʒ� ��
    glTexCoord2f(5.0f, 1.55f); glVertex3f(60.0f, 30.0f, -1.0f);   // ������ �� ��
    glTexCoord2f(0.0f, 1.55f); glVertex3f(-10.0f, 30.0f, -1.0f);  // ���� �� ��
    glEnd();

    glDisable(GL_TEXTURE_2D); // �ؽ�ó ��Ȱ��ȭ
    glEnable(GL_LIGHTING); // ���� ����
}

// �� �׸���
void drawMouth() {
    glDisable(GL_LIGHTING);
    glColor3f(0.0f, 0.0f, 0.0f);
    glLineWidth(2.5f);
    glBegin(GL_LINE_STRIP);
    for (int i = 0; i <= 20; i++) {
        float angle = PI * (i / 20.0f);
        float x = cos(angle) * 0.14f;
        float y = sin(angle) * -0.07f;
        glVertex3f(x, y + 0.4f, 0.703f);
    }
    glEnd();
    glLineWidth(1.0f);
    glEnable(GL_LIGHTING);
}

// �� �׸���
void drawFace() {
    glColor3f(0.0f, 0.0f, 0.0f);
    drawCircle(-0.35f, 0.5f, 0.705f, 0.04f); // ���� ��
    drawCircle(0.35f, 0.5f, 0.705f, 0.04f);  // ������ ��
    drawMouth();
}

// �� �׸���
void drawArm(float x, float y, float z, bool isLeft) {
    glPushMatrix();
    glTranslatef(x, y + 0.425f, z);
    glRotatef(isLeft ? -shoulderAngle : shoulderAngle, 1.0f, 0.0f, 0.0f);
    glTranslatef(0.0f, -0.425f, 0.0f);
    glScalef(0.2f, 0.85f, 0.2f);
    glColor3f(0.4f, 0.85f, 0.8f);
    glutSolidCube(1.0f);
    glPopMatrix();
}

// �ٸ� �׸���
void drawLeg(float x, float y, float z, bool isLeft) {
    glPushMatrix();
    glTranslatef(x, y + 0.3f, z);
    glRotatef(isLeft ? legAngle : -legAngle, 1.0f, 0.0f, 0.0f);
    glTranslatef(0.0f, -0.3f, 0.0f);
    glScalef(0.2f, 0.6f, 0.2f);
    glColor3f(0.4f, 0.85f, 0.8f);
    glutSolidCube(1.0f);
    glPopMatrix();
}

// ���ӱ� ��ư �׸���
void drawButtons() {
    // ������ ��ư
    glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_POLYGON);
    glVertex3f(-0.7f, -0.1f, 0.702f);
    glVertex3f(0.2f, -0.1f, 0.702f);
    glVertex3f(0.2f, -0.15f, 0.702f);
    glVertex3f(-0.7f, -0.15f, 0.702f);
    glEnd();

    // ��� ���� ���� ��ư
    glColor3f(0.0f, 0.0f, 0.0f);
    drawCircle(0.55f, -0.13f, 0.702f, 0.045f);

    // ����� ���� ��ư
    glColor3f(1.0f, 0.9f, 0.2f);
    glBegin(GL_POLYGON);
    // ���� �κ�
    glVertex3f(-0.45f, -0.5f, 0.702f);
    glVertex3f(-0.55f, -0.5f, 0.702f);
    glVertex3f(-0.55f, -0.4f, 0.702f);
    glVertex3f(-0.45f, -0.4f, 0.702f);
    // ���κ�
    glVertex3f(-0.45f, -0.4f, 0.702f);
    glVertex3f(-0.45f, -0.3f, 0.702f);
    glVertex3f(-0.35f, -0.3f, 0.702f);
    glVertex3f(-0.35f, -0.4f, 0.702f);
    // ������ �κ�
    glVertex3f(-0.35f, -0.4f, 0.702f);
    glVertex3f(-0.25f, -0.4f, 0.702f);
    glVertex3f(-0.25f, -0.5f, 0.702f);
    glVertex3f(-0.35f, -0.5f, 0.702f);
    // �Ʒ��κ�
    glVertex3f(-0.35f, -0.5f, 0.702f);
    glVertex3f(-0.35f, -0.6f, 0.702f);
    glVertex3f(-0.45f, -0.6f, 0.702f);
    glVertex3f(-0.45f, -0.5f, 0.702f);
    glEnd();

    // �Ķ��� �ﰢ�� ��ư
    glColor3f(0.0f, 0.7f, 1.0f);
    glBegin(GL_TRIANGLES);
    glVertex3f(0.1f, -0.5f, 0.704f);
    glVertex3f(0.4f, -0.5f, 0.704f);
    glVertex3f(0.25f, -0.3f, 0.704f);
    glEnd();

    // ������ ���� ��ư
    glColor3f(1.0f, 0.2f, 0.2f);
    drawCircle(0.4f, -0.75f, 0.704f, 0.14f);

    // �ʷϻ� ���� ��ư
    glColor3f(0.5f, 1.0f, 0.5f);
    drawCircle(0.6f, -0.45f, 0.704f, 0.06f);
}

// ť�� ���� �׸���
void drawBody() {
    glPushMatrix();
    setCharacterMaterial();
    glColor3f(0.4f, 0.8f, 0.7f);
    glScalef(1.8f, 2.0f, 1.4f);
    glutSolidCube(1.0f);
    glPopMatrix();

    // �� �κ�
    glPushMatrix();
    glColor3f(0.8f, 0.95f, 0.9f);
    glBegin(GL_POLYGON);
    glVertex3f(-0.75f, 0.0f, 0.701f);
    glVertex3f(0.75f, 0.0f, 0.701f);
    glVertex3f(0.75f, 0.8f, 0.701f);
    glVertex3f(-0.75f, 0.8f, 0.701f);
    glEnd();
    glPopMatrix();

    drawFace();
    drawButtons();
}

// "��" ��� �׸���
void drawLShape(float x, float y, float z) {

    glDisable(GL_TEXTURE_2D); // �ؽ�ó ��Ȱ��ȭ (�ϰ��� ���� ����)

    glPushMatrix();
    glColor3f(1.0f, 0.5f, 0.5f); // ������ ���� ������ ����
    glTranslatef(x - 0.25f, y + 0.2f, z); // ������ ��ġ
    glScalef(0.05f, 0.4f, 0.05f);
    glutSolidCube(2.0f);
    glPopMatrix();

    glPushMatrix();
    glColor3f(1.0f, 0.5f, 0.5f); // ������ ���� ������ ����
    glTranslatef(x + 0.1f, y - 0.14f, z); // ���� ��ġ
    glScalef(0.3f, 0.05f, 0.05f);
    glutSolidCube(2.0f);
    glPopMatrix();


    glEnable(GL_TEXTURE_2D); // �ؽ�ó ����
}

void drawSphere(float x, float y, float z) {
    GLUquadric* quad = gluNewQuadric(); // Quadric ��ü ����
    glPushMatrix();

    glDisable(GL_TEXTURE_2D); // �ؽ�ó ��Ȱ��ȭ
    glColor3f(0.0f, 0.7f, 0.0f); // �Ķ��� ����

    glTranslatef(x, y, z); // ��ġ ����
    gluSphere(quad, 0.2f, 8, 8); // �ణ ���� ��
    glEnable(GL_TEXTURE_2D); // �ؽ�ó ����
    glPopMatrix();
    gluDeleteQuadric(quad);             // Quadric ��ü ����
}

// "��"�� ���� �׸���
void drawHorizontalLine(float x, float y, float z) {
    glPushMatrix();

    // �׻� �Ķ������� ���� (�ʷϻ����� ����Ǵ� ���� ����)

    glDisable(GL_TEXTURE_2D); // �ؽ�ó ��Ȱ��ȭ
    glColor3f(0.0f, 0.5f, 1.0f); // �Ķ��� ����

    glTranslatef(x, y, z); // ��ġ ����
    glScalef(0.5f, 0.05f, 0.05f); // x�� �������� �� ť��
    glutSolidCube(3.0f);

    glEnable(GL_TEXTURE_2D); // �ؽ�ó ����
    glPopMatrix();
}

// ������ "��" ��ü ��ġ - ������ �κ�
void drawFullName(float x, float y, float z) {
    // "��" �׸��� with ������ Y�� ȸ��
    glPushMatrix();
    setNameMaterial(isDarkMode); // ��ũ��� ���ο� ���� ���� ����
    glColor3f(1.0f, 0.5f, 0.5f); // ������ ������ ����
    glTranslatef(x, y + 1.0f, z);
    glRotatef(nameItemRotationAngle, 0.0f, 1.0f, 0.0f); // Y�� ȸ��
    drawLShape(0.0f, 0.0f, 0.0f);
    glPopMatrix();

    // "��" �׸��� with ���� �ݰ����� �¿� ȸ��
    glPushMatrix();
    setNameMaterial(isDarkMode); // ��ũ��� ���ο� ���� ���� ����
    glTranslatef(x, y + 0.45f, z);

    float smallRadius = 0.1f; // ���� �ݰ� ����
    glTranslatef(smallRadius * sin(nameItemRotationAngle * PI / 180.0f), 0.0f, smallRadius * cos(nameItemRotationAngle * PI / 180.0f)); // �¿� ȸ��
    drawSphere(0.0f, 0.0f, 0.0f);
    glPopMatrix();

    // ���� ť�� �׸��� with X�� ȸ��
    glPushMatrix();
    setNameMaterial(isDarkMode); // ��ũ��� ���ο� ���� ���� ����
    glColor3f(0.0f, 0.5f, 1.0f); // ���� ť�� ���� ����
    glTranslatef(x, y, z);
    glRotatef(nameItemRotationAngle, 0.0f, -1.0f, 0.0f); // X�� ȸ��
    drawHorizontalLine(0.0f, 0.0f, 0.0f);
    glPopMatrix();
}

// ����, ���� �׸��� �Լ�
void drawTexturedCone(float baseRadius, float height, int slices, int stacks) {
    GLUquadric* quad = gluNewQuadric();
    gluQuadricTexture(quad, GL_TRUE);
    gluQuadricNormals(quad, GLU_SMOOTH);
    gluCylinder(quad, baseRadius, 0.0f, height, slices, stacks);
    gluDeleteQuadric(quad);
}

void drawTexturedFlatCylinder(float x, float y, float z, float radius, float height, int slices) {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texID2);
    glColor3f(1.0f, 1.0f, 1.0f); // �ؽ�ó ���� ����

    glPushMatrix();
    glTranslatef(x, y, z);

    float scaledRadius = radius * 1.1f; // �������� 110%�� ����

    // ���� �׸���
    glBegin(GL_QUAD_STRIP);
    for (int i = 0; i <= slices; i++) {
        float theta = 2.0f * PI * float(i) / float(slices);
        float dx = scaledRadius * cos(theta);
        float dy = scaledRadius * sin(theta);

        float s = float(i) / float(slices);
        glTexCoord2f(s, 0.0f); glVertex3f(dx, dy, 0.0f);
        glTexCoord2f(s, 1.0f); glVertex3f(dx, dy, height);
    }
    glEnd();

    // ���� �׸���
    glBegin(GL_TRIANGLE_FAN);
    glTexCoord2f(0.5f, 0.5f); glVertex3f(0.0f, 0.0f, height);
    for (int i = 0; i <= slices; i++) {
        float theta = 2.0f * PI * float(i) / float(slices);
        float dx = scaledRadius * cos(theta);
        float dy = scaledRadius * sin(theta);

        float s = 0.5f + 0.5f * cos(theta);
        float t = 0.5f + 0.5f * sin(theta);
        glTexCoord2f(s, t); glVertex3f(dx, dy, height);
    }
    glEnd();

    // �Ʒ��� �׸���
    glBegin(GL_TRIANGLE_FAN);
    glTexCoord2f(0.5f, 0.5f); glVertex3f(0.0f, 0.0f, 0.0f);
    for (int i = 0; i <= slices; i++) {
        float theta = -2.0f * PI * float(i) / float(slices);
        float dx = scaledRadius * cos(theta);
        float dy = scaledRadius * sin(theta);

        float s = 0.5f + 0.5f * cos(theta);
        float t = 0.5f + 0.5f * sin(theta);
        glTexCoord2f(s, t); glVertex3f(dx, dy, 0.0f);
    }
    glEnd();

    glPopMatrix();
    glDisable(GL_TEXTURE_2D);
}

// ��ֹ� ���� ���� �Լ�
void setObstacleMaterial(bool darkMode) {
    if (darkMode) {
        // �߱� ������ �� ũ�� ����
        GLfloat mat_emission[] = { emissionIntensity, 0.16f * emissionIntensity, 0.0f, 1.0f };
        GLfloat mat_diffuse[] = { 1.0f, 0.5f, 0.0f, 1.0f };  // ��Ȳ�� ���ݻ�
        GLfloat mat_ambient[] = { 0.2f, 0.1f, 0.0f, 1.0f };
        GLfloat mat_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
        GLfloat mat_shininess[] = { 50.0f };

        glMaterialfv(GL_FRONT, GL_EMISSION, mat_emission);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
        glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
        glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
        glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
    }
    else {
        // �߱� ����
        GLfloat mat_emission[] = { 0.0f, 0.0f, 0.0f, 1.0f };
        GLfloat mat_diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };  // �⺻ ���
        glMaterialfv(GL_FRONT, GL_EMISSION, mat_emission);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    }
}

// ��ֹ� �׸���
void drawObstacles() {
    glEnable(GL_TEXTURE_2D); // ��ֹ� �׸��� ���� �ؽ�ó Ȱ��ȭ
    glBindTexture(GL_TEXTURE_2D, texID1); // ��ֹ� �ؽ�ó ���ε�

    // �ؽ�ó ȯ���� MODULATE�� �����Ͽ� �߱��� �ؽ�ó ������ ���������� ��
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    // ��ֹ� ��ġ �迭�� ���������Ƿ� ���� �� �Ʒ��� �������� ������ ����
    // ���� ��ֹ� �׸��⸸ ����

    // ��ֹ� ��ġ (���� ����)
    float obstaclePositions[] = { 8.0f, 12.0f, 21.0f, 25.0f, 31.0f, 36.0f };
    const int numObstacles = sizeof(obstaclePositions) / sizeof(float);

    for (int i = 0; i < numObstacles; i++) {
        glPushMatrix();
        glTranslatef(obstaclePositions[i], -1.7f, 0.0f);

        // ��ħ�� �׸���
        glPushMatrix();
        setObstacleMaterial(isDarkMode); // ��ũ��忡 ���� ���� ����
        glColor3f(1.0f, 0.5f, 0.0f); // ��Ȳ�� ����
        glScalef(0.75f, 0.15f, 0.75f);
        glutSolidCube(1.0f);
        glPopMatrix();

        // ������ �׸���
        glPushMatrix();
        glTranslatef(0.0f, -0.03f, 0.0f);
        glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
        setObstacleMaterial(isDarkMode); // ��ũ��忡 ���� ���� ����
        drawTexturedCone(0.4f, 0.9f, 20, 20);
        glPopMatrix();

        glPopMatrix();
    }

    glDisable(GL_TEXTURE_2D); // ��ֹ� �׸��� �� �ؽ�ó ��Ȱ��ȭ
}

// ���� �׸���
void drawCoin(float x, float y, float z) {
    glPushMatrix();
    glTranslatef(x, y, z);                              // ���� ��ġ ����
    glRotatef(coinRotationAngle, 0.0f, 1.0f, 0.0f);     // Y�� ���� ȸ��

    // ���� ���� ����
    GLfloat mat_emission[] = { emissionIntensity, 0.16f * emissionIntensity, 0.0f, 1.0f }; // ��Ȳ�� �߱�
    GLfloat mat_diffuse[] = { 1.0f, 0.8f, 0.0f, 1.0f };  // �ݻ� ���ݻ�
    GLfloat mat_ambient[] = { 0.2f, 0.16f, 0.0f, 1.0f };
    GLfloat mat_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    GLfloat mat_shininess[] = { 50.0f };

    glMaterialfv(GL_FRONT, GL_EMISSION, mat_emission);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);

    drawTexturedFlatCylinder(0.0f, 0.0f, 0.0f, 0.2f, 0.1f, 50); // ���� �׸���

    glPopMatrix();
}

// �̸� �κ� �׸���
void drawNameParts() {
    glEnable(GL_TEXTURE_2D); // �ؽ�ó Ȱ��ȭ

    // "��" �̸� ������ �� ����
    if (nameItemsRemoved < 1) {
        // "��" �׸���
        glPushMatrix();
        setNameMaterial(isDarkMode); // ��ũ��� ���ο� ���� ���� ����
        glColor3f(1.0f, 0.5f, 0.5f);
        glTranslatef(16.5f, -1.7f, 0.0f);
        glRotatef(nameItemRotationAngle, 0.0f, 1.0f, 0.0f); // ȸ��
        drawLShape(0.0f, 0.0f, 0.0f);
        glPopMatrix();

        // ù ��° ���� �׸���
        glBindTexture(GL_TEXTURE_2D, texID2);
        drawCoin(16.5f, -0.2f, 0.0f);
    }

    // "��" �̸� ������ �� ����
    if (nameItemsRemoved < 2) {
        // "��" �׸���
        glPushMatrix();
        setNameMaterial(isDarkMode); // ���� ���� �߰�
        glColor3f(0.8f, 1.0f, 0.0f);
        glTranslatef(28.0f, -1.7f, 0.0f);
        float smallRadius = 0.1f; // ���� �ݰ� ����
        glTranslatef(smallRadius * sin(nameItemRotationAngle * PI / 180.0f), 0.0f, smallRadius * cos(nameItemRotationAngle * PI / 180.0f)); // �¿� ȸ��
        drawSphere(0.0f, 0.0f, 0.0f);
        glPopMatrix();

        // �� ��° ���� �׸���
        glBindTexture(GL_TEXTURE_2D, texID2);
        drawCoin(28.0f, -0.2f, 0.0f);
    }

    // "��" �̸� ������ �� ����
    if (nameItemsRemoved < 3) {
        // "��" �׸���
        glPushMatrix();
        setNameMaterial(isDarkMode); // ���� ���� �߰�
        glTranslatef(38.0f, -1.7f, 0.0f);
        glRotatef(nameItemRotationAngle, 0.0f, -1.0f, 0.0f); // X�� ȸ��
        drawHorizontalLine(0.0f, 0.0f, 0.0f);
        glPopMatrix();

        // �� ��° ���� �׸���
        glBindTexture(GL_TEXTURE_2D, texID2);
        drawCoin(38.0f, -0.2f, 0.0f);
    }

    // ������ "��" ��ü ��ġ
    glPushMatrix();
    setNameMaterial(isDarkMode); // ���� ���� �߰�
    drawFullName(45.0f, -1.7f, 0.0f);
    glPopMatrix();

    glDisable(GL_TEXTURE_2D);
}

// ĳ���� �׸���
void drawCharacter() {
    glPushMatrix();
    glTranslatef(characterPosX, jumpHeight, 0.0f);
    glRotatef(characterRotation, 0.0f, 1.0f, 0.0f); // ĳ���� ȸ��
    glRotatef(20, 0.0f, 1.0f, 0.0f);
    glScalef(0.6f, 1.2f, 0.6f);

    setCharacterMaterial();
    drawBody();
    drawArm(-1.0f, -0.8f, 0.0f, true);
    drawArm(1.0f, -0.8f, 0.0f, false);
    drawLeg(-0.3f, -1.3f, 0.0f, true);
    drawLeg(0.3f, -1.3f, 0.0f, false);

    // ���� ���� ����

    if (isDarkMode && currentLightType == 3) {
        GLfloat angleRad = characterRotation * PI / 180.0f;
        GLfloat dirX = sin(angleRad);
        GLfloat dirZ = -cos(angleRad);

        GLfloat spot_position[] = { characterPosX, lightPosY + jumpHeight + 1.0f, 0.0f, 1.0f };
        GLfloat spot_direction[] = { dirX, -0.1f, dirZ };

        glLightfv(GL_LIGHT3, GL_POSITION, spot_position);
        glLightfv(GL_LIGHT3, GL_SPOT_DIRECTION, spot_direction);
    }

    glPopMatrix();
}

// �ȱ� �ִϸ��̼�
void handleWalkAnimation() {
    if (!isWalking) return;

    float speed = 0.5f;

    // ��
    if (armDirection) {
        shoulderAngle += speed;
        if (shoulderAngle >= 30.0f) armDirection = false;
    }
    else {
        shoulderAngle -= speed;
        if (shoulderAngle <= -30.0f) armDirection = true;
    }

    // �ٸ�
    if (legDirection) {
        legAngle -= speed;
        if (legAngle <= -30.0f) legDirection = false;
    }
    else {
        legAngle += speed;
        if (legAngle >= 30.0f) legDirection = true;
    }

    // ĳ���� �̵�
    if (movingRight) {
        characterPosX += characterSpeed;
        characterRotation = 0.0f;
    }
    else {
        characterPosX -= characterSpeed;
        characterRotation = 180.0f;
    }

    lightPosX = characterPosX;
    glutPostRedisplay();
}

// ���� �ִϸ��̼�
void handleJumpAnimation() {
    if (!isJumping) return;
    if (jumpUp) {
        jumpHeight += jumpSpeed;
        if (jumpHeight >= 1.6f) jumpUp = false;
    }
    else {
        jumpHeight -= jumpSpeed;
        if (jumpHeight <= 0.0f) {
            jumpHeight = 0.0f;
            isJumping = false;
            jumpUp = true;
        }
    }
    glutPostRedisplay();
}

void updateEmission() {
    if (isDarkMode) {
        emissionIntensity += emissionIncreasing ? 0.02f : -0.02f;
        if (emissionIntensity > 1.5f) { // �ִ� ���� ����
            emissionIntensity = 1.5f;
            emissionIncreasing = false;
        }
        else if (emissionIntensity < 0.1f) {
            emissionIntensity = 0.1f;
            emissionIncreasing = true;
        }
        emissionPhase += 0.05f; // ���� ���� �ӵ�
        if (emissionPhase > 2.0f * PI) emissionPhase -= 2.0f * PI;
    }
    else {
        emissionIntensity = 0.0f; // �Ϲ� ��忡�� �߱� ����
    }
}


void idleFunction() {
    handleWalkAnimation();
    handleJumpAnimation();
    if (isCameraAnimationActive) {
        animationTimer += 0.0004f; // �ִϸ��̼� ���� �ӵ�
        float t = animationTimer / animationDuration;
        if (t > 1.0f) t = 1.0f;

        CameraStep targetStep = cameraSteps[currentZoomStep];

        animCamX = startCamX + (targetStep.camX - startCamX) * t;
        animCamY = startCamY + (targetStep.camY - startCamY) * t;
        animCamZ = startCamZ + (targetStep.camZ - startCamZ) * t;

        animLookAtX = startLookAtX + (targetStep.lookAtX - startLookAtX) * t;
        animLookAtY = startLookAtY + (targetStep.lookAtY - startLookAtY) * t;
        animLookAtZ = startLookAtZ + (targetStep.lookAtZ - startLookAtZ) * t;

        if (animationTimer >= animationDuration) {
            animationTimer = 0.0f;
            currentZoomStep++;
            if (currentZoomStep >= totalZoomSteps) {
                isCameraAnimationActive = false; // �ִϸ��̼� ����
            }
            else {
                startCamX = animCamX;
                startCamY = animCamY;
                startCamZ = animCamZ;
                startLookAtX = animLookAtX;
                startLookAtY = animLookAtY;
                startLookAtZ = animLookAtZ;
            }
        }
    }

    if (isNameZoomAnimationActive) {
        nameZoomTimer += 0.001f; // Ÿ�̸� ����
        float t = nameZoomTimer / nameZoomDuration;
        if (t > 1.0f) t = 1.0f;
        // "��"�� "��", "��", "��" ������ �ܰ躰�� ����
        if (nameZoomStage == 0) { // "��"�� "��" ����
            animCamX = 45.0f; // "��"�� X ��ġ
            animCamY = -1.0f;  // ī�޶� ���� (���� ������ ������)
            animCamZ = 3.0f - 0.5f * t; // "��"�� �������
            animLookAtX = 45.0f; // "��" �߽� �ٶ󺸱�
            animLookAtY = 0.0f;  // "��"�� Y ��ġ
            animLookAtZ = -0.5f;  // "��"�� Z ��ġ
        }
        else if (nameZoomStage == 1) { // "��"�� "��" ����
            animCamX = 45.0f; // "��"�� X ��ġ
            animCamY = 0.8f;  // ī�޶� ���� (���� ����)
            animCamZ = 2.0f - 1.0f * t; // "��"�� �������
            animLookAtX = 45.0f; // "��" �߽� �ٶ󺸱�
            animLookAtY = -1.4f; // "��"�� Y ��ġ
            animLookAtZ = 0.0f;  // "��"�� Z ��ġ
        }
        else if (nameZoomStage == 2) { // "��"�� "��" ����
            animCamX = 45.0f; // "��"�� X ��ġ
            animCamY = 0.0f;  // ī�޶� ���� (���� ����)
            animCamZ = 3.0f - 1.0f * t; // "��"�� �������
            animLookAtX = 45.0f; // "��" �߽� �ٶ󺸱�
            animLookAtY = -3.0f; // "��"�� Y ��ġ
            animLookAtZ = 0.0f;  // "��"�� Z ��ġ
        }

        else if (nameZoomStage == 3) { // ���� ����
            animCamX = 45.0f;
            animCamY = 1.0f;
            animCamZ = 5.0f - 2.0f * t; // ����
            animLookAtX = 45.0f;
            animLookAtY = -1.7f;
            animLookAtZ = 0.0f;
        }
        else if (nameZoomStage == 4) { // �밢�� �� ������
            animCamX = 45.0f + 2.0f * t;
            animCamY = -1.0f + 3.0f * t;
            animCamZ = 3.0f + 2.0f * (1.0f - t);
            animLookAtX = 45.0f;
            animLookAtY = -1.7f;
            animLookAtZ = 0.0f;
        }
        else if (nameZoomStage == 5) { // �밢�� �� ����
            animCamX = 45.0f - 2.0f * t;
            animCamY = -1.0f + 3.0f * t;
            animCamZ = 3.0f + 2.0f * (1.0f - t);
            animLookAtX = 45.0f;
            animLookAtY = -1.7f;
            animLookAtZ = 0.0f;
        }
        else if (nameZoomStage == 6) { // "��" ����
            animCamX = 45.0f;  // "��"�� X ��ǥ�� ����
            animCamY = 0.5f;   // ī�޶� ���̸� ���� �÷� ���鿡 ������
            animCamZ = 0.5f;   // "��"���� ������ �Ÿ� ����

            animLookAtX = 45.0f;  // "��"�� X �߽��� ��Ȯ�� �ٶ󺸱�
            animLookAtY = -1.7f;  // "��"�� �߽ɺθ� �ٶ󺸵��� ����
            animLookAtZ = 0.0f;   // Z �� �������� "��"�� �߽��� ��Ȯ�� �ٶ�

            isNameZoomAnimationActive = false; // �ִϸ��̼� ����
        }

        // �ܰ� ��ȯ
        if (nameZoomTimer >= nameZoomDuration) {
            nameZoomTimer = 0.0f;
            nameZoomStage++;

            if (nameZoomStage >= totalNameZoomStages) {
                nameZoomStage = totalNameZoomStages - 1; // ���� ���� ����
            }
        }

        glutPostRedisplay();
    }

    // ȸ�� ������Ʈ
    coinRotationAngle += 0.4f;
    if (coinRotationAngle >= 360.0f) coinRotationAngle -= 360.0f;

    nameItemRotationAngle += 0.1f; // ȸ�� �ӵ� ����
    if (nameItemRotationAngle >= 360.0f) nameItemRotationAngle -= 360.0f;

    // �߱� �ִϸ��̼� ������Ʈ
    updateEmission();



    glutPostRedisplay();
}

// BMP �ε� �Լ�
unsigned char headerBMP_load[54];
unsigned int dataPos_load;
unsigned int width_load, height_load;
unsigned int imageSize_load;

unsigned char* loadBMP_load(char* fname, bool& is32Bit) {
    FILE* file = fopen(fname, "rb");
    if (!file) {

        return NULL;
    }
    if (fread(headerBMP_load, 1, 54, file) != 54) {

        fclose(file);
        return NULL;
    }
    if (headerBMP_load[0] != 'B' || headerBMP_load[1] != 'M') {

        fclose(file);
        return NULL;
    }

    dataPos_load = *(int*)&(headerBMP_load[0x0A]);
    width_load = *(int*)&(headerBMP_load[0x12]);
    height_load = *(int*)&(headerBMP_load[0x16]);
    imageSize_load = *(int*)&(headerBMP_load[0x22]);
    unsigned short bpp = *(unsigned short*)&(headerBMP_load[0x1C]);

    if (bpp == 32) is32Bit = true;
    else if (bpp == 24) is32Bit = false;
    else {

        fclose(file);
        return NULL;
    }

    if (imageSize_load == 0) imageSize_load = width_load * height_load * (bpp / 8);
    if (dataPos_load == 0) dataPos_load = 54;

    unsigned char* bitmap = new unsigned char[imageSize_load];
    fread(bitmap, 1, imageSize_load, file);
    fclose(file);

    return bitmap;
}

// �ؽ�ó �ʱ�ȭ �Լ�
int initTexture_load() {
    unsigned char* bitmap;
    bool is32Bit;

    glEnable(GL_TEXTURE_2D);

    // ��ֹ� �ؽ�ó �ε�
    glGenTextures(1, &texID1);
    glBindTexture(GL_TEXTURE_2D, texID1);
    bitmap = loadBMP_load((char*)"obstacle.bmp", is32Bit);
    if (!bitmap) return -1;
    if (is32Bit) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width_load, height_load, 0, GL_BGRA_EXT, GL_UNSIGNED_BYTE, bitmap);
    }
    else {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width_load, height_load, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, bitmap);
    }
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    gluBuild2DMipmaps(GL_TEXTURE_2D, is32Bit ? GL_RGBA : GL_RGB, width_load, height_load, is32Bit ? GL_BGRA_EXT : GL_BGR_EXT, GL_UNSIGNED_BYTE, bitmap);
    delete[] bitmap;

    // ���� �ؽ�ó �ε�
    glGenTextures(1, &texID2);
    glBindTexture(GL_TEXTURE_2D, texID2);
    bitmap = loadBMP_load((char*)"coin.bmp", is32Bit);
    if (!bitmap) return -1;
    if (is32Bit) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width_load, height_load, 0, GL_BGRA_EXT, GL_UNSIGNED_BYTE, bitmap);
    }
    else {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width_load, height_load, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, bitmap);
    }
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    gluBuild2DMipmaps(GL_TEXTURE_2D, is32Bit ? GL_RGBA : GL_RGB, width_load, height_load, is32Bit ? GL_BGRA_EXT : GL_BGR_EXT, GL_UNSIGNED_BYTE, bitmap);
    delete[] bitmap;

    // �ؽ�ó ID ���� �� ���ε� - ��� �ؽ�ó
    glGenTextures(1, &texID3);
    glBindTexture(GL_TEXTURE_2D, texID3);
    bitmap = loadBMP_load((char*)"background2.bmp", is32Bit);
    if (bitmap == NULL) {

        return -1;
    }

    // �ؽ�ó �ε� ���� (32��Ʈ�� 24��Ʈ�� ����)
    if (is32Bit) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width_load, height_load, 0, GL_BGRA_EXT, GL_UNSIGNED_BYTE, bitmap);
    }
    else {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width_load, height_load, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, bitmap);
    }

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    gluBuild2DMipmaps(GL_TEXTURE_2D, is32Bit ? GL_RGBA : GL_RGB, width_load, height_load, is32Bit ? GL_BGRA_EXT : GL_BGR_EXT, GL_UNSIGNED_BYTE, bitmap);
    delete[] bitmap;


    return 0;
}

// �ʱ�ȭ
void initializeRendering() {
    glEnable(GL_DEPTH_TEST);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_NORMALIZE);
    glEnable(GL_LIGHTING); // ���� Ȱ��ȭ
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
}


void display() {
    if (isDarkMode) {
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    }
    else {
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    }
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum(-0.4, 0.4, -0.4, 0.4, 1.0, 100.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    handleCameraControls();
    setupLighting();

    drawObstacles();
    // drawItems(); // ������ �׸��� ����
    if (!isDarkMode) drawBackground(); // ��ũ��忡���� ��� �����
    drawNameParts();
    drawCharacter();
    drawFlashlight();

    glutSwapBuffers();
}

// Ű���� �Է� ó��
void handleKeyboard(unsigned char key, int x, int y) {
    switch (key) {
    case 'u': // Light position ���� �̵� (Y�� ����)
        lightPosY += 0.8f;
        setupLighting(); // ����� ���� ���� �ݿ�
        break;

    case 'j': // Light position �Ʒ��� �̵� (Y�� ����)
        lightPosY -= 1.1f;
        setupLighting(); // ����� ���� ���� �ݿ�
        break;

    case 'h': // Light position �������� �̵� (X�� ����)
        lightPosX -= 1.1f;
        setupLighting(); // ����� ���� ���� �ݿ�
        break;

    case 'y': // Light position ���������� �̵� (X�� ����)
        lightPosX += 1.1f;
        setupLighting(); // ����� ���� ���� �ݿ�
        break;

    case 'n': // ��ũ��� ���
        isDarkMode = !isDarkMode;
        break;
    case 'l': // ���������� �̵� ����
        isWalking = true;
        movingRight = true;
        break;
    case 'o': // �ȱ� ����
        isWalking = false;
        break;
    case 'a': // ����
        if (!isJumping) {
            isJumping = true;
        }
        break;
    case 'm': // ī�޶� ���� ����: front->top->side
        cameraState = (cameraState + 1) % 3;
        break;
    case 'k': // ��ֹ��� �ٶ󺸵��� ���� ��ȯ
        if (!lookingAtObstacles) {
            lookAtX = 15.0f;    // ���� ��ֹ� X ��ǥ
            lookAtY = -1.2f;    // ��ֹ� Y ��ǥ (���� ����)
            lookAtZ = 0.0f;     // ��ֹ� Z ��ǥ
            lookingAtObstacles = true;
        }
        else {
            lookAtX = characterPosX;
            lookAtY = 0.0f;
            lookAtZ = 0.0f;
            lookingAtObstacles = false;
        }
        break;
    case 'z': // ����
        cameraDistance -= 0.5f;
        if (cameraDistance < minCamDistance) cameraDistance = minCamDistance;
        break;
    case 'x': // �ܾƿ�
        cameraDistance += 0.5f;
        if (cameraDistance > maxCamDistance) cameraDistance = maxCamDistance;
        break;

    case 'd': // up-vector X ����
        upVectorX -= 0.1f;
        break;
    case 'f': // up-vector X ����
        upVectorX += 0.1f;
        break;
    case '1': // Switch to Directional Light (Flashlight Type)
        currentLightType = 1;
        break;
    case '2': // Switch to Point Light (Flashlight Type)
        currentLightType = 2;
        break;
    case '3': // Switch to Spotlight (Flashlight Type)
        currentLightType = 3;
        break;

    case '4': // Ambient ���� ����
        ambientIntensity += 0.05f;
        if (ambientIntensity > 1.0f) ambientIntensity = 1.0f;
        break;
    case '5': // Ambient ���� ����
        ambientIntensity -= 0.05f;
        if (ambientIntensity < 0.0f) ambientIntensity = 0.0f;
        break;
    case '6': // Diffuse ���� ����
        diffuseIntensity += 0.05f;
        if (diffuseIntensity > 1.0f) diffuseIntensity = 1.0f;
        break;
    case '7': // Diffuse ���� ����
        diffuseIntensity -= 0.05f;
        if (diffuseIntensity < 0.0f) diffuseIntensity = 0.0f;
        break;
    case '8': // Specular ���� ����
        specularIntensity += 0.1f;
        if (specularIntensity > 5.0f) specularIntensity = 5.0f;
        break;
    case '9': // Specular ���� ����
        specularIntensity -= 0.05f;
        if (specularIntensity < 0.0f) specularIntensity = 0.0f;
        break;
    case 'q': // Shininess ����
        shininessValue += 5.0f;
        if (shininessValue > 128.0f) shininessValue = 128.0f;
        setCharacterMaterial(); // ĳ���� ���� ����
        setNameMaterial(isDarkMode); // �̸� ���� ����
        break;

    case 'w': // Shininess ����
        shininessValue -= 5.0f;
        if (shininessValue < 0.0f) shininessValue = 0.0f;
        setCharacterMaterial();
        setNameMaterial(isDarkMode);
        break;

    case 'e': // Emission ����
        emissionIntensity += 0.1f;
        if (emissionIntensity > 1.0f) emissionIntensity = 10.0f;
        setCharacterMaterial();
        setNameMaterial(isDarkMode);
        break;

    case 'r': // Emission ����
        emissionIntensity -= 0.1f;
        if (emissionIntensity < 0.0f) emissionIntensity = 0.0f;
        setCharacterMaterial();
        setNameMaterial(isDarkMode);
        break;

    case 'p': // ī�޶� �ִϸ��̼� Ʈ����
        if (!isCameraAnimationActive) {
            isCameraAnimationActive = true;
            currentZoomStep = 0;
            animationTimer = 0.0f;

            // ������ �ܰ��� ĳ���ͷ� ���ƿ��� ��ġ ����
            cameraSteps[3].camX = characterPosX;
            cameraSteps[3].camY = lightPosY + jumpHeight + 3.0f;
            cameraSteps[3].camZ = cameraDistance;
            cameraSteps[3].lookAtX = characterPosX;
            cameraSteps[3].lookAtY = jumpHeight;
            cameraSteps[3].lookAtZ = 0.0f;

            // ù ��° �ܰ��� ���� ��ġ ����
            startCamX = characterPosX;
            startCamY = lightPosY + jumpHeight + 3.0f;
            startCamZ = cameraDistance;
            startLookAtX = characterPosX;
            startLookAtY = jumpHeight;
            startLookAtZ = 0.0f;

            // ù ��° �ܰ�� �̵�
            animCamX = startCamX;
            animCamY = startCamY;
            animCamZ = startCamZ;
            animLookAtX = startLookAtX;
            animLookAtY = startLookAtY;
            animLookAtZ = startLookAtZ;
        }
        break;

    case 'i': // �̸� ������ ����
        if (nameItemsRemoved < 3) {
            nameItemsRemoved++;
        }
        if (nameItemsRemoved == 3) {
            isWalking = false; // ĳ���� ����
            isNameZoomAnimationActive = true; // "��" ���� �ִϸ��̼� ����
            nameZoomStage = 0;
            nameZoomTimer = 0.0f;
        }
        break;

    }
    setupLighting(); // ���� ���� �ݿ�
    glutPostRedisplay();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
    glutInitWindowSize(800, 800);
    glutCreateWindow("�⸻ ��ü ����");
    initializeRendering();
    if (initTexture_load() != 0) {
        return -1;
    }

    // �ʱ� ��� ������ 'display()'���� �����ϹǷ� ���⼭�� �������� ����.

    glutDisplayFunc(display);
    glutIdleFunc(idleFunction);
    glutKeyboardFunc(handleKeyboard);

    glutMainLoop();
    return 0;
}