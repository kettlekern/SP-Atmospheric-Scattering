#version 410 core


layout(location = 0) in vec3 vertPos;
uniform mat4 V;

out vec3 fragPos;

void main() {
    //Multiply by view to rotate the atmosphere view
    fragPos = vertPos;
    gl_Position = vec4(vertPos, 1.0);
}
