void mainImage( out vec4 fragColor, in vec2 fragCoord )
   {
       // Normalized pixel coordinates (from 0 to 1)
       vec2 uv = fragCoord/iResolution.xy;

       // FFT Value is stored in red channel, at y == 0.25
       // x represents the FFT bucket, so a basic spectrum display is just x,0.25 + cutoff
       vec4 channel0FFT = texture(iChannel0, vec2(uv.x, 0.25));

       if( uv.y > channel0FFT.r ) {
         fragColor = vec4(0.0,0.0,0.0,1.0); // vec4(0.5 + 0.5*cos(iTime+uv.xyx+vec3(0,2,4)), 1.0);
       } else {
         fragColor = channel0FFT;
       }
   }
