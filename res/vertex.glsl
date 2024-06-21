#version 410

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec2 in_uv;

uniform mat4 matrix;

out vec2 out_uv;

void main()
{
    gl_Position = matrix * vec4(in_position, 1.);
    out_uv = vec2(in_uv.x, in_uv.y);
}