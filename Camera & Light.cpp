#include <iostream>
#include <windows.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <glut.h> 
#include <math.h>
#include <stdio.h>
using namespace std;

#define PI 3.141592

// 상태 변수
float shoulderAngle = 0.0f;  // 팔 각도
float legAngle = 0.0f;       // 다리 각도
bool armDirection = true;    // 팔 움직임 방향
bool legDirection = true;    // 다리 움직임 방향
bool movingRight = true;     // 캐릭터 이동 방향
float characterSpeed = 0.0015f; // 캐릭터 이동 속도
bool isWalking = false;      // 걷는 애니메이션 활성화 여부
float characterRotation = 0.0f;  // 캐릭터 몸의 회전 각도
float characterPosX = 0.0f;  // 캐릭터의 현재 X 좌표
float jumpHeight = 0.0f;
bool isJumping = false;
float jumpSpeed = 0.002f;
bool jumpUp = true;

// 카메라 관련 변수
int cameraState = 0; // 0: 정면, 1: 상단, 2: 측면
float cameraDistance = 10.0f; // 카메라와 캐릭터 사이의 거리(줌 제어에 사용)
const float minCamDistance = 2.0f;
const float maxCamDistance = 200.0f;

bool isNameZoomAnimationActive = false; // "노" 줌인/줌아웃 애니메이션 활성화 여부
int nameZoomStage = 0; // "노" 강조 단계 (정면, 대각선, 뒤 등)
float nameZoomTimer = 0.0f; // 줌인/줌아웃 타이머
const float nameZoomDuration = 3.0f; // 각 단계의 지속 시간
float finalZoomDistance = 2.0f; // 마지막 "노" 강조 거리

// "노"의 위치
const float nameX = 45.0f;
const float nameY = -1.7f;
const float nameZ = 0.0f;

// Up-vector 초기값 설정
float upVectorX = 0.0f;
float upVectorY = 1.0f;
float upVectorZ = 0.0f;

// === 카메라 애니메이션 구조체 및 변수 ===
struct CameraStep {
    float camX, camY, camZ;       // 카메라 위치
    float lookAtX, lookAtY, lookAtZ; // 바라보는 지점
};

// 총 줌인할 아이템 수
const int totalZoomSteps = 4; // "ㄴ", 구, 수평선, 캐릭터로 돌아오기

CameraStep cameraSteps[4] = {
    // Step 0: 첫 번째 아이템 "ㄴ"으로 줌인
    {16.5f, 0.0f, 3.0f, 16.5f, -1.2f, 0.0f},

    // Step 1: 두 번째 아이템 구로 줌인
    {28.0f, 0.0f, 3.0f, 28.0f, -1.2f, 0.0f},

    // Step 2: 세 번째 아이템 수평선으로 줌인
    {38.0f, 0.0f, 3.0f, 38.0f, -1.2f, 0.0f},

    // Step 3: 캐릭터로 돌아오기 (이 단계는 동적으로 설정)
    {0.0f, 3.0f, 10.0f, 0.0f, 0.0f, 0.0f} // 나중에 캐릭터 위치로 업데이트
};

const int totalNameZoomStages = 7; // 총 줌 단계 (각 강조 포함)

// 애니메이션 상태 변수
bool isCameraAnimationActive = false;      // 카메라 애니메이션 활성화 여부
int currentZoomStep = 0;                    // 현재 줌인 단계 인덱스
float animationTimer = 0.0f;                // 애니메이션 타이머
const float animationDuration = 2.0f;       // 각 단계의 애니메이션 지속 시간 (초)

// 애니메이션 중 카메라 위치 및 바라보는 지점
float animCamX = 0.0f, animCamY = 0.0f, animCamZ = 0.0f;
float animLookAtX = 0.0f, animLookAtY = 0.0f, animLookAtZ = 0.0f;

// 시작 카메라 위치 (애니메이션 시작 시점)
float startCamX = 0.0f, startCamY = 0.0f, startCamZ = 0.0f;
float startLookAtX = 0.0f, startLookAtY = 0.0f, startLookAtZ = 0.0f;

// 발광 색상 애니메이션 제어 변수
float emissionPhase = 0.0f; // 색상 변화 위상
const float emissionPhaseSpeed = 0.02f; // 색상 변화 속도 (조절 가능)

// 조명 위치
float lightPosX = 0.0f;
float lightPosY = 1.5f; // Y축 위치 조절 변수

// 회전 각도
float coinRotationAngle = 0.0f; // 동전 회전 각도
// Global variables for rotation
float nameItemRotationAngle = 0.0f;



// Look-at 포인트 변수
float lookAtX = 0.0f;
float lookAtY = 0.0f;
float lookAtZ = 0.0f;

// 다크모드 상태 변수
bool isDarkMode = false; // 전역 변수로 한 번만 정의하고 false로 초기화
// 장애물을 바라보는지 여부를 추적
bool lookingAtObstacles = false;


// 텍스처 ID
GLuint texID1, texID2, texID3;

// Light type variable
int currentLightType = 1; // 1: Directional (GL_LIGHT1), 2: Point (GL_LIGHT2), 3: Spotlight (GL_LIGHT3)

// 추가된 변수: Spotlight angle and attenuation controls
float spotlightAngle = 45.0f; // 초기 Spotlight 각도
float spotlightExponent = 10.0f; // 초기 Spotlight 집중도
float spotlightAttenuation = 1.0f; // 초기 Attenuation

