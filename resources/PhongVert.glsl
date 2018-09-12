#version 410 core

layout(location = 0) in vec3 vertPos;
layout(location = 2) in vec3 vertNor;
layout(location = 1) in vec3 vertTex;
uniform mat4 P;
uniform mat4 M;
uniform mat4 V;


out vec3 fragNor;
out vec3 fragPos;

void main()
{
	fragNor = normalize(transpose(inverse(mat3(M))) * vertNor);
	fragPos = (M * vec4(vertPos, 1.0)).xyz;
	gl_Position = P * V * M * vec4(vertPos, 1.0);
}
