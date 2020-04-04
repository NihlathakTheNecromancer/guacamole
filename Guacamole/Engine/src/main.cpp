#include <iostream>
#include <vector>
#include <random>
#include <chrono>

#define GLEW_STATIC 1   // This allows linking with Static Library on Windows, without DLL
#include <GL/glew.h>    // Include GLEW - OpenGL Extension Wrangler

#include <GLFW/glfw3.h> // GLFW provides a cross-platform interface for creating a graphical context,
                        // initializing OpenGL and binding inputs

#include <glm/glm.hpp>  // GLM is an optimized math library with syntax to similar to OpenGL Shading Language
#include <glm/gtc/matrix_transform.hpp> // include this to create transformation matrices

#include <Geometry.hpp>
#include <Scene.hpp>
#include <ShaderProgram.hpp>
#include <Model.hpp>
#include <Primitive.hpp>
#include <Animation.hpp>

// Global Variables
// ---------------------------------

unsigned int window_width = 1024;
unsigned int window_height = 768;

float GridUnit = 0.0f;

// Input Dependencies
float lastFrameTime = 0.0f;
float lastButtonTime = 0.0f;

// Olaf Controls
float fOlafMovementSpeed = 5.0f;
float fOlafRotationSpeed = 1.5f;
float fOlafScaleUpPercentage = 1.0f + 0.001f;
float fOlafScaleDownPercentage = 1.0f - 0.001f;
float fOlafAnimationSpeed = 1.5f;

// Camera/Mouse Controls
double lastCursorPosX = -1.0f;
double lastCursorPosY = -1.0f;
bool isLeftButtonPressed = false;
bool isRightButtonPressed = false;
bool isMiddleButtonPressed = false;
float fCameraRotationSpeed = 10.0f;
float fCameraZoomSpeed = 0.1f;

// Function prototypes
void OlafController(GLFWwindow* window, Model* mOlaf, float dt);
void CameraController(GLFWwindow* window, Model* mGrid, float dt);
void mouseButtonCallback(GLFWwindow *window, int button, int action, int mods);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);

