#version 410 core
#define MAX_VAL  1000.0
#define MIN_VAL 0.001
#define PI 3.1415926535897932384626433832795
#define ks 1.0
#define kd 2.0
#define kt 0.8

/*
*	In From quad.vert
*/
in vec2 texCoord;
in vec4 position;

/*
*	Uniforms from C++
*/
uniform sampler2D tex;
uniform mat4 M_film2World;
uniform int depth;
uniform float time;
uniform vec2 dimensions;
uniform float leftSpeed;
uniform float rightSpeed;
uniform float centerSpeed;

// Rigid Physics
uniform vec3 pos1;
uniform vec3 pos2;
uniform vec3 pos3;


/*
*	Out to frame buffer
*/
out vec4 fragColor;

////////////////////////////////////////////////////////////////////////////////////////////////

/*
*	STRUCTS
*/
//////////////////////////////////////////////////////////////////////////////////////////////
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
	float ior;
};

struct Data {
	bool isIntersect;
	vec4 normal;
	Material mat;
	float t;
};

struct Sphere {
	mat4 transformation;

	Material mat;

        // Rigid physics
//        vec3 velocity;
//        vec3 force;
//        float radius;
//        float mass;
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
//////////////////////////////////////////////////////////////////////////////////////////
Light sceneLighting[]  = Light[3](
								Light( vec4(1.f, 1.5f, -2.f, 1.f), vec4(0.3f, 0.3f, 0.3f, 1.f) ), // key light
								Light( vec4(-1, 1.f, -2.f, 1.f), vec4(0.6f, 0.2f, 0.2f, 1.f) ), // rim light
								Light( vec4(3.f, 3.f, 3.f, 1.f), vec4(0.2f, 0.2f, 0.6f, 1.f) ) // back light
								);

Material foggyGlass = Material(
							vec4(240.f, 234.f, 214.f, 255.f)/255.f, 
							vec4(1.f, 1.f, 1.f, 1.f), 
							vec4(1.f, 1.f, 0.9f, 1.f), 
							vec4(0.f),
							4.f,
							1.5f
							);
Material salmon = Material(
							vec4(250.f, 128.f, 114.f, 255.f)/255.f, 
							vec4(0.8f, 0.1f, 0.1f, 1.f), 
							vec4(1.f, 1.f, 1.f, 1.f), 
							vec4(0.f),
							5.f,
							1.369f
							);

Material iron = Material(
							vec4(203.f, 205.f, 205.f, 255.f)/255.f, 
							vec4(0.1f, 0.8f, 0.1f, 1.f), 
							vec4(1.f, 1.f, 1.f, 1.f), 
							vec4(0.f),
							3.f,
							1.0972
							);

Material oak = Material(
						vec4(120.f, 81.f, 45.f, 255.f)/255.f, 
						vec4(1.f, 1.f, 1.f, 1.f), 
						vec4(1.f, 1.f, 0.9f, 1.f), 
						vec4(0),
						1.f,
						1.f
						);
						
float smallRadius = 0.25f;
//mat4 leftSphereTransformation = transpose(mat4(
//                                                                        smallRadius, 0.f, 0.f, -0.5f,
//                                                                        0.f, smallRadius, 0.f, 0.5f*sin(1.f/leftSpeed*time),
//                                                                        0.f, 0.f, smallRadius, -3.f,
//                                                                        0.f, 0.f, 0.f, 1.f
//                                                                        ));

mat4 leftSphereTransformation = transpose(mat4(
                                                                        smallRadius, 0.f, 0.f, pos1[0],
                                                                        0.f, smallRadius, 0.f, pos1[2],
                                                                        0.f, 0.f, smallRadius, pos1[1],
                                                                        0.f, 0.f, 0.f, 1.f
                                                                        ));

float bigRadius = 1.1f;
//mat4 rightSphereTransformation = transpose(mat4(
//										bigRadius, 0.f, 0.f, 0.2f,
//										0.f, bigRadius, 0.f, 0.01f*cos(1.f/rightSpeed*time),
//										0.f, 0.f, bigRadius, -5.f,
//										0.f, 0.f, 0.f, 1.f
//									));

mat4 rightSphereTransformation = transpose(mat4(
                                                                                bigRadius, 0.f, 0.f, 0.2f,
                                                                                0.f, bigRadius, 0.f, 0.01f,
                                                                                0.f, 0.f, bigRadius, -5.f,
                                                                                0.f, 0.f, 0.f, 1.f
                                                                        ));


//mat4 centerSphereTransformation = transpose(mat4(
//											1.f, 0.f, 0.f, 0.f,
//                                                                                        0.f, 1.f, 0.f, -0.25f*sin(1.f/centerSpeed*time),
//											0.f, 0.f, 1.f, 0.f,
//                                                                                        0.f, 0.f, 0.f, 1.f
//										));

mat4 centerSphereTransformation = transpose(mat4(
                                                                                        1.f, 0.f, 0.f, 0.f,
                                                                                        0.f, 1.f, 0.f, -0.25f,
                                                                                        0.f, 0.f, 1.f, 0.f,
                                                                                        0.f, 0.f, 0.f, 1.f
                                                                                ));
//mat4 updateTransformMat(mat4 transformMat, vec3 scale, vec3 translation){
//    if (scale[0] != 0.0f || scale[1] != 0.0f || scale[2] != 0.0f){
//        transformMat *= scale;
//    }
//    if (scale[0] != 0.0f || scale[1] != 0.0f || scale[2] != 0.0f){
//        transformMat *= translation;
//    }
//}

// Rigid physics

Plane backWall = Plane(vec4(0.f, 0.f, -2.f, 1.f), vec4(0.f, 0.f, 1.f, 0.f), oak);
Plane frontWall = Plane(vec4(0.f, 0.f, 0.1f, 1.f), vec4(0.f, 0.f, -1.f, 0.f), oak);
Plane leftWall = Plane(vec4(-1.5f, 0.f, 0.f, 1.f), vec4(1.f, 0.f, 0.f, 0.f), oak);
Plane rightWall = Plane(vec4(1.5f, 0.f, 0.f, 1.f), vec4(-1.f, 0.f, 0.f, 0.f), oak);
Plane topWall = Plane(vec4(0.f, 1.5f, 0.f, 1.f), vec4(0.f, -1.f, 0.f, 0.f), oak);
Plane bottomWall = Plane(vec4(0.f, -1.5f, 0.f, 1.f), vec4(0.f, 1.f, 0.f, 0.f), oak);
Plane walls[] = Plane[6](backWall, frontWall, leftWall, rightWall, topWall, bottomWall);

//vec3 velocity;
//vec3 force;
//float radius;
//float mass;

Sphere leftSphere = Sphere(leftSphereTransformation, salmon);
Sphere rightSphere = Sphere(rightSphereTransformation, iron);
Sphere centerSphere = Sphere(centerSphereTransformation, foggyGlass);

Sphere sceneSpheres[] = Sphere[3](leftSphere, centerSphere, rightSphere);

Plane plane = Plane(vec4(0.f, 0.f, 0.f, 1.f), vec4(0.f, 1.f, 0.f, 0.f), oak);


/////////////////////////////////////////////////////////////////////////////////
/*
*	Physics
*/
////////////////////////////////////////////////////////////////////////////////


//void simulateRigidPhysics() {
//    bool noCollision=false;
//    while (true){
//        for (int i = 0; i < sceneSpheres.length(); i++){ // iterate over spheres
//            Sphere sphere1 = sceneSpheres[i];

//        }
//        break;
//    }
//}

//void avoidCollision(){
//    bool noCollision=false;
//    while (true){
//        for (int j=0;j<3;j++){ // repeat the following processes multiple times
//            for (int i = 0; i < sceneSpheres.length(); i++){ // iterate over spheres
//                Sphere sphere1 = sceneSpheres[i];
//                for (int k = 0; k < sceneSpheres.length(); k++){ // iterate over spheres
//                    Sphere sphere2 = sceneSpheres[k];

//                }
//            }
//            for (int i = 0; i < walls.length(); i++){ // iterate over walls
//                Plane wall = walls[i];
//            }
//        }
//        break;
//    }
//}

void updatePosition(){
//    sceneSpheres[0].transformation[3].xyz = pos1;
//    sceneSpheres[1].transformation[3].xyz = pos2;
//    sceneSpheres[2].transformation[3].xyz = pos3;
}






/////////////////////////////////////////////////////////////////////////////////
/*
*	RAY TRACER
*/
////////////////////////////////////////////////////////////////////////////////
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
	Data data; // blank data
	data.t = MAX_VAL;
	data.normal = vec4(0.f, 0.f, 0.f, 0.f);
	vec4 darkness = vec4(0.f, 0.f, 0.f, 0.f);
	data.mat = Material(darkness, darkness, darkness, darkness, 0.f, 0.f);

