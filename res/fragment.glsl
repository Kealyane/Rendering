#version 410

out vec4 out_color;
//uniform vec2 varHello;

in vec3 vertex_position;
in vec2 out_uv;

void main()
{
    //out_color = vec4(0.04, 0.93, 0.99, 1.0);
    
    //out_color = vec4(vertex_position, 1.);
    out_color = vec4(out_uv.x, out_uv.y, 0., 1.);
}