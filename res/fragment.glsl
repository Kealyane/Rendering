#version 410

out vec4 out_color;

in vec2 out_uv;
in vec3 out_normal;
in vec3 out_position;

uniform sampler2D my_texture;
uniform vec3 light_direction;
uniform vec3 point_light;

const float ambiant_light = 0.3;
const float intensity = 0.4;

void main()
{
    vec4 texture_color = texture(my_texture, out_uv);
    vec3 normal = normalize(out_normal);
    float lightDirectional = max(dot(normal, light_direction), 0.0);

    vec3 lightPoint_direction = normalize(point_light - out_position);
    float lightPoint_distance = length(point_light - out_position);
    float lightPoint = max(dot(normal, lightPoint_direction), 0.0);

    float decreaseLight = 1 / (lightPoint_distance * lightPoint_distance);

    float light = lightDirectional + ambiant_light + (lightPoint * decreaseLight * intensity) ;

    out_color = vec4(texture_color.rgb * light, 1.0);
    
    //out_color = texture_color;
    
    //vec3 normal_color = (out_normal * 0.5) + 0.5;
    //out_color = vec4(normal_color, 1.0);
}