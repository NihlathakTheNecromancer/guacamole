#ifndef _SCENE_H
#define _SCENE_H

#include "Model.hpp"
#include "ShaderProgram.hpp"
#include "Primitive.hpp"

#include <vector>

class Primitive;
class Model;
class ShaderProgram;
class Scene
{   
protected:
    /* Scene Shader Program */
    ShaderProgram*      scene_shader_program;

    /* Models in the scene */
    std::vector<Model*> models = std::vector<Model*>();
    
    /* Scene Camera */
    float        camera_aspect_ratio;               // Aspect Ratio
    float        camera_fov;                        // Field of View
    float        camera_near;                       // Near
    float        camera_far;                        // Far
    glm::vec3    camera_position;                   // Camera Position
    glm::vec3    camera_front;                      // Point Camera is looking at
    glm::vec3    camera_up;                         // Up Direction of camera
    float        camera_yaw;                        // Camera Yaw angle
    float        camera_pitch;                      // Camera Pitch angle
    float        camera_bound_x_neg = -1000*Unit;   // Camera negative X boundary
    float        camera_bound_x_pos = 1000*Unit;    // Camera positive X boundary
    float        camera_bound_z_neg = -1000*Unit;   // Camera negative Z boundary
    float        camera_bound_z_pos = 1000*Unit;    // Camera positive Z boundary

    /* Lighting */
    glm::vec3    light_position_one;                // Position of scene light One
    glm::vec3    light_direction_one;               // Direction of scene light One
    glm::vec4    light_colour_one = glm::vec4(1.0f);// Colour of scene light One
    float        light_cutoff_one = glm::cos(glm::radians(180.0f));
    bool         light_switch_one = false;

    glm::vec3    light_position_two;                // Position of scene light Two
    glm::vec3    light_direction_two;               // Direction of scene light Two
    glm::vec4    light_colour_two = glm::vec4(1.0f);// Colour of scene light Two
    float        light_cutoff_two = glm::cos(glm::radians(180.0f));
    bool         light_switch_two = false;

    glm::vec3    light_position_three;              // Position of scene light Three
    glm::vec3    light_direction_three;             // Direction of scene light Three
    glm::vec4    light_colour_three = glm::vec4(1.0f); // Colour of scene light Three
    float        light_cutoff_three = glm::cos(glm::radians(180.0f));
    bool         light_switch_three = false;

    /* Matrices */
    glm::mat4    view_matrix;
    glm::mat4    projection_matrix;

    /* World Scale */
    float Unit = 1.0f;

    /* Construct */
    Scene(void);

public:

    /* Toggle */
    void                ToggleSceneTextures(void);

    /* Scene Parameters */
    ShaderProgram*      GetSceneShaderProgram(void);
    std::vector<Model*> GetSceneModels(void);
    // glm::mat4       GetSceneLightViewMatrix(void);
    // glm::mat4       GetSceneLightProjectionMatrix(void);
    void                SetSceneShaderProgram(ShaderProgram* shader);

    /* Camera Parameters */
    glm::mat4       GetCameraViewMatrix(void);
    glm::mat4       GetCameraProjectionMatrix(void);
    glm::vec3       GetCameraPosition(void);
    void            SetCameraPerspective(float fov, float ar, float near, float far);
    void            SetCameraOrthographic(float a, float b, float c, float d, float near, float far);
    void            SetCameraView(glm::vec3 cameraPosition, glm::vec3 cameraFront, glm::vec3 cameraUp);
    float           GetCameraYaw(void);
    float           GetCameraPitch(void);
    void            UpdateCameraView(void);

    /* Camera Controller */
    void            TranslateCameraRight(float distance);
    void            TranslateCameraLeft(float distance);
    void            TranslateCameraForward(float distance);
    void            TranslateCameraBackward(float distance);
    void            RotateCameraEuler(float yaw, float pitch);
    void            SetCameraBoundingBox(float neg_x, float pos_x, float neg_z, float pos_z);
    bool            CheckBoundingBoxCollision(glm::vec3 pos, glm::vec3 dir, float distance);


    /* Lighting */
    glm::vec3   inline  GetSceneLightPositionOne(void){ return light_position_one;}
    glm::vec3   inline  GetSceneLightDirectionOne(void){ return light_direction_one;}
    glm::vec4   inline  GetSceneLightColourOne(void){ return light_colour_one;}
    float       inline  GetSceneLightCutoffOne(void){ return light_cutoff_one;}
    bool        inline  GetSceneLightSwitchOne(void){ return light_switch_one;}
    void        inline  SetSceneLightPositionOne(glm::vec3 position){ light_position_one = position;}
    void        inline  SetSceneLightCutoffOne(float cutoff){ light_cutoff_one = cutoff;}
    void        inline  SetSceneLightDirectionOne(glm::vec3 dir){ light_direction_one = dir;}
    void        inline  SetSceneLightSwitchOne(bool isOn){ light_switch_one = isOn;}
    void        inline  SetSceneLightColourOne(glm::vec4 colour){ light_colour_one = colour;}

    glm::vec3   inline  GetSceneLightPositionTwo(void){ return light_position_two;}
    glm::vec3   inline  GetSceneLightDirectionTwo(void){ return light_direction_two;}
    glm::vec4   inline  GetSceneLightColourTwo(void){ return light_colour_two;}
    float       inline  GetSceneLightCutoffTwo(void){ return light_cutoff_two;}
    bool        inline  GetSceneLightSwitchTwo(void){ return light_switch_two;}
    void        inline  SetSceneLightPositionTwo(glm::vec3 position){ light_position_two = position;}
    void        inline  SetSceneLightCutoffTwo(float cutoff){ light_cutoff_two = cutoff;}
    void        inline  SetSceneLightDirectionTwo(glm::vec3 dir){ light_direction_two = dir;}
    void        inline  SetSceneLightSwitchTwo(bool isOn){ light_switch_two = isOn;}
    void        inline  SetSceneLightColourTwo(glm::vec4 colour){ light_colour_two = colour;}

    glm::vec3   inline  GetSceneLightPositionThree(void){ return light_position_three;}
    glm::vec3   inline  GetSceneLightDirectionThree(void){ return light_direction_three;}
    glm::vec4   inline  GetSceneLightColourThree(void){ return light_colour_three;}
    float       inline  GetSceneLightCutoffThree(void){ return light_cutoff_three;}
    bool        inline  GetSceneLightSwitchThree(void){ return light_switch_three;}
    void        inline  SetSceneLightPositionThree(glm::vec3 position){ light_position_three = position;}
    void        inline  SetSceneLightCutoffThree(float cutoff){ light_cutoff_three = cutoff;}
    void        inline  SetSceneLightDirectionThree(glm::vec3 dir){ light_direction_three = dir;}
    void        inline  SetSceneLightSwitchThree(bool isOn){ light_switch_three = isOn;}
    void        inline  SetSceneLightColourThree(glm::vec4 colour){ light_colour_three = colour;}
    

    /* Create */
    Geometry*       primitive_plane;
    Geometry*       primitive_cube;
    Geometry*       primitive_sphere;
    Geometry*       primitive_line;
    Model*          CreateModelPrimitive(PrimitiveType type, Model* parent);
    
    /* Draw */
    void            DrawScene(void);
    virtual void    Initialize(void) = 0;
    virtual void    Update(float dt) = 0;
    virtual void    InputController(GLFWwindow* window) = 0;
    virtual void    SetKeyCallback(GLFWwindow* window) = 0;

};
#endif /* _SCENE_H */