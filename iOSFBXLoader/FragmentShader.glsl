precision highp float;

varying vec4 vDestinationColor;

varying highp vec2 TexCoordOut;
uniform sampler2D Texture;

void main()
{
//      gl_FragColor = vDestinationColor; //vec4(1.0, 0.0, 0.0, 1.0);
    gl_FragColor = vDestinationColor * texture2D(Texture, TexCoordOut); //vec4(1.0, 0.0, 0.0, 1.0);
//    gl_FragColor = texture2D(Texture, TexCoordOut); //vec4(1.0, 0.0, 0.0, 1.0);
}