#version 410

layout(location = 0) in vec2 in_position;
layout(location = 1) in vec2 in_uv;

out vec2 out_uv;

void main()
{
    gl_Position = vec4(in_position, 0.,1.);
    out_uv = vec2(in_uv.x, in_uv.y);
}