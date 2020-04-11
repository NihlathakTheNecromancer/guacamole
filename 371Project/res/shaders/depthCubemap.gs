#version 330 core

layout (triangles) in;
layout (triangle_strip, max_vertices=18) out;

uniform mat4 view_projections[6];

out vec4 frag_position;

void main()
{
	// Meant for omnidirectional depth mapping (6 sided cube)
	// But works fine with fewer frustums as long as the FOVs do not overlap
	for (int face=0; face < 6; face++)
	{
		gl_Layer = face;
		for(int i=0; i < 3; ++i)
		{
			// Pass position along to the fragment shader
			frag_position = gl_in[i].gl_Position;

			// Incoming vertex data has had model matrix already applied in vertex shader
			// Need to apply the view and projection matrices from the light source perspective
			gl_Position = view_projections[face] * frag_position;

			EmitVertex();
		}
		EndPrimitive();
	}
}