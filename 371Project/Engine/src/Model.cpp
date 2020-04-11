
#include "../include/Model.hpp"

Model::Model(Geometry* g, Scene* s, Model* p)
{
    /* Defaults */
    model_matrix        = glm::mat4(1.0f);                        /* No transformations by default */
    fragment_colour     = glm::vec4(1.0f);                        /* Model fragments are white by default */
    position            = new glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);  /* Models start at origin by default */    
    forward_direction   = new glm::vec4(0.0f, 0.0f, 1.0f, 0.0f);  /* Models are facing in the positive z direction by default */
    right_direction     = new glm::vec4(-1.0f, 0.0f, 0.0f, 0.0f); /* -------------------------------------------------------- */
    children            = std::vector<Model*>();

    geometry = g; 
    scene = s;
    parent = p;

    dimensions = glm::vec3(1.0f); // This assumes all primitives are initially 1 unit across (length/diameter)

    /* If a parent model was assigned, make this model a child of that parent */
    if(parent)
        parent->AddChild(this);
}

void Model::SetModelTransparency(bool isTransparent)
{
    transparency_enabled = true;
}

void Model::SetModelShaderProgram(ShaderProgram* shader)
{
    model_shader_program = shader;
}

void Model::AddChild(Model* child)
{
    children.push_back(child);
}

glm::vec3 Model::GetModelPosition(void)
{
    return glm::vec3(*position);
}

glm::vec3 Model::GetModelForwardDirectionVector(void)
{
    return glm::vec3(*forward_direction);
}

glm::vec3 Model::GetModelRightDirectionVector(void)
{
    return glm::vec3(*right_direction);
}

void Model::AddModelAnimation(Animation anim)
{
    model_animations.push_back(anim);
}

void Model::StepModelAnimations(float input_direction, float dt)
{
    for(int i = 0; i < model_animations.size(); i++)
    {
        model_animations[i].Step(input_direction, dt);
    }
}

void Model::TranslateModel(glm::vec3 translation)
{   
    glm::mat4 t = glm::translate(glm::mat4(1.0f), translation);

    model_matrix = t * model_matrix;

    *position = t * (*position);
}

void Model::TranslateModelAndChildren(glm::vec3 translation)
{
    glm::mat4 t = glm::translate(glm::mat4(1.0f), translation);

    model_matrix = t * model_matrix;

    *position = t * (*position);

    for(unsigned int i = 0; i < children.size(); i++)
    {        
        if(children[i]->children.size() > 0)
        {
            children[i]->TranslateModelAndChildren(translation);
        }
        else
        {
            children[i]->model_matrix = t * children[i]->model_matrix;

            *children[i]->position = t * (*children[i]->position);
        }        
    }
}



void Model::ScaleModel(glm::vec3 scalars)
{
    /* Apply scaling */
    glm::mat4 t = glm::scale(glm::mat4(1.0f), scalars);

    model_matrix = t * model_matrix;

    *position = t * (*position);
}

void Model::ScaleCollisionBoundaries(glm::vec3 scalars)
{
    dimensions.x *= scalars.x / 2;
    dimensions.y *= scalars.y / 2;
    dimensions.z *= scalars.z / 2;
}

void Model::ScaleModelAndChildren(glm::vec3 scalars)
{
    /* Apply scaling */
    glm::mat4 t = glm::scale(glm::mat4(1.0f), scalars);

    model_matrix = t * model_matrix;

    *position = t * (*position);

    for(unsigned int i = 0; i < children.size(); i++)
    {        
        if(children[i]->children.size() > 0)
        {
            children[i]->ScaleModelAndChildren(scalars);
        }
        else
        {
            children[i]->model_matrix = t * children[i]->model_matrix;

            *children[i]->position = t * (*children[i]->position);
        }        
    }
}

void Model::RotateModel(float angle, glm::vec3 axis)
{
    /* Apply rotation */
    glm::mat4 t = glm::rotate(glm::mat4(1.0f), angle, axis);

    model_matrix = t * model_matrix;

    *right_direction    = t * (*right_direction);
    *forward_direction  = t * (*forward_direction);
    *position           = t * (*position);
}

void Model::RotateModelAndChildren(float angle, glm::vec3 axis)
{
    /* Apply rotation */
    glm::mat4 t = glm::rotate(glm::mat4(1.0f), angle, axis);

    model_matrix = t * model_matrix;
    *right_direction = t * (*right_direction);
    *forward_direction = t * (*forward_direction);
    *position = t * (*position);

    for(unsigned int i = 0; i < children.size(); i++)
    {        
        if(children[i]->children.size() > 0)
        {
            children[i]->RotateModelAndChildren(angle, axis);
        }
        else
        {
            children[i]->model_matrix = t * children[i]->model_matrix;

            *children[i]->right_direction   = t * (*children[i]->right_direction);
            *children[i]->forward_direction = t * (*children[i]->forward_direction);
            *children[i]->position          = t * (*children[i]->position);
        }        
    }
}

