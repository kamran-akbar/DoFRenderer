#version 450 core

#define LAYER_COUNT 4

layout (triangles) in;
layout (triangle_strip) out;
layout (max_vertices = 3 * LAYER_COUNT) out;

in VS_OUT
{
	vec3 ourColor;
	vec2 TexCoord;
	vec3 normal;
	vec3 worldPos;
} gs_in[];

out GS_OUT
{
	vec3 ourColor;
	vec2 TexCoord;
	vec3 normal;
	vec3 worldPos;
} gs_out;

void main(){
	for(int c = 0; c < LAYER_COUNT; c++){
		for(int i = 0; i < gl_in.length(); i++) {
			gs_out.ourColor = gs_in[i].ourColor;
			gs_out.TexCoord = gs_in[i].TexCoord;
			gs_out.normal = gs_in[i].normal;
			gs_out.worldPos = gs_in[i].worldPos;
			gl_Position = gl_in[i].gl_Position;
			gl_Layer = c;
			EmitVertex();
		}
		EndPrimitive();
	}
}