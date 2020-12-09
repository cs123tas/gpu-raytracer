#version 410 core
#define MAX_VAL  1000.0
#define MIN_VAL 0.001
#define PI 3.1415926535897932384626433832795
#define PLANE   0
#define SPHERE  1
#define BOX 2

/*
*	IN From quad.vert
*/
in vec2 texCoord;
in vec4 position;

/*
*	Uniforms from C++
*/
uniform sampler2D tex;
uniform mat4 M_film2World;
//uniform vec4 eye;
uniform float time;
uniform vec2 dimensions;

/*
*	Out to frame buffer
*/
out vec4 fragColor;
////////////////////////////////////////////////////////////////////////////////////////////////

/*
*	STRUCTS
*/

struct Ray {
	vec4 P; 
	vec4 d;
};


struct Material {
	vec4 diffuseColor;
	vec4 specularColor;
	vec4 reflectedColor;
	vec4 tranparencyColor;
	
	float shininess;
};

struct Data {
	bool isIntersect;
	vec4 normal;
	Material mat;
	float t;
};


struct RayPath {
	Ray rays[4]; // 0 shadow ray | 1 primary ray | 2 1st reflection ray | 4 1st refraction ray
	Data data[4];

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

struct Light {
	vec4 position;
	vec4 color;
};

///////////////////////////////////////////////////////////////////////////////////////////
/*
*	SCENE REPRESENTATION

*/
Light sceneLighting[]  = Light[3](
								Light( vec4(3.f, 3.f, 3.f, 1.f), vec4(1.f, 1.f, 1.f, 1.f) ),
								Light( vec4(-2, 2.f, 2.f, 1.f), vec4(1.f, 1.f, 1.f, 1.f) ),
								Light( vec4(-1.f, 2.f, 1.f, 1.f), vec4(1.f, 1.f, 1.f, 1.f) )
								);

// 3 spheres: 
Material eggShell = Material(
							vec4(240.f, 234.f, 214.f, 255.f)/255.f, 
							vec4(1.f, 1.f, 1.f, 1.f), 
							vec4(1.f, 1.f, 0.9f, 1.f), 
							vec4(0),
							3.f
							);
Material salmon = Material(
							vec4(250.f, 128.f, 114.f, 255.f)/255.f, 
							vec4(1.f, 1.f, 1.f, 1.f), 
							vec4(1.f, 1.f, 1.f, 1.f), 
							vec4(0),
							4.f
							);

Material forest = Material(
							vec4(34.f, 139.f, 34.f, 255.f)/255.f, 
							vec4(1.f, 1.f, 1.f, 1.f), 
							vec4(1.f, 1.f, 1.f, 1.f), 
							vec4(0),
							1.f
							);

Material oak = Material(
						vec4(120.f, 81.f, 45.f, 255.f)/255.f, 
						vec4(1.f, 1.f, 1.f, 1.f), 
						vec4(1.f, 1.f, 0.9f, 1.f), 
						vec4(0),
						1.f
						);
						
float smallRadius = 0.25f;
mat4 leftSphereTransformation = transpose(mat4(
									smallRadius, 0.f, 0.f, sin(10.f*time),
									0.f, smallRadius, 0.f, 0.f,
									0.f, 0.f, smallRadius, cos(10.f*time),
									0.f, 0.f, 0.f, 1.f
									));

float bigRadius = 1.1f;
mat4 rightSphereTransformation = transpose(mat4(
										bigRadius, 0.f, 0.f, 0.2f,
										0.f, bigRadius, 0.f, 0.01f*cos(100.f*time),
										0.f, 0.f, bigRadius, -5.f,
										0.f, 0.f, 0.f, 1.f
									));


mat4 centerSphereTransformation = transpose(mat4(
											1.f, 0.f, 0.f, 0.f,
											0.f, 1.f, 0.f, -0.25f*sin(50.f*time),
											0.f, 0.f, 1.f, 0.f,
											0.f, 0.f, 0.f, 1.f
										));


Sphere leftSphere = Sphere(leftSphereTransformation, salmon);
Sphere rightSphere = Sphere(rightSphereTransformation, forest);
Sphere centerSphere = Sphere(centerSphereTransformation, eggShell);

Sphere sceneSpheres[] = Sphere[3](centerSphere, rightSphere, leftSphere);

Plane plane = Plane(vec4(0.f, 0.f, 0.f, 1.f), vec4(	0.f, 1.f, 0.f, 0.f), oak);

/////////////////////////////////////////////////////////////////////////////////
/*
*	RAY TRACER
*/

// TODO: shadows
int checkOcclusions(inout Light light) {

	return 0;
}


vec4 computeLighting(inout Ray ray, inout Data data) {
	vec4 radiance = vec4(0.f, 0.f, 0.f, 1.f);

	for (int i = 0; i < 3; i++) {
		Light light = sceneLighting[i];

		vec4 I = light.color;
		vec4 lightPosition = light.position;
		vec4 vertex = ray.P + data.t*ray.d;
		vec4 normal = data.normal;
		vec4 vertexToLight = lightPosition - vertex;

		float cosTheta =  max(0.f, dot(normalize(normal), normalize(vertexToLight)));
		vec4 diffuseComponent = I*data.mat.diffuseColor*cosTheta;

		vec4 reflected = -normalize(2.f*normal*(dot(normal, vertexToLight)) - vertexToLight);
		float cosPhi = max(0.f, dot(reflected, ray.d));
		vec4 specularComponent = I*data.mat.specularColor*pow(cosPhi, data.mat.shininess); // Add shininess to mat

		radiance += diffuseComponent;
		radiance += specularComponent;

		radiance.x = min(max(radiance.x, 0.f), 1.f);
		radiance.y = min(max(radiance.y, 0.f), 1.f);
		radiance.z = min(max(radiance.z, 0.f), 1.f);
		radiance.w = 1.f;
	}

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


Data planeRayIntersection(inout Plane plane, inout Ray ray) {
	vec4 P = ray.P;
	vec4 d = ray.d;

	Data data;

	float denom = dot(plane.normal, d);
	if (abs(denom) > MIN_VAL) {
		data.t = dot(plane.point - P, plane.normal)/denom;
	}
	return data;
}


Data sphereRayIntersect(inout Sphere sphere, inout Ray ray) {
	vec4 P = ray.P;
	vec4 d = ray.d;

	float R = 0.5f;

	float A = pow(d.x, 2.f) + pow(d.y, 2.f) + pow(d.z, 2.f);
	float B = 2.f*(P.x*d.x + P.y*d.y + P.z*d.z);
	float C = pow(P.x, 2.f) + pow(P.y, 2.f) + pow(P.z, 2.f) - R*R;

	vec2 tVals = getQuadradicRoots(A, B, C);

	Data data;
	data.isIntersect = false; // false until otherwise proven true
	data.mat = sphere.mat;
	
	data.t = min(tVals.x, tVals.y);
	data.normal = normalize(P + data.t*d);
	return data;
}


Data intersect(inout Ray ray) {
	// TODO: restore
	Data data; // blank data
	data.t = MAX_VAL;
	data.normal = vec4(0.f, 0.f, 0.f, 0.f);
	vec4 darkness = vec4(0.f, 0.f, 0.f, 0.f);
	data.mat = Material(darkness, darkness, darkness, darkness, 0.f);

	float t = MAX_VAL;

	for (int i = 0; i < 3; i++){ // for each Sphere
		// TODO: remove, simple sphere ray intersect test
		Sphere sphere = sceneSpheres[i];
		mat4 transformation = sphere.transformation;
		Ray rayInObjectSpace = Ray(
									inverse(transformation)*ray.P,
									inverse(transformation)*ray.d
									);

		Data retrieved = sphereRayIntersect(sphere, rayInObjectSpace);

		float t_prime = retrieved.t; // first 

		if (t_prime < t) {
			t = t_prime;
			retrieved.isIntersect = true;
			mat4 MtInv = inverse(transpose(transformation));
			retrieved.normal = MtInv*retrieved.normal;
			data = retrieved;
		}
	}

	return data;
}


// inout is how you pass by ref in glsl
vec4 traceRay(inout Ray ray) {
	Data data = intersect(ray);

	bool isIntersect = data.isIntersect;
	vec4 radiance = vec4(0.f, 0.f, 0.f, 1.f);
	// TODO: restore when ready
	if (isIntersect) {
		radiance = computeLighting(ray, data);
	}
	return radiance;
}


vec4 traceRays(inout RayPath rayPath) {
	vec4 radiance = vec4(0.f, 0.f, 0.f, 0.f);
	
	// DIRECT PASS
	Ray primaryRay = rayPath.rays[1];

	Data primaryData = intersect(primaryRay);
	rayPath.data[1] = primaryData; // primary hit data here


	// REFLECTION PASS
	float t = primaryData.t;
	vec4 vertex = primaryRay.P + t*primaryRay.d;

	vec4 normal = primaryData.normal;
	vec4 v = primaryRay.P + primaryRay.d;

	vec4 reflected = normalize(2.f*normal*dot(normal, v) - v);
	Ray reflectionRay = Ray(vertex + MIN_VAL*reflected, reflected);
	Data reflectionData = intersect(reflectionRay);

	// REFRACTION PASS
	

	// LIGHTING COMP
	if (primaryData.isIntersect) { // directLight
		radiance += computeLighting(primaryRay, primaryData);
	} 
	if (reflectionData.isIntersect) { // indirect light
		vec4 reflectionColor = reflectionData.mat.reflectedColor;
		radiance += reflectionColor*computeLighting(reflectionRay, reflectionData);
	}
	
	radiance.x = min(radiance.x, 1.f);
	radiance.y = min(radiance.y, 1.f);
	radiance.z = min(radiance.z, 1.f);
	radiance.w = 1.f;

	return radiance;
};

//////////////////////////////////////////////////////////////////////////////////////////////////////

/*
*  Driver code
*/

// TODO: Use timer for animation
void main() {
	fragColor = vec4(0.f, 0.f, 0.f, 1.f);
	float x = position.x; //in film
	float y = position.y; //in film


	vec4 d = vec4(0.f, 0.f, -1.f, 0.f);
	vec4 P = vec4(x, y, 0.f, 1.f);

	RayPath path;
	for (int i = 0; i < 4; i++) { // initialize
		path.rays[i] = Ray(vec4(0.f), vec4(0.f));
	}
	Ray primaryRay = Ray(P, d);
	path.rays[1] = primaryRay;
	
	
	// TODO: restore
	// fragColor += traceRay(primaryRay);
	fragColor += traceRays(path);

	// TODO: remove, debugging lines
	// fragColor += vec4(time/1000.f, 1.f - time/1000.f, 0.f, 1.f);
	// fragColor += d*0.5f + 0.5f;
	// fragColor += vec4(time, 0.f, 0.f, 1.f);
	// fragColor = vec4(1.f, 0.f, 0.f, 1.f);
}
