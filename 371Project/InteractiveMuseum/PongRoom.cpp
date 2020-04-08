
#include <iostream>

#include "../Engine/include/Scene.hpp"

class PongRoomScene : public Scene
{
public:
    /* Scene Globals */
    /* ------------- */
    float window_width, window_height;
    float room_width = 30 * Unit;
    float room_height = 15 * Unit;
    float room_bound_neg_x = (-room_width  - 0.5) * Unit;
    float room_bound_pos_x = (room_width - 0.5) * Unit;
    float room_bound_neg_z = (-room_width - 0.5) * Unit;
    float room_bound_pos_z = (room_width - 0.5) * Unit;

    float ballRadius = 3.0 * Unit;
    float teeHeight = 1.0 * Unit;

    /* Interaction */
    float ballSpeed;
    glm::vec3 ballMoveDir;
    float rebound = 0.0f;

    bool checkObstacles = false;

    /* Models */
    Model* mGround;
    Model* mAxisX, * mAxisY, * mAxisZ;
    Model* mFloor, * mCeiling, * mLeftWall, * mRightWall, * mFarWall, * mNearWall;
    Model* mPongBall;
    Model* mBlock1, *mBlock2, *mBlock3, *mBlock4, *mBlock5;
    std::vector<Model*> obstacles;


    /* Loading Shaders
    --------------------------------*/
    ShaderProgram* scene_shader_program = new ShaderProgram("./res/shaders/artifact.fs", "./res/shaders/artifact.vs");

    /* Loading Textures
    --------------------------------*/
    Texture* ceiling_texture = new Texture("./res/textures/white-painted-ceiling-texture.jpg");
    Texture* artifact_arm_texture = new Texture("./res/textures/bark.jpg");


    /* Constructor */
    PongRoomScene(float windowWidth, float windowHeight)
    {
window_width = windowWidth;
window_height = windowHeight;
    }

