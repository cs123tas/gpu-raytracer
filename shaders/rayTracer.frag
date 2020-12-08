#version 410 core
#define MAX_VAL  10000.0
#define PI 3.1415926535897932384626433832795
#define PLANE   0
#define SPHERE  1
#define BOX 2

/*
*	IN From quad.vert
*/
in vec2 texCoord;

/*
*	Uniforms from C++
*/

uniform sampler2D tex;
uniform mat4 M_film2World;
uniform vec4 eye;
uniform int depth;
uniform int width;
uniform int height;
uniform float time;
uniform vec2 dimensions;


/*
*	Out to frame buffer
*/
out vec4 fragColor;


/*
*	Structs for scene represention
*/
struct Ray {
	vec4 P; 
	vec4 d;
};


//// TODO Remove
//struct SurfaceElement {
//	float t;
//	vec4 intersection; // world space intersect
//	vec4 normal;
//	
//	vec4 diffuseColor;
//	vec4 specularColor;
//	vec4 reflectedColor;
//	vec4 tranparencyColor;
//};


struct Material {
	vec4 diffuseColor;
	vec4 specularColor;
	vec4 reflectedColor;
	vec4 tranparencyColor;
};

struct Sphere {
	mat4 transformation;

	Material mat;
};

// Ground
struct Plane {
	vec4 point;
	vec4 normal;

	Material mat;
};


struct HitData {
	bool isIntersect;
	vec2 tVals; // store second hit for refractions
	vec4 normal;
	Material mat;
};


struct Light {
	vec4 position;
	vec4 color;
};

/*
*	Scene representation
*
*	TODO: artistic vision
*/
// Three point lighting: key light: (2, 2, 2), brightest | fill light (-1, 0, 1) | backlight (0, 3, -2)
Light sceneLighting[]  = Light[](
								Light( vec4(2.f, 2.f, 2.f, 1.f), vec4(1.f, 1.f, 1.f, 1.f) ),
								Light( vec4(-1.f, 0.f, 1.f, 1.f), vec4(1.f, 1.f, 1.f, 0.5f) ),
								Light( vec4(0.f, 3.f, -2.f, 1.f), vec4(0.2f, 0.2f, 0.6f, 0.2f) )
								);

// 3 spheres: 
Material eggShell = Material(
							vec4(240.f, 234.f, 214.f, 255.f)/255.f, 
							vec4(1.f, 1.f, 1.f, 1.f), 
							vec4(1.f, 1.f, 0.9f, 1.f), 
							vec4(0)
							);
Material salmon = Material(
							vec4(250.f, 128.f, 114.f, 255.f)/255.f, 
							vec4(1.f, 1.f, 1.f, 1.f), 
							vec4(1.f, 1.f, 0.9f, 1.f), 
							vec4(0)
							);

Material forest = Material(
							vec4(34.f, 139.f, 34.f, 255.f)/255.f, 
							vec4(1.f, 1.f, 1.f, 1.f), 
							vec4(1.f, 1.f, 0.9f, 1.f), 
							vec4(0)
							);
// TODO: animate here? Hard coded translation scale 
mat4 leftSphereTransformation = mat4(
									0.5f, 0.f, 0.f, -5.f,
									0.f, 0.5f, 0.f, 0.f,
									0.f, 0.f, 0.5f, -2.f,
									0.f, 0.f, 0.f, 1.f
									);

mat4 rightSphereTransformation = mat4(
										2.f, 0.f, 0.f, 6.f,
										0.f, 2.f, 0.f, 0.f,
										0.f, 0.f, 2.f, -3.f,
										0.f, 0.f, 0.f, 1.f
									);

mat4 centerSphereTransformation = mat4(1.f);


Sphere leftSphere = Sphere(leftSphereTransformation, salmon);
Sphere rightSphere = Sphere(rightSphereTransformation, forest);
Sphere centerSphere = Sphere(mat4(1.f), eggShell);

// TODO: restore
Sphere sceneSpheres[] = Sphere[](
								centerSphere,
								rightSphere,
								leftSphere
								);

/*
*	Ray tracer code
*/

// TODO: shadows
int checkOcclusions(inout Light light) {

	return 0;
}


// TODO: reflections and refractions, no recursion, just use secondary rays unless the fancy wavefront pattern
vec4 estimateIndirectLight(inout Ray ray, inout HitData data) {

	return vec4(0.f);
}


