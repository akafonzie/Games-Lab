#version 400

layout (location = 0) in vec3 vertexPos;
layout (location = 1) in vec3 vertexNorm;
layout (location = 2) in vec2 vertexUV;

uniform mat4 ModelView;
uniform mat4 View;
uniform mat4 Projection;
uniform vec3 lightPosWorld;

out vec3 fragmentColor;
out vec2 UV;
out vec3 lightDirCamSpace;
out vec3 eyeDirCamSpace;
out vec3 posWorldSpace;
out vec3 normCamSpace;



void main()
{

	gl_Position =  Projection * View* ModelView * vec4(vertexPos,1);	
	posWorldSpace = (ModelView*vec4(vertexPos, 1)).xyz;

	vec3 vertPosCamSpace =  ( View * ModelView * vec4(vertexPos, 1)).xyz;
	eyeDirCamSpace = vec3(0,0,0) - vertPosCamSpace;
	
	vec3 lightPosCamSpace = (View*vec4(lightPosWorld, 1)).xyz;
	lightDirCamSpace = lightPosCamSpace + eyeDirCamSpace;

	normCamSpace = (View*ModelView*vec4(vertexNorm,0)).xyz;	
	UV = vertexUV;
}