// 발광 애니메이션 변수
bool emissionIncreasing = true;

// 조명 속성 변수 추가
float ambientIntensity = 0.7f;
float diffuseIntensity = 0.8f;
float specularIntensity = 0.5f;
float shininessValue = 50.0f; // 초기 하이라이트 강도
float emissionIntensity = 0.0f; // 초기 발광 강도


// === 이름 아이템 관련 변수 ===
int nameItemsRemoved = 0; // 'i' 키를 눌러 제거된 이름 아이템의 수 (0~3)

// 조명 설정 함수
void setupLighting() {
    glEnable(GL_LIGHTING); // 조명 활성화

    if (!isDarkMode) {
        // 일반 모드 설정
        glEnable(GL_LIGHT0);
        glDisable(GL_LIGHT1);
        glDisable(GL_LIGHT2);
        glDisable(GL_LIGHT3);

        GLfloat lightPos[] = { lightPosX, lightPosY + jumpHeight, -0.02f, 0.0f }; // 방향성 조명
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
        // 다크모드 설정
        GLfloat ambientLight[] = { 0.05f, 0.05f, 0.05f, 1.0f };
        glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientLight);

        // 현재 조명 타입에 따라 조명 설정
        switch (currentLightType) {
        case 1: // 방향성 조명
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
        case 2: // 포인트 조명
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

                // 감쇠 설정
                glLightf(GL_LIGHT2, GL_CONSTANT_ATTENUATION, 1.0f);
                glLightf(GL_LIGHT2, GL_LINEAR_ATTENUATION, 0.05f);
                glLightf(GL_LIGHT2, GL_QUADRATIC_ATTENUATION, 0.001f);
            }
            break;
        case 3: // 스포트라이트
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

                // 감쇠 설정
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

// Flashlight 그리기 함수 정의
void drawFlashlight() {
    if (!isDarkMode || currentLightType == 0) return; // Only in dark mode with flashlight on

    glPushMatrix();
    // 캐릭터의 위치를 기준으로 손전등 위치 설정
    glTranslatef(characterPosX + 0.5f, jumpHeight + 1.5f, 0.5f);
    glRotatef(characterRotation, 0.0f, 1.0f, 0.0f); // 캐릭터 회전에 맞춰 손전등 회전
    glScalef(0.1f, 0.05f, 0.3f); // 손전등의 크기 설정

    // 손전등 재질 설정
    GLfloat mat_flashlight_ambient[] = { 0.2f, 0.2f, 0.2f, 1.0f };
    GLfloat mat_flashlight_diffuse[] = { 1.0f, 0.5f, 0.8f, 1.0f };
    GLfloat mat_flashlight_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    GLfloat mat_flashlight_shininess[] = { 50.0f };

    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_flashlight_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_flashlight_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_flashlight_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_flashlight_shininess);

    glColor3f(0.8f, 0.8f, 0.8f); // 연한 회색 색상
    glutSolidCube(1.0f); // 손전등 형태를 큐브로 그리기
    glPopMatrix();
}

// Handle Camera Controls
void handleCameraControls() {
    float camX = 0.0f;
    float camY = 0.0f;
    float camZ = 0.0f;

    // 우선순위: "노" 강조 애니메이션 완료 상태 확인 -> "노" 애니메이션 진행 중 확인 -> p 애니메이션 확인 -> 기본 카메라 로직
    // "노" 강조 애니메이션 종료 후 고정
    if (nameZoomStage == 6) {
        // "노" 강조 애니메이션 완료 시 고정된 카메라 뷰
        gluLookAt(45.0f, 0.5f, 3.0f,
            45.0f, -1.2f, 0.0f,
            0.0f, 1.0f, 0.0f);
        return;
    }
    else if (isNameZoomAnimationActive) {
        // "노" 강조 애니메이션 진행 중일 때
        gluLookAt(animCamX, animCamY, animCamZ,
            animLookAtX, animLookAtY, animLookAtZ,
            upVectorX, upVectorY, upVectorZ);
        return;
    }
    else if (isCameraAnimationActive) {
        // p로 시작하는 카메라 애니메이션(장애물 줌인) 진행 중일 때
        gluLookAt(animCamX, animCamY, animCamZ,
            animLookAtX, animLookAtY, animLookAtZ,
            upVectorX, upVectorY, upVectorZ);
        return;
    }

    // 여기까지 오면 어떤 애니메이션도 진행 중이지 않으므로 기본 카메라 상태
    if (lookingAtObstacles) {
        // 장애물을 바라보도록 설정
        camX = characterPosX;
        camY = 3.0f;
        camZ = cameraDistance;
        gluLookAt(camX, camY, camZ,
            lookAtX, lookAtY, lookAtZ,
            0.0f, 1.0f, 0.0f);
    }
    else {
        // cameraState에 따른 기존 카메라 위치 설정
        if (cameraState == 0) { // 정면 뷰
            camX = characterPosX;
            camY = 3.0f;
            camZ = cameraDistance;
            gluLookAt(camX, camY, camZ,
                characterPosX, 0.0f, 0.0f,
                upVectorX, upVectorY, upVectorZ);
        }
        else if (cameraState == 1) { // 상단 뷰
            camX = characterPosX;
            camY = cameraDistance;
            camZ = 0.0f;
            gluLookAt(camX, camY, camZ,
                characterPosX, 0.0f, 0.0f,
                upVectorX, upVectorY, -1.0f); // 위에서 내려다보므로 up 벡터 조정
        }
        else { // 측면 뷰
            camX = characterPosX - cameraDistance;
            camY = 3.0f;
            camZ = 0.0f;
            gluLookAt(camX, camY, camZ,
                characterPosX, 0.0f, 0.0f,
                upVectorX, upVectorY, upVectorZ);
        }
    }
}


