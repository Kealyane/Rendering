#version 410

out vec4 out_color;
in vec2 out_uv;

uniform sampler2D my_texture;

void main()
{
    vec4 texture_color = texture(my_texture, out_uv);
    out_color = texture_color;
}