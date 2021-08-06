#version 450 core

layout (location = 0) out vec4 fragColor;

in VS_OUT {
	vec3 normal;
	vec3 fragPos;
} fs_in;

uniform vec3 viewPos;
uniform vec3 baseColor;

const vec3 lightDir = normalize(vec3(-1.0, -1.0, -1.0));
const vec3 coolColor = vec3(0.0, 0.0, 0.6);
const vec3 warmColor = vec3(0.6, 0.0, 0.0);

void main()
{
  const vec3 cool  = min(coolColor + 0.3 * baseColor, 1.0);
  const vec3 warm  = min(warmColor + 0.3 * baseColor, 1.0); 
  const vec3 diffuseColor = mix(cool, warm, (dot(-lightDir, fs_in.normal) + 1.0) * 0.5);
  const vec3 reflectDir  = normalize(normalize(reflect(lightDir, fs_in.normal)));
  const vec3 viewDir = normalize(fs_in.fragPos - viewPos);
  const vec3 specularColor = vec3(pow(max(dot(reflectDir, -viewDir), 0.0), 32.0));
  
  fragColor.rgb = min(diffuseColor + specularColor, vec3(1.0));
  fragColor.a = 1.0;
}