// 원 그리기
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

// 캐릭터용 재질
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
        // 발광을 사용하지 않고, spotlight이 캐릭터를 비추므로 emission을 0으로 유지
        GLfloat noEmission[] = { 0.0f, 0.0f, 0.0f, 1.0f };
        glMaterialfv(GL_FRONT, GL_EMISSION, noEmission);
    }
    else {
        // 발광 제거
        GLfloat noEmission[] = { 0.0f, 0.0f, 0.0f, 1.0f };
        glMaterialfv(GL_FRONT, GL_EMISSION, noEmission);
    }
}

void setNameMaterial(bool darkMode) {
    GLfloat mat_ambient[] = { 0.2f, 0.2f, 0.2f, 1.0f };
    GLfloat mat_diffuse[] = { 0.0f, 0.5f, 0.9f, 1.0f }; // 기본 재질: 밝은 색상
    GLfloat mat_specular[] = { 1.0f, 0.0f, 0.0f, 1.0f }; // 높은 반사광
    GLfloat mat_shininess[] = { shininessValue };       // 동적으로 설정된 반짝임 강도
    GLfloat mat_emission[4] = { 0.0f, 0.0f, 0.0f, 1.0f }; // 발광 속성 초기화

    if (darkMode) {
        // 다크모드에서 발광 효과 적용
        float r = (sin(emissionPhase) + 1.0f) / 2.0f; // 0 ~ 1
        float g = (sin(emissionPhase + 2.094f) + 1.0f) / 2.0f; // 120도 위상
        float b = (sin(emissionPhase + 4.188f) + 1.0f) / 2.0f; // 240도 위상

        // 발광 색상 설정
        mat_emission[0] = r * emissionIntensity; // R
        mat_emission[1] = g * emissionIntensity; // G
        mat_emission[2] = b * emissionIntensity; // B
        mat_emission[3] = 1.0f;                  // 알파 값
    }

    // 재질 설정
    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
    glMaterialfv(GL_FRONT, GL_EMISSION, mat_emission);
}

// 배경 그리기 함수
void drawBackground() {
    glDisable(GL_LIGHTING); // 조명 비활성화
    glEnable(GL_TEXTURE_2D); // 텍스처 활성화
    glBindTexture(GL_TEXTURE_2D, texID3); // 배경 텍스처 바인딩
    glColor3f(1.0f, 1.0f, 1.0f); // 텍스처 색상 유지

    glBegin(GL_QUADS);
    // 텍스처 좌표와 정점 좌표를 설정
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-10.0f, -30.0f, -1.0f); // 왼쪽 아래 뒤
    glTexCoord2f(5.0f, 0.0f); glVertex3f(60.0f, -30.0f, -1.0f);  // 오른쪽 아래 뒤
    glTexCoord2f(5.0f, 1.55f); glVertex3f(60.0f, 30.0f, -1.0f);   // 오른쪽 위 뒤
    glTexCoord2f(0.0f, 1.55f); glVertex3f(-10.0f, 30.0f, -1.0f);  // 왼쪽 위 뒤
    glEnd();

    glDisable(GL_TEXTURE_2D); // 텍스처 비활성화
    glEnable(GL_LIGHTING); // 조명 복원
}

// 입 그리기
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

// 얼굴 그리기
void drawFace() {
    glColor3f(0.0f, 0.0f, 0.0f);
    drawCircle(-0.35f, 0.5f, 0.705f, 0.04f); // 왼쪽 눈
    drawCircle(0.35f, 0.5f, 0.705f, 0.04f);  // 오른쪽 눈
    drawMouth();
}

// 팔 그리기
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

// 다리 그리기
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

