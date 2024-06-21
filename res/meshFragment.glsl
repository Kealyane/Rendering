#version 410

in vec3 frag_normal;
in vec2 frag_texcoord;

out vec4 frag_color;

uniform sampler2D texture_diffuse;

void main() {
    vec3 color = texture(texture_diffuse, frag_texcoord).rgb;
    vec3 normal = normalize(frag_normal);
    float light = max(dot(normal, vec3(0.0, 0.0, 1.0)), 0.0);
    frag_color = vec4(color * light, 1.0);
}