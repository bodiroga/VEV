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
		
		// Ambiente		 
		ambient += theLights[i].ambient;
		// Difusa
		float aportacion_diffuse = max(0.0, dot(lightDirection,normal));
		diffuse += theLights[i].diffuse*aportacion_diffuse;
		// Especular
		float f1 = dot(normal,lightDirection);
		if (f1 > 0.0) {
			vec3 r = (2*f1*normal)-lightDirection;
			float f2 = dot(r,viewDirection);
			if (f2 > 0.0) {
				float aportacion_specular = max(0.0, pow(f2,theMaterial.shininess));
				specular += theLights[i].specular*aportacion_specular;
			}
		}
}

void point_light(const in int i,
				 const in vec3 lightDirection,
				 const in vec3 viewDirection,
				 const in vec3 normal,
				 inout vec3 ambient, inout vec3 diffuse, inout vec3 specular) {
		
		// Ambiente	 
		ambient = ambient + theLights[i].ambient;
		// Difusa
		float aportacion_diffuse = max(0.0, dot(lightDirection,normal));
		diffuse += theLights[i].diffuse*aportacion_diffuse;
		// Especular
		float f1 = dot(normal,lightDirection);
		if (f1 > 0.0) {
			vec3 r = (2*f1*normal)-lightDirection;
			float f2 = dot(r,viewDirection);
			if (f2 > 0.0) {
				float aportacion_specular = max(0.0, pow(f2,theMaterial.shininess));
				specular += theLights[i].specular*aportacion_specular;
			}
		}
}

void spot_light(const in int i,
				const in vec3 lightDirection,
				const in vec3 viewDirection,
				const in vec3 normal,
				inout vec3 ambient, inout vec3 diffuse, inout vec3 specular) {
		
		// Ambiente
		ambient += theLights[i].ambient;
		// Difusa
		float aportacion_diffuse = max(0.0, dot(lightDirection,normal));
		diffuse += theLights[i].diffuse*aportacion_diffuse;
		// Especular
		float f1 = dot(normal,lightDirection);
		if (f1 > 0.0) {
			vec3 r = (2*f1*normal)-lightDirection;
			float f2 = dot(r,viewDirection);
			if (f2 > 0.0) {
				float aportacion_specular = max(0.0, pow(f2,theMaterial.shininess));
				specular += theLights[i].specular*aportacion_specular;
			}
		}
}

void main() {

	vec3 L, v, n, A, D, S;
	vec3 pEye, nEye;
	
	A = vec3(0.0);
	D = vec3(0.0, 0.0, 0.0);
	S = vec3(0.0); 

	// pEye: posición del vértice en el sistema de la cámara
	// nEye: normal del vértice en el sistema de la cámara
	pEye = (modelToCameraMatrix * vec4(v_position, 1.0)).xyz;
	nEye = normalize((modelToCameraMatrix * vec4(v_normal, 0.0)).xyz);
	
	// Direccional         ->     L = -PL.xyz / ||PL.xyz||
	// Posicional y spot   ->     L = (PL - Vp(Eye)) / ||PL - Vp(Eye)||

	for(int i=0; i < active_lights_n; ++i) {
	 	if(theLights[i].position.w == 0.0) { // direction light
	 		L = -normalize(theLights[i].position.xyz);
	 		v =  -pEye;
	 	  	direction_light(i, L, v, nEye, A, D, S);
	 	} else {
	 	  if (theLights[i].cosCutOff == 0.0) { // point light
	 	  		L = normalize(theLights[i].position.xyz - pEye);
	 	  		v =  -pEye;
	 			point_light(i, L, v, nEye, A, D, S);
	 	  } else { // spot light
	 	  		 L = normalize(theLights[i].position.xyz - pEye);
	 	  		 v =  -pEye;
	 			 spot_light(i, L, v, nEye, A, D, S);
	 	  }
	 	}
	 }
	
	f_color = vec4(0.0, 0.0, 0.0, 1.0);
	f_color.xyz = A * theMaterial.ambient + D * theMaterial.diffuse + S * theMaterial.specular;
	
	gl_Position = modelToClipMatrix * vec4(v_position, 1);
}
