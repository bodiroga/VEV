#version 120

uniform int active_lights_n; // Number of active lights (< AGA_MAX_LIGHT)

uniform struct light_t {
	vec4 position;    // Camera space
	vec3 ambient;     // rgb
	vec3 diffuse;     // rgb
	vec3 specular;    // rgb
	vec3 attenuation; // (constant, lineal, quadratic)
	vec3 spotDir;     // Camera space
	float cosCutOff;  // cutOff cosine
	float exponent;
} theLights[4];     // AGA_MAX_LIGHTS

uniform struct material_t {
	vec3  ambient;
	vec3  diffuse;
	vec3  specular;
	float alpha;
	float shininess;
} theMaterial;

uniform sampler2D texture0;

varying vec3 f_positionEye;
varying vec3 f_viewDirection;
varying vec3 f_normal;
varying vec2 f_texCoord;

float diffuse_channel(const vec3 n,
					  const vec3 l) {
	return 1.0;
}

float specular_channel(const vec3 n,
					   const vec3 l,
					   const vec3 v,
					   float m) {

	return 1.0;
}

void direction_light(const in int i,
					 const in vec3 lightDirection,
					 const in vec3 viewDirection,
					 const in vec3 normal,
					 inout vec3 ambient, inout vec3 diffuse, inout vec3 specular) {

}

void point_light(const in int i,
				 const in vec3 lightDirection,
				 const in vec3 viewDirection,
				 const in vec3 normal,
				 inout vec3 ambient, inout vec3 diffuse, inout vec3 specular) {
}

void spot_light(const in int i,
				const in vec3 lightDirection,
				const in vec3 viewDirection,
				const in vec3 normal,
				inout vec3 ambient, inout vec3 diffuse, inout vec3 specular) {

}

void main() {

	gl_FragColor = vec4(1.0);
}
