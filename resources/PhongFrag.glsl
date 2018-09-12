#version 410 core
uniform vec3 uSunPos;
uniform vec3 campos;

in vec3 fragNor;
in vec3 fragPos;

out vec4 color;

void main()
{
	vec3 uMatAmb = vec3(1.0, 1.0, 1.0);
	vec3 uMatDif = vec3(0.4, 0.9, 0.3);
	vec3 uMatSpec = vec3(0.4, 0.3, 0.9);
	float uShine = 12.0;

	vec3 normal = normalize(fragNor);
	vec3 viewDir = normalize(campos - fragPos);
	vec3 surfaceToLight = normalize(uSunPos - fragPos);
	float lambertian = max(dot(normal, surfaceToLight), 0.0);
	vec3 H = normalize(surfaceToLight + viewDir);
    float specAngle = max(dot(H, normal), 0.0);
	
	vec3 difLight = uMatDif * lambertian;

    float specular = pow(specAngle, uShine);
	vec3 shineLight = lambertian > 0.0 ? uMatSpec * specular : vec3(0.0);
	
	vec3 col = uMatAmb * 0.1 + difLight + shineLight;
	color = vec4(col, 1.0f);
}