// 게임기 버튼 그리기
void drawButtons() {
    // 검정색 버튼
    glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_POLYGON);
    glVertex3f(-0.7f, -0.1f, 0.702f);
    glVertex3f(0.2f, -0.1f, 0.702f);
    glVertex3f(0.2f, -0.15f, 0.702f);
    glVertex3f(-0.7f, -0.15f, 0.702f);
    glEnd();

    // 상단 검정 원형 버튼
    glColor3f(0.0f, 0.0f, 0.0f);
    drawCircle(0.55f, -0.13f, 0.702f, 0.045f);

    // 노란색 십자 버튼
    glColor3f(1.0f, 0.9f, 0.2f);
    glBegin(GL_POLYGON);
    // 왼쪽 부분
    glVertex3f(-0.45f, -0.5f, 0.702f);
    glVertex3f(-0.55f, -0.5f, 0.702f);
    glVertex3f(-0.55f, -0.4f, 0.702f);
    glVertex3f(-0.45f, -0.4f, 0.702f);
    // 윗부분
    glVertex3f(-0.45f, -0.4f, 0.702f);
    glVertex3f(-0.45f, -0.3f, 0.702f);
    glVertex3f(-0.35f, -0.3f, 0.702f);
    glVertex3f(-0.35f, -0.4f, 0.702f);
    // 오른쪽 부분
    glVertex3f(-0.35f, -0.4f, 0.702f);
    glVertex3f(-0.25f, -0.4f, 0.702f);
    glVertex3f(-0.25f, -0.5f, 0.702f);
    glVertex3f(-0.35f, -0.5f, 0.702f);
    // 아랫부분
    glVertex3f(-0.35f, -0.5f, 0.702f);
    glVertex3f(-0.35f, -0.6f, 0.702f);
    glVertex3f(-0.45f, -0.6f, 0.702f);
    glVertex3f(-0.45f, -0.5f, 0.702f);
    glEnd();

    // 파란색 삼각형 버튼
    glColor3f(0.0f, 0.7f, 1.0f);
    glBegin(GL_TRIANGLES);
    glVertex3f(0.1f, -0.5f, 0.704f);
    glVertex3f(0.4f, -0.5f, 0.704f);
    glVertex3f(0.25f, -0.3f, 0.704f);
    glEnd();

    // 빨간색 원형 버튼
    glColor3f(1.0f, 0.2f, 0.2f);
    drawCircle(0.4f, -0.75f, 0.704f, 0.14f);

    // 초록색 원형 버튼
    glColor3f(0.5f, 1.0f, 0.5f);
    drawCircle(0.6f, -0.45f, 0.704f, 0.06f);
}

// 큐브 몸통 그리기
void drawBody() {
    glPushMatrix();
    setCharacterMaterial();
    glColor3f(0.4f, 0.8f, 0.7f);
    glScalef(1.8f, 2.0f, 1.4f);
    glutSolidCube(1.0f);
    glPopMatrix();

    // 얼굴 부분
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

// "ㄴ" 모양 그리기
void drawLShape(float x, float y, float z) {

    glDisable(GL_TEXTURE_2D); // 텍스처 비활성화 (일관된 색상 유지)

    glPushMatrix();
    glColor3f(1.0f, 0.5f, 0.5f); // 동일한 연한 빨간색 설정
    glTranslatef(x - 0.25f, y + 0.2f, z); // 수직선 위치
    glScalef(0.05f, 0.4f, 0.05f);
    glutSolidCube(2.0f);
    glPopMatrix();

    glPushMatrix();
    glColor3f(1.0f, 0.5f, 0.5f); // 동일한 연한 빨간색 설정
    glTranslatef(x + 0.1f, y - 0.14f, z); // 수평선 위치
    glScalef(0.3f, 0.05f, 0.05f);
    glutSolidCube(2.0f);
    glPopMatrix();


    glEnable(GL_TEXTURE_2D); // 텍스처 복원
}

void drawSphere(float x, float y, float z) {
    GLUquadric* quad = gluNewQuadric(); // Quadric 객체 생성
    glPushMatrix();

    glDisable(GL_TEXTURE_2D); // 텍스처 비활성화
    glColor3f(0.0f, 0.7f, 0.0f); // 파란색 설정

    glTranslatef(x, y, z); // 위치 조정
    gluSphere(quad, 0.2f, 8, 8); // 약간 각진 구
    glEnable(GL_TEXTURE_2D); // 텍스처 복원
    glPopMatrix();
    gluDeleteQuadric(quad);             // Quadric 객체 삭제
}

// "ㅗ"의 수평선 그리기
void drawHorizontalLine(float x, float y, float z) {
    glPushMatrix();

    // 항상 파란색으로 설정 (초록색으로 변경되는 문제 방지)

    glDisable(GL_TEXTURE_2D); // 텍스처 비활성화
    glColor3f(0.0f, 0.5f, 1.0f); // 파란색 설정

    glTranslatef(x, y, z); // 위치 조정
    glScalef(0.5f, 0.05f, 0.05f); // x축 방향으로 긴 큐브
    glutSolidCube(3.0f);

    glEnable(GL_TEXTURE_2D); // 텍스처 복원
    glPopMatrix();
}

// 마지막 "노" 전체 배치 - 수정된 부분
void drawFullName(float x, float y, float z) {
    // "ㄴ" 그리기 with 동일한 Y축 회전
    glPushMatrix();
    setNameMaterial(isDarkMode); // 다크모드 여부에 따른 재질 설정
    glColor3f(1.0f, 0.5f, 0.5f); // 동일한 빨간색 설정
    glTranslatef(x, y + 1.0f, z);
    glRotatef(nameItemRotationAngle, 0.0f, 1.0f, 0.0f); // Y축 회전
    drawLShape(0.0f, 0.0f, 0.0f);
    glPopMatrix();

    // "구" 그리기 with 작은 반경으로 좌우 회전
    glPushMatrix();
    setNameMaterial(isDarkMode); // 다크모드 여부에 따른 재질 설정
    glTranslatef(x, y + 0.45f, z);

    float smallRadius = 0.1f; // 작은 반경 설정
    glTranslatef(smallRadius * sin(nameItemRotationAngle * PI / 180.0f), 0.0f, smallRadius * cos(nameItemRotationAngle * PI / 180.0f)); // 좌우 회전
    drawSphere(0.0f, 0.0f, 0.0f);
    glPopMatrix();

    // 수평 큐브 그리기 with X축 회전
    glPushMatrix();
    setNameMaterial(isDarkMode); // 다크모드 여부에 따른 재질 설정
    glColor3f(0.0f, 0.5f, 1.0f); // 수평 큐브 색상 설정
    glTranslatef(x, y, z);
    glRotatef(nameItemRotationAngle, 0.0f, -1.0f, 0.0f); // X축 회전
    drawHorizontalLine(0.0f, 0.0f, 0.0f);
    glPopMatrix();
}

// 원통, 원뿔 그리기 함수
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
    glColor3f(1.0f, 1.0f, 1.0f); // 텍스처 색상 유지

    glPushMatrix();
    glTranslatef(x, y, z);

    float scaledRadius = radius * 1.1f; // 반지름을 110%로 설정

    // 옆면 그리기
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

    // 윗면 그리기
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

    // 아랫면 그리기
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

// 장애물 재질 설정 함수
void setObstacleMaterial(bool darkMode) {
    if (darkMode) {
        // 발광 강도를 더 크게 조정
        GLfloat mat_emission[] = { emissionIntensity, 0.16f * emissionIntensity, 0.0f, 1.0f };
        GLfloat mat_diffuse[] = { 1.0f, 0.5f, 0.0f, 1.0f };  // 주황색 난반사
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
        // 발광 제거
        GLfloat mat_emission[] = { 0.0f, 0.0f, 0.0f, 1.0f };
        GLfloat mat_diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };  // 기본 흰색
        glMaterialfv(GL_FRONT, GL_EMISSION, mat_emission);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    }
}

