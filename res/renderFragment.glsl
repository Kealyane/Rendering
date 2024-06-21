#version 410

out vec4 out_color;
in vec2 out_uv;

uniform sampler2D textureCube;

void main()
{
    vec4 texture_color = texture(textureCube, out_uv);
    out_color = texture_color;
}