#version 410

layout(location = 0) in vec2 in_position;


void main()
{
   gl_Position = vec4(in_position*2, 0., 1.);
}