int main()
{
    srand(static_cast <unsigned int> (time(0)));

    /* Initializing Window
	--------------------------------*/
    glfwInit();

    // On windows, we set OpenGL version to 2.1, to support more hardware
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);

    // Create Window and rendering context using GLFW, resolution is 800x600
    GLFWwindow* window = glfwCreateWindow(window_width, window_height, "Comp371 - Assignment 2 - Christian Galante", NULL, NULL);
    if (window == NULL)
    {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // Initialize GLEW
    glewExperimental = true; // Needed for core profile
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to create GLEW" << std::endl;
        glfwTerminate();
        return -1;
    }

    /* Defining Shaders 
    --------------------------------*/
    ShaderProgram* scene_shader_program = new ShaderProgram("res/shaders/default.fs","res/shaders/default.vs");
    ShaderProgram* shadow_shader_program = new ShaderProgram("res/shaders/shadows.fs", "res/shaders/shadows.vs");
    //ShaderProgram* scene_shader_program = new ShaderProgram("res/shaders/debug.fs","res/shaders/debug.vs");



    /* Initializing Primitive Geometry
    --------------------------------*/
    Primitives* oPrimitives = new Primitives();
    Geometry* gGrid = oPrimitives->InitializeGrid();
    Geometry* gPlane = oPrimitives->InitializePlane(50);
    Geometry* gCube = oPrimitives->InitializeCube();
    Geometry* gSphere = oPrimitives->InitializeSphere(50,50);
    GridUnit = oPrimitives->GridUnit;

    /* Defining Olaf Scene */
    glm::mat4 transform;

    Scene* sOlafScene = new Scene(scene_shader_program, shadow_shader_program);
    float lightNearPlane = 1.0f;
    float lightFarPlane = 200.0f;
    
        sOlafScene->InitializeShadowMap();
        sOlafScene->SetSceneView(glm::vec3(0.0f, 5.0f, 25.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        sOlafScene->SetScenePerspective(70.0f, (float)window_width / (float)window_height, 1.0f, 200.0f);
        //sOlafScene->SetSceneOrthographic(-20.0f, 20.0f, -20.0f, 20.0f, lightNearPlane, lightFarPlane);
        sOlafScene->light_position = glm::vec3(0.0f, 50.0f, 50.0f);
        //sOlafScene->SetSceneView(sOlafScene->light_position, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f));
        //sOlafScene->SetScenePerspective(70.0f, (float)1024/(float)1024, GridUnit, 40*GridUnit);
        glm::mat4 light_view = glm::lookAt(sOlafScene->light_position, glm::vec3(0.0f, 0.0f, -50.0f), glm::vec3(0.0f, 0.0f, -1.0f));
        glm::mat4 light_projection = glm::ortho(-50.0f, 50.0f, -50.0f, 50.0f, lightNearPlane, lightFarPlane);
        //glm::mat4 light_projection = glm::perspective(70.0f, (float)1024 / (float)1024, lightNearPlane, lightFarPlane);
        //glm::mat4 light_projection = glm::frustum(-10.0f, 10.0f, -10.0f, 10.0f, lightNearPlane, lightFarPlane);
        sOlafScene->light_vp = light_projection * light_view;


    // Grid Model
    //Model* mGrid = new Model(gGrid, sOlafScene, NULL);
    //mGrid->SetModelRenderMode(GL_LINES);

    // Ground Model
    Model* mGround = new Model(gPlane, sOlafScene, NULL);
        //transform = glm::scale(glm::mat4(1.0f), glm::vec3(1000.0f, 0.0f, 1000.0f));
        //mGround->TransformModel(transform);
        mGround->SetModelTexture("res/textures/snow-tileable.jpg");

    // Axes Model
    Model* mAxisX = new Model(gCube, sOlafScene, mGround);
        mAxisX->ScaleModel(glm::vec3(2.0f, 0.05f, 0.05f));
        mAxisX->TranslateModel(glm::vec3(GridUnit, 0.0f, 0.0f));
        mAxisX->SetModelFragmentColour(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));

    Model* mAxisY = new Model(gCube, sOlafScene, mGround);
        mAxisY->ScaleModel(glm::vec3(2.0f, 0.05f, 0.05f));
        mAxisY->RotateModel(glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        mAxisY->TranslateModel(glm::vec3(0.0f, GridUnit, 0.0f));
        mAxisY->SetModelFragmentColour(glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));    

    Model* mAxisZ = new Model(gCube, sOlafScene, mGround);
        mAxisZ->ScaleModel(glm::vec3(2.0f, 0.05f, 0.05f));
        mAxisZ->RotateModel(glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        mAxisZ->TranslateModel(glm::vec3(0.0f, 0.0f, GridUnit));
        mAxisZ->SetModelFragmentColour(glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));

    // Olaf Model
    Model* mOlafBodyLower = new Model(gSphere, sOlafScene, mGround);
        mOlafBodyLower->ScaleModel(glm::vec3(1.8f, 1.8f, 1.8f));
        mOlafBodyLower->TranslateModel(glm::vec3(0.0f, GridUnit, 0.0f));
    //->
        Model* mOlafBodyUpper = new Model(gSphere, sOlafScene, mOlafBodyLower);
            mOlafBodyUpper->ScaleModel(glm::vec3(1.6f, 1.6f, 1.6f));
            mOlafBodyUpper->TranslateModel(glm::vec3(0.0f, (7*GridUnit/4), 0.0f));

        Model* mOlafArmLeft = new Model(gCube, sOlafScene, mOlafBodyLower);
            mOlafArmLeft->ScaleModel(glm::vec3(0.1f, 0.1f, 1.0f));
            mOlafArmLeft->TranslateModel(glm::vec3((GridUnit / 2), (8 * GridUnit / 4), GridUnit));
            mOlafArmLeft->SetModelFragmentColour(glm::vec4(0.542f, 0.270f, 0.0742f, 1.0f));
            Animation aOlafArmLeft = Animation(mOlafArmLeft);
                      aOlafArmLeft.Register(2, mOlafArmLeft->right_direction, -fOlafAnimationSpeed, glm::radians(-25.0f), glm::radians(25.0f));
                      mOlafBodyLower->AddModelAnimation(aOlafArmLeft);
        //->
            Model* mOlafArmLeftBranch1 = new Model(gCube, sOlafScene, mOlafArmLeft);
                mOlafArmLeftBranch1->ScaleModel(glm::vec3(0.075f, 0.075f, 0.3f));
                mOlafArmLeftBranch1->RotateModel(glm::radians(-35.0f), glm::vec3(0.0f, 1.0f, 0.0f));
                mOlafArmLeftBranch1->TranslateModel(glm::vec3((GridUnit / 2)-(GridUnit*0.1), (8 * GridUnit / 4), 4*GridUnit/3));
                mOlafArmLeftBranch1->SetModelFragmentColour(glm::vec4(0.542f, 0.270f, 0.0742f, 1.0f));

            Model* mOlafArmLeftBranch2 = new Model(gCube, sOlafScene, mOlafArmLeft);
                mOlafArmLeftBranch2->ScaleModel(glm::vec3(0.075f, 0.075f, 0.3f));
                mOlafArmLeftBranch2->RotateModel(glm::radians(35.0f), glm::vec3(0.0f, 1.0f, 0.0f));
                mOlafArmLeftBranch2->TranslateModel(glm::vec3((GridUnit / 2)+(GridUnit*0.1), (8 * GridUnit / 4), 4*GridUnit/3));
                mOlafArmLeftBranch2->SetModelFragmentColour(glm::vec4(0.542f, 0.270f, 0.0742f, 1.0f));

        Model* mOlafArmRight = new Model(gCube, sOlafScene, mOlafBodyLower);
            mOlafArmRight->ScaleModel(glm::vec3(0.1f, 0.1f, 1.0f));
            mOlafArmRight->TranslateModel(glm::vec3(-(GridUnit / 2), (8 * GridUnit / 4), GridUnit));
            mOlafArmRight->SetModelFragmentColour(glm::vec4(0.542f, 0.270f, 0.0742f, 1.0f));
            Animation aOlafArmRight = Animation(mOlafArmRight);
                      aOlafArmRight.Register(2, mOlafArmRight->right_direction, fOlafAnimationSpeed, glm::radians(-25.0f), glm::radians(25.0f));
                      mOlafBodyLower->AddModelAnimation(aOlafArmRight);
        //->
            Model* mOlafArmRightBranch1 = new Model(gCube, sOlafScene, mOlafArmRight);
                mOlafArmRightBranch1->ScaleModel(glm::vec3(0.075f, 0.075f, 0.3f));
                mOlafArmRightBranch1->RotateModel(glm::radians(-35.0f), glm::vec3(0.0f, 1.0f, 0.0f));
                mOlafArmRightBranch1->TranslateModel(glm::vec3(-(GridUnit / 2)-(GridUnit*0.1), (8 * GridUnit / 4), 4*GridUnit/3));
                mOlafArmRightBranch1->SetModelFragmentColour(glm::vec4(0.542f, 0.270f, 0.0742f, 1.0f));

             Model* mOlafArmRightBranch2 = new Model(gCube, sOlafScene, mOlafArmRight);
                mOlafArmRightBranch2->ScaleModel(glm::vec3(0.075f, 0.075f, 0.3f));
                mOlafArmRightBranch2->RotateModel(glm::radians(35.0f), glm::vec3(0.0f, 1.0f, 0.0f));
                mOlafArmRightBranch2->TranslateModel(glm::vec3(-(GridUnit / 2)+(GridUnit*0.1), (8 * GridUnit / 4), 4*GridUnit/3));
                mOlafArmRightBranch2->SetModelFragmentColour(glm::vec4(0.542f, 0.270f, 0.0742f, 1.0f));

        Model* mOlafLegLeft = new Model(gSphere, sOlafScene, mOlafBodyLower);
            mOlafLegLeft->ScaleModel(glm::vec3(0.5f, 0.5f, 0.5f));
            mOlafLegLeft->TranslateModel(glm::vec3(-(GridUnit / 2), GridUnit/4, 0.0f));
            mOlafLegLeft->SetModelFragmentColour(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
            Animation aOlafLegLeft = Animation(mOlafLegLeft);
                      aOlafLegLeft.Register(0, mOlafLegLeft->forward_direction, fOlafAnimationSpeed, -(GridUnit/2), (GridUnit/2));
                      mOlafBodyLower->AddModelAnimation(aOlafLegLeft);

        Model* mOlafLegRight = new Model(gSphere, sOlafScene, mOlafBodyLower);
            mOlafLegRight->ScaleModel(glm::vec3(0.5f, 0.5f, 0.5f));
            mOlafLegRight->TranslateModel(glm::vec3((GridUnit / 2), GridUnit/4, 0.0f));
            mOlafLegRight->SetModelFragmentColour(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
            Animation aOlafLegRight = Animation(mOlafLegRight);
                      aOlafLegRight.Register(0, mOlafLegRight->forward_direction, -fOlafAnimationSpeed, -(GridUnit/2), (GridUnit/2));
                      mOlafBodyLower->AddModelAnimation(aOlafLegRight);

        Model* mOlafHead = new Model(gSphere, sOlafScene, mOlafBodyLower);
            mOlafHead->ScaleModel(glm::vec3(1.1f, 1.1f, 1.1f));
            mOlafHead->TranslateModel(glm::vec3(0.0f, (3*GridUnit), 0.0f));
        //->
            Model* mOlafNose = new Model(gCube, sOlafScene, mOlafHead);
                mOlafNose->ScaleModel(glm::vec3(0.1f, 0.2f, 0.1f));
                mOlafNose->TranslateModel(glm::vec3(0.0f, (3*GridUnit)-(GridUnit/6), (GridUnit/2)+(GridUnit*0.025)));
                mOlafNose->SetModelTexture("res/textures/carrot.jpg");
                mOlafNose->SetModelFragmentColour(glm::vec4(1.0f, 0.55f, 0.0f, 1.0f));

            Model* mOlafEyeLeft = new Model(gCube, sOlafScene, mOlafHead);
                mOlafEyeLeft->ScaleModel(glm::vec3(0.1f, 0.1f, 0.05f));
                mOlafEyeLeft->TranslateModel(glm::vec3((GridUnit / 4), (3*GridUnit), (GridUnit/2)-(GridUnit*0.025)));
                mOlafEyeLeft->SetModelFragmentColour(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));

            Model* mOlafEyeRight = new Model(gCube, sOlafScene, mOlafHead);
                mOlafEyeRight->ScaleModel(glm::vec3(0.1f, 0.1f, 0.05f));
                mOlafEyeRight->TranslateModel(glm::vec3(-(GridUnit / 4), (3*GridUnit), (GridUnit/2)-(GridUnit*0.025)));
                mOlafEyeRight->SetModelFragmentColour(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));

            Model* mOlafHatBrim = new Model(gCube, sOlafScene, mOlafHead);
                mOlafHatBrim->ScaleModel(glm::vec3(0.75f, 0.05f, 0.75f));
                mOlafHatBrim->TranslateModel(glm::vec3(0.0f, (3.5*GridUnit), 0.0f));
                mOlafHatBrim->SetModelTexture("res/textures/silver-brushed-metal-texture.jpg");
                mOlafHatBrim->SetModelFragmentColour(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));

            Model* mOlafHatTop = new Model(gCube, sOlafScene, mOlafHead);
                mOlafHatTop->ScaleModel(glm::vec3(0.5f, 0.5f, 0.5f));
                mOlafHatTop->TranslateModel(glm::vec3(0.0f, (3.5*GridUnit), 0.0f));
                mOlafHatTop->SetModelTexture("res/textures/silver-brushed-metal-texture.jpg");
                mOlafHatTop->SetModelFragmentColour(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));

    // Initialize GLFW Input
	glfwSetMouseButtonCallback(window, mouseButtonCallback);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

    //Set background colour
    glClearColor(0.4f, 0.8f, 0.8f, 1.0f);

    glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);

    // Entering Main Loop
    float dt;
    int width, height;

    // float* depth_buffer = new float[1024*1024]; 
    
    while(!glfwWindowShouldClose(window))
    {
		// Frame time calculation
		dt = glfwGetTime() - lastFrameTime;
		lastFrameTime += dt;

        /* Drawing Scene
	    --------------------------------*/
         /* Render shadow map from light perspective */

        glUseProgram(sOlafScene->shadow_shader_program->id);

        glBindFramebuffer(GL_FRAMEBUFFER, sOlafScene->fbo);

        glClear(GL_DEPTH_BUFFER_BIT);

        glViewport(0, 0, sOlafScene->shadow_width, sOlafScene->shadow_height);

        sOlafScene->DrawScene(true);

        // glReadPixels(0, 0, sOlafScene->shadow_width, sOlafScene->shadow_height, GL_DEPTH_COMPONENT, GL_FLOAT, depth_buffer);
        // for(int i = 0; i < 1024; i++)
        // {
        //     for(int j = 0; j < 1024; j++)
        //     {
        //         std::cout << depth_buffer[(1024*i) + j];
        //     }
        //     std::cout << std::endl;
        // }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        /* Render normally from camera perspective */

        glUseProgram(sOlafScene->scene_shader_program->id);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glfwGetFramebufferSize(window, &width, &height);
        glViewport(0, 0, width, height);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, sOlafScene->shadow_map);

        sOlafScene->DrawScene(false);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, 0);

		// Handle Inputs
        OlafController(window, mOlafBodyLower, dt); // Point of Reference is Olaf's Lower Body
        CameraController(window, mGround, dt); // Point of Reference is the Grid

		// End Frame
		glfwSwapBuffers(window);
		glfwPollEvents();
    }
    
    // Shutdown GLFW
    glfwTerminate();
    
	return 0;
}

