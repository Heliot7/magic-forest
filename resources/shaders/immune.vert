uniform float size;
varying vec4 mPosition;
varying vec4 mColor;
varying vec3 mNormal;
varying vec2 mTexCoord;

void main(void)
{
	mPosition = gl_ModelViewProjectionMatrix*gl_Vertex;
	mColor = gl_Color;
	mNormal = gl_NormalMatrix*gl_Normal;
		
	vec4 v = vec4(gl_Vertex);
	v.xyz = v.xyz * (1 + size);
		
	gl_Position = gl_ModelViewProjectionMatrix * v;
	gl_FrontColor = gl_Color;
}
