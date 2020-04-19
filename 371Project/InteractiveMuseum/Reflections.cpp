
/*Patrick Olivier Jean Baptiste 40003278*/
//COMP 371
/*Sources for sound and image see credits.txt*/
/*tutorial from https://learnopengl.com/In-Practice/2D-Game/Audio for irrklang*/


#include <iostream>

#include "../ThirdParty/irrKlang-1.6.0/include/irrklang.h"
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

//#pragma comment(lib, "../ThirdParty/irrKlang-1.6.0/lib/Win32-visualStudio/irrKlang.lib") //link with irrKlang.dll


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
    Model* mPaintingFrame, * mPaintingBody, * mPaintingLeftString, * mPaintingRightString;

    /*Sound Engine*/
    ISoundEngine* engine = createIrrKlangDevice();
    ISoundSource* water_sound = engine->addSoundSourceFromFile("./res/audio/water.wav");
    ISoundSource* piano_sound = engine->addSoundSourceFromFile("./res/audio/piano.wav");
    ISoundSource* rocks_sound = engine->addSoundSourceFromFile("./res/audio/rocks.wav");
    ISoundSource* rain_sound = engine->addSoundSourceFromFile("./res/audio/rain.wav");
    ISound* music;
    GLboolean musicOn = false;

    /* Loading Shaders
    --------------------------------*/
    ShaderProgram* scene_shader = new ShaderProgram("./res/shaders/reflection.fs", "./res/shaders/reflection.vs");

    /* Loading Textures
    --------------------------------*/
    Texture* ceiling_texture = new Texture("./res/textures/white-painted-ceiling-texture.jpg");
    Texture* water_texture = new Texture("./res/textures/water.jpg");
    Texture* wall_marble_texture = new Texture("./res/textures/marble.jpg");
    Texture* stone_texture = new Texture("./res/textures/stone.jpg");
    Texture* music_texture = new Texture("./res/textures/music.jpg");
    Texture* rain_texture = new Texture("./res/textures/rain.jpg");

    /*Lighting*/
        /* Lighting */
    glm::vec3    light_position;                // Position of scene light One
    glm::vec3    light_direction;               // Direction of scene light One
    glm::vec4    light_colour = glm::vec4(1.0f);// Colour of scene light One
    float        light_cutoff = glm::cos(glm::radians(180.0f));
    bool         light_switch = false;

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
        SetSceneLightPosition(glm::vec3(0.0f, 10.0 * Unit, 22.5 * Unit));
        SetSceneLightDirection(glm::vec3(0.0f, 0.0f, -10.0f));
        //SetSceneLightColour(glm::vec4(1.0f, 0.1f, 0.1f, 1.0f));
        SetSceneLightCutoff(glm::cos(glm::radians(180.0f)));
        SetSceneLightSwitch(true);

        /* Modelling */
        // Floor Model
        mFloor = CreateModelPrimitive(PLANE, NULL);
        mFloor->ScaleModel(glm::vec3(room_width, 1.0f, room_width));
        //mFloor->SetModelFragmentColour(glm::vec4(0.5f, 0.5f, 0.5f, 1.0f));
        mFloor->SetModelTexture(wall_marble_texture);

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
        //mLeftWall->SetModelFragmentColour(glm::vec4(0.4f, 0.4f, 0.4f, 1.0f));
        mLeftWall->SetModelTexture(wall_marble_texture);

        // Right Wall Model
        mRightWall = CreateModelPrimitive(PLANE, NULL);
        mRightWall->ScaleModel(glm::vec3(room_width, 1.0f, room_height));
        mRightWall->RotateModel(glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        mRightWall->RotateModel(glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        mRightWall->TranslateModel(glm::vec3(room_width, room_height, 0.0f));
        //mRightWall->SetModelFragmentColour(glm::vec4(0.4f, 0.4f, 0.4f, 1.0f));
        mRightWall->SetModelTexture(wall_marble_texture);

        // Far Wall Model
        mFarWall = CreateModelPrimitive(PLANE, NULL);
        mFarWall->ScaleModel(glm::vec3(room_width, 1.0f, room_height));
        mFarWall->RotateModel(glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        mFarWall->TranslateModel(glm::vec3(0.0f, room_height, -room_width));
        //mFarWall->SetModelFragmentColour(glm::vec4(0.4f, 0.4f, 0.4f, 1.0f));
        mFarWall->SetModelTexture(wall_marble_texture);

        // Near Wall Model
        mNearWall = CreateModelPrimitive(PLANE, NULL);
        mNearWall->ScaleModel(glm::vec3(room_width, 1.0f, room_height));
        mNearWall->RotateModel(glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        mNearWall->TranslateModel(glm::vec3(0.0f, room_height, room_width));
        mNearWall->SetModelTexture(wall_marble_texture);


        ///* Model Metronome Box */
        //mRedCarpet = CreateModelPrimitive(CUBE, NULL);
        //mRedCarpet->ScaleModel(glm::vec3(2.5 * Unit, 0.05 * Unit, 50 * Unit));
        //mRedCarpet->TranslateModel(glm::vec3(0.0f, 0.0f, 25 * Unit));
        //mRedCarpet->SetModelFragmentColour(glm::vec4(0.4f, 0.0f, 0.0f, 1.0f));

        //mRedSquare = CreateModelPrimitive(CUBE, mRedCarpet);
        //mRedSquare->ScaleModel(glm::vec3(5 * Unit, 0.05 * Unit, 5 * Unit));
        //mRedSquare->SetModelFragmentColour(glm::vec4(0.4f, 0.0f, 0.0f, 1.0f));


        ///* Model Pedestal 0*/
        mPaintingFrame = CreateModelPrimitive(CUBE, NULL);
        mPaintingFrame->ScaleModel(glm::vec3(6 * Unit, 12 * Unit, 0.2 * Unit));
        mPaintingFrame->TranslateModel(glm::vec3(0.0f, 1.5 * Unit, 0.0f));
        mPaintingFrame->RotateModel(glm::radians(180.0f), vec3(0.0, 1.0, 0.0));
        mPaintingFrame->SetModelFragmentColour(glm::vec4(0.54f, 0.27f, 0.07f, 1.0f));

        mPaintingBody = CreateModelPrimitive(CUBE, NULL);
        mPaintingBody->ScaleModel(glm::vec3(5 * Unit, 11 * Unit, 0.2 * Unit));
        mPaintingBody->TranslateModel(glm::vec3(0.0f, 2 * Unit, -0.2f));
        mPaintingBody->RotateModel(glm::radians(180.0f), vec3(0.0, 1.0, 0.0));
        mPaintingBody->SetModelFragmentColour(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
        mPaintingBody->SetModelTexture(water_texture);

    }

    /* This is called once per frame */
    void inline Update(float dt)
    {
        if (bFinalState)
        {
            timer0 += dt;
            if (timer0 >= 2.0f)
            {
                SetSceneLightPosition(glm::vec3(0.0f, 10.0 * Unit, 22.5 * Unit));
                SetSceneLightDirection(glm::vec3(0.0f, 0.0f, -10.0f));
                //SetSceneLightColour(glm::vec4(1.0f, 0.1f, 0.1f, 1.0f));
                SetSceneLightCutoff(glm::cos(glm::radians(180.0f)));

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
            if (glm::length(mPaintingBody->GetModelPosition() - GetCameraPosition()) <= fInteractionLength && !bReflections0Enabled)
            {
                bReflections0Enabled = true;

                SetSceneLightPosition(glm::vec3(0.0f, 10.0 * Unit, 22.5 * Unit));
                SetSceneLightDirection(glm::vec3(0.0f, 0.0f, -10.0f));
                //SetSceneLightColour(glm::vec4(0.7f, 0.1f, 0.1f, 1.0f));
                SetSceneLightCutoff(glm::cos(glm::radians(180.0f)));
                SetSceneLightSwitch(true);
            }
            if (bReflections0Enabled && bReflections1Enabled && bReflections2Enabled && bReflections3Enabled)
            {
                bFinalState = true;
            }
            
        }
        if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS)
        {
            if (musicOn == false) {
                music = engine->play3D(piano_sound, vec3df(0, 45, 0), GL_TRUE, GL_FALSE, GL_TRUE);
                musicOn = true;
            }
            else {
                engine->setAllSoundsPaused();
                music = engine->play3D(piano_sound, vec3df(0, 45, 0), GL_TRUE, GL_FALSE, GL_TRUE);
            }
            mPaintingBody->SetModelTexture(music_texture);
            
        }
        if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS)
        {
            if (musicOn == false) {
                music = engine->play3D(water_sound, vec3df(0, 5, 0), GL_TRUE, GL_FALSE, GL_TRUE);
                musicOn = true;
            }
            else {
                engine->setAllSoundsPaused();
                music = engine->play3D(water_sound, vec3df(0, 5, 0), GL_TRUE, GL_FALSE, GL_TRUE);
            }
            mPaintingBody->SetModelTexture(water_texture);         
        }
        if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS)
        {
            if (musicOn == false) {
                music = engine->play3D(rocks_sound, vec3df(0, 5, 0), GL_TRUE, GL_FALSE, GL_TRUE);
                musicOn = true;
            }
            else {
                engine->setAllSoundsPaused();
                music = engine->play3D(rocks_sound, vec3df(0, 5, 0), GL_TRUE, GL_FALSE, GL_TRUE);
            }
            mPaintingBody->SetModelTexture(stone_texture);
        }
        if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
        {
            if (musicOn == false) {
                music = engine->play3D(rain_sound, vec3df(0, 5, 0), GL_TRUE, GL_FALSE, GL_TRUE);
                musicOn = true;
            }
            else {
                engine->setAllSoundsPaused();
                music = engine->play3D(rain_sound, vec3df(0, 5, 0), GL_TRUE, GL_FALSE, GL_TRUE);
            }
            mPaintingBody->SetModelTexture(rain_texture);
        }
    }

    void BindReflectionsRoomUniforms() {
        glUseProgram(scene_shader->id);
        unsigned int uniformLocation;

        uniformLocation = glGetUniformLocation(scene_shader->id, "light_position_one");
        glUniform3fv(uniformLocation, 1, &GetSceneLightPosition()[0]);

        uniformLocation = glGetUniformLocation(scene_shader->id, "light_direction_one");
        glUniform3fv(uniformLocation, 1, &GetSceneLightDirection()[0]);

        uniformLocation = glGetUniformLocation(scene_shader->id, "light_colour_one");
        glUniform4fv(uniformLocation, 1, &GetSceneLightColour()[0]);

        uniformLocation = glGetUniformLocation(scene_shader->id, "light_cutoff_one");
        glUniform1f(uniformLocation, GetSceneLightCutoff());

        uniformLocation = glGetUniformLocation(scene_shader->id, "light_switch_one");
        glUniform1i(uniformLocation, GetSceneLightSwitch());

        glUseProgram(0);
    }
    /* Lighting */
    glm::vec3   inline  GetSceneLightPosition(void) { return light_position; }
    glm::vec3   inline  GetSceneLightDirection(void) { return light_direction; }
    glm::vec4   inline  GetSceneLightColour(void) { return light_colour; }
    float       inline  GetSceneLightCutoff(void) { return light_cutoff; }
    bool        inline  GetSceneLightSwitch(void) { return light_switch; }
    void        inline  SetSceneLightPosition(glm::vec3 position) { light_position = position; }
    void        inline  SetSceneLightCutoff(float cutoff) { light_cutoff = cutoff; }
    void        inline  SetSceneLightDirection(glm::vec3 dir) { light_direction = dir; }
    void        inline  SetSceneLightSwitch(bool isOn) { light_switch = isOn; }
    void        inline  SetSceneLightColour(glm::vec4 colour) { light_colour = colour; }
};