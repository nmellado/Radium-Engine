struct Material
{
    sampler3D buffer;
};

uniform Material material;
out vec4 fragColor;

layout (location = 0) in vec3 in_position;
layout (location = 1) in vec3 in_texcoord;
//layout (location = 2) in vec3 in_viewVector;
layout (location = 3) in vec3 in_cameraInModelSpace;

float stepsize = 0.01;


vec4 shade(vec3 position, in float value) {
//    if ( value > 1)
        return (vec4(0.1, 0., 0., 0.1));
}

void main(void) {
	float value;
	vec4 color;
	vec4 frgColor = vec4(0.0);

	float att=1.0;

	// le rayon
	vec3 raypos = in_texcoord;//in_position;
    vec3 camera = in_cameraInModelSpace;
    vec3 raydir = normalize(in_position - camera);
     

    for(int i = 0; 
//        i< int(4./stepsize); 
        i<1;
        i++) {
        value = texture(material.buffer, raypos).r;
//        if (value > 0.000000000) {
            color = shade(raypos, value); 
            frgColor += color;
            att = 0;
//            break;
//        }
        raypos = raypos + raydir * stepsize;
    }

//    if (value == 0) fragColor = vec4(position,1.);//vec4(1.0, 0., 0., 1.0);
//    else fragColor = vec4(0.0, 0., 1., 1.0);


    //if (att > 0.99) discard;
//    frgColor.a = 1.0;
    fragColor = frgColor;
//   fragColor = vec4(raydir.zzz, 1.0);
//   fragColor = vec4(normalize(in_position), 1.0);
//    fragColor = vec4(1.0, 0., 0., 1.0);
}
