#version 450 core

layout (location = 0) out vec4 fragColor;

in VS_OUT {
	vec3 w_normal;
	vec3 w_fragPos;
} fs_in;

uniform vec3 viewPos;
uniform vec3 color;

const vec3 LIGHT_DIR = normalize(vec3(-1.0, -1.0, -1.0));
const vec3 COOL_COLOR = vec3(0.0, 0.0, 0.6);
const vec3 WARM_COLOR = vec3(0.6, 0.0, 0.0);

void main()
{
  const vec3 cool  = min(COOL_COLOR + 0.3 * color, 1.0);
  const vec3 warm  = min(WARM_COLOR + 0.3 * color, 1.0); 
  const vec3 diffuseColor = mix(cool, warm, (dot(-LIGHT_DIR, fs_in.w_normal) + 1.0) * 0.5);
  const vec3 reflectDir  = normalize(normalize(reflect(LIGHT_DIR, fs_in.w_normal)));
  const vec3 viewDir = normalize(fs_in.w_fragPos - viewPos);
  const vec3 specularColor = vec3(pow(max(dot(reflectDir, -viewDir), 0.0), 32.0));
  
  fragColor.rgb = min(diffuseColor + specularColor, vec3(1.0));
  fragColor.a = 1.0;
}