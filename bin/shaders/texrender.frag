#version 430

in vec2 texPos;
layout(location = 0) uniform sampler3D voxmap;
layout(location = 1) uniform sampler1D colormap;

out vec4 color;

int voxmapLevels = textureSize(voxmap, 0).y;

void main() {
    float alpha = 1.0 / voxmapLevels;
    float mixedValue = 0.0;
    //for(int i = 0; i < voxmapLevels; ++i)
    //{
    //    vec3 layerPos = vec3(texPos.x, (1.0 / voxmapLevels) * (i + 0.5), texPos.y);
    //    mixedValue += texture(voxmap, layerPos).r;
    //}
    for(int i = 0; i < voxmapLevels; ++i)
    {
        ivec3 texelPos = ivec3(vec3(texPos.x, 0, texPos.y) * textureSize(voxmap, 0));
        texelPos.y = i;
        mixedValue += alpha * texelFetch(voxmap, texelPos, 0).r / 100.0;
    }
    //color = vec4(texelFetch(colormap, ivec2(mixedValue, 0), 0).rgb, 1.0);
    //color = vec4(mixedValue, mixedValue, mixedValue, 1.0);
    //color = vec4(vec3(mixedValue), 1.0);
    if (mixedValue == 0.0)
    {
        color = vec4(0.0, 0.0, 0.0, 1.0);
    }
    else
    {
        color = texture(colormap, mixedValue);
    }
}