// 장애물 그리기
void drawObstacles() {
    glEnable(GL_TEXTURE_2D); // 장애물 그리기 전에 텍스처 활성화
    glBindTexture(GL_TEXTURE_2D, texID1); // 장애물 텍스처 바인드

    // 텍스처 환경을 MODULATE로 설정하여 발광과 텍스처 색상이 곱해지도록 함
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    // 장애물 위치 배열을 제거했으므로 안전 콘 아래에 아이템이 없음을 가정
    // 따라서 장애물 그리기만 수행

    // 장애물 위치 (수동 설정)
    float obstaclePositions[] = { 8.0f, 12.0f, 21.0f, 25.0f, 31.0f, 36.0f };
    const int numObstacles = sizeof(obstaclePositions) / sizeof(float);

    for (int i = 0; i < numObstacles; i++) {
        glPushMatrix();
        glTranslatef(obstaclePositions[i], -1.7f, 0.0f);

        // 받침대 그리기
        glPushMatrix();
        setObstacleMaterial(isDarkMode); // 다크모드에 따른 재질 설정
        glColor3f(1.0f, 0.5f, 0.0f); // 주황색 설정
        glScalef(0.75f, 0.15f, 0.75f);
        glutSolidCube(1.0f);
        glPopMatrix();

        // 안전콘 그리기
        glPushMatrix();
        glTranslatef(0.0f, -0.03f, 0.0f);
        glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
        setObstacleMaterial(isDarkMode); // 다크모드에 따른 재질 설정
        drawTexturedCone(0.4f, 0.9f, 20, 20);
        glPopMatrix();

        glPopMatrix();
    }

    glDisable(GL_TEXTURE_2D); // 장애물 그리기 후 텍스처 비활성화
}

// 동전 그리기
void drawCoin(float x, float y, float z) {
    glPushMatrix();
    glTranslatef(x, y, z);                              // 동전 위치 설정
    glRotatef(coinRotationAngle, 0.0f, 1.0f, 0.0f);     // Y축 기준 회전

    // 동전 재질 설정
    GLfloat mat_emission[] = { emissionIntensity, 0.16f * emissionIntensity, 0.0f, 1.0f }; // 주황색 발광
    GLfloat mat_diffuse[] = { 1.0f, 0.8f, 0.0f, 1.0f };  // 금색 난반사
    GLfloat mat_ambient[] = { 0.2f, 0.16f, 0.0f, 1.0f };
    GLfloat mat_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    GLfloat mat_shininess[] = { 50.0f };

    glMaterialfv(GL_FRONT, GL_EMISSION, mat_emission);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);

    drawTexturedFlatCylinder(0.0f, 0.0f, 0.0f, 0.2f, 0.1f, 50); // 동전 그리기

    glPopMatrix();
}

