/*
Copyright 2020 Gareth Francis

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

// Adjust a coordinate to compensate for the aspect ratio
// y will be -1.0 -> 1.0, x will not
vec2 correctAspect( vec2 coord ) {
    float xOff = 0.5 - coord.x;
    xOff *= iResolution.x / iResolution.y;
    coord.x = 0.5 + xOff;
    return coord;
}

// To render a circle - Get distance from circle's edge
float circleDistance( vec2 origin, float radius, vec2 coord ) {
    coord = correctAspect(coord);
    
    float d = distance( coord, origin );
    float rd = distance( origin + vec2(radius), origin );
    return d - rd;
}

// Angle between axis and coord, around center of screen
float angle( vec2 origin, vec2 axis, vec2 coord ) {
    vec2 a = normalize( axis );
    vec2 b = normalize( origin - correctAspect(coord) );
    return acos( dot(a, b) );
}

vec2 rotate(vec2 v, float a, vec2 origin) {
    v = v - origin;
	float s = sin(a);
	float c = cos(a);
	mat2 m = mat2(c, -s, s, c);
	return (m * v) + origin;
}

void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
    vec2 uv = fragCoord / iResolution.xy;
    
    // Tile
    // uv = vec2(fract(uv.x * 4.0), fract(uv.y * 4.0));
    vec2 circleOrigin = vec2(0.5, 0.4);
    // circleOrigin = vec2(fract(circleOrigin.x * 4.0), fract(circleOrigin.y * 4.0));
    
    // Sample fft for freq around circle outer
    float uvAnglePercent = angle(circleOrigin, vec2(0.0, -1.0), uv) / radians(180.0);
   
    // Zoom/Repeat effect
    // uv = fract(uv + cos(iTime / 8.0) /*+ ((bassFFT2.x * bassFFT1.x) / 2.0)*/);
    
    // FFT Value is stored in red channel, at y == 0.25
    // x represents the FFT bucket, so a basic spectrum display is just x,0.25 + cutoff
    // The fft sample to display at x
    float angleFFT = texture(iChannel0, vec2(uvAnglePercent, 0.25)).r;
    float channel0FFTAtXTime = texture(iChannel0, vec2((uv.x + (0.2 * iTime)), 0.25)).r;
    float channel0FFTAtX = texture(iChannel0, vec2(uv.x, 0.25)).r;
    float channel0FFTAtY = texture(iChannel0, vec2(uv.y, 0.25)).r;
    float channel0FFTBass1 = texture(iChannel0, vec2(0.01, 0.25)).r;
    
    // Amplitude is stored in red channel, at y == 0.75
    // x represents time, seems to roughly be last frame time <= x <= current frame time?
    // (TODO) I believe each X pixel is a single sample from the input, so the texture may not
    // contain a whole frame worth of audio
    float channel0AmplitudeAtX = texture(iChannel0, vec2(uv.x, 0.75)).r;
    float channel0AmplitudeAtY = texture(iChannel0, vec2(uv.y, 0.75)).r;
    
    // Time varying pixel color
    vec3 colSweep1 = 0.7 + 0.5*cos(iTime+uv.xyx+vec3(0,2,4));
    vec3 colSweep2 = 0.5 + 0.5*cos((0.6 * iTime + uv.y)+uv.xyx+vec3(1,6,3));
    
    // Colour to render
    vec4 fftColour = vec4(channel0FFTAtXTime);
    vec4 backgroundColour = vec4(vec3(0.0), 1.0);//vec4(0.2 * colSweep2, 1.0);
    vec4 centerColour = vec4(0.8, 0.2, 0.2, 1.0);//vec4(colSweep2, 1.0);
    vec3 backGridColour = vec3(0.2, 0.8, 0.2);
    
    // Threshold (determines wave shape)
    float threshold = channel0FFTAtXTime;
        
    float cd = circleDistance( circleOrigin, 0.15 + (0.02 * channel0FFTBass1), uv );
    
    // Inside the circle
    if( cd < 0.0 )
    {
      cd = abs(cd);
        
      fragColor = centerColour * cd * 6.0 * channel0FFTBass1;
      return;
    }
    
    // Circle's edge/spectrum
    if( cd < angleFFT * 0.25 )
    {
        float brightness = 1.0;//0.6 + channel0FFTAtY * 2.0;
        fragColor = vec4(colSweep2, 1.0) * brightness;
        return;
    }
    
    // Background aliasing pattern
    float ampR = distance((uv / 2.0), vec2(channel0AmplitudeAtX, channel0AmplitudeAtY));
    //float ampG = distance((vec2(fract(iTime * uv.x), uv.y) / 2.0), vec2(channel0AmplitudeAtX, channel0AmplitudeAtY));
    //float ampB = distance((vec2(uv.x, fract(iTime * uv.y)) / 2.0), vec2(channel0AmplitudeAtX, channel0AmplitudeAtY));
    float ampG = distance(uv / 4.0, vec2(channel0AmplitudeAtX, channel0AmplitudeAtY));
    float ampB = distance(uv / 8.0, vec2(channel0AmplitudeAtX, channel0AmplitudeAtY));
    fragColor = vec4(ampR, ampG, ampB, 1.0);
    return;
}
