vec2 rotate(vec2 v, float a, vec2 origin) {
       v = v - origin;
           float s = sin(a);
           float c = cos(a);
           mat2 m = mat2(c, -s, s, c);
           return (m * v) + origin;
   }

   float circleDistance( in vec2 origin, float r, in vec2 p ) {
     float dis = distance(origin, p);
     return dis < r ? 1.0 : dis;
   }

   bool circle( in vec2 origin, float r, in vec2 p ) {
     return distance(origin, p) < r;
   }

   void mainImage( out vec4 fragColor, in vec2 fragCoord )
   {
       // Normalized pixel coordinates (from 0 to 1)
       vec2 uv = fragCoord / iResolution.xy;

       // read frequency info (for le bass)
       vec4 bassFFT1 = texture( iChannel0, vec2(0.05, uv.y) );
       vec4 bassFFT2 = texture( iChannel0, vec2(0.10, uv.y) );
       vec4 bassFFT3 = texture( iChannel0, vec2(0.15 , uv.y) );

           // Mirror
       uv = uv.x > 0.5 ? vec2(1.0 - uv.x, uv.y) : uv.xy;

       // Rotate around the center
       uv = rotate(uv,(2.0 * sin(iTime * 0.5)) + (bassFFT1.x), vec2(0.0, 0.0));

       // Mirror again <3
       uv = uv.y > 0.5 ? vec2(uv.x, 1.0 - uv.y) : uv.xy;

       // Zoom/Repeat effect
       uv = fract(uv + cos(iTime) + ((bassFFT2.x * bassFFT1.x) / 2.0));

       // read frequency data from texture
       vec4 fft = texture( iChannel0, vec2(uv.x, uv.y) );

       vec3 targetColour = vec3(fft.x * sin(sqrt(uv.y)), fft.x * cos(iTime), fft.x * sin(iTime * 4.0));

       vec3 baseColour = vec3(0.0, 0.0, 0.0);

       vec3 col = mix(baseColour, targetColour, sqrt(uv.y));

       float stencil = circleDistance( vec2(0.3, 0.3), 0.1 * bassFFT2.x + fft.x * 0.1, uv.xy ) +
                       circleDistance( vec2(0.7, 0.3), 0.1 * bassFFT2.x + fft.x * 0.1, uv.xy ) +
                                   circleDistance( vec2(0.3, 0.7), 0.1 * bassFFT2.x + fft.x * 0.1, uv.xy ) +
                                   circleDistance( vec2(0.7, 0.7), 0.1 * bassFFT2.x + fft.x * 0.1, uv.xy )
                                   / 4.0;

       col = col * stencil;

       vec3 backgroundColour = vec3(0.0, 0.0, 0.0);
       col = mix(backgroundColour, col, stencil);

       // Output to screen
       fragColor = vec4(col,1.0);
   }
