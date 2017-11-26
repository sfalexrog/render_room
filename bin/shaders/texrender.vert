#version 430

layout(location = 0) in vec2 quadPos;
out vec2 texPos;

void main() {
    texPos = (vec2(1.0, 1.0) + quadPos) / 2.0;
    gl_Position = vec4(quadPos, 1.0, 1.0);
}
