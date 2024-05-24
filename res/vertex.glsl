#version 410

layout(location = 0) in vec3 in_position;
layout(location = 0) in vec2 in_positionSquare;
layout(location = 1) in vec2 in_uv;
uniform float aspect_ratio;
uniform float offsetTime;
uniform float squareSize;
uniform mat4 matrix;

out vec3 vertex_position;

out vec2 out_uv;

void main()
{
    
    out_uv = in_uv;
    vec2 newPosRatio = vec2(in_positionSquare.x/aspect_ratio, in_positionSquare.y)*2;
    gl_Position = vec4(newPosRatio, 0., 1.);


    //vec2 newPos = in_positionSquare;
    //vec2 newPosRatio = vec2(newPos.x/aspect_ratio, newPos.y);
    //float timeX = newPosRatio.x + cos(offsetTime*0.5) * squareSize;
    //float timeY = newPosRatio.y + sin(offsetTime*0.5) * squareSize;
    //vec2 newPosTime = vec2(timeX, timeY);
    //gl_Position = vec4(newPosTime, 0., 1.);
    
    //vertex_position = in_position;
    //gl_Position = matrix * vec4(in_position, 1.);
}