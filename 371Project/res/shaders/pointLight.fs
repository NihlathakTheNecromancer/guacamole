#version 330 core

in vec3 fragment_position;
in vec3 normal;
in vec2 UV;

out vec4 pixel_colour;

uniform sampler2D u_texture;
uniform samplerCube shadow_map;

uniform vec3 fragment_colour;

uniform vec3 light_position;
uniform vec3 camera_position;

uniform float far_plane;

float CalculateShadow(vec3 normalDir, vec3 lightDir)
{
    // determine the vector at which to sample the depth map
    vec3 frag_light = fragment_position - light_position;
    // get the normalized distance between the light source and the closest point from the depth map
    float closest_depth = texture(shadow_map, frag_light).r;
    // scale it back up into the range [0, far_plane]
    closest_depth *= far_plane;
    // get the depth distance between the current fragment and the light source
    float current_depth = length(frag_light);
    // compare these depths to determine whether the current fragment is in shadow
    //float bias = max(0.05* (1.0 - dot(normal, lightDir)), 0.005);
    float bias = 0.25;
    float shadow_result = current_depth - bias > closest_depth ? 1.0f : 0.0f; // how it should be
    
    // Uncomment the line below (and comment out the pixel_colour line in main() to see the depthMap)
    //pixel_colour = vec4(vec3(closest_depth / far_plane), 1.0); 

    return shadow_result;
}

void main()
{           
    vec3 color = texture(u_texture, UV).rgb;
    //if (color == vec3(0.0f)) { // if no texture has been set
        color = fragment_colour;//.xyz; // use the color that was fed in via uniform
    //}
    vec3 normal = normalize(normal);
    //vec3 light_color = vec3(0.3);
    vec3 light_color = vec3(1.0);
    // ambient
    vec3 ambient = 0.3 * color;
    // diffuse
    vec3 light_dir = normalize(light_position - fragment_position);
    float diff = max(dot(light_dir, normal), 0.0);
    vec3 diffuse = diff * light_color;
    // specular
    vec3 view_dir = normalize(camera_position - fragment_position);
    float spec = 0.0;
    vec3 halfway_dir = normalize(light_dir + view_dir);  
    spec = pow(max(dot(normal, halfway_dir), 0.0), 64.0);
    vec3 specular = spec * light_color;    
    // calculate shadow
    float shadow = CalculateShadow(normal, light_dir);

    //float shadow_debug = shadow > 0.5f ? 0.1f : 0.9f;
   

    vec3 lighting = (ambient + (1.0f - shadow) * (diffuse + specular)) * color;

    pixel_colour = vec4(lighting, 1.0f);
}

