
#include <iostream>

#include "../Dependencies/include/irrKlang/include/irrKlang.h"
#include "../Engine/include/Scene.hpp"
#include <algorithm>

#define GLEW_STATIC 1   // This allows linking with Static Library on Windows, without DLL
#include <GL/glew.h>    // Include GLEW - OpenGL Extension Wrangler

#include <GLFW/glfw3.h> // GLFW provides a cross-platform interface for creating a graphical context,
// initializing OpenGL and binding inputs

#include <glm/glm.hpp>  // GLM is an optimized math library with syntax to similar to OpenGL Shading Language
#include <glm/gtc/matrix_transform.hpp> // include this to create transformation matrices
#include <glm/gtc/type_ptr.hpp>

using namespace glm;
using namespace std;
using namespace irrklang;

//#pragma comment(lib, "../Dependencies/include/irrKlang/lib/Win32-visualStudio/irrKlang.lib") //link with irrKlang.dll


class ReflectionsRoomScene : public Scene
{
public:
    /* Scene Globals */
    /* ------------- */
    float window_width, window_height;
    float room_width = 50 * Unit;
    float room_height = 25 * Unit;
    float room_bound_neg_x = -49.5 * Unit;
    float room_bound_pos_x = 49.5 * Unit;
    float room_bound_neg_z = -49.5 * Unit;
    float room_bound_pos_z = 49.5 * Unit;

    /* Interaction */
    bool bReflections0Enabled = false;
    bool bReflections1Enabled = false;
    bool bReflections2Enabled = false;
    bool bReflections3Enabled = false;
    bool bFinalState = false;
    float fInteractionLength = 2 * Unit;
    float timer0 = 0.0f;

    /* Models */
    Model* mGround;
    Model* mAxisX, * mAxisY, * mAxisZ;
    Model* mFloor, * mCeiling, * mLeftWall, * mRightWall, * mFarWall, * mNearWall;
    Model* mPedestal0Foot, * mPedestal0Column, * mPedestal0Top;
    Model* mReflections0Arm0, * mReflections0Arm1, * mReflections0Arm2, * mReflections0Arm3, * mReflections0Sphere;
    Model* mRedCarpet, * mRedSquare;
    Model* mPedestal1Foot, * mPedestal1Column, * mPedestal1Top;
    Model* mReflections1Arm0, * mReflections1Arm1, * mReflections1Arm2, * mReflections1Arm3, * mReflections1Sphere;
    Model* mPedestal2Foot, * mPedestal2Column, * mPedestal2Top;
    Model* mReflections2Arm0, * mReflections2Arm1, * mReflections2Arm2, * mReflections2Arm3, * mReflections2Sphere;
    Model* mPedestal3Foot, * mPedestal3Column, * mPedestal3Top;
    Model* mReflections3Arm0, * mReflections3Arm1, * mReflections3Arm2, * mReflections3Arm3, * mReflections3Sphere;


    /* Loading Shaders
    --------------------------------*/
    ShaderProgram* scene_shader = new ShaderProgram("./res/shaders/artifact.fs", "./res/shaders/artifact.vs");

    /* Loading Textures
    --------------------------------*/
    Texture* ceiling_texture = new Texture("./res/textures/white-painted-ceiling-texture.jpg");
    Texture* Reflections_arm_texture = new Texture("./res/textures/bark.jpg");


    /* Constructor */
    ReflectionsRoomScene(float windowWidth, float windowHeight)
    {
        window_width = windowWidth;
        window_height = windowHeight;
    }

