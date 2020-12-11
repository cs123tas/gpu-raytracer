#version 410 core
#define MAX_VAL  1000.f
#define MIN_VAL 0.01f
#define PI 3.1415926535897932384626433832795
#define ks 2.f
#define kd 2.f
#define kt 2.f

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
layout(location = 0) out vec4 fragColor;

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
Light sceneLighting[]  = Light[](
                                                                Light( vec4(2.f, 2.f, -2.f, 1.f), vec4(0.5f, 0.5f, 0.5f, 1.f) ), // key light
                                                                Light( vec4(-1.f, -2.f, -3.f, 1.f), vec4(0.3f, 0.3f, 0.3f, 1.f) ), // rim light
                                                                Light( vec4(2.f, 6.f, 6.f, 1.f), vec4(0.5f, 0.5f, 1.f, 1.f) ), // back light
                                                                Light( vec4(0.f, -3.f, -1.f, 1.f), vec4(0.3f, 0.3f, 0.3f, 1.f) ) // needs a little umph
                                                                );

Material foggyGlass = Material(
                                                        vec4(240.f, 240.f, 210.f, 0.f)/255.f,
                                                        vec4(vec3(1.f), 0.f),
                                                        vec4(vec3(1.f), 0.f),
                                                        vec4(vec3(1.f), 0.f),
                                                        3.f,
                                                        1.6f
                                                        );
Material salmon = Material(
                                                        vec4(250.f, 130.f,  110.f, 100.f)/255.f,
                                                        vec4(vec3(1.f), 0.f),
                                                        vec4(1.f),
                                                        vec4(1.f),
                                                        3.f,
                                                        1.3f
                                                        );

Material iron = Material(
                                                        vec4(203.f, 205.f, 205.f, 255.f)/255.f,
                                                        vec4(1.f),
                                                        vec4(1.f),
                                                        vec4(1.f),
                                                        6.f,
                                                        1.7f
                                                        );

Material oak = Material(
                                                vec4(120.f, 81.f, 45.f, 255.f)/255.f,
                                                vec4(1.f, 1.f, 1.f, 1.f),
                                                vec4(1.f, 1.f, 0.9f, 1.f),
                                                vec4(1.f),
                                                1.f,
                                                1.f
                                                );


//float smallRadius = 0.25f;
//mat4 leftSphereTransformation = transpose(mat4(
//                                                                       smallRadius, 0.f, 0.f, -0.5f,
//                                                                       0.f, smallRadius, 0.f, 0.5f*sin(1.f/leftSpeed*time),
//                                                                       0.f, 0.f, smallRadius, -3.f,
//                                                                       0.f, 0.f, 0.f, 1.f
//                                                                       ));

//float bigRadius = 1.1f;
//mat4 rightSphereTransformation = transpose(mat4(
//                                                                               bigRadius, 0.f, 0.f, 0.2f,
//                                                                               0.f, bigRadius, 0.f, 0.01f*cos(1.f/rightSpeed*time),
//                                                                               0.f, 0.f, bigRadius, -5.f,
//                                                                               0.f, 0.f, 0.f, 1.f
//                                                                       ));


//mat4 centerSphereTransformation = transpose(mat4(
//                                                                                       1.f, 0.f, 0.f, 0.f,
//                                                                                       0.f, 1.f, 0.f, -0.25f*sin(1.f/centerSpeed*time),
//                                                                                       0.f, 0.f, 1.f, 0.f,
//                                                                                       0.f, 0.f, 0.f, 1.f
//                                                                               ));


  float smallRadius = 0.25f;
  mat4 leftSphereTransformation = transpose(mat4(
                                                                        smallRadius, 0.f, 0.f, pos1[0],
                                                                        0.f, smallRadius, 0.f, pos1[1],
                                                                        0.f, 0.f, smallRadius, pos1[2],
                                                                        0.f, 0.f, 0.f, 1.f
                                                                        ));

  float bigRadius = 1.1f;
  mat4 rightSphereTransformation = transpose(mat4(
                                                                                bigRadius, 0.f, 0.f, pos2[0],
                                                                                0.f, bigRadius, 0.f, pos2[1],
                                                                                0.f, 0.f, bigRadius, pos2[2],
                                                                                0.f, 0.f, 0.f, 1.f
                                                                        ));


  mat4 centerSphereTransformation = transpose(mat4(
                                                                                        1.f, 0.f, 0.f, pos3[0],
                                                                                        0.f, 1.f, 0.f, pos3[1],
                                                                                        0.f, 0.f, 1.f, pos3[2],
                                                                                        0.f, 0.f, 0.f, 1.f
                                                                                ));


