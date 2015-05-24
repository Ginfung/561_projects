varying float ang;

void main()
{
	vec4 color;
	if (ang > 0.95)
		color = vec4(0.8,0.8,0.1,1.0);
	else if (ang > 0.4)
		color = vec4(0.8,0.6,0.1,1.0);
	else if (ang > 0.3)
		color = vec4(0.8,0.4,0.1,1.0);
	else
		color = vec4(0.8,0.2,0.1,1.0);
	gl_FragColor = color;
}