    /* This initializes the scene */
    void inline Initialize(void)
    {
        ballSpeed = 4.0f;
        ballMoveDir = glm::vec3(0.0f);

        SetSceneShaderProgram(scene_shader_program);
        SetCameraView(glm::vec3(0.0f, 2 * Unit, 25 * Unit), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        SetCameraPerspective(70.0f, window_width / window_height, 0.001 * Unit, 200 * Unit);
        SetCameraBoundingBox(room_bound_neg_x, room_bound_pos_x, room_bound_neg_z, room_bound_pos_z);

        /* Lighting */
        SetSceneLightPositionOne(glm::vec3(0.0f, (room_height + 5) * Unit, 0.0f));
        SetSceneLightDirectionOne(glm::vec3(0.0f, -1.0f, 0.0f));
        //SetSceneLightColourOne(glm::vec4(0.7f, 0.1f, 0.1f, 1.0f));
        SetSceneLightCutoffOne(glm::cos(glm::radians(180.0f)));
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
        mAxisX = CreateModelPrimitive(LINE, NULL);
        mAxisX->SetModelRenderMode(GL_LINES);
        mAxisX->ScaleModel(glm::vec3(1.0f));
        mAxisX->SetModelFragmentColour(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));

        mAxisY = CreateModelPrimitive(LINE, NULL);
        mAxisY->SetModelRenderMode(GL_LINES);
        mAxisY->ScaleModel(glm::vec3(1.0f));
        mAxisY->RotateModel(glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        mAxisY->SetModelFragmentColour(glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));

        mAxisZ = CreateModelPrimitive(LINE, NULL);
        mAxisZ->SetModelRenderMode(GL_LINES);
        mAxisZ->ScaleModel(glm::vec3(1.0f));
        mAxisZ->RotateModel(glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        mAxisZ->SetModelFragmentColour(glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));

        // The ball
        mPongBall = CreateModelPrimitive(SPHERE, NULL);
        mPongBall->ScaleCollisionBoundaries(glm::vec3(ballRadius * 2));
        mPongBall->ScaleModel(glm::vec3(ballRadius * 2));
        mPongBall->TranslateModel(glm::vec3(0.0f, room_height / 2, 0.0f));
        mPongBall->SetModelFragmentColour(glm::vec4(1.0f, 1.0f, 0.0f, 1.0f));
        
        // A block
        mBlock1 = CreateModelPrimitive(CUBE, NULL);
        mBlock1->ScaleCollisionBoundaries(glm::vec3(5.0f));
        mBlock1->ScaleModel(glm::vec3(5.0f));
        mBlock1->TranslateModel(glm::vec3(10.0f, 0.0f, -5.0f));
        mBlock1->SetModelFragmentColour(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
        obstacles.push_back(mBlock1);

        // A block
        mBlock2 = CreateModelPrimitive(CUBE, NULL);
        mBlock2->ScaleCollisionBoundaries(glm::vec3(4.0f));
        mBlock2->ScaleModel(glm::vec3(4.0f));
        mBlock2->TranslateModel(glm::vec3(-10.0f, 10.0f, 10.0f));
        mBlock2->SetModelFragmentColour(glm::vec4(1.0f, 1.0f, 0.0f, 1.0f));
        obstacles.push_back(mBlock2);

        // A block
        mBlock3 = CreateModelPrimitive(CUBE, NULL);
        mBlock3->ScaleCollisionBoundaries(glm::vec3(1.0f));
        mBlock3->ScaleModel(glm::vec3(1.0f));
        mBlock3->TranslateModel(glm::vec3(-5.0f, 15.0f, -25.0f));
        mBlock3->SetModelFragmentColour(glm::vec4(1.0f, 0.0f, 1.0f, 1.0f));
        obstacles.push_back(mBlock3);

        // A block
        mBlock4 = CreateModelPrimitive(CUBE, NULL);
        mBlock4->ScaleCollisionBoundaries(glm::vec3(2.0f));
        mBlock4->ScaleModel(glm::vec3(2.0f));
        mBlock4->TranslateModel(glm::vec3(25.0f, 7.5f, 5.0f));
        mBlock4->SetModelFragmentColour(glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
        obstacles.push_back(mBlock4);

        // A block
        mBlock5 = CreateModelPrimitive(CUBE, NULL);
        mBlock5->ScaleCollisionBoundaries(glm::vec3(3.0f));
        mBlock5->ScaleModel(glm::vec3(3.0f));
        mBlock5->TranslateModel(glm::vec3(-15.0f, 4.0f, -2.0f));
        mBlock5->SetModelFragmentColour(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
        obstacles.push_back(mBlock5);
    }

    /* This is called once per frame */
    void inline Update(float dt)
    {
        rebound += 1.0f;

        glm::vec3 ballPosition = mPongBall->GetModelPosition();

        float leftWall = mLeftWall->GetModelPosition().x;
        float rightWall = mRightWall->GetModelPosition().x;
        float ceiling = mCeiling->GetModelPosition().y;
        float floor = mFloor->GetModelPosition().y;
        float farWall = mFarWall->GetModelPosition().z;
        float nearWall = mNearWall->GetModelPosition().z;

        if (ballPosition.x - leftWall <= ballRadius ||
            rightWall - ballPosition.x <= ballRadius) {
            ballMoveDir.x *= -1;
        }
        if (ballPosition.y - floor <= ballRadius ||
            ceiling - ballPosition.y <= ballRadius) {
            ballMoveDir.y *= -1;
        }
        if (ballPosition.z - farWall <= ballRadius ||
            nearWall - ballPosition.z <= ballRadius)
        {
            std::cout << "ball radius " << ballRadius << std::endl;
            std::cout << "ball pos " << ballPosition.x << "\t" << ballPosition.y << "\t"  << ballPosition.z << "\t" << std::endl;
            //std::cout << "left" << ballPosition.x - leftWall << std::endl;
            //std::cout << "right" << rightWall - ballPosition.x << std::endl;
            //std::cout << "floor" << ballPosition.y - floor << std::endl;
            //std::cout << "ceiling" << ceiling - ballPosition.y << std::endl;
            //std::cout << "far" << ballPosition.z - farWall << std::endl;
            //std::cout << "near" << nearWall - ballPosition.z << std::endl;

            ballMoveDir.z *= -1;
        }

        for (Model* m : obstacles) {
            if (checkObstacles) {
                if (SphereRectCollision(mPongBall, m)) {
                    if (rebound > 10.0f) {
                        rebound = 0.0f;
                        //ballMoveDir = glm::vec3(0);
                        ballMoveDir = -ballMoveDir;
                    }
                    std::cout << "the cube" << std::endl;
                }
            }
            
        }

        // Travel some
        mPongBall->TranslateModel(ballMoveDir * dt * ballSpeed);
                
        glLineWidth(5.0f);
        DrawScene();
        glLineWidth(1.0f);
    }

    void SetKeyCallback(GLFWwindow* window)
    {
        glfwSetWindowUserPointer(window, (void*)this);
        glfwSetKeyCallback(window, key_callback);
    }

    static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
    {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

        PongRoomScene* roomInstance = (PongRoomScene*)glfwGetWindowUserPointer(window);

        if (key == GLFW_KEY_RIGHT_BRACKET && action == GLFW_RELEASE)
        {
            float newBallRadius = roomInstance->ballRadius * 2.0f;
            roomInstance->ballRadius = newBallRadius;

            roomInstance->mPongBall->model_matrix = glm::inverse(roomInstance->mPongBall->model_matrix);
            /*glm::vec3 pos = roomInstance->mPongBall->GetModelPosition();
            roomInstance->mPongBall->TranslateModel(-pos);
            roomInstance->mPongBall->ScaleModel(glm::vec3(newBallRadius));
            roomInstance->mPongBall->TranslateModel(pos);*/
        }
        if (key == GLFW_KEY_LEFT_BRACKET && action == GLFW_RELEASE)
        {
            float newBallRadius = roomInstance->ballRadius * 0.5f;
            roomInstance->ballRadius = newBallRadius;
            //roomInstance->mPongBall->model_matrix = glm::mat4(1.0f);
            glm::vec3 pos = roomInstance->mPongBall->GetModelPosition();
            roomInstance->mPongBall->TranslateModel(-pos);
            roomInstance->mPongBall->ScaleModel(glm::vec3(newBallRadius));
            roomInstance->mPongBall->TranslateModel(pos);
        }
    }

    /* Define Key Bindings */
    void inline InputController(GLFWwindow* window)
    {
        if (glfwGetKey(window, GLFW_KEY_EQUAL) == GLFW_PRESS)
            ballSpeed += 1.0f;

        if (glfwGetKey(window, GLFW_KEY_MINUS) == GLFW_PRESS)
            ballSpeed -= 1.0f;

        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
            ballMoveDir = glm::normalize(camera_front);
            checkObstacles = true;
        }
    }

    bool SphereRectCollision(Model* sphere, Model* rect)
    {
        float sphereXDistance = abs(sphere->position->x - rect->position->x);
        float sphereYDistance = abs(sphere->position->y - rect->position->y);
        float sphereZDistance = abs(sphere->position->z - rect->position->z);
        if (sphereXDistance >= (rect->dimensions.x + sphere->dimensions.x)) { return false; }
        if (sphereYDistance >= (rect->dimensions.y + sphere->dimensions.x)) { return false; }
        if (sphereZDistance >= (rect->dimensions.z + sphere->dimensions.x)) { return false; }

        if (sphereXDistance < (rect->dimensions.x)) { return true; }
        if (sphereYDistance < (rect->dimensions.y)) { return true; }
        if (sphereZDistance < (rect->dimensions.z)) { return true; }

        float cornerDistance_sq = ((sphereXDistance - rect->dimensions.x) * (sphereXDistance - rect->dimensions.x)) +
            ((sphereYDistance - rect->dimensions.y) * (sphereYDistance - rect->dimensions.y) +
            ((sphereYDistance - rect->dimensions.z) * (sphereYDistance - rect->dimensions.z)));

        return (cornerDistance_sq < (sphere->dimensions.x * sphere->dimensions.x) );
    }
};