Sphere leftSphere = Sphere(leftSphereTransformation, salmon);
Sphere rightSphere = Sphere(rightSphereTransformation, iron);
Sphere centerSphere = Sphere(centerSphereTransformation, foggyGlass);

Sphere sceneSpheres[] = Sphere[](leftSphere, centerSphere, rightSphere);

Plane plane = Plane(vec4(0.f, 0.f, 0.f, 1.f), vec4(	0.f, 1.f, 0.f, 0.f), oak);


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

        vec4 vertex = P + data.t*d;
        data.normal = vec4(normalize(vertex.xyz), 0.f);
        return data;
}


Data intersect(inout Ray ray) {
        Data data; // blank data
        data.t = MAX_VAL;
        data.normal = vec4(0.f, 0.f, 0.f, 0.f);
        vec4 darkness = vec4(0.f, 0.f, 0.f, 0.f);
        data.mat = Material(darkness, darkness, darkness, darkness, 0.f, 0.f);

        float t = MAX_VAL;

        for (int i = 0; i < 3; i++){ // for each Sphere
                // TODO: remove, simple sphere ray intersect test
                Sphere sphere = sceneSpheres[i];
                mat4 transformation = sphere.transformation;

                vec4 P_tilde = inverse(transformation)*ray.P;
                vec4 d_tilde = inverse(transformation)*ray.d;

                Ray rayInObjectSpace = Ray(vec4(P_tilde.xyz, 1.f), vec4(d_tilde.xyz, 0.f));

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

        for (int i = 0; i < 3; i++) { // for each light in the scene
                Light light = sceneLighting[i];

                if (checkOcclusions(ray, data, light)) {
                        continue;
                }

                vec4 I = light.color;
                vec4 lightPosition = light.position;
                vec4 vertex = ray.P + data.t*ray.d;
                vec4 normal = data.normal;
                vec4 vertexToLight = lightPosition - vertex;

                float dist = distance(light.position, vertex);
                //float denom = 0.01f + 0.1f*dist + 1.f*dist*dist;
                float denom = 0.01f*exp(dist);
                float attenuation = min(1.f, 1.f/denom);
                //float attenuation = 1.f;

                float cosTheta =  max(0.f, dot(normalize(normal), normalize(vertexToLight)));
                vec4 diffuseComponent = attenuation*I*kd*data.mat.diffuseColor*cosTheta;

                vec4 reflected = normalize(reflect(vertexToLight, normal));
                float cosPhi = max(0.f, dot(reflected, ray.d));
                vec4 specularComponent = I*ks*data.mat.specularColor*pow(cosPhi, data.mat.shininess);

                vec4 ambientComponent = vec4(0.01f, 0.01f, 0.01f, 0.01f);

                radiance += ambientComponent;
                radiance += diffuseComponent;
                radiance += specularComponent;

                radiance.x = min(max(radiance.x, 0.f), 1.f);
                radiance.y = min(max(radiance.y, 0.f), 1.f);
                radiance.z = min(max(radiance.z, 0.f), 1.f);
                radiance.w = min(max(radiance.w, 0.f), 1.f);
        }
        return radiance;
}


vec4 traceRays(inout Ray primaryRay) {

        vec4 radiance = vec4(0.03f, 0.02f, 0.03f, 1.f); // ambient lighting
        Data primaryData = intersect(primaryRay);
        if (primaryData.isIntersect) { // directLight
                radiance += computeLighting(primaryRay, primaryData);
        }

        Ray currentRay = primaryRay;
        Data currentData = primaryData;

        float gamma = 1.f;

        for (int i = 0; i < depth + 1; i++) { // How many layers we want

                float t = currentData.t;
                vec4 vertex = currentRay.P + t*currentRay.d;

                vec4 normal = currentData.normal;
                vec4 v = currentRay.P + currentRay.d;

                float ior = currentData.mat.ior;
                float r0 = pow((1.f - ior)/(1.f + ior), 2.f); // assume everything else is just air
                float F = r0 + (1.f - r0)*pow((1.f - dot(normal, v)), 5.f);

                vec4 reflected = normalize(reflect(-v, normal));
                Ray reflectionRay = Ray(vertex + MIN_VAL*reflected, reflected);
                Data reflectionData = intersect(reflectionRay);

                if (reflectionData.isIntersect) {
                        vec4 reflectionColor = reflectionData.mat.reflectedColor;
                        radiance += max(vec4(0.f), gamma*F*(ks*reflectionColor*computeLighting(reflectionRay, reflectionData)));
                }


                vec4 p = vertex;
                vec4 n_p = currentData.normal;
                p = p + MIN_VAL*(-n_p);
                vec4 d_p = normalize(refract(n_p, v, 1.f/ior));
                Ray throughRay = Ray(p, d_p);
                Data throughData = intersect(throughRay);

                vec4 q = p + throughData.t*d_p;
                vec4 n_q = throughData.normal;

                q = q + MIN_VAL*n_q;
                vec4 d_q = normalize(refract(n_q, p + throughData.t*d_p, ior));
                Ray refractedRay = Ray(q, d_q);
                Data refractionData = intersect(refractedRay);

//
//		vec4 throughVector = normalize(refract(normal, v, 1.f/ior));
//		vec4 oppositeSideVertex = vertex + 1*throughVector;
//		vec4 refracted = normalize(refract(-normal, throughVector, 1.f));
//		Ray refractedRay = Ray(oppositeSideVertex + MIN_VAL*refracted, refracted);
//		Data refractionData = intersect(refractedRay);

                if (refractionData.isIntersect) {
                        vec4 refractionColor = refractionData.mat.tranparencyColor;
                        vec4 refractedContrib = max(vec4(0.f), gamma*(1.f - F) * (kt*refractionColor*computeLighting(refractedRay, refractionData)));
                        radiance += refractedContrib;
                }

                gamma = 1.f/exp(gamma*gamma);

                currentRay = reflectionRay; // relfections and refractions of reflections
                currentData = reflectionData;
        }


        radiance.x = min(radiance.x, 1.f);
        radiance.y = min(radiance.y, 1.f);
        radiance.z = min(radiance.z, 1.f);
        radiance.w = min(radiance.w, 1.f);

        return radiance;
};

//////////////////////////////////////////////////////////////////////////////////////////////////////
/*
*  Driver code
*/
/////////////////////////////////////////////////////////////////////////////////////////////////////
void main() {
        vec4 radiance = vec4(0.1f, 0.1f, 0.1f, 0.1f);
        float x = position.x; //in film
        float y = position.y; //in film

        // TODO: perspective
//	vec4 eye = transpose(M_film2World)*vec4(0.f, 0.f, 0.f, 1.f);
//	vec4 pt_film = vec4(x, y, -1.f, 0.f);
//	vec4 pt_world = transpose(M_film2World)*pt_film;
//	vec4 d = normalize(pt_world - eye);
//	vec4 P = eye;

        vec4 d = vec4(0.f, 0.f, -1.f, 0.f); // Ortho mode until camera setup
        vec4 P = vec4(x, y, 0.f, 1.f);

        Ray primaryRay = Ray(P, d);

        radiance += traceRays(primaryRay);
        fragColor = radiance;

        //fragColor = P + d;
        // TODO: remove, debugging lines
        // fragColor += vec4(time/1000.f, 1.f - time/1000.f, 0.f, 1.f);
        //fragColor += P;
//
//	if (x > 0.1 && x < 0.5) {
//		fragColor = vec4(1.f, 0.f, 0.f, 1.f);
//	} else {
//		fragColor = vec4(0.f);
//	}

        // fragColor += vec4(time, 0.f, 0.f, 1.f);
        // fragColor = vec4(1.f, 0.f, 0.f, 1.f);
}
