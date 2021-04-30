#version 460
#extension GL_EXT_texture_array : enable

in vec3 p_textureCoord;
in vec3 p_color;
in float p_cursor;

uniform sampler2DArray map;
uniform vec2 ssHorizontalCuttoff;

out vec4 frag;

void main()
{

    vec4 tex = texture2DArray(map, p_textureCoord);
    frag = vec4(p_color, mix(1.0, tex.r, step(0.5, p_cursor)));
}