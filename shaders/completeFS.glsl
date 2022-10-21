#version 330 core
out vec4 FragColor;

in vec3 Normal;
in vec2 texCoord;
in vec3 currentPos;
in mat3 TBN;

const float PI = 3.14159265f;

uniform vec3 Osize;
uniform vec3 lightPos;
uniform vec3 cameraPos;
uniform bool activateNormalMap;
uniform bool activatePBR;
uniform bool hasNormalMap;
uniform bool hasLights;

uniform sampler2D Texture;
uniform sampler2D NormalMap;
uniform sampler2D MetalMap;
uniform sampler2D RouthnessMap;
uniform sampler2D AOMap;
uniform samplerCube irradianceMap;
uniform samplerCube prefilterMap;
uniform sampler2D brdfLUT;

float GeometrySmith(float NdotV, float NdotL, float k)
{
    float ggx1 = NdotV / (NdotV * (1.0f - k) + k);
    float ggx2 = NdotL / (NdotL * (1.0f - k) + k);
	
    return ggx1 * ggx2;
}


float DistributionGGX(vec3 N, vec3 H, float a)
{
    float a2     = a*a;
    float NdotH  = max(dot(N, H), 0.0f);
	
    float nom    = a2;
    float denom  = (NdotH * NdotH * (a2 - 1.0f) + 1.0f);
    denom        = PI * denom * denom;
	
    return nom / max(denom, 0.0000001);
}

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness)
{
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}   

void main()
{
    vec3 normal = Normal;
    if (hasNormalMap && activateNormalMap ) {
        normal = texture(NormalMap, texCoord).rgb;
        normal = normal * 2 - 1;
        normal = normalize(TBN * normal);
    }
    
    vec3 Color = texture(Texture, texCoord).rgb;

    if (!activatePBR) {
        float ambient = 0.2f;
        vec3 lightDirection = normalize(lightPos - currentPos);
        float diffuse = max(dot(normal, lightDirection), 0.0f);

        float specularLight = 0.5f;
        vec3 viewDirection = normalize(cameraPos - currentPos);
        vec3 reflectionDirection = reflect(-lightDirection, normal);
        float specAmmunt = pow(max(dot(viewDirection, reflectionDirection), 0.0f), 16);
        float specular = specAmmunt * specularLight;
        
        FragColor = vec4(Color * (diffuse + ambient + specular), 1.0f);
    }
    else {
        float metallic = texture(MetalMap, texCoord).r;
        float roughness = texture(RouthnessMap, texCoord).r;
        float ao = texture(AOMap, texCoord).r;

        // calculate reflectance at normal incidence; if dia-electric (like plastic) use F0 
        // of 0.04 and if it's a metal, use the albedo color as F0 (metallic workflow)    
        vec3 F0 = Color; 
        F0 = mix(F0, Color, metallic);

        vec3 V = normalize(cameraPos - currentPos);
        float NdotV = max(dot(normal, V), 0.0000001f);
        vec3 Lo = vec3(0.0f);

        if (hasLights) {
            // start of per light calcul
            vec3 L = normalize(lightPos - currentPos);
            vec3 H = normalize(V + L);
            float distance = length(lightPos - currentPos) / (Osize.x * 1.5);
            float attenuation = 1.0f / (distance * distance);
            vec3 radiance = vec3(1.0f) * attenuation; //light color * attenuation

            float NdotL = max(dot(normal, L), 0.0000001f);

            float D = DistributionGGX(normal, H, roughness);
            float G = GeometrySmith(NdotV, NdotL, roughness);
            vec3 F = fresnelSchlick(max(dot(H, V), 0.0f), F0);

            vec3 specular = (D * G * F) / (4.0f * NdotV * NdotL);

            vec3 kD = vec3(1.0f) - F;

            kD *= 1.0f - metallic;
            Lo += (kD * Color / PI + specular) * radiance * NdotL;
            // end of per light calcul
        }

        vec3 F = fresnelSchlickRoughness(NdotV, F0, roughness);
        vec3 kD = (1.0 - F) * (1.0 - metallic);
        vec3 diffuse    = texture(irradianceMap, normal).rgb * kD * Color;

        const float MAX_REFLECTION_LOD = 4.0;
        vec3 prefilteredColor = textureLod(prefilterMap, reflect(-V, normal),  roughness * MAX_REFLECTION_LOD).rgb;   
        vec2 envBRDF  = texture(brdfLUT, vec2(NdotV, roughness)).rg;
        vec3 specular = prefilteredColor * (F * envBRDF.x + envBRDF.y);
        
        vec3 ambient = (diffuse + specular) * ao;
        vec3 color = ambient + Lo;

    //    color = color / (color + vec3(1.0f)); //tonemapping
    //    color = pow(color, vec3(1.0f/2.2f));

        FragColor = vec4(color, 1.0f);
    }
}