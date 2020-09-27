varying vec4 mPosition;
varying vec4 mColor;
varying vec3 mNormal;

void main()
{
	vec3 normal = normalize(mNormal);
	mPosition.z = -1*mPosition.z;
	vec3 lightDir = normalize(gl_LightSource[0].position.xyz - mPosition.xyz);
	float intensity = dot(lightDir, normal);
	vec4 colorAdd;
	
	vec4 diffuse = gl_LightSource[0].diffuse.rgba;

	if(mColor.r > 0.75)
		mColor.r = 1.0;
	else if(mColor.r > 0.50)
		mColor.r = 0.75;
	else if(mColor.r > 0.25)
		mColor.r = 0.50;
	else
		mColor.r = 0.25;
		
	if(mColor.g > 0.75)
		mColor.g = 1.0;
	else if(mColor.g > 0.50)
		mColor.g = 0.75;
	else if(mColor.g > 0.25)
		mColor.g = 0.50;
	else
		mColor.g = 0.25;
		
	if(mColor.b > 0.75)
		mColor.b = 1.0;
	else if(mColor.b > 0.50)
		mColor.b = 0.75;
	else if(mColor.b > 0.25)
		mColor.b = 0.5;
	else
		mColor.b = 0.25;		

	if (intensity > 0.75)
		colorAdd = vec4(1.0,1.0,1.0,1.0);
	else if (intensity > 0.50)
		colorAdd = vec4(0.85,0.85,0.85,1.0);
	else if (intensity > 0.25)
		colorAdd = vec4(0.65,0.65,0.65,1.0);
	else
		colorAdd = vec4(0.50,0.50,0.50,1.0);
	
	gl_FragColor = mColor*colorAdd*diffuse;
}