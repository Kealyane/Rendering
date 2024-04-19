#version 410

layout(location = 0) in vec2 in_position;

void main()
{
    vec2 newPos = in_position + vec2(0.4, 0.4);
    gl_Position = vec4(newPos, 0., 1.);
}