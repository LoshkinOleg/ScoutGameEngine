#version 450 core

layout (location = 0) out vec4 fragColor;

in VS_OUT {
	vec3 w_fragPos;
	vec3 w_normal;
	vec2 uv;
} fs_in;

uniform sampler2D albedoMap;
uniform sampler2D specularMap;
uniform float shininess;

uniform vec3 viewPos;

const vec3 LIGHT_DIR = normalize(vec3(-1.0, -1.0, -1.0));
const float AMBIENT_FACTOR = 0.01;

void main()
{
	const vec3 fragPos = fs_in.w_fragPos;
	const vec3 normal = fs_in.w_normal;
	const float alpha = texture(albedoMap, fs_in.uv).a;
	if (alpha < 0.01) discard;
	const vec3 albedo = texture(albedoMap, fs_in.uv).rgb;
	const vec3 specular = texture(specularMap, fs_in.uv).rgb;

	const vec3 viewDir = normalize(fragPos - viewPos);
	const vec3 halfway = normalize(-LIGHT_DIR - viewDir);

	const float diffuseIntensity = max(dot(-LIGHT_DIR, normal), 0.0);
	const float specularIntensity = pow(max(dot(normal, halfway), 0.0), shininess);

	const vec3 ambientColor = AMBIENT_FACTOR * albedo;
	const vec3 diffuseColor = diffuseIntensity * albedo;
	const vec3 specularColor = specularIntensity * specular;

	fragColor.rgb = ambientColor + diffuseColor + specularColor;
	fragColor.a = alpha;
}