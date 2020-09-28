/*
Copyright 2020 Gareth Francis

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


// Get y value of line at x
float shapeY(float x) {
  return 0.5 + (0.5 * cos(x + (4.0 * iTime)));
}

// To render a circle - Get distance from circle's edge
float circleDistance( vec2 origin, float radius, vec2 coord ) {
    float d = distance( origin, coord );
    d -= distance( origin, origin + vec2(radius) );
    
    return d;
}

void renderCircle( out vec4 fragColor, vec2 origin, float radius, vec2 coord, float thickness ) {
	float cd = abs(circleDistance( origin, radius, coord ));
    if( cd < thickness ) {
        fragColor = vec4(1.0, 0.0, 1.0, 1.0);
    }   
}

void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
    vec2 uv = fragCoord / iResolution.xy;
        
    if( uv.y > 0.5 ) {
        uv.y = (uv.y * -1.0) + 1.0;
    }
    
    // FFT Value is stored in red channel, at y == 0.25
    // x represents the FFT bucket, so a basic spectrum display is just x,0.25 + cutoff
    // The fft sample to display at x
    float channel0FFTAtXTime = texture(iChannel0, vec2((uv.x + (0.2 * iTime)), 0.25)).r;
    float channel0FFTAtX = texture(iChannel0, vec2(uv.x, 0.25)).r;
    float channel0FFTAtY = texture(iChannel0, vec2(uv.y, 0.25)).r;
    
    // Amplitude is stored in red channel, at y == 0.75
    // x represents time, seems to roughly be last frame time <= x <= current frame time?
    // (TODO) I believe each X pixel is a single sample from the input, so the texture may not
    // contain a whole frame worth of audio
    float channel0Amplitude = texture(iChannel0, vec2(uv.x, 0.75)).r;
    
    // Time varying pixel color
    vec3 colSweep1 = 0.7 + 0.5*cos(iTime+uv.xyx+vec3(0,2,4));
    vec3 colSweep2 = 0.5 + 0.5*cos((2.0 * iTime + uv.y)+uv.xyx+vec3(1,6,3));
    vec3 colSweep3 = 0.5 + 0.5*cos((0.2 * iTime + channel0FFTAtX)+uv.xyx+vec3(4,2,2));
    
    // Colour to render
    vec4 fftColour = vec4(channel0FFTAtXTime);
    vec4 backgroundColour = vec4(channel0FFTAtY * 0.3);
    
    // Threshold (determines wave shape)
    float threshold = channel0FFTAtXTime;
    float relative = abs((uv.y - shapeY(uv.x)) * 4.0);
    
    float cd = abs(circleDistance( vec2(0.5,0.5), 0.2 + channel0FFTAtY / 6.0, uv ));
    if( cd  < (0.02 + channel0FFTAtY / 8.0) - (uv.y * 0.1) ) {
        float brightness = 0.6 + channel0FFTAtY * 2.0;
        fragColor = vec4(colSweep2, 1.0) * brightness;
    } else {
      fragColor = vec4(colSweep3, 1.0) * 0.2;   
    }
    
    /*
    if( relative > threshold ) {
        // renderCircle( fragColor, vec2(0.5, 0.5), 0.1, uv, 0.01 );
      // renderCircle( fragColor, vec2(0.0, 0.2), channel0FFTAtY / 3.0, uv, 0.1 );
      // renderCircle( fragColor, vec2(1.0, 0.2), channel0FFTAtY / 3.0, uv, 0.1 );
    } else {
      fragColor = fftColour;
    }*/
}
