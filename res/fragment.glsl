#version 410

out vec4 out_color;
//uniform vec2 varHello;

in vec3 vertex_position;
in vec2 out_uv;

uniform sampler2D my_texture;

void main()
{
    //out_color = vec4(0.04, 0.93, 0.99, 1.0);
    
    //out_color = vec4(vertex_position, 1.);
    //out_color = vec4(out_uv.x, out_uv.y, 0., 1.);
    vec4 texture_color = texture(my_texture, out_uv);
    out_color = texture_color;
}