// 이름 부분 그리기
void drawNameParts() {
    glEnable(GL_TEXTURE_2D); // 텍스처 활성화

    // "ㄴ" 이름 아이템 및 동전
    if (nameItemsRemoved < 1) {
        // "ㄴ" 그리기
        glPushMatrix();
        setNameMaterial(isDarkMode); // 다크모드 여부에 따른 재질 설정
        glColor3f(1.0f, 0.5f, 0.5f);
        glTranslatef(16.5f, -1.7f, 0.0f);
        glRotatef(nameItemRotationAngle, 0.0f, 1.0f, 0.0f); // 회전
        drawLShape(0.0f, 0.0f, 0.0f);
        glPopMatrix();

        // 첫 번째 동전 그리기
        glBindTexture(GL_TEXTURE_2D, texID2);
        drawCoin(16.5f, -0.2f, 0.0f);
    }

    // "구" 이름 아이템 및 동전
    if (nameItemsRemoved < 2) {
        // "구" 그리기
        glPushMatrix();
        setNameMaterial(isDarkMode); // 재질 설정 추가
        glColor3f(0.8f, 1.0f, 0.0f);
        glTranslatef(28.0f, -1.7f, 0.0f);
        float smallRadius = 0.1f; // 작은 반경 설정
        glTranslatef(smallRadius * sin(nameItemRotationAngle * PI / 180.0f), 0.0f, smallRadius * cos(nameItemRotationAngle * PI / 180.0f)); // 좌우 회전
        drawSphere(0.0f, 0.0f, 0.0f);
        glPopMatrix();

        // 두 번째 동전 그리기
        glBindTexture(GL_TEXTURE_2D, texID2);
        drawCoin(28.0f, -0.2f, 0.0f);
    }

    // "ㅡ" 이름 아이템 및 동전
    if (nameItemsRemoved < 3) {
        // "ㅡ" 그리기
        glPushMatrix();
        setNameMaterial(isDarkMode); // 재질 설정 추가
        glTranslatef(38.0f, -1.7f, 0.0f);
        glRotatef(nameItemRotationAngle, 0.0f, -1.0f, 0.0f); // X축 회전
        drawHorizontalLine(0.0f, 0.0f, 0.0f);
        glPopMatrix();

        // 세 번째 동전 그리기
        glBindTexture(GL_TEXTURE_2D, texID2);
        drawCoin(38.0f, -0.2f, 0.0f);
    }

    // 마지막 "노" 전체 배치
    glPushMatrix();
    setNameMaterial(isDarkMode); // 재질 설정 추가
    drawFullName(45.0f, -1.7f, 0.0f);
    glPopMatrix();

    glDisable(GL_TEXTURE_2D);
}

// 캐릭터 그리기
void drawCharacter() {
    glPushMatrix();
    glTranslatef(characterPosX, jumpHeight, 0.0f);
    glRotatef(characterRotation, 0.0f, 1.0f, 0.0f); // 캐릭터 회전
    glRotatef(20, 0.0f, 1.0f, 0.0f);
    glScalef(0.6f, 1.2f, 0.6f);

    setCharacterMaterial();
    drawBody();
    drawArm(-1.0f, -0.8f, 0.0f, true);
    drawArm(1.0f, -0.8f, 0.0f, false);
    drawLeg(-0.3f, -1.3f, 0.0f, true);
    drawLeg(0.3f, -1.3f, 0.0f, false);

    // 조명 설정 제거

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

// 걷기 애니메이션
void handleWalkAnimation() {
    if (!isWalking) return;

    float speed = 0.5f;

    // 팔
    if (armDirection) {
        shoulderAngle += speed;
        if (shoulderAngle >= 30.0f) armDirection = false;
    }
    else {
        shoulderAngle -= speed;
        if (shoulderAngle <= -30.0f) armDirection = true;
    }

    // 다리
    if (legDirection) {
        legAngle -= speed;
        if (legAngle <= -30.0f) legDirection = false;
    }
    else {
        legAngle += speed;
        if (legAngle >= 30.0f) legDirection = true;
    }

    // 캐릭터 이동
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

// 점프 애니메이션
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
        if (emissionIntensity > 1.5f) { // 최대 강도 증가
            emissionIntensity = 1.5f;
            emissionIncreasing = false;
        }
        else if (emissionIntensity < 0.1f) {
            emissionIntensity = 0.1f;
            emissionIncreasing = true;
        }
        emissionPhase += 0.05f; // 위상 증가 속도
        if (emissionPhase > 2.0f * PI) emissionPhase -= 2.0f * PI;
    }
    else {
        emissionIntensity = 0.0f; // 일반 모드에서 발광 제거
    }
}