// TODO: diffuse and specular
vec4 estimateDirectLight(inout Ray ray, inout HitData data) {
	
	vec4 lightPosition = vec4(10.f); // TODO: restore
	// vec4 vertex = surfel.intersection; // TODO: restore
	vec4 vertex = ray.P + data.tVals.x*ray.d;
	vec4 n = data.normal;
	vec4 vertexToLight = normalize(vec4(10.f)- vertex);
	float cosTheta =  max(0.f, dot(normalize(n), vertexToLight));
	vec4 diffuseComponent = data.mat.diffuseColor*cosTheta; // mellow blue sphere

	vec4 reflected = -normalize(2.f*n*(dot(n, vertexToLight)) - vertexToLight);
    float cosPhi = max(0.f, dot(reflected, ray.d));
    vec4 specularComponent = data.mat.specularColor*pow(cosPhi, 1); // with white highlights

    vec4 radiance = diffuseComponent + specularComponent;

	return radiance;
}


vec2 getQuadradicRoots(float A, float B, float C) {
	float det = B*B - 4*A*C;
	
	if (det > 0) {
		float t1 = (-B + sqrt(det))/(2.f*A);
		float t2 = (-B - sqrt(det))/(2.f*A);

		if (t1 < 0.f) {
			t1 = MAX_VAL;
		} 
		if (t2 < 0.f) {
			t2 = MAX_VAL;
		}
		return vec2(t1, t2);
	}
	return vec2(MAX_VAL, MAX_VAL);
}


HitData sphereRayIntersect(inout Sphere sphere, inout Ray ray) {
	vec4 P = ray.P;
	vec4 d = ray.d;

	float R = 0.5f;

	float A = pow(d.x, 2.f) + pow(d.y, 2.f) + pow(d.z, 2.f);
	float B = 2.f*(P.x*d.x + P.y*d.y + P.z*d.z);
	float C = pow(P.x, 2.f) + pow(P.y, 2.f) + pow(P.z, 2.f) - R*R;

	vec2 tVals = getQuadradicRoots(A, B, C);

	HitData data;
	data.isIntersect = false; // false until otherwise proven true
	data.mat = sphere.mat;
	
	float first = min(tVals.x, tVals.y);
	float second = max(tVals.x, tVals.y);

	data.tVals = vec2(first, second);

	data.normal = normalize(P + data.tVals.x*d);
	return data;
}


// TODO: how to represent the scene, I'm thinking hardcoded SDF like lab 10
HitData intersect(inout Ray ray) {
	// TODO: restore
	HitData data; // blank data
	data.tVals = vec2(MAX_VAL, MAX_VAL);
	data.normal = vec4(0.f, 0.f, 0.f, 0.f);
	vec4 darkness = vec4(0.f, 0.f, 0.f, 0.f);
	data.mat = Material(darkness, darkness, darkness, darkness);

	float t = MAX_VAL;

//	// TODO: remove, simple sphere ray intersect test
//	Sphere egg = sceneSpheres[0];
//	HitData retrieved = sphereRayIntersect(egg, ray);
//
//	float t_prime = retrieved.tVals.x;
//
//	if (t_prime < MAX_VAL) {
//		t = t_prime;
//		retrieved.isIntersect = true;
//		data = retrieved;
//	}
//
//	return data;
//
//
//	float t = MAX_VAL;
	for (int i = 0; i < 3; i++){ // for each primitive in scene
		// TODO: remove, simple sphere ray intersect test
		Sphere sphere = sceneSpheres[i];
		mat4 modelMatrix = sphere.transformation;
		Ray rayInObjectSpace = Ray(
									inverse(modelMatrix)*vec4(ray.P.xyz, 1.f), 
									inverse(modelMatrix)*vec4(ray.d.xyz, 0.f)
									);

		HitData retrieved = sphereRayIntersect(sphere, rayInObjectSpace);

		float t_prime = retrieved.tVals.x; // first 

		if (t_prime < t) {
			t = t_prime;
			retrieved.isIntersect = true;
			data = retrieved;
		}
	}

	return data;
}


// inout is how you pass by ref in glsl
vec4 traceRay(inout Ray ray) {
	HitData data = intersect(ray);

	bool isIntersect = data.isIntersect;
	vec4 radiance = vec4(0.f, 0.f, 0.f, 1.f);
	// TODO: restore when ready
	if (isIntersect) {
		radiance = estimateDirectLight(ray, data) + estimateIndirectLight(ray, data);
	}
	return radiance;
}



// TODO: Use timer for animation
void main() {
	float x = ((2.f*float(gl_FragCoord.x))/dimensions.x) - 1.f;
	float y = ((2.f*float(gl_FragCoord.y))/dimensions.y) - 1.f;

	vec4 radiance = vec4(0.f, 0.f, 0.f, 1.f);

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
}
