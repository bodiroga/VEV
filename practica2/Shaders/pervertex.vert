#version 120

uniform mat4 modelToCameraMatrix;
uniform mat4 cameraToClipMatrix;
uniform mat4 modelToClipMatrix;

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

attribute vec3 v_position;
attribute vec3 v_normal;
attribute vec2 v_texCoord;

varying vec4 f_color;
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

	// for(int i=0; i < active_lights_n; ++i) {
	// 	if(theLights[i].position.w == 0.0) {
	// 	  // direction light
	// 	} else {
	// 	  if (theLights[i].cosCutOff == 0.0) {
	// 		// point light
	// 	  } else {
	// 		// spot light
	// 	  }
	// 	}
	// }

	f_color = vec4(1, 1, 1, 1);
	gl_Position = modelToClipMatrix * vec4(v_position, 1);
}
