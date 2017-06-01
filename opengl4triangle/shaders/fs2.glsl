#version 400

//What is read in to the shader
in vec2 UV;
in vec3 lightDirCamSpace;
in vec3 eyeDirCamSpace;
in vec3 posWorldSpace;
in vec3 normCamSpace;

//What is given out from the shader
out vec3 fragColor;

//what stays the same in the shader
uniform vec3 lightPosWorld;
uniform sampler2D texture;


void main()
{
	vec3 lightCol = vec3(1.0, 1.0, 1.0);
	float lightPow = 500.0f;
	float lDist = length(lightPosWorld - posWorldSpace);
	
	vec3 material_diffuse = texture2D(texture, UV).rgb;
	vec3 material_ambient = vec3(0.1, 0.1, 0.1) * material_diffuse;
	vec3 material_specular = vec3(0.3,0.3,0.3);
	
	vec3 ncf = normalize(normCamSpace);
	vec3 ldf = normalize(lightDirCamSpace);
	//angle between norm & light dir
	float ct = clamp( dot(ncf,ldf), 0, 1); 
	
	vec3 eye = normalize(eyeDirCamSpace);
	vec3 ref = reflect(-ldf, ncf);
	//angle between eye vec & spec reflection
	float ca = clamp( dot (eye, ref), 0, 1); 
	
	fragColor = material_ambient + material_diffuse * lightCol * lightPow * ct / 	(lDist*lDist) + material_specular * lightCol * lightPow * pow(ca, 5) / (lDist*lDist);
}