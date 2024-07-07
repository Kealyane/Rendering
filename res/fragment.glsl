#version 410

out vec4 out_color;

in vec2 out_uv;
in vec3 out_normal;
in vec3 out_position;

in vec3 out_tangent;
in vec3 out_bitangent;

uniform sampler2D my_texture;
uniform sampler2D my_normal_map;

uniform vec3 light_direction;
uniform vec3 point_light;

uniform vec3 colorPointLight;
uniform vec3 colorAmbiantLight;
uniform vec3 colorDirectionalLight;

const float ambiant_light = 0.3;
const float intensity = 0.4;

void main()
{
    vec4 texture_color = texture(my_texture, out_uv);
    vec3 normal = normalize(out_normal);

    vec3 normalFromNM = texture(my_normal_map, out_uv).rgb;
    normalFromNM = normalize(normalFromNM * 2.0 - 1.0); 

    mat3 TBN = mat3(out_tangent, out_bitangent, out_normal);
    vec3 world_normal = normalize(TBN * normalFromNM);

    float lightDirectional = max(dot(world_normal, light_direction), 0.0);

    vec3 lightPoint_direction = normalize(point_light - out_position);
    float lightPoint_distance = length(point_light - out_position);
    float lightPoint = max(dot(world_normal, lightPoint_direction), 0.0);

    float decreaseLight = 1 / (lightPoint_distance * lightPoint_distance);

    vec3 light = lightDirectional * colorDirectionalLight + ambiant_light * colorAmbiantLight + (lightPoint * decreaseLight * intensity) * colorPointLight;

    out_color = vec4(texture_color.rgb * light, 1.0);
    
}