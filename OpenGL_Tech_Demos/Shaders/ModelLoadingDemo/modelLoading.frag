#version 330 core

in VS_OUT {
	vec3 FragmentPosition;
	vec3 Normal;
	vec2 TexCoords;
	vec3 TangentLightPos;
	vec3 TangentCameraPos;
	vec3 TangentFragPos;
} fs_in;

struct Material {	// remove the ambient material color vector since the ambient color is in almost all cases equal to the diffuse color so there's no need to store it separately
	sampler2D texture_diffuse1;
	sampler2D texture_specular1;
	sampler2D texture_normal1;
	float shininess;	// impacts the scattering/radius of the specular highlight
};

uniform vec3 lightPosition;
uniform vec3 cameraPosition;
uniform bool normalMapping;
uniform bool inTangentSpace;
uniform Material material;

uniform samplerCube skybox;

out vec4 color;

void main()
{
	vec3 normal, lightDirection, viewDirection;
	if(!normalMapping)
	{
		normal = normalize(fs_in.Normal);
		// The light's direction vector is the difference vector between the light's 
		// position vector and the fragment's position vector. Öe can easily calculate
		// this difference by subtracting both vectors. LightDirection from the fraagment
		lightDirection = normalize(lightPosition - fs_in.FragmentPosition); 
		// From a fragment to camera
		viewDirection = normalize(cameraPosition - fs_in.FragmentPosition);
	}
	else
	{
		if(inTangentSpace)
		{
			// Obtain normal from normal map in range [0,1]
			normal = texture(material.texture_normal1, fs_in.TexCoords).rgb; 
			// Transform normal vector to range [-1,1].  This normal is in tangent space
			normal = normalize(normal * 2.0 - 1.0);
			// the light's direction vector is the difference vector between the light's position 
			// vector and the fragment's position vector. Öe can easily calculate this difference
			// by subtracting both vectors. LightDirection from the fraagment
			lightDirection = normalize(fs_in.TangentLightPos - fs_in.TangentFragPos);
			// from a fragment to camera
			viewDirection = normalize(fs_in.TangentCameraPos - fs_in.TangentFragPos); 
		}
		else
		{
			// normal = normalize(fs_in.Normal);
			normal = texture(material.texture_normal1, fs_in.TexCoords).rgb; 
			// Obtain normal from normal map in range [0,1]
			normal = normalize(normal * 2.0 - 1.0);
			// Transform normal vector to range [-1,1].  This normal is in tangent space
			// The light's direction vector is the difference vector between the 
			// light's position vector and the fragment's position vector. Öe can easily calculate this difference by subtracting both vectors. LightDirection from the fraagment
			lightDirection = normalize(lightPosition - fs_in.FragmentPosition); 
			// From a fragment to camera
			viewDirection = normalize(cameraPosition - fs_in.FragmentPosition);
		}
	}

	vec3 texColor = texture(material.texture_diffuse1, fs_in.TexCoords).rgb;
	// Ambient
	float ambientStrength = 0.1f;
	vec3 ambient = ambientStrength * texColor;

	// Difuse
	// vec3 lightDirection = normalize(fs_in.TangentLightPos - fs_in.TangentFragPos); //the light's direction vector is the difference vector between the light's position vector and the fragment's position vector. Öe can easily calculate this difference by subtracting both vectors. LightDirection from the fraagment
    float diff = max(dot(normal, lightDirection), 0.0f); /*If the angle between both vectors is greater than 90 degrees then the result of the dot product will actually become negative and we end up with a negative diffuse component. For that reason we use the max function that returns the highest of both its parameters to make sure the diffuse component (and thus the colors) never become negative. From 0 to 1 */
	vec3 diffuse = diff * texColor;

	// Specular
	vec3 I = normalize(fs_in.FragmentPosition - cameraPosition);
	vec3 R = reflect(I, normalize(normal));
	float reflectIntensity = texture(material.texture_specular1, fs_in.TexCoords).r;
	vec4 reflectColor;
	if(reflectIntensity > 0.1)
		reflectColor = texture(skybox, R) * (reflectIntensity / 1.9f);

	float specularStrength = 0.2f; //a specular intensity value to give the specular highlight a medium-bright color so that it doesn't have too much of an impact
	// vec3 viewDirection = normalize(fs_in.TangentCameraPos - fs_in.TangentFragPos); // from a fragment to camera
	float spec = 0.0f;
	vec3 halfwayDirection = normalize(lightDirection + viewDirection); /*unit vector exactly halfway between the view direction and the light direction. The closer this halfway vector aligns with the surface's normal vector, the higher the specular contribution.When the view direction is perfectly aligned with the (now imaginary) reflection vector, the halfway vector aligns perfectly with the normal vector. Thus the closer the viewer looks in the original reflection direction, the stronger the specular highlight becomes.Here you can see that whatever direction the viewer is look from, the angle between the halfway vector and the surface normal never exceeds 90 degrees (unless the light is far below the surface of course). This produces slightly different results compared to Phong reflections, but mostly looks slightly more visually plausible, especially with low specular exponents. */
	/*Another subtle difference between Phong and Blinn-Phong shading is that the angle between the halfway vector and the surface normal is often shorter than the angle between the view and reflection vector. As a result, to get similar results to Phong shading the specular shininess exponent has to be set a bit higher. A general rule of thumb is to set it between 2 and 4 times the Phong shininess exponent.*/
	spec = pow(max(dot(normal, halfwayDirection), 0.0f), /*1*/32);
	vec3 specular = vec3(specularStrength) * spec * texture(material.texture_specular1, fs_in.TexCoords).rgb;
	
	vec3 diffuse_color = texture(material.texture_diffuse1, fs_in.TexCoords).rgb;
	color = vec4(ambient + diffuse_color + vec3(reflectColor.x, reflectColor.y, reflectColor.z) + specular, 1.0f);// + reflectColor; 
}