varying vec4 mPosition;
varying vec4 mColor;
varying vec3 mNormal;
varying vec2 mTexCoord;

void main()
{
	mPosition = gl_ModelViewProjectionMatrix*gl_Vertex;
	mColor = gl_Color;
	mNormal = gl_NormalMatrix*gl_Normal;
	mTexCoord = gl_MultiTexCoord0.xy;
	
	gl_Position = ftransform();
	gl_FrontColor = gl_Color;
	
} 