    /* This initializes the scene */
    void inline Initialize(void)
    {
        SetSceneShaderProgram(scene_shader);
        SetCameraView(glm::vec3(0.0f, 2 * Unit, 25 * Unit), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        SetCameraPerspective(70.0f, window_width / window_height, 0.001 * Unit, 200 * Unit);
        SetCameraBoundingBox(room_bound_neg_x, room_bound_pos_x, room_bound_neg_z, room_bound_pos_z);

        /* Lighting */
        SetSceneLightPositionOne(glm::vec3(0.0f, 45 * Unit, 0.0f));
        SetSceneLightDirectionOne(glm::vec3(0.0f, -1.0f, 0.0f));
        //SetSceneLightColourOne(glm::vec4(0.7f, 0.1f, 0.1f, 1.0f));
        SetSceneLightCutoffOne(glm::cos(glm::radians(12.5f)));
        SetSceneLightSwitchOne(true);

        // SetSceneLightPositionTwo(glm::vec3(45*Unit, 45*Unit, 0.0f));
        // SetSceneLightDirectionTwo(glm::vec3(0.0f, -1.0f, 0.0f));
        // SetSceneLightColourTwo(glm::vec4(0.1f, 0.7f, 0.1f, 1.0f));
        // SetSceneLightCutoffTwo(glm::cos(glm::radians(12.5f)));
        // SetSceneLightSwitchTwo(true);

        // SetSceneLightPositionThree(glm::vec3(-45*Unit, 45*Unit, 0.0f));
        // SetSceneLightDirectionThree(glm::vec3(0.0f, -1.0f, 0.0f));
        // SetSceneLightColourThree(glm::vec4(0.1f, 0.1f, 0.7f, 1.0f));
        // SetSceneLightCutoffThree(glm::cos(glm::radians(12.5f)));
        // SetSceneLightSwitchThree(true);

        /* Modelling */
        // Floor Model
        mFloor = CreateModelPrimitive(PLANE, NULL);
        mFloor->ScaleModel(glm::vec3(room_width, 1.0f, room_width));
        mFloor->SetModelFragmentColour(glm::vec4(0.5f, 0.5f, 0.5f, 1.0f));

        // Ceiling Model
        mCeiling = CreateModelPrimitive(PLANE, NULL);
        mCeiling->ScaleModel(glm::vec3(room_width, 1.0f, room_width));
        mCeiling->RotateModel(glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        mCeiling->TranslateModel(glm::vec3(0.0f, room_width, 0.0f));
        mCeiling->SetModelTexture(ceiling_texture);

        // Left Wall Model
        mLeftWall = CreateModelPrimitive(PLANE, NULL);
        mLeftWall->ScaleModel(glm::vec3(room_width, 1.0f, room_height));
        mLeftWall->RotateModel(glm::radians(-90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        mLeftWall->RotateModel(glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        mLeftWall->TranslateModel(glm::vec3(-room_width, room_height, 0.0f));
        mLeftWall->SetModelFragmentColour(glm::vec4(0.4f, 0.4f, 0.4f, 1.0f));

        // Right Wall Model
        mRightWall = CreateModelPrimitive(PLANE, NULL);
        mRightWall->ScaleModel(glm::vec3(room_width, 1.0f, room_height));
        mRightWall->RotateModel(glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        mRightWall->RotateModel(glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        mRightWall->TranslateModel(glm::vec3(room_width, room_height, 0.0f));
        mRightWall->SetModelFragmentColour(glm::vec4(0.4f, 0.4f, 0.4f, 1.0f));

        // Far Wall Model
        mFarWall = CreateModelPrimitive(PLANE, NULL);
        mFarWall->ScaleModel(glm::vec3(room_width, 1.0f, room_height));
        mFarWall->RotateModel(glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        mFarWall->TranslateModel(glm::vec3(0.0f, room_height, -room_width));
        mFarWall->SetModelFragmentColour(glm::vec4(0.4f, 0.4f, 0.4f, 1.0f));

        // Near Wall Model
        mNearWall = CreateModelPrimitive(PLANE, NULL);
        mNearWall->ScaleModel(glm::vec3(room_width, 1.0f, room_height));
        mNearWall->RotateModel(glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        mNearWall->TranslateModel(glm::vec3(0.0f, room_height, room_width));
        mNearWall->SetModelFragmentColour(glm::vec4(0.4f, 0.4f, 0.4f, 1.0f));

        // Axis Models
        // mAxisX = CreateModelPrimitive(CUBE, NULL);
        // mAxisX->ScaleModel(glm::vec3(2.0f, 0.05f, 0.05f));
        // mAxisX->TranslateModel(glm::vec3(Unit, 0.0f, 0.0f));
        // mAxisX->SetModelFragmentColour(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));

        // mAxisY = CreateModelPrimitive(CUBE, NULL);
        // mAxisY->ScaleModel(glm::vec3(2.0f, 0.05f, 0.05f));
        // mAxisY->RotateModel(glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        // mAxisY->TranslateModel(glm::vec3(0.0f, Unit, 0.0f));
        // mAxisY->SetModelFragmentColour(glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));    

        // mAxisZ = CreateModelPrimitive(CUBE, NULL);
        // mAxisZ->ScaleModel(glm::vec3(2.0f, 0.05f, 0.05f));
        // mAxisZ->RotateModel(glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        // mAxisZ->TranslateModel(glm::vec3(0.0f, 0.0f, Unit));
        // mAxisZ->SetModelFragmentColour(glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));

        /* Model Red Carpet */
        mRedCarpet = CreateModelPrimitive(CUBE, NULL);
        mRedCarpet->ScaleModel(glm::vec3(2.5 * Unit, 0.05 * Unit, 50 * Unit));
        mRedCarpet->TranslateModel(glm::vec3(0.0f, 0.0f, 25 * Unit));
        mRedCarpet->SetModelFragmentColour(glm::vec4(0.4f, 0.0f, 0.0f, 1.0f));

        mRedSquare = CreateModelPrimitive(CUBE, mRedCarpet);
        mRedSquare->ScaleModel(glm::vec3(5 * Unit, 0.05 * Unit, 5 * Unit));
        mRedSquare->SetModelFragmentColour(glm::vec4(0.4f, 0.0f, 0.0f, 1.0f));


        /* Model Pedestal 0*/
        mPedestal0Foot = CreateModelPrimitive(CUBE, NULL);
        mPedestal0Foot->ScaleModel(glm::vec3(2 * Unit, 0.1 * Unit, 2 * Unit));
        mPedestal0Foot->SetModelFragmentColour(glm::vec4(0.3f, 0.3f, 0.3f, 1.0f));

        mPedestal0Column = CreateModelPrimitive(CUBE, mPedestal0Foot);
        mPedestal0Column->ScaleModel(glm::vec3(1.5 * Unit, 0.5 * Unit, 1.5 * Unit));
        mPedestal0Column->TranslateModel(glm::vec3(0.0f, 0.1 * Unit, 0.0f));
        mPedestal0Column->SetModelFragmentColour(glm::vec4(0.3f, 0.3f, 0.3f, 1.0f));

        mPedestal0Top = CreateModelPrimitive(CUBE, mPedestal0Foot);
        mPedestal0Top->ScaleModel(glm::vec3(2 * Unit, 0.1 * Unit, 2 * Unit));
        mPedestal0Top->TranslateModel(glm::vec3(0.0f, 0.6 * Unit, 0.0f));
        mPedestal0Top->SetModelFragmentColour(glm::vec4(0.3, 0.3f, 0.3f, 1.0f));

        /* Model Reflections 0*/
        mReflections0Sphere = CreateModelPrimitive(SPHERE, NULL);
        mReflections0Sphere->ScaleModel(glm::vec3(0.65f, 0.65f, 0.65f));
        mReflections0Sphere->TranslateModel(glm::vec3(0.0f, 0.75 * Unit, 0.0f));
        mReflections0Sphere->SetModelFragmentColour(glm::vec4(1.0f, 1.0f, 1.0f, 0.4f));
        mReflections0Sphere->SetModelTransparency(true);

        mReflections0Arm0 = CreateModelPrimitive(CUBE, mReflections0Sphere);
        mReflections0Arm0->ScaleModel(glm::vec3(1.5 * Unit, 0.1f, 0.1f));
        mReflections0Arm0->RotateModel(glm::radians(55.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        mReflections0Arm0->RotateModel(glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        mReflections0Arm0->SetModelTexture(Reflections_arm_texture);

        mReflections0Arm1 = CreateModelPrimitive(CUBE, mReflections0Sphere);
        mReflections0Arm1->ScaleModel(glm::vec3(1.5 * Unit, 0.1f, 0.1f));
        mReflections0Arm1->RotateModel(glm::radians(55.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        mReflections0Arm1->RotateModel(glm::radians(315.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        mReflections0Arm1->SetModelTexture(Reflections_arm_texture);

        mReflections0Arm2 = CreateModelPrimitive(CUBE, mReflections0Sphere);
        mReflections0Arm2->ScaleModel(glm::vec3(1.5 * Unit, 0.1f, 0.1f));
        mReflections0Arm2->RotateModel(glm::radians(55.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        mReflections0Arm2->RotateModel(glm::radians(225.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        mReflections0Arm2->SetModelTexture(Reflections_arm_texture);

        mReflections0Arm3 = CreateModelPrimitive(CUBE, mReflections0Sphere);
        mReflections0Arm3->ScaleModel(glm::vec3(1.5 * Unit, 0.1f, 0.1f));
        mReflections0Arm3->RotateModel(glm::radians(55.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        mReflections0Arm3->RotateModel(glm::radians(135.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        mReflections0Arm3->SetModelTexture(Reflections_arm_texture);

        mReflections0Sphere->TranslateModelAndChildren(glm::vec3(0.0f, 1.25 * Unit, 0.0f));

        /* Model Pedestal 1*/
        mPedestal1Foot = CreateModelPrimitive(CUBE, NULL);
        mPedestal1Foot->ScaleModel(glm::vec3(2 * Unit, 0.1 * Unit, 2 * Unit));
        mPedestal1Foot->SetModelFragmentColour(glm::vec4(0.3f, 0.3f, 0.3f, 1.0f));

        mPedestal1Column = CreateModelPrimitive(CUBE, mPedestal1Foot);
        mPedestal1Column->ScaleModel(glm::vec3(1.5 * Unit, 0.5 * Unit, 1.5 * Unit));
        mPedestal1Column->TranslateModel(glm::vec3(0.0f, 0.1 * Unit, 0.0f));
        mPedestal1Column->SetModelFragmentColour(glm::vec4(0.3f, 0.3f, 0.3f, 1.0f));

        mPedestal1Top = CreateModelPrimitive(CUBE, mPedestal1Foot);
        mPedestal1Top->ScaleModel(glm::vec3(2 * Unit, 0.1 * Unit, 2 * Unit));
        mPedestal1Top->TranslateModel(glm::vec3(0.0f, 0.6 * Unit, 0.0f));
        mPedestal1Top->SetModelFragmentColour(glm::vec4(0.3, 0.3f, 0.3f, 1.0f));

        mPedestal1Foot->TranslateModelAndChildren(glm::vec3(-48 * Unit, 0.0f, 0.0f));

        /* Model Reflections 1*/
        mReflections1Sphere = CreateModelPrimitive(SPHERE, NULL);
        mReflections1Sphere->ScaleModel(glm::vec3(0.65f, 0.65f, 0.65f));
        mReflections1Sphere->TranslateModel(glm::vec3(0.0f, 0.75 * Unit, 0.0f));
        mReflections1Sphere->SetModelFragmentColour(glm::vec4(1.0f, 0.0f, 0.0f, 0.4f));
        mReflections1Sphere->SetModelTransparency(true);

        mReflections1Arm0 = CreateModelPrimitive(CUBE, mReflections1Sphere);
        mReflections1Arm0->ScaleModel(glm::vec3(1.5 * Unit, 0.1f, 0.1f));
        mReflections1Arm0->RotateModel(glm::radians(55.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        mReflections1Arm0->RotateModel(glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        mReflections1Arm0->SetModelTexture(Reflections_arm_texture);

        mReflections1Arm1 = CreateModelPrimitive(CUBE, mReflections1Sphere);
        mReflections1Arm1->ScaleModel(glm::vec3(1.5 * Unit, 0.1f, 0.1f));
        mReflections1Arm1->RotateModel(glm::radians(55.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        mReflections1Arm1->RotateModel(glm::radians(315.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        mReflections1Arm1->SetModelTexture(Reflections_arm_texture);

        mReflections1Arm2 = CreateModelPrimitive(CUBE, mReflections1Sphere);
        mReflections1Arm2->ScaleModel(glm::vec3(1.5 * Unit, 0.1f, 0.1f));
        mReflections1Arm2->RotateModel(glm::radians(55.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        mReflections1Arm2->RotateModel(glm::radians(225.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        mReflections1Arm2->SetModelTexture(Reflections_arm_texture);

        mReflections1Arm3 = CreateModelPrimitive(CUBE, mReflections1Sphere);
        mReflections1Arm3->ScaleModel(glm::vec3(1.5 * Unit, 0.1f, 0.1f));
        mReflections1Arm3->RotateModel(glm::radians(55.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        mReflections1Arm3->RotateModel(glm::radians(135.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        mReflections1Arm3->SetModelTexture(Reflections_arm_texture);

        mReflections1Sphere->TranslateModelAndChildren(glm::vec3(-48 * Unit, 1.25 * Unit, 0.0f));

        /* Model Pedestal 2*/
        mPedestal2Foot = CreateModelPrimitive(CUBE, NULL);
        mPedestal2Foot->ScaleModel(glm::vec3(2 * Unit, 0.1 * Unit, 2 * Unit));
        mPedestal2Foot->SetModelFragmentColour(glm::vec4(0.3f, 0.3f, 0.3f, 1.0f));

        mPedestal2Column = CreateModelPrimitive(CUBE, mPedestal2Foot);
        mPedestal2Column->ScaleModel(glm::vec3(1.5 * Unit, 0.5 * Unit, 1.5 * Unit));
        mPedestal2Column->TranslateModel(glm::vec3(0.0f, 0.1 * Unit, 0.0f));
        mPedestal2Column->SetModelFragmentColour(glm::vec4(0.3f, 0.3f, 0.3f, 1.0f));

        mPedestal2Top = CreateModelPrimitive(CUBE, mPedestal2Foot);
        mPedestal2Top->ScaleModel(glm::vec3(2 * Unit, 0.1 * Unit, 2 * Unit));
        mPedestal2Top->TranslateModel(glm::vec3(0.0f, 0.6 * Unit, 0.0f));
        mPedestal2Top->SetModelFragmentColour(glm::vec4(0.3, 0.3f, 0.3f, 1.0f));

        mPedestal2Foot->TranslateModelAndChildren(glm::vec3(48 * Unit, 0.0f, 0.0f));

        /* Model Reflections 2*/
        mReflections2Sphere = CreateModelPrimitive(SPHERE, NULL);
        mReflections2Sphere->ScaleModel(glm::vec3(0.65f, 0.65f, 0.65f));
        mReflections2Sphere->TranslateModel(glm::vec3(0.0f, 0.75 * Unit, 0.0f));
        mReflections2Sphere->SetModelFragmentColour(glm::vec4(0.0f, 0.0f, 1.0f, 0.4f));
        mReflections2Sphere->SetModelTransparency(true);

        mReflections2Arm0 = CreateModelPrimitive(CUBE, mReflections2Sphere);
        mReflections2Arm0->ScaleModel(glm::vec3(1.5 * Unit, 0.1f, 0.1f));
        mReflections2Arm0->RotateModel(glm::radians(55.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        mReflections2Arm0->RotateModel(glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        mReflections2Arm0->SetModelTexture(Reflections_arm_texture);

        mReflections2Arm1 = CreateModelPrimitive(CUBE, mReflections2Sphere);
        mReflections2Arm1->ScaleModel(glm::vec3(1.5 * Unit, 0.1f, 0.1f));
        mReflections2Arm1->RotateModel(glm::radians(55.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        mReflections2Arm1->RotateModel(glm::radians(315.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        mReflections2Arm1->SetModelTexture(Reflections_arm_texture);

        mReflections2Arm2 = CreateModelPrimitive(CUBE, mReflections2Sphere);
        mReflections2Arm2->ScaleModel(glm::vec3(1.5 * Unit, 0.1f, 0.1f));
        mReflections2Arm2->RotateModel(glm::radians(55.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        mReflections2Arm2->RotateModel(glm::radians(225.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        mReflections2Arm2->SetModelTexture(Reflections_arm_texture);

        mReflections2Arm3 = CreateModelPrimitive(CUBE, mReflections2Sphere);
        mReflections2Arm3->ScaleModel(glm::vec3(1.5 * Unit, 0.1f, 0.1f));
        mReflections2Arm3->RotateModel(glm::radians(55.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        mReflections2Arm3->RotateModel(glm::radians(135.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        mReflections2Arm3->SetModelTexture(Reflections_arm_texture);

        mReflections2Sphere->TranslateModelAndChildren(glm::vec3(48 * Unit, 1.25 * Unit, 0.0f));

        /* Model Pedestal 3*/
        mPedestal3Foot = CreateModelPrimitive(CUBE, NULL);
        mPedestal3Foot->ScaleModel(glm::vec3(2 * Unit, 0.1 * Unit, 2 * Unit));
        mPedestal3Foot->SetModelFragmentColour(glm::vec4(0.3f, 0.3f, 0.3f, 1.0f));

        mPedestal3Column = CreateModelPrimitive(CUBE, mPedestal3Foot);
        mPedestal3Column->ScaleModel(glm::vec3(1.5 * Unit, 0.5 * Unit, 1.5 * Unit));
        mPedestal3Column->TranslateModel(glm::vec3(0.0f, 0.1 * Unit, 0.0f));
        mPedestal3Column->SetModelFragmentColour(glm::vec4(0.3f, 0.3f, 0.3f, 1.0f));

        mPedestal3Top = CreateModelPrimitive(CUBE, mPedestal3Foot);
        mPedestal3Top->ScaleModel(glm::vec3(2 * Unit, 0.1 * Unit, 2 * Unit));
        mPedestal3Top->TranslateModel(glm::vec3(0.0f, 0.6 * Unit, 0.0f));
        mPedestal3Top->SetModelFragmentColour(glm::vec4(0.3, 0.3f, 0.3f, 1.0f));

        mPedestal3Foot->TranslateModelAndChildren(glm::vec3(0.0f, 0.0f, -48 * Unit));

        /* Model Reflections 3*/
        mReflections3Sphere = CreateModelPrimitive(SPHERE, NULL);
        mReflections3Sphere->ScaleModel(glm::vec3(0.65f, 0.65f, 0.65f));
        mReflections3Sphere->TranslateModel(glm::vec3(0.0f, 0.75 * Unit, 0.0f));
        mReflections3Sphere->SetModelFragmentColour(glm::vec4(0.0f, 1.0f, 0.0f, 0.4f));
        mReflections3Sphere->SetModelTransparency(true);

        mReflections3Arm0 = CreateModelPrimitive(CUBE, mReflections3Sphere);
        mReflections3Arm0->ScaleModel(glm::vec3(1.5 * Unit, 0.1f, 0.1f));
        mReflections3Arm0->RotateModel(glm::radians(55.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        mReflections3Arm0->RotateModel(glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        mReflections3Arm0->SetModelTexture(Reflections_arm_texture);

        mReflections3Arm1 = CreateModelPrimitive(CUBE, mReflections3Sphere);
        mReflections3Arm1->ScaleModel(glm::vec3(1.5 * Unit, 0.1f, 0.1f));
        mReflections3Arm1->RotateModel(glm::radians(55.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        mReflections3Arm1->RotateModel(glm::radians(315.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        mReflections3Arm1->SetModelTexture(Reflections_arm_texture);

        mReflections3Arm2 = CreateModelPrimitive(CUBE, mReflections3Sphere);
        mReflections3Arm2->ScaleModel(glm::vec3(1.5 * Unit, 0.1f, 0.1f));
        mReflections3Arm2->RotateModel(glm::radians(55.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        mReflections3Arm2->RotateModel(glm::radians(225.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        mReflections3Arm2->SetModelTexture(Reflections_arm_texture);

        mReflections3Arm3 = CreateModelPrimitive(CUBE, mReflections3Sphere);
        mReflections3Arm3->ScaleModel(glm::vec3(1.5 * Unit, 0.1f, 0.1f));
        mReflections3Arm3->RotateModel(glm::radians(55.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        mReflections3Arm3->RotateModel(glm::radians(135.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        mReflections3Arm3->SetModelTexture(Reflections_arm_texture);

        mReflections3Sphere->TranslateModelAndChildren(glm::vec3(0.0f, 1.25 * Unit, -48 * Unit));
    }

    /* This is called once per frame */
    void inline Update(float dt)
    {
        if (bFinalState)
        {
            timer0 += dt;
            if (timer0 >= 2.0f)
            {
                SetSceneLightPositionOne(glm::vec3(0.0f, 45 * Unit, 0.0f));
                SetSceneLightDirectionOne(glm::vec3(0.0f, -1.0f, 0.0f));
                SetSceneLightColourOne(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
                SetSceneLightCutoffOne(glm::cos(glm::radians(25.0f)));
                SetSceneLightSwitchTwo(false);
                SetSceneLightSwitchThree(false);
                bFinalState = false;
            }
        }
        BindReflectionsRoomUniforms();
        DrawScene();
    }

    void SetKeyCallback(GLFWwindow* window)
    {
        glfwSetKeyCallback(window, ReflectionsRoom_keyCallback);
    }

    static void ReflectionsRoom_keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
    {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);
    }

    /* Define Key Bindings */
    void inline InputController(GLFWwindow* window)
    {
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        {
            /* Calculate distances to Reflectionss */
            if (glm::length(mReflections0Sphere->GetModelPosition() - GetCameraPosition()) <= fInteractionLength && !bReflections0Enabled)
            {
                bReflections0Enabled = true;

                SetSceneLightPositionOne(glm::vec3(0.0f, 45 * Unit, -45 * Unit));
                SetSceneLightDirectionOne(glm::vec3(0.0f, -1.0f, 0.0f));
                SetSceneLightColourOne(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
                SetSceneLightCutoffOne(glm::cos(glm::radians(12.5f)));
                SetSceneLightSwitchOne(true);

                SetSceneLightPositionTwo(glm::vec3(45 * Unit, 45 * Unit, 0.0f));
                SetSceneLightDirectionTwo(glm::vec3(0.0f, -1.0f, 0.0f));
                SetSceneLightColourTwo(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
                SetSceneLightCutoffTwo(glm::cos(glm::radians(12.5f)));
                SetSceneLightSwitchTwo(true);

                SetSceneLightPositionThree(glm::vec3(-45 * Unit, 45 * Unit, 0.0f));
                SetSceneLightDirectionThree(glm::vec3(0.0f, -1.0f, 0.0f));
                SetSceneLightColourThree(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
                SetSceneLightCutoffThree(glm::cos(glm::radians(12.5f)));
                SetSceneLightSwitchThree(true);

            }
            if (glm::length(mReflections1Sphere->GetModelPosition() - GetCameraPosition()) <= fInteractionLength && bReflections0Enabled && !bReflections1Enabled)
            {
                bReflections1Enabled = true;
                SetSceneLightPositionThree(glm::vec3(-45 * Unit, 45 * Unit, 0.0f));
                SetSceneLightColourThree(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
                SetSceneLightCutoffThree(glm::cos(glm::radians(12.5f)));
            }
            if (glm::length(mReflections2Sphere->GetModelPosition() - GetCameraPosition()) <= fInteractionLength && bReflections0Enabled && !bReflections2Enabled)
            {
                bReflections2Enabled = true;
                SetSceneLightPositionTwo(glm::vec3(45 * Unit, 45 * Unit, 0.0f));
                SetSceneLightColourTwo(glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));
                SetSceneLightCutoffTwo(glm::cos(glm::radians(12.5f)));
            }
            if (glm::length(mReflections3Sphere->GetModelPosition() - GetCameraPosition()) <= fInteractionLength && bReflections0Enabled && !bReflections3Enabled)
            {
                bReflections3Enabled = true;
                SetSceneLightPositionOne(glm::vec3(0.0f, 45 * Unit, -45 * Unit));
                SetSceneLightColourOne(glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
                SetSceneLightCutoffOne(glm::cos(glm::radians(12.5f)));
            }
            if (bReflections0Enabled && bReflections1Enabled && bReflections2Enabled && bReflections3Enabled)
            {
                bFinalState = true;
            }
        }
    }

    void BindReflectionsRoomUniforms() {
        glUseProgram(scene_shader->id);
        unsigned int uniformLocation;

        uniformLocation = glGetUniformLocation(scene_shader->id, "light_position_one");
        glUniform3fv(uniformLocation, 1, &GetSceneLightPositionOne()[0]);

        uniformLocation = glGetUniformLocation(scene_shader->id, "light_direction_one");
        glUniform3fv(uniformLocation, 1, &GetSceneLightDirectionOne()[0]);

        uniformLocation = glGetUniformLocation(scene_shader->id, "light_colour_one");
        glUniform4fv(uniformLocation, 1, &GetSceneLightColourOne()[0]);

        uniformLocation = glGetUniformLocation(scene_shader->id, "light_cutoff_one");
        glUniform1f(uniformLocation, GetSceneLightCutoffOne());

        uniformLocation = glGetUniformLocation(scene_shader->id, "light_switch_one");
        glUniform1i(uniformLocation, GetSceneLightSwitchOne());

        uniformLocation = glGetUniformLocation(scene_shader->id, "light_position_two");
        glUniform3fv(uniformLocation, 1, &GetSceneLightPositionTwo()[0]);

        uniformLocation = glGetUniformLocation(scene_shader->id, "light_direction_two");
        glUniform3fv(uniformLocation, 1, &GetSceneLightDirectionTwo()[0]);

        uniformLocation = glGetUniformLocation(scene_shader->id, "light_colour_two");
        glUniform4fv(uniformLocation, 1, &GetSceneLightColourTwo()[0]);

        uniformLocation = glGetUniformLocation(scene_shader->id, "light_cutoff_two");
        glUniform1f(uniformLocation, GetSceneLightCutoffTwo());

        uniformLocation = glGetUniformLocation(scene_shader->id, "light_switch_two");
        glUniform1i(uniformLocation, GetSceneLightSwitchTwo());

        uniformLocation = glGetUniformLocation(scene_shader->id, "light_position_three");
        glUniform3fv(uniformLocation, 1, &GetSceneLightPositionThree()[0]);

        uniformLocation = glGetUniformLocation(scene_shader->id, "light_direction_three");
        glUniform3fv(uniformLocation, 1, &GetSceneLightDirectionThree()[0]);

        uniformLocation = glGetUniformLocation(scene_shader->id, "light_colour_three");
        glUniform4fv(uniformLocation, 1, &GetSceneLightColourThree()[0]);

        uniformLocation = glGetUniformLocation(scene_shader->id, "light_cutoff_three");
        glUniform1f(uniformLocation, GetSceneLightCutoffThree());

        uniformLocation = glGetUniformLocation(scene_shader->id, "light_switch_three");
        glUniform1i(uniformLocation, GetSceneLightSwitchThree());

        glUseProgram(0);
    }
};