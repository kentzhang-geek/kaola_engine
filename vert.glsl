#version 400
in vec3 vp;
uniform mat4 pvm;
uniform mat4 m;
uniform float timein;
out vec3 normal;
void main() {
	vec4 inpos = vec4(vp, 1.0);
	// 这里计算波幅
	float fctor = (sqrt(pow(inpos.x, 2.0) + pow(inpos.y, 2.0)) * 0.6 - timein);
	inpos.z = cos(fctor);
	// 根据偏导数算法向量
	vec3 px = vec3(1.0, 0.0, -sin(fctor) * inpos.x / fctor * 0.36);
	vec3 py = vec3(0.0, 1.0, -sin(fctor) * inpos.y / fctor * 0.36);
	normal = cross(px, py);

    gl_Position = pvm * m * inpos;
}

// #version 120
// attribute vec3 vp;
// uniform mat4 pvm;
// uniform mat4 m;
// uniform float timein;
// varying vec3 normal;
// void main() {
// 	vec4 inpos = vec4(vp, 1.0);
// 	// 这里计算波幅
// 	float fctor = (sqrt(pow(inpos.x, 2.0) + pow(inpos.y, 2.0)) * 0.6 - timein);
// 	inpos.z = cos(fctor);
// 	// 根据偏导数算法向量
// 	vec3 px = vec3(1.0, 0.0, -sin(fctor) * inpos.x / fctor * 0.36);
// 	vec3 py = vec3(0.0, 1.0, -sin(fctor) * inpos.y / fctor * 0.36);
// 	normal = cross(px, py);

//     gl_Position = pvm * m * inpos;
// }
