#version 120

uniform mat4 modelToCameraMatrix;
uniform mat4 cameraToClipMatrix;
uniform mat4 modelToClipMatrix;

attribute vec4 v_position;

varying vec3 f_texCoord;

void main() {

	gl_Position = modelToClipMatrix * v_position;
	f_texCoord = v_position.xyz;
}
