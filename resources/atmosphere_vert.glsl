#version 410 core


layout(location = 0) in vec3 vertPos;
uniform mat4 M;
uniform mat4 V;
uniform mat4 P;
uniform vec3 uSunPos;

out vec4 fragPos;

void main() {
    //Multiply by view to rotate the atmosphere view
    fragPos = V * M * vec4(vertPos, 1.0);
    gl_Position = vec4(vertPos, 1.0);
}
