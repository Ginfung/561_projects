varying float ang;
uniform vec3 eyePos;

void main()
{
	vec3 lightDir = normalize(vec3(gl_LightSource[0].position));
	ang = dot(lightDir,gl_Normal);

	gl_Position = ftransform();
}