void Model::SetModelRenderMode(unsigned int m)
{
    render_mode = m;
    for(unsigned int i = 0; i < children.size(); i++)
    {
        children[i]->SetModelRenderMode(m);
    }
}

void Model::SetModelTexture(Texture* tex)
{
    texture = tex;
    textures_enabled = true;
}

void Model::ToggleModelTexture(void)
{
    if(texture)
    {
        if(textures_enabled)
            textures_enabled = false;
        else
            textures_enabled = true;       
    }
}

void Model::SetModelFragmentColour(glm::vec4 c)
{
    fragment_colour = c;
}

void Model::BindUniforms(void) {
    // TODO pretty sure this will never be invoked
    BindUniforms(model_shader_program);
}

void Model::BindUniforms(ShaderProgram* shader)
{
    unsigned int uniformLocation; 

    uniformLocation = glGetUniformLocation(shader->id, "model_matrix");
    glUniformMatrix4fv(uniformLocation, 1, GL_FALSE, &model_matrix[0][0]);

    uniformLocation = glGetUniformLocation(shader->id, "fragment_colour");
    glUniform4fv(uniformLocation, 1, &fragment_colour[0]);

    uniformLocation = glGetUniformLocation(shader->id, "is_textured");
    glUniform1i(uniformLocation, textures_enabled);

    if (textures_enabled)
    {
        uniformLocation = glGetUniformLocation(shader->id, "u_texture");
        glUniform1i(uniformLocation, texture->GetTextureSlot());
    }

    // TODO These are so specific it hurts me
    {
        uniformLocation = glGetUniformLocation(model_shader_program->id, "light_position_one");
        glUniform3fv(uniformLocation, 1, &scene->GetSceneLightPositionOne()[0]);

        uniformLocation = glGetUniformLocation(model_shader_program->id, "light_direction_one");
        glUniform3fv(uniformLocation, 1, &scene->GetSceneLightDirectionOne()[0]);

        uniformLocation = glGetUniformLocation(model_shader_program->id, "light_colour_one");
        glUniform4fv(uniformLocation, 1, &scene->GetSceneLightColourOne()[0]);

        uniformLocation = glGetUniformLocation(model_shader_program->id, "light_cutoff_one");
        glUniform1f(uniformLocation, scene->GetSceneLightCutoffOne());

        uniformLocation = glGetUniformLocation(model_shader_program->id, "light_switch_one");
        glUniform1i(uniformLocation, scene->GetSceneLightSwitchOne());

        uniformLocation = glGetUniformLocation(model_shader_program->id, "light_position_two");
        glUniform3fv(uniformLocation, 1, &scene->GetSceneLightPositionTwo()[0]);

        uniformLocation = glGetUniformLocation(model_shader_program->id, "light_direction_two");
        glUniform3fv(uniformLocation, 1, &scene->GetSceneLightDirectionTwo()[0]);

        uniformLocation = glGetUniformLocation(model_shader_program->id, "light_colour_two");
        glUniform4fv(uniformLocation, 1, &scene->GetSceneLightColourTwo()[0]);

        uniformLocation = glGetUniformLocation(model_shader_program->id, "light_cutoff_two");
        glUniform1f(uniformLocation, scene->GetSceneLightCutoffTwo());

        uniformLocation = glGetUniformLocation(model_shader_program->id, "light_switch_two");
        glUniform1i(uniformLocation, scene->GetSceneLightSwitchTwo());

        uniformLocation = glGetUniformLocation(model_shader_program->id, "light_position_three");
        glUniform3fv(uniformLocation, 1, &scene->GetSceneLightPositionThree()[0]);

        uniformLocation = glGetUniformLocation(model_shader_program->id, "light_direction_three");
        glUniform3fv(uniformLocation, 1, &scene->GetSceneLightDirectionThree()[0]);

        uniformLocation = glGetUniformLocation(model_shader_program->id, "light_colour_three");
        glUniform4fv(uniformLocation, 1, &scene->GetSceneLightColourThree()[0]);

        uniformLocation = glGetUniformLocation(model_shader_program->id, "light_cutoff_three");
        glUniform1f(uniformLocation, scene->GetSceneLightCutoffThree());

        uniformLocation = glGetUniformLocation(model_shader_program->id, "light_switch_three");
        glUniform1i(uniformLocation, scene->GetSceneLightSwitchThree());

    }
}

void Model::Draw() {
    Draw(model_shader_program);
}

void Model::Draw(ShaderProgram* shader)
{
    glUseProgram(shader->id); // This was invoked by DrawScene, which then called this function
    if(textures_enabled) texture->Bind(0);
    if(transparency_enabled) glEnable(GL_BLEND);
    BindUniforms(shader);

    glBindVertexArray(geometry->vao);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, geometry->ibo);
    glDrawElements(render_mode, geometry->indices.size(), GL_UNSIGNED_INT, nullptr);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    if(transparency_enabled) glDisable(GL_BLEND);
    if(textures_enabled) texture->Unbind();
    glUseProgram(0); // Will be invoked by DrawScene once the rendering is finished
}