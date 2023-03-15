varying vec2 TexCoord;
uniform vec3 color;

void main() {
   float s = TexCoord.s - 0.5;
   float t = TexCoord.t - 0.5;
   
   float distance = sqrt(s * s + t * t);
   float alpha = 1.0;
   
   if(distance > 0.5) {
      alpha = 0.0;
   }
   else {
      alpha = pow(1.0 - 2.0*distance, 3.0);
   }

   gl_FragColor = vec4(color, alpha);
}
