#version 430 core
#define MAX_VAL  10000.0
#define PI 3.1415926535897932384626433832795
#define PLANE   0
#define SPHERE  1
#define BOX 2

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


// TODO: reflections and refractions, no recursion, just use secondary rays unless the fancy wavefront pattern
vec4 estimateIndirectLight(inout SurfaceElement surfel, inout Ray ray) {

	return vec4(0.f);
}


// TODO: diffuse and specular
vec4 estimateDirectLight(inout SurfaceElement surfel, inout Ray ray) {
	
	vec4 kdOd = surfel.diffuseColor;
	vec4 ksOs = surfel.specularColor;

	vec4 lightPosition = vec4(10.f); // TODO: restore
	vec4 vertex = surfel.intersection;
	vec4 n = normalize(vertex); // TODO: restore
	vec4 vertexToLight = normalize(vec4(10.f)- vertex);
	float cosTheta =  max(0.f, dot(normalize(n), vertexToLight));
	vec4 diffuseComponent = vec4(0.3, 0.2, 0.5, 1.f)*cosTheta; // mellow blue sphere

	vec4 reflected = -normalize(2.f*n*(dot(n, vertexToLight)) - vertexToLight);
    float cosPhi = max(0.f, dot(reflected, ray.d));
    vec4 specularComponent = vec4(1.f, 1.f, 1.f, 1.f)*pow(cosPhi, 4); // with white highlights
//
    vec4 radiance = diffuseComponent + specularComponent;

	return radiance;
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

	float R = 0.5f;
	float A = pow(d.x, 2.f) + pow(d.y, 2.f) + pow(d.z, 2.f);
	float B = 2.f*(P.x*d.x + P.y*d.y + P.z*d.z);
	float C = pow(P.x, 2.f) + pow(P.y, 2.f) + pow(P.z, 2.f) - R*R;

	vec2 tVals = getQuadradicRoots(A, B, C);

	return min(tVals.x, tVals.y);
}


// TODO: how to represent the scene, I'm thinking hardcoded SDF like lab 10
bool intersect(inout Ray ray, inout SurfaceElement surfel) {
	
	// TODO: remove, simple sphere ray intersect test
	float t = sphereRayIntersect(ray, surfel);
	if (t < MAX_VAL) {
		// TODO: paint surfel here
		surfel.intersection = ray.P + t*ray.d;
		surfel.normal = normalize(surfel.intersection); // TODO: change when ready for other shapes
		return true;
	} else {
		return false;
	}
//	float t = sphereRayIntersect(ray, surfel);
//	if (t < MAX_VAL) {
//		return true;
//	}
//	return false;

//	vec4 P = ray.P;
//	vec4 d = ray.d;
//
//	float A = pow(d.x, 2.f) + pow(d.y, 2.f) + pow(d.z, 2.f);
//	float B = 2.f*(P.x*d.x + P.y*d.y + P.z*d.z);
//	float C = pow(P.x, 2.f) + pow(P.y, 2.f) + pow(P.z, 2.f) - 5;
//
//	float det = B*B - 4*A*C;
//	if (det < 0.f) {
//		return false;
//	} else {
//		return true;
//	}
}


// inout is how you pass by ref in glsl
vec4 traceRay(inout Ray ray) {

	SurfaceElement surfel;
	bool isIntersect = intersect(ray, surfel);

	vec4 radiance = vec4(0.f, 0.f, 0.f, 1.f);
	// TODO: restore when ready
	if (isIntersect) {
		radiance = estimateDirectLight(surfel, ray) + estimateIndirectLight(surfel, ray);
	}

//	if (isIntersect) {
//		radiance = vec4(0.5f*ray.d.x + 0.5f, 0.5f*ray.d.y + 0.5f, 0.5f*ray.d.z + 0.5f, 1.f); // red sphere for now
//	} 
	
	return radiance;
};



// TODO: Use timer for animation
// TODO: send in model matrix?
void main() {
	float x = ((2.f*float(gl_FragCoord.x))/dimensions.x) - 1.f;
	float y = ((2.f*float(gl_FragCoord.y))/dimensions.y) - 1.f;

	vec4 radiance = vec4(0.f, 0.f, 0.f, 1.f);

//	if (y < 0) {
//		fragColor += vec4(1.f, 0.f, 0.f, 0.f); // negative x should be more red
//	} else if (y > 0)  {
//		fragColor += vec4(0.f, 1.f, 0.f, 0.f); // positiive x should be more green
//	}


	// TODO: why isnt this working (perspective)	
	vec4 pt_film = vec4(x, y, -1.f, 1.f);
	vec4 pt_world = M_film2World*pt_film;

	vec4 d = vec4(0.f, 0.f, -1.f, 0.f);
	vec4 P = vec4(x, y, 0.f, 1.f);
//	vec4 P = M_film2World*eye;
//	vec4 d = normalize(pt_world - eye);

	Ray primaryRay;
	primaryRay.P = P;
	primaryRay.d = d;

	fragColor = vec4(0.f, 0.f, 0.f, 1.f);
	fragColor += traceRay(primaryRay);

//	float R = 0.25f;
//	float A = pow(d.x, 2.f) + pow(d.y, 2.f) + pow(d.z, 2.f);
//	float B = 2.f*(P.x*d.x + P.y*d.y + P.z*d.z);
//	float C = pow(P.x, 2.f) + pow(P.y, 2.f) + pow(P.z, 2.f) - R*R;
//
//	vec2 t = getQuadradicRoots(A, B, C);
//
//	float det = B*B - 4*A*C;
//	if (det >= 0.f) {
//		float hitT = min(t.x, t.y);
//		vec4 vertex = P + hitT*d;
//		vec4 vertexToLight = normalize(vec4(10.f)- vertex);
//		vec4 vertexNormal = normalize(vertex);
//		float cosTheta =  max(0.f, dot(normalize(vertexNormal), vertexToLight));
//		vec4 diffuseComponent = vec4(0.3, 0.2, 0.5, 1.f)*cosTheta; // mellow blue sphere
//		
//		vec4 reflected = -normalize(2.f*vertexNormal*(dot(vertexNormal, vertexToLight)) - vertexToLight);
//		float cosPhi = max(0.f, dot(reflected, d));
//		vec4 specularComponent = vec4(1.f, 1.f, 1.f, 1.f)*cosPhi; // with white highlights
//
//		fragColor = diffuseComponent + specularComponent;
//
//	} else {
//		fragColor = vec4(0.f, 0.f, 0.f, 1.f);
//	}

}
