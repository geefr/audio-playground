   void mainImage( out vec4 fragColor, in vec2 fragCoord )
   {
       // Normalized pixel coordinates (from 0 to 1)
       vec2 uv = fragCoord/iResolution.xy;

       // Amplitude is stored in red channel, at y == 0.75
       // x represents time, seems to roughly be last frame time <= x <= current frame time?
       // (TODO) I believe each X pixel is a single sample from the input, so the texture may not
       // contain a whole frame worth of audio
       vec4 channel0Amplitude = texture(iChannel0, vec2(uv.x, 0.75));

       if( abs(0.5 - uv.y) > channel0Amplitude.r * 0.5 ) {
           fragColor = vec4(0.0,0.0,0.0,1.0); // vec4(0.5 + 0.5*cos(iTime+uv.xyx+vec3(0,2,4)), 1.0);
       } else {
           fragColor = channel0Amplitude;
       }
   }
