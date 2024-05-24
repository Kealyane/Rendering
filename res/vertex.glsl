#version 410

layout(location = 0) in vec3 in_position;
uniform float aspect_ratio;
uniform float offsetTime;
uniform float squareSize;
uniform mat4 matrix;

void main()
{
    /*
    vec2 newPos = in_position;
    vec2 newPosRatio = vec2(newPos.x/aspect_ratio, newPos.y);

    float timeX = newPosRatio.x + cos(offsetTime*0.5) * squareSize;
    float timeY = newPosRatio.y + sin(offsetTime*0.5) * squareSize;
    vec2 newPosTime = vec2(timeX, timeY);
    gl_Position = vec4(newPosTime, 0., 1.);
    */

    gl_Position = matrix * vec4(in_position, 1.);
}