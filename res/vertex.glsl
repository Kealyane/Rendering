#version 410

layout(location = 0) in vec2 in_position;
uniform float aspect_ratio;

void main()
{
    vec2 newPos = in_position + vec2(0.4, 0.4);
    vec2 newPosRatio = vec2(newPos.x/aspect_ratio, newPos.y);
    gl_Position = vec4(newPosRatio, 0., 1.);
}