void OlafController(GLFWwindow* window, Model* mOlaf, float dt)
{
    glm::mat4 transform;
    
    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) // Change render mode to points
    {
        mOlaf->SetModelRenderMode(GL_POINTS);
    }
    if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) // Change render mode to line loop
    {
        mOlaf->SetModelRenderMode(GL_LINES);
    }
    if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS) // Change render mode to triangles
    {
        mOlaf->SetModelRenderMode(GL_TRIANGLES);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) // move Olaf left
    {
        mOlaf->TranslateModelAndChildren(-fOlafMovementSpeed*dt * mOlaf->GetModelRightDirectionVector());
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) // move Olaf right
    {
        mOlaf->TranslateModelAndChildren(fOlafMovementSpeed*dt * mOlaf->GetModelRightDirectionVector());
    }
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) // move Olaf forward
    {
        mOlaf->TranslateModelAndChildren(fOlafMovementSpeed*dt * mOlaf->GetModelForwardDirectionVector());
        mOlaf->StepModelAnimations(1.0f, dt);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) // move Olaf backward
    {
        mOlaf->TranslateModelAndChildren(-fOlafMovementSpeed*dt * mOlaf->GetModelForwardDirectionVector());
        mOlaf->StepModelAnimations(-1.0f, dt);
    }
    if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS) // scale Olaf up
    {
        glm::vec3 current_pos = glm::vec3(mOlaf->position);
        mOlaf->TranslateModelAndChildren(-current_pos);
        mOlaf->ScaleModelAndChildren(glm::vec3(fOlafScaleUpPercentage));
        mOlaf->TranslateModelAndChildren(current_pos);
    }
    if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS) // scale Olaf down
    {
        glm::vec3 current_pos = glm::vec3(mOlaf->position);
        mOlaf->TranslateModelAndChildren(-current_pos);
        mOlaf->ScaleModelAndChildren(glm::vec3(fOlafScaleDownPercentage));
        mOlaf->TranslateModelAndChildren(current_pos);
    }
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) // rotate Olaf left
    {
        float rotationAngle = fOlafRotationSpeed * dt;
        glm::vec3 current_pos = glm::vec3(mOlaf->position);
        mOlaf->TranslateModelAndChildren(-current_pos);
        mOlaf->RotateModelAndChildren(rotationAngle, glm::vec3(0.0f, 1.0f, 0.0f));
        mOlaf->TranslateModelAndChildren(current_pos);
    }
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) // rotate Olaf right
    {
        float rotationAngle = fOlafRotationSpeed * dt;
        glm::vec3 current_pos = glm::vec3(mOlaf->position);
        mOlaf->TranslateModelAndChildren(-current_pos);
        mOlaf->RotateModelAndChildren(-rotationAngle, glm::vec3(0.0f, 1.0f, 0.0f));
        mOlaf->TranslateModelAndChildren(current_pos);
    }
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) // Randomly reposition Olaf on the grid
    {
        float timeElapsed = glfwGetTime() - lastButtonTime;
        if(timeElapsed > 0.25f)
        {
            lastButtonTime = glfwGetTime();
            int random_int_x = 50 - rand()%101;
            int random_int_z = 50 - rand()%101;
            glm::vec3 olaf_position = mOlaf->GetModelPosition();
            mOlaf->TranslateModelAndChildren(-olaf_position);
            mOlaf->TranslateModelAndChildren(glm::vec3(random_int_x*GridUnit, olaf_position.y, random_int_z*GridUnit));
        }
    }
    if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS) // Toggle Textures
    {
        float timeElapsed = glfwGetTime() - lastButtonTime;
        if(timeElapsed > 0.25f)
        {
            lastButtonTime = glfwGetTime();
            mOlaf->scene->ToggleSceneTextures();
        }
    }
}

