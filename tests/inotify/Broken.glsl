void mainImage(out vec4 fragColor, in vec2 fragCoord) {
	vec2 uv = fragCoord / iResolution.xy;

	float t = iGlobalTime;
	mat2 m = mat2(
		cos(t), sin(t),
		-sin(t), cos(t)
	);

	fragColor = vec4(uv * m, pow(sin(t), 2), 1);
//}
