#version 460

// in layout(location=0) vec2 vertex;
in layout(location=0) vec2 pos;
in layout(location=1) vec2 size;
in layout(location=2) int textureID;
in layout(location=3) float layer;
in layout(location=4) vec3 color;

uniform mat4 orthographic;

out vec3 p_textureCoord;
out vec3 p_color;
out float p_cursor;

void main()
{
    vec2 vertexPos = vec2(gl_VertexID >> 1, gl_VertexID & 1);
    gl_Position = orthographic * vec4(pos + size * vertexPos, -layer, 1.0);
    p_textureCoord = vec3(vertexPos, textureID);
    p_color = color;

    p_cursor = step(-0.5, float(textureID));
}