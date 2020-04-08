#include <iostream>

#include "../Engine/include/Scene.hpp"

class BlackHoleRoomScene : public Scene
{
public:
    /* Scene Globals */
    /* ------------- */
    float window_width, window_height;
    float room_width = 100 * Unit;
    float room_height = 70 * Unit;
    float spinningAngle = 0.0f;

    /* Models */
    Model* mGround;
    Model* mAxisX, * mAxisY, * mAxisZ;
    Model* mFloor, * mBridge, * mCeiling, * mLeftWall, * mRightWall, * mFarWall, * mNearWall, * mFarWallDoor, * mNearWallDoor;


    /* Loading Shaders
    --------------------------------*/
    ShaderProgram* scene_shader_program = new ShaderProgram("res/shaders/default.fs", "res/shaders/default.vs");

    /* Loading Textures 
    --------------------------------*/
    Texture* stars_texture = new Texture("res/textures/2k_stars.jpg");

    /* Constructor */
    BlackHoleRoomScene(float windowWidth, float windowHeight)
    {
        window_width = windowWidth;
        window_height = windowHeight;
    }

    /* This initializes the scene */
    void inline Initialize(void)
    {
        SetSceneShaderProgram(scene_shader_program);
        SetSceneLightPositionOne(glm::vec3(0.0f, 45 * Unit, 0.0f));
        SetCameraView(glm::vec3(0.0f, 2 * Unit, 25 * Unit), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        SetCameraPerspective(70.0f, (window_width / window_height), 0.001 * Unit, 2000 * Unit);

        /* Modelling */
        // Floor Model
        mFloor = CreateModelPrimitive(PLANE, NULL);
        mFloor->ScaleModel(glm::vec3(room_width, 1.0f, room_width));
        mFloor->TranslateModel(glm::vec3(0.0f, -room_width / 2.0f, 0.0f));
        mFloor->SetModelFragmentColour(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
        mFloor->SetModelTexture(stars_texture);

        // Bridge Model          /////////////////////////////
        mBridge = CreateModelPrimitive(PLANE, NULL);
        mBridge->ScaleModel(glm::vec3(2.0f, 1.0f, room_width*3/4));
        mBridge->TranslateModel(glm::vec3(0.0f, 0.1f, 0.0f));
        mBridge->SetModelFragmentColour(glm::vec4(0.12f, 0.12f, 0.12f, 1.0f));


        // Ceiling Model
        mCeiling = CreateModelPrimitive(PLANE, NULL);
        mCeiling->ScaleModel(glm::vec3(room_width, 1.0f, room_width));
        mCeiling->RotateModel(glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        mCeiling->TranslateModel(glm::vec3(0.0f, room_width/2.0f, 0.0f));
        mCeiling->SetModelTexture(stars_texture);

        // Left Wall Model
        mLeftWall = CreateModelPrimitive(PLANE, NULL);
        mLeftWall->ScaleModel(glm::vec3(room_width, 1.0f, room_height));
        mLeftWall->RotateModel(glm::radians(-90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        mLeftWall->RotateModel(glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        mLeftWall->TranslateModel(glm::vec3(-room_width, 0.0f, 0.0f));
        //mLeftWall->SetModelFragmentColour(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
        mLeftWall->SetModelTexture(stars_texture);

        // Right Wall Model
        mRightWall = CreateModelPrimitive(PLANE, NULL);
        mRightWall->ScaleModel(glm::vec3(room_width, 1.0f, room_height));
        mRightWall->RotateModel(glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        mRightWall->RotateModel(glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        mRightWall->TranslateModel(glm::vec3(room_width, 0.0f, 0.0f));
        mRightWall->SetModelTexture(stars_texture);
        //RightWall->SetModelFragmentColour(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));

        // Far Wall Model
        mFarWall = CreateModelPrimitive(PLANE, NULL);
        mFarWall->ScaleModel(glm::vec3(room_width, 1.0f, room_height));
        mFarWall->RotateModel(glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        mFarWall->TranslateModel(glm::vec3(0.0f, 0.0f, -room_width));
        mFarWall->SetModelTexture(stars_texture);
        //mFarWall->SetModelFragmentColour(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));

        // Far Wall Door Model                            /////////////////
        mFarWallDoor = CreateModelPrimitive(PLANE, NULL);
        mFarWallDoor->ScaleModel(glm::vec3(2, 1.0f, 4));
        mFarWallDoor->RotateModel(glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        mFarWallDoor->TranslateModel(glm::vec3(0.0f, 0.0f, -room_width+1.0f));
        mFarWallDoor->SetModelFragmentColour(glm::vec4(0.25f, 0.25f, 0.25f, 1.0f));

        // Near Wall Model
        mNearWall = CreateModelPrimitive(PLANE, NULL);
        mNearWall->ScaleModel(glm::vec3(room_width, 1.0f, room_height));
        mNearWall->RotateModel(glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        mNearWall->TranslateModel(glm::vec3(0.0f, 0.0f, room_width));
        mNearWall->SetModelFragmentColour(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
        mNearWall->SetModelTexture(stars_texture);
        
        
        // Near Wall Door Model //////////////////////////////////////////////////////////////////////////
        mNearWallDoor = CreateModelPrimitive(PLANE, NULL);
        mNearWallDoor->ScaleModel(glm::vec3(2, 1.0f, 4));
        mNearWallDoor->RotateModel(glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        mNearWallDoor->TranslateModel(glm::vec3(0.0f, 0.0f, room_width-1.0f));
        mNearWallDoor->SetModelFragmentColour(glm::vec4(0.25f, 0.25f, 0.25f, 1.0f));
    }

    /* This is called once per frame */
    void inline Update(float dt)
    {
        spinningAngle += 0.01f * dt;
        mFloor->RotateModel(glm::radians(spinningAngle), glm::vec3(1.0f, 0.0f, 1.0f));
        mCeiling->RotateModel(glm::radians(spinningAngle), glm::vec3(1.0f, 0.0f, 1.0f));
        mLeftWall->RotateModel(glm::radians(spinningAngle), glm::vec3(1.0f, 0.0f, 1.0f));
        mRightWall->RotateModel(glm::radians(spinningAngle), glm::vec3(1.0f, 0.0f, 1.0f));
        mNearWall->RotateModel(glm::radians(spinningAngle), glm::vec3(1.0f, 0.0f, 1.0f));
        mFarWall->RotateModel(glm::radians(spinningAngle), glm::vec3(1.0f, 0.0f, 1.0f));
        DrawScene();
    }

    /* Define Key Bindings */
    void inline InputController(GLFWwindow* window)
    {

    }
};