void idleFunction() {
    handleWalkAnimation();
    handleJumpAnimation();
    if (isCameraAnimationActive) {
        animationTimer += 0.0004f; // 애니메이션 진행 속도
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
                isCameraAnimationActive = false; // 애니메이션 종료
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
        nameZoomTimer += 0.001f; // 타이머 증가
        float t = nameZoomTimer / nameZoomDuration;
        if (t > 1.0f) t = 1.0f;
        // "노"의 "ㄴ", "구", "ㅡ" 강조를 단계별로 구현
        if (nameZoomStage == 0) { // "노"의 "ㄴ" 강조
            animCamX = 45.0f; // "ㄴ"의 X 위치
            animCamY = -1.0f;  // 카메라 높이 (조금 위에서 내려봄)
            animCamZ = 3.0f - 0.5f * t; // "ㄴ"에 가까워짐
            animLookAtX = 45.0f; // "ㄴ" 중심 바라보기
            animLookAtY = 0.0f;  // "ㄴ"의 Y 위치
            animLookAtZ = -0.5f;  // "ㄴ"의 Z 위치
        }
        else if (nameZoomStage == 1) { // "노"의 "구" 강조
            animCamX = 45.0f; // "구"의 X 위치
            animCamY = 0.8f;  // 카메라 높이 (수평 유지)
            animCamZ = 2.0f - 1.0f * t; // "구"에 가까워짐
            animLookAtX = 45.0f; // "구" 중심 바라보기
            animLookAtY = -1.4f; // "구"의 Y 위치
            animLookAtZ = 0.0f;  // "구"의 Z 위치
        }
        else if (nameZoomStage == 2) { // "노"의 "ㅡ" 강조
            animCamX = 45.0f; // "ㅡ"의 X 위치
            animCamY = 0.0f;  // 카메라 높이 (수평 유지)
            animCamZ = 3.0f - 1.0f * t; // "ㅡ"에 가까워짐
            animLookAtX = 45.0f; // "ㅡ" 중심 바라보기
            animLookAtY = -3.0f; // "ㅡ"의 Y 위치
            animLookAtZ = 0.0f;  // "ㅡ"의 Z 위치
        }

        else if (nameZoomStage == 3) { // 정면 강조
            animCamX = 45.0f;
            animCamY = 1.0f;
            animCamZ = 5.0f - 2.0f * t; // 줌인
            animLookAtX = 45.0f;
            animLookAtY = -1.7f;
            animLookAtZ = 0.0f;
        }
        else if (nameZoomStage == 4) { // 대각선 위 오른쪽
            animCamX = 45.0f + 2.0f * t;
            animCamY = -1.0f + 3.0f * t;
            animCamZ = 3.0f + 2.0f * (1.0f - t);
            animLookAtX = 45.0f;
            animLookAtY = -1.7f;
            animLookAtZ = 0.0f;
        }
        else if (nameZoomStage == 5) { // 대각선 위 왼쪽
            animCamX = 45.0f - 2.0f * t;
            animCamY = -1.0f + 3.0f * t;
            animCamZ = 3.0f + 2.0f * (1.0f - t);
            animLookAtX = 45.0f;
            animLookAtY = -1.7f;
            animLookAtZ = 0.0f;
        }
        else if (nameZoomStage == 6) { // "노" 고정
            animCamX = 45.0f;  // "노"의 X 좌표에 맞춤
            animCamY = 0.5f;   // 카메라 높이를 조금 올려 정면에 가깝게
            animCamZ = 0.5f;   // "노"와의 적당한 거리 설정

            animLookAtX = 45.0f;  // "노"의 X 중심을 정확히 바라보기
            animLookAtY = -1.7f;  // "노"의 중심부를 바라보도록 조정
            animLookAtZ = 0.0f;   // Z 축 방향으로 "노"의 중심을 정확히 바라봄

            isNameZoomAnimationActive = false; // 애니메이션 종료
        }

        // 단계 전환
        if (nameZoomTimer >= nameZoomDuration) {
            nameZoomTimer = 0.0f;
            nameZoomStage++;

            if (nameZoomStage >= totalNameZoomStages) {
                nameZoomStage = totalNameZoomStages - 1; // 고정 상태 유지
            }
        }

        glutPostRedisplay();
    }

    // 회전 업데이트
    coinRotationAngle += 0.4f;
    if (coinRotationAngle >= 360.0f) coinRotationAngle -= 360.0f;

    nameItemRotationAngle += 0.1f; // 회전 속도 조정
    if (nameItemRotationAngle >= 360.0f) nameItemRotationAngle -= 360.0f;

    // 발광 애니메이션 업데이트
    updateEmission();



    glutPostRedisplay();
}

// BMP 로드 함수
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

// 텍스처 초기화 함수
int initTexture_load() {
    unsigned char* bitmap;
    bool is32Bit;

    glEnable(GL_TEXTURE_2D);

    // 장애물 텍스처 로드
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

    // 동전 텍스처 로드
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

    // 텍스처 ID 생성 및 바인딩 - 배경 텍스처
    glGenTextures(1, &texID3);
    glBindTexture(GL_TEXTURE_2D, texID3);
    bitmap = loadBMP_load((char*)"background2.bmp", is32Bit);
    if (bitmap == NULL) {

        return -1;
    }

    // 텍스처 로딩 설정 (32비트와 24비트를 구분)
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

// 초기화
void initializeRendering() {
    glEnable(GL_DEPTH_TEST);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_NORMALIZE);
    glEnable(GL_LIGHTING); // 조명 활성화
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
    // drawItems(); // 아이템 그리기 제거
    if (!isDarkMode) drawBackground(); // 다크모드에서는 배경 미출력
    drawNameParts();
    drawCharacter();
    drawFlashlight();

    glutSwapBuffers();
}

