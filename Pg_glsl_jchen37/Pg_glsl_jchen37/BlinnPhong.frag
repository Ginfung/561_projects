varying vec3 normal, lightDir, eyeVec;

void main()
{
	vec3 N = normalize(normal);
	vec3 L = normalize(lightDir);
	
	float NL = dot(N, L);
	
	vec4 color = gl_LightSource[0].ambient * gl_FrontMaterial.ambient;
	
	if(NL > 0.0)
	{
		color += gl_LightSource[0].diffuse * gl_FrontMaterial.diffuse * NL;	
		
		vec3 V = normalize(eyeVec);
		vec3 R = reflect(-L, N);
		float specular = pow( max(dot(R, V), 0.0), gl_FrontMaterial.shininess );
		color += gl_LightSource[0].specular * gl_FrontMaterial.specular * specular;
	}
	
	gl_FragColor = color;
}
