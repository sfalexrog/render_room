#version 430

in vec2 texPos;
layout(location = 0) uniform sampler3D voxmap;
layout(location = 1) uniform sampler1D colormap;

out vec4 color;

int voxmapLevels = textureSize(voxmap, 0).y;

const float gaussian[5][5] = {{1, 4, 7, 4, 1}, {4, 16, 26, 16, 4}, {7, 26, 41, 26, 7},
                                {4, 16, 26, 16, 4}, {1, 4, 7, 4, 1}};

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
        float boxValue = 0;
        for(int x = -2; x <= 2; ++x)
        {
            for(int y = -2; y <= 2; ++y)
            {
                ivec3 texelPos = ivec3(vec3(texPos.x, 0, texPos.y) * textureSize(voxmap, 0)) + ivec3(x, i, y);
                boxValue += texelFetch(voxmap, texelPos, 0).r * gaussian[x + 1][y + 1] / 273.0;
            }
        }
        mixedValue += alpha * boxValue / 100.0;
    }
    //color = vec4(texelFetch(colormap, ivec2(mixedValue, 0), 0).rgb, 1.0);
    //color = vec4(mixedValue, mixedValue, mixedValue, 1.0);
    //color = vec4(vec3(mixedValue), 1.0);
    mixedValue = clamp(mixedValue, 1.0 / 512.0, 1.0);
    color = texture(colormap, mixedValue);
}
