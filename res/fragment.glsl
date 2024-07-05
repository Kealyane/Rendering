#version 410

out vec4 out_color;
in vec2 out_uv;
in vec3 out_normal;

uniform sampler2D my_texture;

void main()
{
    //vec4 texture_color = texture(my_texture, out_uv);
    //out_color = texture_color;
    vec3 normal_color = (out_normal * 0.5) + 0.5;
    out_color = vec4(normal_color, 1.0);
}