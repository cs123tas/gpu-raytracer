#version 430 core
#define MAX_VAL  1000000000
#define PI 3.1415926535897932384626433832795
#define PLANE   0
#define SPHERE  1

in vec2 texCoord;

uniform sampler2D tex;
uniform mat4 M_film2World;
uniform vec4 eye;
uniform int depth;
uniform int width;
uniform int height;
uniform float time;
uniform vec2 dimensions;

out vec4 fragColor;

struct Ray {
	vec4 P; 
	vec4 d;
};

struct SurfaceElement {
	float t;
	vec4 intersection; // world space intersect
	vec4 normal;
	
	vec4 diffuseColor;
	vec4 specularColor;
	vec4 reflectedColor;
	vec4 tranparencyColor;
};


struct LightData {
	int type;
	vec4 color;
};


// TODO: shadows
int checkOcclusions(inout SurfaceElement surfel, inout LightData lightData) {

	return 0;
}


// TODO: reflections and refractions
vec4 estimateIndirectLight(inout SurfaceElement surfel, inout Ray ray) {

	return vec4(0.f);
}


// TODO: diffuse and specular
vec4 estimateDirectLight(inout SurfaceElement surfel, inout Ray ray) {
	
	vec4 kdOd = surfel.diffuseColor;
	vec4 ksOs = surfel.specularColor;

	return vec4(0.f);

}


vec2 getQuadradicRoots(float A, float B, float C) {
	float det = B*B - 4*A*C;
	
	if (det > 0) {
		float t1 = (-B + sqrt(det))/(2.f*A);
		float t2 = (-B - sqrt(det))/(2.f*A);

		return vec2(t1, t2);
	}
	return vec2(MAX_VAL, MAX_VAL);
}


float sphereRayIntersect(inout Ray ray, inout SurfaceElement surfel) {
	vec4 P = ray.P;
	vec4 d = ray.d;

	float A = pow(d.x, 2.f) + pow(d.y, 2.f) + pow(d.z, 2.f);
	float B = 2.f*(P.x*d.x + P.y*d.y + P.z*d.z);
	float C = pow(P.x, 2.f) + pow(P.y, 2.f) + pow(P.z, 2.f) - 0.25;

	vec2 tVals = getQuadradicRoots(A, B, C);

	return min(tVals.x, tVals.y);
}


// TODO: how to represent the scene, I'm thinking hardcoded SDF like lab 10
bool intersect(inout Ray ray, inout SurfaceElement surfel) {
	
//	// TODO: remove, simple sphere ray intersect test
//	float t = sphereRayIntersect(ray);
//	if (t < MAX_VAL) {
//		// TODO: paint surfel here
//		return 1;
//	}
//	return 0;

	float t = sphereRayIntersect(ray, surfel);
	if (t < MAX_VAL) {
		return true;
	}
	return false;

}


// inout is how you pass by ref in glsl
vec4 traceRay(inout Ray ray, int depth) {

	SurfaceElement surfel;
	bool isIntersect = intersect(ray, surfel);

	vec4 radiance = vec4(0.f, 0.f, 0.f, 1.f);
	// TODO: restore when ready
//	if (isIntersect == 1) {
//		radiance = estimateDirectLight(surfel, ray) + estimateIndirectLight(surfel, ray);
//	}

	if (isIntersect) {
		radiance = vec4(1.f, 0.f, 0.f, 1.f); // red sphere for now
	} 
	
	return radiance;
};



// TODO: Use timer for animation
void main() {
//	float x = ((2.f*float(gl_FragCoord.x))/dimensions.x) - 1.f;
//	float y = 1.f - ((2.f*float(gl_FragCoord.y))/dimensions.y);
	
	float x = gl_FragCoord.x;
	float y = gl_FragCoord.y;

	vec4 pt_film = vec4(x, y, 0.f, 1.f);
	vec4 pt_world = M_film2World*pt_film;

	vec4 d = normalize(pt_world - eye);
	vec4 P = eye;

	// TODO: restore
	Ray ray;
	ray.P = P;
	ray.d = d;
	fragColor = traceRay(ray, depth);
//	fragColor = vec4(1.f, 0.f, 0.f, 1.f);
	
//	if (int(gl_FragCoord.x) > 25) {
//		fragColor = vec4(1.f, 0.f, 0.f, 1.f);
//	} else {
//		fragColor = vec4(0.f, 1.f, 0.f, 1.f);
//	}

	// pixel = vec4(1.f, 0.f, 0.f, 1.f);

	// // TODO: remove debugging line: should make everything red if works
	// pixel = vec4(1.f, 0.f, 0.f, 1.f);
}
