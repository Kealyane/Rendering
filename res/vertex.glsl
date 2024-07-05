#version 410

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec2 in_uv;
layout(location = 2) in vec3 in_normal;

uniform mat4 model_matrix;
uniform mat4 normal_matrix;

out vec2 out_uv;
out vec3 out_normal;
out vec3 out_position;

vec3 apply_matrix_to_position(mat4 matrix, vec3 point)
{
    vec4 tmp = matrix * vec4(point, 1.);
    return tmp.xyz / tmp.w;
}

vec3 apply_matrix_to_direction(mat4 matrix, vec3 direction)
{
    vec4 tmp = matrix * vec4(direction, 0.);
    return normalize(tmp.xyz);
}

void main()
{
    vec3 worldPos = apply_matrix_to_position(model_matrix, in_position);
    vec3 worldNormal = apply_matrix_to_direction(normal_matrix, in_normal);

    gl_Position = vec4(worldPos, 1.);
    out_position = worldPos;
    out_uv = in_uv; 
    out_normal = worldNormal;

    //gl_Position = matrix * vec4(in_position, 1.);
    //out_position = in_position;
    //out_uv = in_uv ; 
    //out_normal = in_normal;
}
