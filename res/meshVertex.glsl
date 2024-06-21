#version 410

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_normal;
layout(location = 2) in vec2 in_texcoord;

out vec3 frag_normal;
out vec2 frag_texcoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    frag_normal = in_normal;
    frag_texcoord = in_texcoord;
    gl_Position = projection * view * model * vec4(in_position, 1.0);
}