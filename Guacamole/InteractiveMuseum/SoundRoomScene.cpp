
#include <iostream>

#include "../Engine/include/Scene.hpp"

class SoundRoomScene : public Scene
{
public:
    /* Scene Globals */
    /* ------------- */
    float window_width, window_height;
    float room_width = 50*Unit;
    float room_height = 25*Unit;

    /* Models */
    Model *mGround;
    Model *mAxisX, *mAxisY, *mAxisZ;
    Model *mFloor, *mCeiling, *mLeftWall, *mRightWall, *mFarWall, *mNearWall;
    Model *mPedestalFoot, *mPedestalColumn, *mPedestalTop;
    Model *mArtifactArm0, *mArtifactArm1, *mArtifactArm2,*mArtifactArm3, *mArtifactSphere;
    Model *mRedCarpet, *mRedSquare;


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
        SetSceneLightPositionOne(glm::vec3(0.0f, 45*Unit, 0.0f));
        SetCameraView(glm::vec3(0.0f, 2*Unit, 25*Unit), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        SetCameraPerspective(70.0f, window_width / window_height, 0.001*Unit, 200*Unit);

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
        mCeiling->SetModelTexture("res/textures/white-painted-ceiling-texture.jpg");

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
        mFarWall->TranslateModel(glm::vec3(0.0f,room_height, -room_width));
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
        mRedCarpet->ScaleModel(glm::vec3(2.5*Unit, 0.05*Unit, 50*Unit));
        mRedCarpet->TranslateModel(glm::vec3(0.0f, 0.0f, 25*Unit));
        mRedCarpet->SetModelFragmentColour(glm::vec4(0.4f, 0.0f, 0.0f, 1.0f));

        mRedSquare = CreateModelPrimitive(CUBE, mRedCarpet);
        mRedSquare->ScaleModel(glm::vec3(5*Unit, 0.05*Unit, 5*Unit));
        mRedSquare->SetModelFragmentColour(glm::vec4(0.4f, 0.0f, 0.0f, 1.0f));


        /* Model Pedestal */
        mPedestalFoot = CreateModelPrimitive(CUBE, NULL);
        mPedestalFoot->ScaleModel(glm::vec3(2*Unit, 0.1*Unit, 2*Unit));
        mPedestalFoot->SetModelFragmentColour(glm::vec4(0.3f, 0.3f, 0.3f, 1.0f));

        mPedestalColumn = CreateModelPrimitive(CUBE, mPedestalFoot);
        mPedestalColumn->ScaleModel(glm::vec3(1.5*Unit, 0.5*Unit, 1.5*Unit));
        mPedestalColumn->TranslateModel(glm::vec3(0.0f, 0.1*Unit, 0.0f));
        mPedestalColumn->SetModelFragmentColour(glm::vec4(0.3f, 0.3f, 0.3f, 1.0f));

        mPedestalTop = CreateModelPrimitive(CUBE, mPedestalFoot);
        mPedestalTop->ScaleModel(glm::vec3(2*Unit, 0.1*Unit, 2*Unit));
        mPedestalTop->TranslateModel(glm::vec3(0.0f, 0.6*Unit, 0.0f));
        mPedestalTop->SetModelFragmentColour(glm::vec4(0.3, 0.3f, 0.3f, 1.0f));

        /* Model Artifact */
        mArtifactSphere = CreateModelPrimitive(SPHERE, NULL);
        mArtifactSphere->ScaleModel(glm::vec3(0.65f, 0.65f, 0.65f));
        mArtifactSphere->TranslateModel(glm::vec3(0.0f, 0.75*Unit, 0.0f));
        mArtifactSphere->SetModelFragmentColour(glm::vec4(1.0f, 0.0f, 0.0f, 0.4f));
        mArtifactSphere->SetModelTransparency(true);

        mArtifactArm0 = CreateModelPrimitive(CUBE, mArtifactSphere);
        mArtifactArm0->ScaleModel(glm::vec3(1.5*Unit, 0.1f, 0.1f));
        mArtifactArm0->RotateModel(glm::radians(55.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        mArtifactArm0->RotateModel(glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        //mArtifactArm0->TranslateModel(glm::vec3(-0.5f, 0.0f, 0.5f));
        mArtifactArm0->SetModelTexture("res/textures/bark.jpg");

        mArtifactArm1 = CreateModelPrimitive(CUBE, mArtifactSphere);
        mArtifactArm1->ScaleModel(glm::vec3(1.5*Unit, 0.1f, 0.1f));
        mArtifactArm1->RotateModel(glm::radians(55.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        mArtifactArm1->RotateModel(glm::radians(315.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        //mArtifactArm1->TranslateModel(glm::vec3(-0.5f, 0.0f, -0.5f));
        mArtifactArm1->SetModelTexture("res/textures/bark.jpg");

        mArtifactArm2 = CreateModelPrimitive(CUBE, mArtifactSphere);
        mArtifactArm2->ScaleModel(glm::vec3(1.5*Unit, 0.1f, 0.1f));
        mArtifactArm2->RotateModel(glm::radians(55.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        mArtifactArm2->RotateModel(glm::radians(225.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        //mArtifactArm2->TranslateModel(glm::vec3(0.5f, 0.0f, -0.5f));
        mArtifactArm2->SetModelTexture("res/textures/bark.jpg");

        mArtifactArm3 = CreateModelPrimitive(CUBE, mArtifactSphere);
        mArtifactArm3->ScaleModel(glm::vec3(1.5*Unit, 0.1f, 0.1f));
        mArtifactArm3->RotateModel(glm::radians(55.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        mArtifactArm3->RotateModel(glm::radians(135.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        //mArtifactArm3->TranslateModel(glm::vec3(0.5f, 0.0f, 0.5f));
        mArtifactArm3->SetModelTexture("res/textures/bark.jpg");

        mArtifactSphere->TranslateModelAndChildren(glm::vec3(0.0f, 1.25*Unit, 0.0f));

    }

    /* This is called once per frame */
    void inline Update(float dt)
    {
        DrawScene();
    }

    /* Define Key Bindings */
    void inline InputController(GLFWwindow* window)
    {
        if(glfwGetKey(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
        {

        }
    }
};