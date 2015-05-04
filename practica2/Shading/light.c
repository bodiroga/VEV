#include "tools.h"
#include "light.h"
#include "scene.h"

// forward declarations

void DestroyLight( Light **theLight ) {
	Light   *thisLight = *theLight;
	if( ! thisLight ) return;

	free( thisLight );
	*theLight = NULL;
}


Light *CreateVoidLight() {
	Light       *newLight;
	RenderState *rs;

	newLight = malloc( sizeof(*newLight) * 1 );

	// directional light
	newLight->type                 = AGA_POSITIONAL;
	newLight->switched             = TRUE;
	newLight->twoside              = FALSE;

	newLight->ambient[0]           =  0.2f;
	newLight->ambient[1]           =  0.2f;
	newLight->ambient[2]           =  0.2f;

	newLight->diffuse[0]           =  0.8f;
	newLight->diffuse[1]           =  0.8f;
	newLight->diffuse[2]           =  0.8f;

	newLight->specular[0]          =  0.0f;
	newLight->specular[1]          =  0.0f;
	newLight->specular[2]          =  0.0f;

	newLight->position[0]          =  0.0f;
	newLight->position[1]          =  5.0f;        // at "y" axis
	newLight->position[2]          =  0.0f;
	newLight->position[3]          =  1.0f;        // positional

	newLight->positionEye[0]       =  0.0f;
	newLight->positionEye[1]       =  5.0f;
	newLight->positionEye[2]       =  0.0f;
	newLight->positionEye[3]       =  1.0f;

	newLight->spotDirection[0]     = 0.577; // sqtr(3)/3
	newLight->spotDirection[1]     = 0.577;
	newLight->spotDirection[2]     = 0.577;

	newLight->spotDirectionEye[0]  = 0.577; // sqtr(3)/3
	newLight->spotDirectionEye[1]  = 0.577;
	newLight->spotDirectionEye[2]  = 0.577;

	newLight->spotExponent         = 10.0f;
	newLight->spotCutOff           = 30.0f;        // must be degrees

	newLight->att[2] = 0.0f;                    // GL_QUADRATIC_ATTENUATION
	newLight->att[1] = 0.0f;                    // GL_LINEAR_ATTENUATION
	newLight->att[0] = 1.0f;                    // GL_CONSTANT_ATTENUATION

	newLight->needsUpdate = TRUE;

	// Put the light into render state
	rs = RenderStateScene();
	AddLightRS(rs, newLight);
	SwitchLight(newLight, 1);

	return newLight;
}


void SetSpotLight(Light *theLight,
				  float *direction, float cutOff, float exponent) {

	theLight->type = AGA_SPOT;
	theLight->spotDirection[0] = direction[0];
	theLight->spotDirection[1] = direction[1];
	theLight->spotDirection[2] = direction[2];
	theLight->spotDirectionEye[0] = direction[0];
	theLight->spotDirectionEye[1] = direction[1];
	theLight->spotDirectionEye[2] = direction[2];
	VectorNormalize(&theLight->spotDirectionEye[0], &theLight->spotDirectionEye[1], &theLight->spotDirectionEye[2]);
	theLight->spotCutOff = DEGREE_TO_RAD * cutOff;
	theLight->spotExponent = exponent;
	theLight->needsUpdate = TRUE;

}


void SetAmbientLight(Light *thisLight, float *rgb) {

	thisLight->ambient[0]           =  rgb[0];
	thisLight->ambient[1]           =  rgb[1];
	thisLight->ambient[2]           =  rgb[2];
	thisLight->needsUpdate = TRUE;
}

float *GetAmbientLight(Light *thisLight) {
	return &(thisLight->ambient[0]);
}


void SetDiffuseLight(Light *thisLight, float *rgb) {

	thisLight->diffuse[0]           =  rgb[0];
	thisLight->diffuse[1]           =  rgb[1];
	thisLight->diffuse[2]           =  rgb[2];
	thisLight->needsUpdate = TRUE;
};

float *GetDiffuseLight(Light *thisLight) {
	return &(thisLight->diffuse[0]);
}