void CameraController(GLFWwindow* window, Model* mGrid, float dt)
{
    Scene* sScene = mGrid->scene;
    glm::mat4 transform;
    // Reset world view
    if (glfwGetKey(window, GLFW_KEY_HOME) == GLFW_PRESS)
    {
        sScene->camera_right = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
        sScene->SetSceneView(glm::vec3(0.0f, 5.0f, 25.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        sScene->SetScenePerspective(70.0f, (float)window_width / (float)window_height, 1.0f, 200.0f);
    }
    // rotate world about -y
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) 
    {
        float rotationAngle = fCameraRotationSpeed * dt;
        transform = glm::rotate(glm::mat4(1.0f), -rotationAngle, glm::vec3(0.0f, 1.0f, 0.0f));
        sScene->camera_position = transform * sScene->camera_position;
        sScene->camera_right = transform * sScene->camera_right;
        sScene->SetSceneView(sScene->camera_position, sScene->camera_look_at, sScene->camera_up);
    }
    // rotate world about y
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) 
    {
        float rotationAngle = fCameraRotationSpeed * dt;
        transform = glm::rotate(glm::mat4(1.0f), rotationAngle, glm::vec3(0.0f, 1.0f, 0.0f));
        sScene->camera_position = transform * sScene->camera_position;
        sScene->camera_right = transform * sScene->camera_right;
        sScene->SetSceneView(sScene->camera_position, sScene->camera_look_at, sScene->camera_up);
    }
    // Zooming
    if (isLeftButtonPressed)
    {
        double xPos, yPos;
        glfwGetCursorPos(window, &xPos, &yPos);

        if (lastCursorPosY != -1.0f)
        {
            double dy = yPos - lastCursorPosY;
            //transform = glm::translate(glm::mat4(1.0f),-(float)(dy*0.001) * (glm::vec3(sScene->camera_look_at) - glm::vec3(sScene->camera_position)));
            //sScene->camera_position = transform * sScene->camera_position;
            //sScene->SetSceneView(sScene->camera_position, sScene->camera_look_at, sScene->camera_up);
            sScene->SetScenePerspective(sScene->camera_fov + (dy*fCameraZoomSpeed), sScene->camera_aspect_ratio, sScene->camera_near, sScene->camera_far);
        }
        lastCursorPosY = yPos;
    }
    // Panning
    if (isRightButtonPressed)
    {
        double xPos, yPos;
        glfwGetCursorPos(window, &xPos, &yPos);

        if (lastCursorPosX != -1.0f)
        {
            double dx = xPos - lastCursorPosX;
            transform = glm::translate(glm::mat4(1.0f),(float)(dx*0.2) * glm::vec3(sScene->camera_right.x, sScene->camera_right.y, sScene->camera_right.z));
            sScene->camera_look_at = transform * sScene->camera_look_at;
            sScene->SetSceneView(sScene->camera_position, sScene->camera_look_at, sScene->camera_up);
        }
        lastCursorPosX = xPos;
    }
    // Tilting
    if (isMiddleButtonPressed)
    {
        double xPos, yPos;
        glfwGetCursorPos(window, &xPos, &yPos);

        if (lastCursorPosY != -1.0f)
        {
            double dy = yPos - lastCursorPosY;
            transform = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -(float)dy*0.2, 0.0f));
            sScene->camera_look_at = transform * sScene->camera_look_at;
            sScene->SetSceneView(sScene->camera_position, sScene->camera_look_at, sScene->camera_up);
        }
        lastCursorPosY = yPos;
    }
}

