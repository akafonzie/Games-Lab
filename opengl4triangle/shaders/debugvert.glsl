//************************************************//
//   Implemented by Alice                         //
//   Vertex shader for displaying  points or      //   
//	 lines.         							  //
//************************************************//

#version 400


layout (location = 0) in vec3 vertexPos;

uniform mat4 Projection;
uniform mat4 View;

void main(void)
{		
	gl_Position = Projection * View * vec4(vertexPos, 1.0);
	//gl_Position = vec4(vertexPos, 1.0);

}