#version 300 es

in vec4 a_position;
in vec2 a_texCoord;
out vec2 v_texCoord;
uniform mat4 uMatrix;

void main() {
    gl_Position = uMatrix * a_position;
    v_texCoord = a_texCoord;
}