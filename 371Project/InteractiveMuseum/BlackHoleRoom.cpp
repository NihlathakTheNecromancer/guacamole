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
    float door_width = 2 * Unit;
    float door_height = 3 * Unit;
    float spinningAngle = 0.0f;
    float spinningAngleX = 1.0f;
    float spinningAngleY = 0.0f;
    float spinningAngleZ = 1.0f;
    float rotation_speed = 1.0f;

    float room_bound_neg_x = -49.5*Unit;
    float room_bound_pos_x = 49.5*Unit;
    float room_bound_neg_z = -49.5*Unit;
    float room_bound_pos_z = 49.5*Unit;


    /* Models */
    Model* mGround;
    Model* mAxisX, * mAxisY, * mAxisZ;
    Model* mFloor, * mBridge, * mCeiling, * mLeftWall, * mRightWall, * mFarWall, * mNearWall, * mFarWallDoor, * mNearWallDoor;


    /* Defining Shaders
    --------------------------------*/
    ShaderProgram* scene_shader_program = new ShaderProgram("./res/shaders/default.fs", "./res/shaders/default.vs");
    
    /* Load Textures 
    --------------------------------*/
    Texture* stars_texture = new Texture("./res/textures/2k_stars.jpg");

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
        SetSceneLightDirectionOne(glm::vec3(0.0f, -1.0f, 0.0f));
        SetCameraView(glm::vec3(0.0f, 2 * Unit, 25 * Unit), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        SetCameraPerspective(70.0f, (window_width / window_height), 0.001 * Unit, 2000 * Unit);
        SetCameraBoundingBox(room_bound_neg_x, room_bound_pos_x, room_bound_neg_z, room_bound_pos_z);

        /* Modelling */
        // Floor Model
        mFloor = CreateModelPrimitive(PLANE, NULL);
        mFloor->ScaleModel(glm::vec3(room_width, 1.0f, room_width));
        mFloor->TranslateModel(glm::vec3(0.0f, -room_width / 2.0f, 0.0f));
        mFloor->SetModelFragmentColour(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
        mFloor->SetModelTexture(stars_texture);

        // Bridge Model          /////////////////////////////////////////////////////////////////////////
        mBridge = CreateModelPrimitive(PLANE, NULL);
        mBridge->ScaleModel(glm::vec3(2.0f, 1.0f, room_width*5/8));
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

        // Far Wall Door Model                            ////////////////////////////////////////////////////
        mFarWallDoor = CreateModelPrimitive(PLANE, NULL);
        mFarWallDoor->ScaleModel(glm::vec3(door_width, 1.0f, door_height));
        mFarWallDoor->RotateModel(glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        mFarWallDoor->TranslateModel(glm::vec3(0.0f, door_height, -room_width+room_width*3/8));
        mFarWallDoor->SetModelFragmentColour(glm::vec4(0.25f, 0.25f, 0.25f, 1.0f));
        //mFarWallDoor->SetModelTexture(door);

        // Near Wall Model
        mNearWall = CreateModelPrimitive(PLANE, NULL);
        mNearWall->ScaleModel(glm::vec3(room_width, 1.0f, room_height));
        mNearWall->RotateModel(glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        mNearWall->TranslateModel(glm::vec3(0.0f, 0.0f, room_width));
        mNearWall->SetModelFragmentColour(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
        mNearWall->SetModelTexture(stars_texture);
        
        
        // Near Wall Door Model //////////////////////////////////////////////////////////////////////////
        mNearWallDoor = CreateModelPrimitive(PLANE, NULL);
        mNearWallDoor->ScaleModel(glm::vec3(door_width, 1.0f, door_height));
        mNearWallDoor->RotateModel(glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        mNearWallDoor->TranslateModel(glm::vec3(0.0f, door_height, room_width-room_width*3/8));
        mNearWallDoor->SetModelFragmentColour(glm::vec4(0.25f, 0.25f, 0.25f, 1.0f));
        //mNearWallDoor->SetModelTexture(door);
    }

    /* This is called once per frame */
    void inline Update(float dt)
    {
        spinningAngle += 0.01f * dt/2.0f;
        spinningAngleX = 0.1f * dt / 2.0f;
        spinningAngleY = 0.1f * -dt / 2.0f;
        spinningAngleZ = 0.1f * dt / 2.0f;
        //std::cout << "X: " << spinningAngleX << "\tY: " << spinningAngleY << "\tZ: " << spinningAngleZ << std::endl;
        glm::vec3 spin = glm::normalize(glm::vec3(spinningAngleX, spinningAngleY, spinningAngleZ));
        mFloor->RotateModel(glm::radians(spinningAngle), glm::vec3(spin));
        mCeiling->RotateModel(glm::radians(spinningAngle), glm::vec3(spin));
        mLeftWall->RotateModel(glm::radians(spinningAngle), glm::vec3(spin));
        mRightWall->RotateModel(glm::radians(spinningAngle), glm::vec3(spin));
        mNearWall->RotateModel(glm::radians(spinningAngle), glm::vec3(spin));
        mFarWall->RotateModel(glm::radians(spinningAngle), glm::vec3(spin));
        DrawScene();
    }

    /* Define Key Bindings */
    void inline InputController(GLFWwindow* window)
    {

    }

    void SetKeyCallback(GLFWwindow* window)
    {
        glfwSetKeyCallback(window, BlackHoleRoom_keyCallback);
    }

    static void BlackHoleRoom_keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
    {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);
    }
};