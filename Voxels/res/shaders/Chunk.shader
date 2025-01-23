#shader vertex
#version 330 core

//layout(location = 0) in vec4 position;
layout(location = 0) in int positionPacked;
layout(location = 1) in vec2 uv;
layout(location = 2) in int color;

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform vec3 positionOffset;

out vec2 v_uv;
out vec4 v_color;

void main ()
{
	int bitMaskPosition = 511;
	vec3 position = vec3(float((positionPacked >> 0) & bitMaskPosition), float((positionPacked >> 10) & bitMaskPosition), float((positionPacked >> 20) & bitMaskPosition));
	gl_Position = projectionMatrix * viewMatrix * vec4(position.x + positionOffset.x, position.y + positionOffset.y, position.z + positionOffset.z, 1); //  /*  projectionMatrix * transformationMatrix *  */ vec4(position.x, position.y, position.z, 1);
	//gl_Position =  vec4(position.x, position.y, position.z, 1) * viewMatrix * projectionMatrix; //  /*  projectionMatrix * transformationMatrix *  */ vec4(position.x, position.y, position.z, 1);

	v_uv = uv;
	float r = float((color >>  0) & 255);
	float g = float((color >>  8) & 255);
	float b = float((color >> 16) & 255);
	// im gonna assume 1 on the alpha

	v_color = vec4(r / 255, g / 255, b / 255, 1);
};

#shader fragment
#version 330 core

layout(location = 0) out vec4 color;

in vec2 v_uv;
in vec4 v_color;

uniform sampler2D u_Texture;
uniform float texWidth;
uniform float texHeight;


void main ()
{
	//vec4 texColor = texture(u_Texture, v_uv);
	vec4 texColor = texelFetch(u_Texture, ivec2(v_uv.x * texWidth, v_uv.y * texHeight), 0);
	//color = vec4(v_uv.x, v_uv.y, 0, 1) * v_color; // for checking uvs
	color = texColor * v_color;
	//color = vec4(0, 1, 0, 1); // make sure something is actually displayed
};