// 키보드 입력 처리
void handleKeyboard(unsigned char key, int x, int y) {
    switch (key) {
    case 'u': // Light position 위로 이동 (Y축 증가)
        lightPosY += 0.8f;
        setupLighting(); // 변경된 조명 설정 반영
        break;

    case 'j': // Light position 아래로 이동 (Y축 감소)
        lightPosY -= 1.1f;
        setupLighting(); // 변경된 조명 설정 반영
        break;

    case 'h': // Light position 왼쪽으로 이동 (X축 감소)
        lightPosX -= 1.1f;
        setupLighting(); // 변경된 조명 설정 반영
        break;

    case 'y': // Light position 오른쪽으로 이동 (X축 증가)
        lightPosX += 1.1f;
        setupLighting(); // 변경된 조명 설정 반영
        break;

    case 'n': // 다크모드 토글
        isDarkMode = !isDarkMode;
        break;
    case 'l': // 오른쪽으로 이동 시작
        isWalking = true;
        movingRight = true;
        break;
    case 'o': // 걷기 정지
        isWalking = false;
        break;
    case 'a': // 점프
        if (!isJumping) {
            isJumping = true;
        }
        break;
    case 'm': // 카메라 상태 변경: front->top->side
        cameraState = (cameraState + 1) % 3;
        break;
    case 'k': // 장애물을 바라보도록 시점 전환
        if (!lookingAtObstacles) {
            lookAtX = 15.0f;    // 예시 장애물 X 좌표
            lookAtY = -1.2f;    // 장애물 Y 좌표 (지면 높이)
            lookAtZ = 0.0f;     // 장애물 Z 좌표
            lookingAtObstacles = true;
        }
        else {
            lookAtX = characterPosX;
            lookAtY = 0.0f;
            lookAtZ = 0.0f;
            lookingAtObstacles = false;
        }
        break;
    case 'z': // 줌인
        cameraDistance -= 0.5f;
        if (cameraDistance < minCamDistance) cameraDistance = minCamDistance;
        break;
    case 'x': // 줌아웃
        cameraDistance += 0.5f;
        if (cameraDistance > maxCamDistance) cameraDistance = maxCamDistance;
        break;

    case 'd': // up-vector X 감소
        upVectorX -= 0.1f;
        break;
    case 'f': // up-vector X 증가
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

    case '4': // Ambient 조절 증가
        ambientIntensity += 0.05f;
        if (ambientIntensity > 1.0f) ambientIntensity = 1.0f;
        break;
    case '5': // Ambient 조절 감소
        ambientIntensity -= 0.05f;
        if (ambientIntensity < 0.0f) ambientIntensity = 0.0f;
        break;
    case '6': // Diffuse 조절 증가
        diffuseIntensity += 0.05f;
        if (diffuseIntensity > 1.0f) diffuseIntensity = 1.0f;
        break;
    case '7': // Diffuse 조절 감소
        diffuseIntensity -= 0.05f;
        if (diffuseIntensity < 0.0f) diffuseIntensity = 0.0f;
        break;
    case '8': // Specular 조절 증가
        specularIntensity += 0.1f;
        if (specularIntensity > 5.0f) specularIntensity = 5.0f;
        break;
    case '9': // Specular 조절 감소
        specularIntensity -= 0.05f;
        if (specularIntensity < 0.0f) specularIntensity = 0.0f;
        break;
    case 'q': // Shininess 증가
        shininessValue += 5.0f;
        if (shininessValue > 128.0f) shininessValue = 128.0f;
        setCharacterMaterial(); // 캐릭터 재질 갱신
        setNameMaterial(isDarkMode); // 이름 재질 갱신
        break;

    case 'w': // Shininess 감소
        shininessValue -= 5.0f;
        if (shininessValue < 0.0f) shininessValue = 0.0f;
        setCharacterMaterial();
        setNameMaterial(isDarkMode);
        break;

    case 'e': // Emission 증가
        emissionIntensity += 0.1f;
        if (emissionIntensity > 1.0f) emissionIntensity = 10.0f;
        setCharacterMaterial();
        setNameMaterial(isDarkMode);
        break;

    case 'r': // Emission 감소
        emissionIntensity -= 0.1f;
        if (emissionIntensity < 0.0f) emissionIntensity = 0.0f;
        setCharacterMaterial();
        setNameMaterial(isDarkMode);
        break;

    case 'p': // 카메라 애니메이션 트리거
        if (!isCameraAnimationActive) {
            isCameraAnimationActive = true;
            currentZoomStep = 0;
            animationTimer = 0.0f;

            // 마지막 단계인 캐릭터로 돌아오는 위치 설정
            cameraSteps[3].camX = characterPosX;
            cameraSteps[3].camY = lightPosY + jumpHeight + 3.0f;
            cameraSteps[3].camZ = cameraDistance;
            cameraSteps[3].lookAtX = characterPosX;
            cameraSteps[3].lookAtY = jumpHeight;
            cameraSteps[3].lookAtZ = 0.0f;

            // 첫 번째 단계의 시작 위치 설정
            startCamX = characterPosX;
            startCamY = lightPosY + jumpHeight + 3.0f;
            startCamZ = cameraDistance;
            startLookAtX = characterPosX;
            startLookAtY = jumpHeight;
            startLookAtZ = 0.0f;

            // 첫 번째 단계로 이동
            animCamX = startCamX;
            animCamY = startCamY;
            animCamZ = startCamZ;
            animLookAtX = startLookAtX;
            animLookAtY = startLookAtY;
            animLookAtZ = startLookAtZ;
        }
        break;

    case 'i': // 이름 아이템 제거
        if (nameItemsRemoved < 3) {
            nameItemsRemoved++;
        }
        if (nameItemsRemoved == 3) {
            isWalking = false; // 캐릭터 멈춤
            isNameZoomAnimationActive = true; // "노" 강조 애니메이션 시작
            nameZoomStage = 0;
            nameZoomTimer = 0.0f;
        }
        break;

    }
    setupLighting(); // 조명 설정 반영
    glutPostRedisplay();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
    glutInitWindowSize(800, 800);
    glutCreateWindow("기말 대체 과제");
    initializeRendering();
    if (initTexture_load() != 0) {
        return -1;
    }

    // 초기 배경 색상은 'display()'에서 설정하므로 여기서는 설정하지 않음.

    glutDisplayFunc(display);
    glutIdleFunc(idleFunction);
    glutKeyboardFunc(handleKeyboard);

    glutMainLoop();
    return 0;
}