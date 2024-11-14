#version 330 core

uniform sampler2D diffuseTex;
uniform sampler2D shadowTex;

uniform vec3 cameraPos;
uniform vec3 lightPos;
uniform vec4 lightColour;
uniform float lightRadius;

uniform int useTexture;

in Vertex
{
  vec4 colour;
  vec2 texCoord;
  vec3 normal;
  vec3 worldPos;
  vec4 shadowProj;
} IN;

out vec4 fragColour;

void main(void)
{
  if(useTexture > 0)
  {
  vec3 incident = normalize(lightPos - IN.worldPos);
  vec3 viewDir = normalize(cameraPos - IN.worldPos);
  vec3 halfDir = normalize(incident + viewDir);

  vec4 diffuse = texture(diffuseTex,IN.texCoord);

  float lambert = max(dot(incident,IN.normal),0.0f);
  float distance = length(lightPos - IN.worldPos);
  float attenuation = 1.0 - clamp(distance / lightRadius, 0.0 , 1.0 );

  float specFactor = clamp( dot(halfDir,IN.normal), 0.0 , 1.0 );
  specFactor = pow(specFactor,60.0);

  float shadow = 1.0;

  vec3 shadowNDC = IN.shadowProj.xyz / IN.shadowProj.w;
  if(abs(shadowNDC.x) < 1.0f && abs(shadowNDC.y) < 1.0f && abs(shadowNDC.z) < 1.0f)
  {
    vec3 biasCoord = shadowNDC * 0.5f + 0.5f;
    float shadowZ = texture(shadowTex,biasCoord.xy).x;
    if(shadowZ < biasCoord.z)
    {
      shadow = 0.0f;
    }
  }

  vec3 surface = (diffuse.rgb * attenuation * lightColour.rgb);
  fragColour.rgb = surface * lambert;
  fragColour.rgb += (lightColour.rbg * attenuation * specFactor) * 0.33;
  fragColour.rgb *= shadow;
  fragColour.rgb += surface * 0.1f;
  fragColour.a = diffuse.a;
  }
}
