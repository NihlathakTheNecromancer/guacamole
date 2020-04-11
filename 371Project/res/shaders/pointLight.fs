#version 330 core

in vec3 fragment_position;
in vec3 normal;
in vec2 UV;

out vec4 pixel_colour;

uniform sampler2D   u_texture;
uniform samplerCube shadow_map;
uniform bool        is_textured;
uniform vec4        fragment_colour;
uniform vec3        camera_position;
uniform vec3        light_position;
uniform float       far_plane;

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
    
    // Uncomment the line below to see the depthMap - you must also comment out the pixel_colour line in main()
    //pixel_colour = vec4(vec3(closest_depth / far_plane), 1.0); 

    return shadow_result;
}

void main()
{           
    vec3 light_color = vec3(1.0);
    vec3 fragment_color = is_textured ? texture(u_texture, UV).rgb : fragment_colour.xyz;
    vec3 normal = normalize(normal);
    
    // ambient
    vec3 ambient = 0.3 * fragment_color;

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
    
    // calculate shadow and lighting
    float shadow = CalculateShadow(normal, light_dir);
    vec3 lighting = (ambient + (1.0f - shadow) * (diffuse + specular)) * fragment_color;

    // final color
    pixel_colour = vec4(lighting, 1.0f);
}

