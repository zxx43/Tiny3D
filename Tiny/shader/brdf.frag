#include "shader/util.glsl"
#include "shader/pbr.glsl"

in vec2 vTexcoord;

layout (location = 0) out vec4 FragColor;

vec2 IntegrateBRDF(float NdotV, float roughness) {
    vec3 V;
    V.x = sqrt(1.0 - NdotV*NdotV);
    V.y = 0.0;
    V.z = NdotV;

    vec2 result = vec2(0.0);

    vec3 N = vec3(0.0, 0.0, 1.0);
    
    const uint SAMPLE_COUNT = 1024u;
    for(uint i = 0u; i < SAMPLE_COUNT; ++i) {
        // generates a sample vector that's biased towards the
        // preferred alignment direction (importance sampling).
        vec2 Xi = Hammersley(i, SAMPLE_COUNT);
        vec3 H = ImportanceSampleGGX(Xi, N, roughness);
        vec3 L = normalize(2.0 * dot(V, H) * H - V);

        float NdotL = max(L.z, 0.0);
        float NdotH = max(H.z, 0.0);
        float VdotH = max(dot(V, H), 0.0);

        if(NdotL > 0.0) {
            float G = GeometrySmith(N, V, L, roughness);
            float G_Vis = (G * VdotH) / (NdotH * NdotV);
            float Fc = pow(1.0 - VdotH, 5.0);

            result.x += (1.0 - Fc) * G_Vis;
            result.y += Fc * G_Vis;
        }
    }
    
    result /= float(SAMPLE_COUNT);
    return result;
}

void main() {
    vec2 integratedBRDF = IntegrateBRDF(vTexcoord.x, vTexcoord.y);
    FragColor.rg = integratedBRDF;
}

