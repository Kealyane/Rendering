#version 410

out vec4 out_color;
in vec2 out_uv;

uniform sampler2D textureCube;

out vec4 frag_color;

void main()
{
    vec4 texture_color = texture(textureCube, out_uv);
    
    // niveau de gris
    float gray = dot(texture_color.rgb, vec3(0.3)); 

    float channelR = dot(texture_color.rgb, vec3(1.,0.,0.)); 
    float channelG = dot(texture_color.rgb, vec3(0.,1.,0.)); 
    float channelB = dot(texture_color.rgb, vec3(0.,0.,1.)); 
    frag_color = vec4(vec3(channelB), texture_color.a);

    vec4 red = vec4(texture_color.r, 0., 0., texture_color.a);
    vec4 green = vec4(0., texture_color.g, 0., texture_color.a);
    vec4 blue = vec4(0., 0., texture_color.b, texture_color.a);
    frag_color = blue;

    float contrast = 2.; 
    texture_color.rgb = ((texture_color.rgb - 0.5) * contrast + 0.5);
    frag_color = texture_color;

    float saturation = 2.; 
    texture_color.rgb = mix(vec3(gray), texture_color.rgb, saturation);
    frag_color = texture_color;

    vec2 uv = out_uv;
    uv.y += sin(uv.x * 10.0) * 0.1; 
    vec4 color = texture(textureCube, uv);
    frag_color = color;
    
    out_color = frag_color;
}

