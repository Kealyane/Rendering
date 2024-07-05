#version 410

out vec4 out_color;
in vec2 out_uv;
in vec3 out_normal;

uniform sampler2D my_texture;
uniform vec3 light_direction;

const float ambiant_light = 0.3;

void main()
{
    vec4 texture_color = texture(my_texture, out_uv);
    vec3 normal = normalize(out_normal);
    float light = max(dot(normal, light_direction), 0.0);
    light = light + ambiant_light;
    out_color = vec4(texture_color.rgb * light, 1.0);
    
    //out_color = texture_color;
    
    //vec3 normal_color = (out_normal * 0.5) + 0.5;
    //out_color = vec4(normal_color, 1.0);
}