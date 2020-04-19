#include <iostream>

#include "../Engine/include/Scene.hpp"

//Arturo Santamaria - 40025561
//COMP 371
//Black Hole Room

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
    bool max = false;
    bool sph = true;
    bool sph2 = false;

    float room_bound_neg_x = -4 * Unit;
    float room_bound_pos_x = 4 * Unit;
    float room_bound_neg_z = -room_width * 5 / 8 - 1.0f;
    float room_bound_pos_z = room_width * 5 / 8 + 1.0f;


    //From https://www.solarsystemscope.com/textures/
    std::string stars = "./res/textures/2k_stars.jpg";
    std::string stars2 = "./res/textures/2k_stars_milky_way.jpg";
    std::string stars3 = "./res/textures/spaceTex1.jpg";
    std::string mars = "./res/textures/2k_mars.jpg";
    std::string moon = "./res/textures/2k_moon.jpg";
    std::string spaceDust = "./res/textures/spacedust1.jpg";
    std::string mercury = "./res/textures/2k_mercury.jpg";

    /* Models */
    Model* mGround, * mSphere, * mSphere2, * mSphere3;
    Model* mAxisX, * mAxisY, * mAxisZ;
    Model* mFloor, * mBridge, * mCeiling, * mLeftWall, * mRightWall, * mFarWall, * mNearWall, * mFarWallDoor, * mNearWallDoor;

    /* Lighting */
    glm::vec3    light_position_one;                // Position of scene light One
    glm::vec3    light_direction_one;               // Direction of scene light One
    glm::vec4    light_colour_one = glm::vec4(0.3f);// Colour of scene light One
    float        light_cutoff_one = glm::cos(glm::radians(180.0f));
    bool         light_switch_one = false;


    /* Defining Shaders
    --------------------------------*/
    ShaderProgram* scene_shader_program = new ShaderProgram("./res/shaders/default.fs", "./res/shaders/default.vs");

    /* Load Textures
    --------------------------------*/
    Texture* stars_texture = new Texture(stars);
    Texture* stars2_texture = new Texture(stars2);
    Texture* stars3_texture = new Texture(stars3);
    Texture* mars_texture = new Texture(mars);
    Texture* moon_texture = new Texture(moon);
    Texture* spaceDust_texture = new Texture(spaceDust);
    Texture* mercury_texture = new Texture(mercury);
    Texture* current_texture = stars_texture;

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
        mFloor->SetModelTexture(current_texture);

        // Bridge Model          /////////////////////////////////////////////////////////////////////////
        mBridge = CreateModelPrimitive(PLANE, NULL);
        mBridge->ScaleModel(glm::vec3(2.0f, 1.0f, room_width * 5 / 8));
        mBridge->SetModelFragmentColour(glm::vec4(0.12f, 0.12f, 0.12f, 1.0f));


        // Ceiling Model
        mCeiling = CreateModelPrimitive(PLANE, NULL);
        mCeiling->ScaleModel(glm::vec3(room_width, 1.0f, room_width));
        mCeiling->RotateModel(glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        mCeiling->TranslateModel(glm::vec3(0.0f, room_width / 2.0f, 0.0f));
        mCeiling->SetModelTexture(current_texture);

        // Left Wall Model
        mLeftWall = CreateModelPrimitive(PLANE, NULL);
        mLeftWall->ScaleModel(glm::vec3(room_width, 1.0f, room_height));
        mLeftWall->RotateModel(glm::radians(-90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        mLeftWall->RotateModel(glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        mLeftWall->TranslateModel(glm::vec3(-room_width, 0.0f, 0.0f));
        //mLeftWall->SetModelFragmentColour(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
        mLeftWall->SetModelTexture(current_texture);

        // Right Wall Model
        mRightWall = CreateModelPrimitive(PLANE, NULL);
        mRightWall->ScaleModel(glm::vec3(room_width, 1.0f, room_height));
        mRightWall->RotateModel(glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        mRightWall->RotateModel(glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        mRightWall->TranslateModel(glm::vec3(room_width, 0.0f, 0.0f));
        mRightWall->SetModelTexture(current_texture);
        //RightWall->SetModelFragmentColour(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));

        // Far Wall Model
        mFarWall = CreateModelPrimitive(PLANE, NULL);
        mFarWall->ScaleModel(glm::vec3(room_width, 1.0f, room_height));
        mFarWall->RotateModel(glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        mFarWall->TranslateModel(glm::vec3(0.0f, 0.0f, -room_width));
        mFarWall->SetModelTexture(current_texture);
        //mFarWall->SetModelFragmentColour(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));

        // Far Wall Door Model                            ////////////////////////////////////////////////////
        mFarWallDoor = CreateModelPrimitive(PLANE, NULL);
        mFarWallDoor->ScaleModel(glm::vec3(door_width, 1.0f, door_height));
        mFarWallDoor->RotateModel(glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        mFarWallDoor->TranslateModel(glm::vec3(0.0f, door_height, -room_width + room_width * 3 / 8));
        mFarWallDoor->SetModelFragmentColour(glm::vec4(0.25f, 0.25f, 0.25f, 1.0f));
        //mFarWallDoor->SetModelTexture(door);

        // Near Wall Model
        mNearWall = CreateModelPrimitive(PLANE, NULL);
        mNearWall->ScaleModel(glm::vec3(room_width, 1.0f, room_height));
        mNearWall->RotateModel(glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        mNearWall->TranslateModel(glm::vec3(0.0f, 0.0f, room_width));
        mNearWall->SetModelFragmentColour(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
        mNearWall->SetModelTexture(current_texture);


        // Near Wall Door Model //////////////////////////////////////////////////////////////////////////
        mNearWallDoor = CreateModelPrimitive(PLANE, NULL);
        mNearWallDoor->ScaleModel(glm::vec3(door_width, 1.0f, door_height));
        mNearWallDoor->RotateModel(glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        mNearWallDoor->TranslateModel(glm::vec3(0.0f, door_height, room_width - room_width * 3 / 8));
        mNearWallDoor->SetModelFragmentColour(glm::vec4(0.25f, 0.25f, 0.25f, 1.0f));
        //mNearWallDoor->SetModelTexture(door);
    }

    /* This updates the textures  */
    void inline UpdateTex(void) {
        mFloor->SetModelTexture(current_texture);
        mCeiling->SetModelTexture(current_texture);
        mRightWall->SetModelTexture(current_texture);
        mLeftWall->SetModelTexture(current_texture);
        mNearWall->SetModelTexture(current_texture);
        mFarWall->SetModelTexture(current_texture);
    }

    void inline createSpheres(void) {
        if (sph) {
            mSphere = CreateModelPrimitive(SPHERE, NULL);
            mSphere->ScaleModel(glm::vec3(10.0f, 10.0f, 10.0f));
            mSphere->TranslateModel(glm::vec3(20.0f, 10.0f, 20.0f));
            mSphere->SetModelTexture(spaceDust_texture);

            mSphere2 = CreateModelPrimitive(SPHERE, NULL);
            mSphere2->ScaleModel(glm::vec3(6.0f, 6.0f, 6.0f));
            mSphere2->TranslateModel(glm::vec3(-20.0f, 6.0f, -5.0f));
            mSphere2->SetModelTexture(moon_texture);

            mSphere3 = CreateModelPrimitive(SPHERE, NULL);
            mSphere3->ScaleModel(glm::vec3(8.0f, 8.0f, 8.0f));
            mSphere3->TranslateModel(glm::vec3(5.0f, 40.0f, -3.0f));
            mSphere3->SetModelTexture(mars_texture);
        }
        sph2 = true;
        sph = false;
    }
    void inline deleteSpheres(void) {
        mSphere->TranslateModel(glm::vec3(1000.0f, 1000.0f, 1000.0f));
        mSphere2->TranslateModel(glm::vec3(1000.0f, 1000.0f, 1000.0f));
        mSphere3->TranslateModel(glm::vec3(1000.0f, 1000.0f, 1000.0f));
        sph = true;
        sph2 = false;
    }
    void inline rotateModels(float dt) {

        //Setting max rotation values
        if (max) {
            spinningAngle -= 0.01f * dt / 1.2f;
        }
        else
            spinningAngle += 0.01f * dt / 2.0f;
        if (spinningAngle >= 0.18f)
            max = true;
        else
            if (spinningAngle <= -0.18f)
                max = false;
        spinningAngleX = 0.1f * dt / 2.0f;
        spinningAngleY = 0.1f * -dt / 2.0f;
        spinningAngleZ = 0.1f * dt / 2.0f;
        //std::cout << "X: " << spinningAngleX << "\tY: " << spinningAngleY << "\tZ: " << spinningAngleZ << std::endl;
        glm::vec3 spin = glm::normalize(glm::vec3(spinningAngleX, spinningAngleY, spinningAngleZ));

        //Rotate spheres if they exist
        if (sph2) {
            mSphere->RotateModel(glm::radians(-spinningAngle), spin);
            mSphere2->RotateModel(glm::radians(spinningAngle / 2.0f), spin);
            mSphere3->RotateModel(glm::radians(spinningAngle * 2.0f), -spin);
        }

        //Rotate walls
        mFloor->RotateModel(glm::radians(spinningAngle), glm::vec3(spin));
        mCeiling->RotateModel(glm::radians(spinningAngle), glm::vec3(spin));
        mLeftWall->RotateModel(glm::radians(spinningAngle), glm::vec3(spin));
        mRightWall->RotateModel(glm::radians(spinningAngle), glm::vec3(spin));
        mNearWall->RotateModel(glm::radians(spinningAngle), glm::vec3(spin));
        mFarWall->RotateModel(glm::radians(spinningAngle), glm::vec3(spin));
    }

    /* This is called once per frame */
    void inline Update(float dt)
    {
        rotateModels(dt);
        BindBlackHoleRoomUniforms();
        DrawScene();
    }

    /* Define Key Bindings */
    void inline InputController(GLFWwindow* window)
    {
        if (glfwGetKey(window, GLFW_KEY_C) == true) {
            current_texture = stars_texture;
            UpdateTex();
        }
        if (glfwGetKey(window, GLFW_KEY_V) == true) {
            current_texture = stars2_texture;
            createSpheres();
            UpdateTex();
        }
        if (glfwGetKey(window, GLFW_KEY_B) == true) {
            current_texture = mars_texture;
            UpdateTex();
        }
        if (glfwGetKey(window, GLFW_KEY_N) == true) {
            current_texture = stars3_texture;
            UpdateTex();
        }
        if (glfwGetKey(window, GLFW_KEY_M) == true) {
            current_texture = mercury_texture;
            UpdateTex();
        }
        if (glfwGetKey(window, GLFW_KEY_O) == true) {
            createSpheres();
        }
        if (glfwGetKey(window, GLFW_KEY_P) == true) {
            if (sph == false) {
                deleteSpheres();
            }
        }
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

    void BindBlackHoleRoomUniforms() {
        glUseProgram(scene_shader_program->id);
        unsigned int uniformLocation;

        uniformLocation = glGetUniformLocation(scene_shader_program->id, "light_position_one");
        glUniform3fv(uniformLocation, 1, &GetSceneLightPositionOne()[0]);

        uniformLocation = glGetUniformLocation(scene_shader_program->id, "light_direction_one");
        glUniform3fv(uniformLocation, 1, &GetSceneLightDirectionOne()[0]);

        uniformLocation = glGetUniformLocation(scene_shader_program->id, "light_colour_one");
        glUniform4fv(uniformLocation, 1, &GetSceneLightColourOne()[0]);

        uniformLocation = glGetUniformLocation(scene_shader_program->id, "light_cutoff_one");
        glUniform1f(uniformLocation, GetSceneLightCutoffOne());

        uniformLocation = glGetUniformLocation(scene_shader_program->id, "light_switch_one");
        glUniform1i(uniformLocation, GetSceneLightSwitchOne());

        glUseProgram(0);
    }

    /* Lighting */
    glm::vec3   inline  GetSceneLightPositionOne(void) { return light_position_one; }
    glm::vec3   inline  GetSceneLightDirectionOne(void) { return light_direction_one; }
    glm::vec4   inline  GetSceneLightColourOne(void) { return light_colour_one; }
    float       inline  GetSceneLightCutoffOne(void) { return light_cutoff_one; }
    bool        inline  GetSceneLightSwitchOne(void) { return light_switch_one; }
    void        inline  SetSceneLightPositionOne(glm::vec3 position) { light_position_one = position; }
    void        inline  SetSceneLightCutoffOne(float cutoff) { light_cutoff_one = cutoff; }
    void        inline  SetSceneLightDirectionOne(glm::vec3 dir) { light_direction_one = dir; }
    void        inline  SetSceneLightSwitchOne(bool isOn) { light_switch_one = isOn; }
    void        inline  SetSceneLightColourOne(glm::vec4 colour) { light_colour_one = colour; }
};