void SetSpecularLight(Light *thisLight, float *rgb) {
	thisLight->specular[0]           =  rgb[0];
	thisLight->specular[1]           =  rgb[1];
	thisLight->specular[2]           =  rgb[2];
	thisLight->needsUpdate = TRUE;
};

float *GetSpecularLight(Light *thisLight) {
	return &(thisLight->specular[0]);
}

void SetPositionLight(Light *thisLight, float *pos) {

	thisLight->position[0]           =  pos[0];
	thisLight->position[1]           =  pos[1];
	thisLight->position[2]           =  pos[2];

	thisLight->positionEye[0] = thisLight->position[0];
	thisLight->positionEye[1] = thisLight->position[1];
	thisLight->positionEye[2] = thisLight->position[2];

	if (thisLight->type == AGA_SPOT) {
		thisLight->position[3]    = 1;
		thisLight->positionEye[3] = 1;
		return;
	}

	thisLight->position[3]    = pos[3];
	thisLight->positionEye[3] = pos[3];

	if (thisLight->position[3] == 0.0) {
		thisLight->type = AGA_DIRECTIONAL;
		// Normalize vector
		VectorNormalize(&thisLight->position[0], &thisLight->position[1], &thisLight->position[2]);
		VectorNormalize(&thisLight->positionEye[0], &thisLight->positionEye[1], &thisLight->positionEye[2]);
	} else {
		thisLight->type = AGA_POSITIONAL;
	}
}

// get position in Eye coordinates!

float *GetPositionLight(Light *thisLight) {
	return &(thisLight->positionEye[0]);
}

int IsSpotLight(Light *thisLight) {
	return thisLight->type == AGA_SPOT;
}

// get Spot direction in Eye coordinates!

float *GetSpotDirectionLight(Light *thisLight) {
	static float nullDir[3] = { 0.0f, 0.0f, 0.0f};
	if (thisLight->type != AGA_SPOT)
		return &nullDir[0];
	return &(thisLight->spotDirectionEye[0]);
}

float GetSpotExponentLight(Light *thisLight) {
	if (thisLight->type != AGA_SPOT)
		return 0.0f;
	return thisLight->spotExponent;
}

float GetSpotCutoffLight(Light *thisLight) {
	if (thisLight->type != AGA_SPOT)
		return 0.0f;
	return thisLight->spotCutOff;
}

void SetLinearAttenuationLight(Light* thisLight, float b) {

	thisLight->att[1] = b;
	VectorSumOne(&thisLight->att[2], &thisLight->att[1], &thisLight->att[0]);

	thisLight->needsUpdate = TRUE;
}

float GetLinearAttenuationLight(Light* thisLight) {
	return thisLight->att[1];
}

void SetQuadraticAttenuationLight(Light* thisLight, float a) {


	thisLight->att[2] = a;
	VectorSumOne(&thisLight->att[2], &thisLight->att[1], &thisLight->att[0]);

	thisLight->needsUpdate = TRUE;
}

float GetQuadraticAttenuationLight(Light* thisLight) {
	return thisLight->att[2];
}


void SetConstantAttenuationLight(Light* thisLight, float c) {

	thisLight->att[0] = c;
	VectorSumOne(&thisLight->att[2], &thisLight->att[1], &thisLight->att[0]);

	thisLight->needsUpdate = TRUE;
}

float GetConstantAttenuationLight(Light* thisLight) {
	return   thisLight->att[0];
}

float *GetAttenuationVectorLight(Light* thisLight) {
	return &thisLight->att[0];
}

void SwitchLight( Light *thisLight, int status ) {
	thisLight->switched = (status != 0);
}

int IsOnLight(Light *thisLight) {
	return thisLight->switched;
}

/**
 * @@ TODO: Place the light into the scene. Store the result into positionEye and spotDirectionEye.
 *
 * hints:
 *        - multiply position with current modelView matrix
 */

