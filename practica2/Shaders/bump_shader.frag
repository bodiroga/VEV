#version 120

uniform int active_lights_n; // Number of active lights (< AGA_MAX_LIGHT)

struct material_t {
  	vec3  ambient;
  	vec3  diffuse;
  	vec3  specular;
  	float alpha;
  	float shininess;
};

struct light_t {
  	vec4 position;    // Camera space
  	vec3 ambient;     // rgb
  	vec3 diffuse;     // rgb
  	vec3 specular;    // rgb
  	vec3 attenuation; // (constant, lineal, quadratic)
  	vec3 spotDir;     // Camera space
  	float cosCutOff;  // cutOff cosine
  	float exponent;
};

uniform light_t theLights[4];
uniform material_t theMaterial;

uniform sampler2D texture0; // the color texture
uniform sampler2D bumpmap;  // bump map

varying vec2 f_texCoord;
varying vec3 f_viewDirection;     // tangent space
varying vec3 f_lightDirection[4]; // tangent space
varying vec3 f_spotDirection[4];  // tangent space

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
