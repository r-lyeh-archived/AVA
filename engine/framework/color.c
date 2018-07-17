#include <stdint.h>

uint32_t rgba( uint8_t r, uint8_t g, uint8_t b, uint8_t a );
float alpha( uint32_t rgba );
float *srgba( float srgba[4], uint32_t rgba );

// ----------------------------------------------- 

#ifdef COLOR_C
#pragma once

uint32_t rgba( uint8_t r, uint8_t g, uint8_t b, uint8_t a ) {
    return r << 24 | g << 16 | b << 8 | a;
}

float alpha( uint32_t rgba ) {
    return ( rgba & 255 ) / 255.f;
}

float *srgba( float srgba[4], uint32_t rgba ) {
    srgba[0] = ((rgba >> 24) & 255) / 255.f;
    srgba[1] = ((rgba >> 16) & 255) / 255.f;
    srgba[2] = ((rgba >>  8) & 255) / 255.f;
    srgba[3] = ((rgba >>  0) & 255) / 255.f;
    return srgba;
}



/* @todo
    color (FF,E8,41)
    color_srgb(1.00,0.91,0.25) (srgb = color / 255.0)
    color_linear(1.0,0.8,0.05) ->
        correct:
            linear = srgb <= 0.04045 ? srgb / 12.92 : pow((C_srgb + 0.055) / 1.055, 2.4) 
        approx: (http://chilliant.blogspot.com.es/2012/08/srgb-approximations-for-hlsl.html)
            float3 RGB = sRGB * (sRGB * (sRGB * 0.305306011 + 0.682171111) + 0.012522878);
        approx reverse: (http://chilliant.blogspot.com.es/2012/08/srgb-approximations-for-hlsl.html)
          float3 S1 = sqrt(RGB);
          float3 S2 = sqrt(S1);
          float3 S3 = sqrt(S2);
          float3 sRGB = 0.585122381 * S1 + 0.783140355 * S2 - 0.368262736 * S3;
*/

#endif
