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
    float        camera_aspect_ratio;   // Aspect Ratio
    float        camera_fov;            // Field of View
    float        camera_near;           // Near
    float        camera_far;            // Far
    glm::vec3    camera_position;       // Camera Position
    glm::vec3    camera_front;          // Point Camera is looking at
    glm::vec3    camera_up;             // Up Direction of camera
    float        camera_yaw = 0.0f;            // Camera Yaw angle
    float        camera_pitch = 0.0f;          // Camera Pitch angle

    /* Lighting */
    glm::vec3    light_position;        // Position of scene light

    /* Matrices */
    glm::mat4    view_matrix;
    glm::mat4    projection_matrix;

    /* World Scale */
    float Unit = 1.0f;

    /* Construct */
    Scene(void);

public:

    /* Toggle */
    void            ToggleSceneTextures(void);

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


    /* Lighting */
    glm::vec3       GetSceneLightPosition(void);
    void            SetSceneLightPosition(glm::vec3 position);
    

    /* Create */
    Geometry*       primitive_plane;
    Geometry*       primitive_cube;
    Geometry*       primitive_sphere;
    Model*          CreateModelPrimitive(PrimitiveType type, Model* parent);
    /*void          CreateModelOBJ(std::string obj_path); */
    
    /* Draw */
    void            DrawScene(void);
    virtual void    Initialize(void) = 0;
    virtual void    Update(float dt) = 0;

};
#endif /* _SCENE_H */