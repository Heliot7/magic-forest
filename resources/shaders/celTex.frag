varying vec4 mPosition;
varying vec4 mColor;
varying vec3 mNormal;
varying vec2 mTexCoord;
sampler2D mTexture;

void main()
{
	vec3 normal = normalize(mNormal);
	mPosition.z = -1*mPosition.z;
	vec3 lightDir = normalize(gl_LightSource[0].position.xyz - mPosition.xyz);
	float intensity = dot(lightDir, normal);
	vec4 colorAdd;
	
	vec4 diffuse = gl_LightSource[0].diffuse.rgba;
	vec4 colorTex = texture2D(mTexture, mTexCoord).rgba;
	
	if(colorTex.r > 0.75)
		colorTex.r = 1.0;
	else if(colorTex.r > 0.50)
		colorTex.r = 0.75;
	else if(colorTex.r > 0.25)
		colorTex.r = 0.50;
	else
		colorTex.r = 0.25;
		
	if(colorTex.g > 0.75)
		colorTex.g = 1.0;
	else if(colorTex.g > 0.50)
		colorTex.g = 0.75;
	else if(colorTex.g > 0.25)
		colorTex.g = 0.50;
	else
		colorTex.g = 0.25;
		
	if(colorTex.b > 0.75)
		colorTex.b = 1.0;
	else if(colorTex.b > 0.50)
		colorTex.b = 0.75;
	else if(colorTex.b > 0.25)
		colorTex.b = 0.5;
	else
		colorTex.b = 0.25;		

	if (intensity > 0.75)
		colorAdd = vec4(1.0,1.0,1.0,1.0);
	else if (intensity > 0.50)
		colorAdd = vec4(0.85,0.85,0.85,1.0);
	else if (intensity > 0.25)
		colorAdd = vec4(0.65,0.65,0.65,1.0);
	else
		colorAdd = vec4(0.50,0.50,0.50,1.0);
	
	gl_FragColor = colorTex*colorAdd*diffuse;
}