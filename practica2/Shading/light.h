#ifndef LIGHT_H
#define LIGHT_H

#define AGA_DIRECTIONAL         1
#define AGA_POSITIONAL          2
#define AGA_SPOT                3

typedef struct _Light {

	// The default values that are listed here
	// are those defined by OpenGL
	// Our Default Light has different values

	int      id;                    // Light unique id
	int      type;                  // AGA_DIRECTIONAL, AGA_POSITIONAL, AGA_SPOT (default AGA_DIRECTIONAL)
	int      twoside;               // 0 only front faces  are shaded (those which look towards the light)
									// 1 both front & back faces are shaded
									// Default 0
	int      switched;              // TRUE => ON (default ON)
	float    ambient[3];            // default (0.2, 0.2, 0.2)
	float    diffuse[3];            // default (0.8, 0.8, 0.8)
	float    specular[3];           // default (0.0, 0.0, 0.0)
	float    position[4];           // default (0.0 ,5.0, 0.0, 1.0);
									// Note that, if the fourth coordinate is 0.0, the light is at infinity (directional)
									// And the first 3 parameters denote a direction vector
	float    spotDirection[3];      // GL_SPOT_DIRECTION : default direction is (sqrt(3)/3, sqrt(3)/3, sqrt(3)/3)
									//  significant only when GL_SPOT_CUTOFF is not 180
	float    positionEye[4];        // Light position in eye coordinates
	float    spotDirectionEye[4];   // GL_SPOT_DIRECTION : default direction is (sqrt(3)/3, sqrt(3)/3, sqrt(3)/3)
	float    spotExponent;          // GL_SPOT_EXPONENT [0,128], default 10
									//  0 => uniform light distribution
									//  higher spot => more focused light source,
	float    spotCutOff;            // GL_SPOT_CUTOFF [0,90] & 180, default 30
									//  180 => uniform light distribution
	float    att[3];                // [GL_CONSTANT_ATTENUATION, GL_LINEAR_ATTENUATION, GL_QUADRATIC_ATTENUATION]
	int      needsUpdate;           // internal use.

} Light;

void   DestroyLight( Light **theLight );

/**
 * Create a light with default values
 *
 * @return new light
 */

Light *CreateVoidLight();

/**
 *
 * Set light spot
 *
 * @param direction point to a 3-valued vector representing spot direction
 * @param cutOff the cutoff value in degrees
 * @param exponent the exponent
 *
 */
void SetSpotLight(Light *theLight, float *direction, float cutOff, float exponent);

/**
 * Set light ambient channel
 *
 * @param thisLight the light
 * @param rgb point to a 3-vector with rgb values
 */
void SetAmbientLight(Light *thisLight, float *rgb);

/**
 * Get light ambient channel
 *
 * @param thisLight the light
 * @return a pointer to a 3-vector with rgb values
 */
float *GetAmbientLight(Light *thisLight);

/**
 * Set light diffuse channel
 *
 * @param thisLight the light
 * @param rgb point to a 3-vector with rgb values
 */
void SetDiffuseLight(Light *thisLight, float *rgb);

/**
 * Get light diffuse channel
 *
 * @param thisLight the light
 * @return a pointer to a 3-vector with rgb values
 */
float *GetDiffuseLight(Light *thisLight);

/**
 * Set light specular channel
 *
 * @param thisLight the light
 * @param rgb point to a 3-vector with rgb values
 */
void SetSpecularLight(Light *thisLight, float *rgb);

/**
 * Get light specular channel
 *
 * @param thisLight the light
 * @return a pointer to a 3-vector with rgb values
 */
float *GetSpecularLight(Light *thisLight);

/**
 * Set light position
 *
 * @param thisLight the light
 * @param pos point to a 4-vector indicating position
 * @note position is multiplied by the current MODELVIEW matrix
 */
void SetPositionLight(Light *thisLight, float *pos);

/**
 * Get light position in Eye coordinates
 *
 * @param thisLight the light
 * @return a pointer to a 4-vector with position in Eye coordinates
 */
float *GetPositionLight(Light *thisLight);


/**
 * return 1 if light is spotlight, zero otherwise
 *
 * @param thisLight the light
 * @return 1 if light is spotlight, zero otherwise
 */
int IsSpotLight(Light *thisLight);

/**
 * Get spotlight direction in Eye coordinates
 *
 * @param thisLight the light
 * @return a pointer to a 3-vector with position in Eye coordinates or the vector (0,0,0) if light is not spotlight.
 */
float *GetSpotDirectionLight(Light *thisLight);

/**
 * Get exponent of SpotLight
 *
 * @param thisLight the Light
 *
 * @return the exponent or 0.0 if light is not spotlight
 */
float GetSpotExponentLight(Light *thisLight);

/**
 * Get cutoff factor of SpotLight
 *
 * @param thisLight the Light
 *
 * @return the cuttoff factor or 0.0 if light is not spotlight
 */
float GetSpotCutoffLight(Light *thisLight);


/**
 * Set quadratic attenuation
 *
 * @param thisLight the light
 * @param a quadratic factor
 */
void SetQuadraticAttenuationLight(Light* thisLight, float a);

/**
 * Get quadratic attenuation
 *
 * @param thisLight the light
 * @return a quadratic factor
 */
float GetQuadraticAttenuationLight(Light* thisLight);

/**
 * Set linear attenuation
 *
 * @param thisLight the light
 * @param b linear factor
 */
void SetLinearAttenuationLight(Light* thisLight, float b);

/**
 * Get linear attenuation
 *
 * @param thisLight the light
 * @return b linear factor
 */
float GetLinearAttenuationLight(Light* thisLight);

/**
 * Set constant attenuation
 *
 * @param thisLight the light
 * @param c constant factor
 */
void SetConstantAttenuationLight(Light* thisLight, float c);

/**
 * Get constant attenuation
 *
 * @param thisLight the light
 * @return c constant factor
 */
float GetConstantAttenuationLight(Light* thisLight);

/**
 * Get a vector of 3 elements with attenuations (constant, lineal, quadratic)
 *
 * @param thisLight the light
 * @return a 3-valued vector with constant, lineal and quadratic attenuation factors.
 */
float *GetAttenuationVectorLight(Light* thisLight);

/**
 * Put light into scene (multiply position and possibly spot direction with modelview)
 *
 * @param thisLight the light
 */
void PlaceSceneLight( Light *thisLight );

/**
 * Turn the light on/off according to OpenGL
 *
 * @param thisLight the light
 * @param status on(1)/off(o)
 */
void   SwitchLight( Light *thisLight, int status );

/**
 * return one if light is on, zero otherwise
 *
 * @param thisLight the light
 * @return 1 if light is on, 0 otherwise
 */
int IsOnLight(Light *thisLight);

/**
 * Set light two-sided
 *
 * @param thisLight the light
 * @param twoside twoside yes(1)/no(0)
 */
void LightTwoSides( Light * thisLight, int twoside);

void   PrintLight( Light *thisLight );

#endif /* LIGHT_H */