/* Callback function for mouse controls */
void mouseButtonCallback(GLFWwindow *window, int button, int action, int mods)
{
	double xPos, yPos;
	glfwGetCursorPos(window, &xPos, &yPos);
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
	{
		lastCursorPosY = yPos;
		lastCursorPosX = xPos;
		isLeftButtonPressed = false;
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		return;
	}
	if (button == GLFW_MOUSE_BUTTON_LEFT && action== GLFW_PRESS)
	{
		lastCursorPosY = yPos;
		lastCursorPosX = xPos;
		isLeftButtonPressed = true;
		isRightButtonPressed = false;
		isMiddleButtonPressed = false;
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		return;
	}
	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE)
	{
		lastCursorPosY = yPos;
		lastCursorPosX = xPos;
		isRightButtonPressed = false;
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		return;
	}
	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
	{
		lastCursorPosY = yPos;
		lastCursorPosX = xPos;
		isLeftButtonPressed = false;
		isRightButtonPressed = true;
		isMiddleButtonPressed = false;
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		return;
	}
	if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_RELEASE)
	{
		lastCursorPosY = yPos;
		lastCursorPosX = xPos;
		isMiddleButtonPressed = false;
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		return;
	}
	if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_PRESS)
	{
		lastCursorPosY = yPos;
		lastCursorPosX = xPos;
		isLeftButtonPressed = false;
		isRightButtonPressed = false;
		isMiddleButtonPressed = true;
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		return;
	}
}
// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}