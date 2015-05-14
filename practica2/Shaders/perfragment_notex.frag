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
		
		// Ambiente		 
		ambient = ambient + theLights[i].ambient;
		// Difusa
		float aportacion_diffuse = dot(normal,lightDirection);
		if (aportacion_diffuse > 0.0)
			diffuse = diffuse + theLights[i].diffuse*aportacion_diffuse;
		// Especular
		float f1 = dot(normal,lightDirection);
		if (f1 > 0.0) {
			vec3 r = (2*f1*normal)-lightDirection;
			float f2 = dot(r,viewDirection);
			if (f2 > 0.0) {
				float aportacion_specular = pow(f2,theMaterial.shininess);
				specular = specular + theLights[i].specular*aportacion_specular;
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
		float aportacion_diffuse = dot(normal,lightDirection);
		if (aportacion_diffuse > 0.0)
			diffuse = diffuse + theLights[i].diffuse*aportacion_diffuse;
		// Especular
		float f1 = dot(normal,lightDirection);
		if (f1 > 0.0) {
			vec3 r = (2*f1*normal)-lightDirection;
			float f2 = dot(r,viewDirection);
			if (f2 > 0.0) {
				float aportacion_specular = pow(f2,theMaterial.shininess);
				specular = specular + theLights[i].specular*aportacion_specular;
			}
		}
}

void spot_light(const in int i,
				const in vec3 lightDirection,
				const in vec3 viewDirection,
				const in vec3 normal,
				inout vec3 ambient, inout vec3 diffuse, inout vec3 specular) {
		
		float dot_spot = dot(-lightDirection,theLights[i].spotDir);
		float cspot = 0;
		
		if (dot_spot > theLights[i].cosCutOff)
			cspot = pow(max(0.0,dot_spot), theLights[i].exponent);
			
		// Ambiente
		ambient = ambient + theLights[i].ambient;
		// Difusa
		float aportacion_diffuse = dot(normal,lightDirection);
		if (aportacion_diffuse > 0.0)
			diffuse = diffuse + theLights[i].diffuse*aportacion_diffuse*cspot;
		// Especular
		float f1 = dot(normal,lightDirection);
		if (f1 > 0.0) {
			vec3 r = (2*f1*normal)-lightDirection;
			float f2 = dot(r,viewDirection);
			if (f2 > 0.0) {
				float aportacion_specular = pow(f2,theMaterial.shininess);
				specular = specular + theLights[i].specular*aportacion_specular*cspot;
			}
		}
}

void main() {

	vec3 L, v, n, A, D, S;
	vec3 f_normal_normalized, f_viewDirection_normalized;
	vec4 color;
	
	A = vec3(0.0);
	D = vec3(0.0, 0.0, 0.0);
	S = vec3(0.0); 

	f_normal_normalized = normalize(f_normal);
	f_viewDirection_normalized = normalize(f_viewDirection);
	for(int i=0; i < active_lights_n; ++i) {
		// direction light
	 	if(theLights[i].position.w == 0.0) {
			L = -normalize(theLights[i].position.xyz);
			direction_light(i, L, f_viewDirection_normalized, f_normal_normalized, A, D, S);
	 	} else {
			L = normalize(theLights[i].position.xyz - f_positionEye);
			// point light
			if (theLights[i].cosCutOff == 0.0) {
	 			point_light(i, L, f_viewDirection_normalized, f_normal_normalized, A, D, S);
	 		} else {
	 			spot_light(i, L, f_viewDirection_normalized, f_normal_normalized, A, D, S);
	 		}
	 	}
	}
	
	color = vec4(0.0, 0.0, 0.0, 1.0);
	color.xyz = A * theMaterial.ambient + D * theMaterial.diffuse + S * theMaterial.specular;
	
	gl_FragColor = color;

}
