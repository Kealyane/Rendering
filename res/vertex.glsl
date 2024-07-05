#version 410

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec2 in_uv;
layout(location = 2) in vec3 in_normal;

uniform mat4 matrix;

out vec2 out_uv;
out vec3 out_normal;
out vec3 out_position;

void main()
{
    gl_Position = matrix * vec4(in_position, 1.);
    out_position = in_position;
    out_uv = in_uv ; 
    out_normal = in_normal;
}