
#include <iostream>

#include "../Engine/include/Scene.hpp"

class SoundRoomScene : public Scene
{
public:
    /* Scene Globals */
    /* ------------- */
    float window_width, window_height;

    /* Models */
    Model *mGround;
    Model *mAxisX, *mAxisY, *mAxisZ;
    Model *mFloor, *mCeiling, *mLeftWall, *mRightWall, *mFarWall, *mNearWall;


    /* Defining Shaders 
    --------------------------------*/
    ShaderProgram* scene_shader_program = new ShaderProgram("res/shaders/default.fs","res/shaders/default.vs");

    /* Constructor */
    SoundRoomScene(float windowWidth, float windowHeight)
    {
        window_width = windowWidth;
        window_height = windowHeight;
    }

    /* This initializes the scene */
    void inline Initialize(void)
    {
        SetSceneShaderProgram(scene_shader_program);
        SetSceneLightPosition(glm::vec3(0.0f, 45*Unit, 0.0f));
        SetCameraView(glm::vec3(0.0f, 2*Unit, 25*Unit), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        SetCameraPerspective(70.0f, window_width / window_height, 0.001*Unit, 200*Unit);

        /* Modelling */
        // Floor Model
        mFloor = CreateModelPrimitive(PLANE, NULL);
        mFloor->ScaleModel(glm::vec3(50*Unit, 1.0f, 50*Unit));
        // Ceiling Model
        mCeiling = CreateModelPrimitive(PLANE, NULL);
        mCeiling->ScaleModel(glm::vec3(50*Unit, 1.0f, 50*Unit));
        mCeiling->RotateModel(glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        mCeiling->TranslateModel(glm::vec3(0.0f, 50*Unit, 0.0f));
        // Left Wall Model
        mLeftWall = CreateModelPrimitive(PLANE, NULL);
        mLeftWall->ScaleModel(glm::vec3(25*Unit, 1.0f, 50*Unit));
        mLeftWall->RotateModel(glm::radians(-90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        mLeftWall->TranslateModel(glm::vec3(-50*Unit, 25*Unit, 0.0f));

        // Right Wall Model
        mRightWall = CreateModelPrimitive(PLANE, NULL);
        mRightWall->ScaleModel(glm::vec3(25*Unit, 1.0f, 50*Unit));
        mRightWall->RotateModel(glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        mRightWall->TranslateModel(glm::vec3(50*Unit, 25*Unit, 0.0f));
        // Far Wall Model
        mFarWall = CreateModelPrimitive(PLANE, NULL);
        mFarWall->ScaleModel(glm::vec3(50*Unit, 1.0f, 25*Unit));
        mFarWall->RotateModel(glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        mFarWall->TranslateModel(glm::vec3(0.0f,25*Unit, -50*Unit));
        // Near Wall Model
        mNearWall = CreateModelPrimitive(PLANE, NULL);
        mNearWall->ScaleModel(glm::vec3(50*Unit, 1.0f, 25*Unit));
        mNearWall->RotateModel(glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        mNearWall->TranslateModel(glm::vec3(0.0f, 25*Unit, 50*Unit));

        // Axis Models
        mAxisX = CreateModelPrimitive(CUBE, NULL);
        mAxisX->ScaleModel(glm::vec3(2.0f, 0.05f, 0.05f));
        mAxisX->TranslateModel(glm::vec3(Unit, 0.0f, 0.0f));
        mAxisX->SetModelFragmentColour(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));

        mAxisY = CreateModelPrimitive(CUBE, NULL);
        mAxisY->ScaleModel(glm::vec3(2.0f, 0.05f, 0.05f));
        mAxisY->RotateModel(glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        mAxisY->TranslateModel(glm::vec3(0.0f, Unit, 0.0f));
        mAxisY->SetModelFragmentColour(glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));    

        mAxisZ = CreateModelPrimitive(CUBE, NULL);
        mAxisZ->ScaleModel(glm::vec3(2.0f, 0.05f, 0.05f));
        mAxisZ->RotateModel(glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        mAxisZ->TranslateModel(glm::vec3(0.0f, 0.0f, Unit));
        mAxisZ->SetModelFragmentColour(glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));
    }

    /* This is called once per frame */
    void inline Update(float dt)
    {
        DrawScene();
    }

    /* Define Key Bindings */
    void inline InputController(GLFWwindow* window)
    {

    }
};