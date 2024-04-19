#version 410

layout(location = 0) in vec2 in_position;
uniform float aspect_ratio;
uniform float offsetTime;
uniform float squareSize;

void main()
{
    vec2 newPos = in_position;
    vec2 newPosRatio = vec2(newPos.x/aspect_ratio, newPos.y);

    float time = sin(offsetTime*0.5) * squareSize;
    float x = newPosRatio.x;
    vec2 newPosTime = vec2( x + time, newPosRatio.y);
    
    gl_Position = vec4(newPosTime, 0., 1.);
}