	float t = MAX_VAL;

        for (int i = 0; i < sceneSpheres.length(); i++){ // for each Sphere
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

bool checkOcclusions(inout Ray ray, inout Data data, inout Light light) {
	bool isOccluded = false;

	vec4 lightPosition = light.position;
	vec4 vertex = ray.P + data.t*ray.d;
	vec4 vertexToLight = lightPosition  - vertex;
	Ray shadowRay = Ray(vertex + MIN_VAL*vertexToLight, vertexToLight);

	Data shadowData = intersect(shadowRay);

	if (shadowData.isIntersect) {
		vec4 occluderVertex = shadowRay.P + shadowData.t*shadowRay.d;
		float distanceFromOccluder = distance(occluderVertex, vertex);
		float distanceFromLight = distance(lightPosition, vertex);
		if (distanceFromOccluder < distanceFromLight) {
			isOccluded = true;
		}
	}

	return isOccluded;
}

vec4 computeLighting(inout Ray ray, inout Data data) {
	vec4 radiance = vec4(0.f, 0.f, 0.f, 1.f);

        for (int i = 0; i < sceneLighting.length(); i++) { // for each light in the scene
		Light light = sceneLighting[i];

		vec4 I = light.color;
		vec4 lightPosition = light.position;
		vec4 vertex = ray.P + data.t*ray.d;
		vec4 normal = data.normal;
		vec4 vertexToLight = lightPosition - vertex;

		float cosTheta =  max(0.f, dot(normalize(normal), normalize(vertexToLight)));
		vec4 diffuseComponent = I*kd*data.mat.diffuseColor*cosTheta;

		vec4 reflected = -normalize(2.f*normal*(dot(normal, vertexToLight)) - vertexToLight);
		float cosPhi = max(0.f, dot(reflected, ray.d));
		vec4 specularComponent = I*ks*data.mat.specularColor*pow(cosPhi, data.mat.shininess);

		radiance += diffuseComponent;
		radiance += specularComponent;

		radiance.x = min(max(radiance.x, 0.f), 1.f);
		radiance.y = min(max(radiance.y, 0.f), 1.f);
		radiance.z = min(max(radiance.z, 0.f), 1.f);
		radiance.w = 1.f;
	}

	return radiance;
}

vec4 traceRays(inout Ray primaryRay) {
	// DIRECT PASS
	vec4 radiance = vec4(0.f, 0.f, 0.f, 0.f);
	Data primaryData = intersect(primaryRay);
	if (primaryData.isIntersect) { // directLight
		radiance += computeLighting(primaryRay, primaryData);
	} 

	Ray currentRay = primaryRay;
	Data currentData = primaryData;

	for (int i = 0; i < depth; i++) { // How many layers we want 

		float t = currentData.t;
		vec4 vertex = currentRay.P + t*currentRay.d;

		vec4 normal = currentData.normal;
		vec4 v = primaryRay.P + primaryRay.d;

		float ior = currentData.mat.ior;
		float r0 = pow((1.f - ior)/(1.f + ior), 2.f); // assume everything else is just air
		float F = r0 + (1.f - r0)*pow((1.f - dot(normal, v)), 5.f);

		vec4 reflected = normalize(2.f*normal*dot(normal, v) - v);
		Ray reflectionRay = Ray(vertex + MIN_VAL*reflected, reflected);
		Data reflectionData = intersect(reflectionRay);

		if (reflectionData.isIntersect) {
			vec4 reflectionColor = reflectionData.mat.reflectedColor;
			radiance += ks*reflectionColor*computeLighting(reflectionRay, reflectionData);
		}

		// TODO: refractions?
		currentRay = reflectionRay;
		currentData = reflectionData;
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
/////////////////////////////////////////////////////////////////////////////////////////////////////
void main() {
	fragColor = vec4(0.1f, 0.1f, 0.1f, 1.f);
	float x = position.x; //in film
	float y = position.y; //in film

	vec4 d = vec4(0.f, 0.f, -1.f, 0.f); // Ortho mode until camera setup
	vec4 P = vec4(x, y, 0.f, 1.f);

	Ray primaryRay = Ray(P, d);
	
	// TODO: restore
//        simulateRigidPhysics();
        updatePosition();
//        fragColor = leftSphereTransformation
        fragColor += traceRays(primaryRay);

	//fragColor = P + d;
	// TODO: remove, debugging lines
	// fragColor += vec4(time/1000.f, 1.f - time/1000.f, 0.f, 1.f);
	// fragColor += d*0.5f + 0.5f;
	// fragColor += vec4(time, 0.f, 0.f, 1.f);
	// fragColor = vec4(1.f, 0.f, 0.f, 1.f);
}
