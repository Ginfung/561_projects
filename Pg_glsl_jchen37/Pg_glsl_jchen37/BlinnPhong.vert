varying vec3 normal, lightDir, eyeVec;
uniform vec3 eyePos;

void main()
{	
	normal = normalize(gl_NormalMatrix * gl_Normal);
	vec3 vVertex = vec3(gl_ModelViewMatrix * gl_Vertex);
	lightDir = normalize(vec3(gl_LightSource[0].position.xyz - vVertex));
	eyeVec = -vVertex;
		
	gl_Position = ftransform();
}