void PlaceSceneLight( Light *thisLight ) {

	RenderState      *rs;
	trfm3D           *modelView;

	if( ! thisLight ) return;

	if ( thisLight->switched == 1 ) return;

	rs = RenderStateScene();
	modelView = TopRS(rs, AGA_MODELVIEW);

	thisLight->positionEye[0] = thisLight->position[0];
	thisLight->positionEye[1] = thisLight->position[1];
	thisLight->positionEye[2] = thisLight->position[2];

	if (thisLight->type == AGA_DIRECTIONAL) {
		TransformVectorTrfm3D(modelView, &thisLight->positionEye[0], &thisLight->positionEye[1], &thisLight->positionEye[2]);
	} else {
		Trfm3DTransformPoint(modelView, &thisLight->positionEye[0], &thisLight->positionEye[1], &thisLight->positionEye[2]);
	}

	// La dirección sólo se cambia en la spot
	if (thisLight->type == AGA_SPOT) {
		thisLight->spotDirectionEye[0] = thisLight->spotDirection[0];
		thisLight->spotDirectionEye[1] = thisLight->spotDirection[1];
		thisLight->spotDirectionEye[2] = thisLight->spotDirection[2];
		TransformVectorTrfm3D(modelView, &thisLight->spotDirectionEye[0], &thisLight->spotDirectionEye[1], &thisLight->spotDirectionEye[2]);
	}

}

void PrintLight( Light *thisLight ) {

	if( ! thisLight ) {
		fprintf(stderr, "[E] NULL Light pointer\n");
		return;
	}

	if( thisLight->type == AGA_DIRECTIONAL ) {
		printf("*** Directional Light");
	}
	else if( thisLight->type == AGA_POSITIONAL ) {
		printf("*** Positional Light");
	}
	else {
		printf("*** Spot Light");
	}

	if( thisLight->switched )
		printf(" Switched ON\n" );
	else
		printf(" Switched OFF\n");

	printf(" Ambient  %04.2f %04.2f %04.2f\n",
		   thisLight->ambient[0], thisLight->ambient[1],
		   thisLight->ambient[2]);
	printf(" Diffuse  %04.2f %04.2f %04.2f\n",
		   thisLight->diffuse[0], thisLight->diffuse[1],
		   thisLight->diffuse[2]);
	printf(" Specular %04.2f %04.2f %04.2f\n",
		   thisLight->specular[0], thisLight->specular[1],
		   thisLight->specular[2]);

	if( thisLight->type == AGA_DIRECTIONAL ) {
		printf(" Direction         ");
	} else {
		printf(" Position          ");
	}
	printf("%.2f %.2f %4.2f ; %04.2f\n",
		   thisLight->position[0], thisLight->position[1],
		   thisLight->position[2], thisLight->position[3] );

	if( thisLight->type == AGA_DIRECTIONAL ) {
		printf(" Transformed direction ");
	} else {
		printf(" Transformed position  ");
	}
	printf("%.2f %.2f %4.2f ; %04.2f\n",
		   thisLight->positionEye[0], thisLight->positionEye[1],
		   thisLight->positionEye[2], thisLight->positionEye[3] );

	if( thisLight->type == AGA_SPOT ) {
		printf(" Spot direction    %04.2f %04.2f %04.2f (mod: %4.2f)\n",
			   thisLight->spotDirection[0], thisLight->spotDirection[1],
			   thisLight->spotDirection[2],
			   AGA_MOD(thisLight->spotDirection[0], thisLight->spotDirection[1], thisLight->spotDirection[2]));
		printf(" Transformed spot direction    %04.2f %04.2f %04.2f (mod: %4.2f)\n",
			   thisLight->spotDirectionEye[0], thisLight->spotDirectionEye[1],
			   thisLight->spotDirectionEye[2],
			   AGA_MOD(thisLight->spotDirectionEye[0], thisLight->spotDirectionEye[1], thisLight->spotDirectionEye[2]));
		printf(" Exponent %6.1f Cut Off %4.1f\n",
			   thisLight->spotExponent, thisLight->spotCutOff );
	}

	printf(" Attenuation : Quad=%f Linear=%f Constant=%f\n",
		   thisLight->att[2], thisLight->att[1], thisLight->att[0] );
}
