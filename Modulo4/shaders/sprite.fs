//C�digo fonte do Fragment Shader (em GLSL)
#version 450

// Informa��es recebidas do vertex shader
in vec3 scaledNormal;
in vec3 fragPos;
in vec2 texCoord;

// Propriedades do material do objeto
uniform vec3 ka;    // Coeficiente de reflex�o ambiental
uniform float kd;   // Coeficiente de reflex�o difusa
uniform vec3 ks;    // Coeficiente de reflex�o especular
uniform float q;    // Expoente especular

// Propriedades da fonte de luz
uniform vec3 lightPos;     // Posi��o da fonte de luz
uniform vec3 lightColor;   // Cor da luz

// Posi��o da c�mera 
uniform vec3 cameraPos;

// Buffer de sa�da (color buffer)
out vec4 color;

// Buffer de textura
uniform sampler2D colorBuffer;

void main()
{
    // Ambient
    vec3 ambient =  lightColor * ka;
    
    // Diffuse 
    vec3 N = normalize(scaledNormal);
    vec3 L = normalize(lightPos - fragPos);
    float diff = max(dot(N, L), 0.0);
    vec3 diffuse = diff * lightColor * kd;
    
    // Specular
    vec3 R = reflect(-L, N);
    vec3 V = normalize(cameraPos - fragPos);
    float spec = pow(max(dot(R, V), 0.0), q);
    vec3 specular = spec * ks * lightColor;
    
    // Calcula a cor da textura
    vec4 texColor = texture(colorBuffer, texCoord);
    
    // Combina os componentes de ilumina��o com a cor da textura
    vec3 result = (ambient + diffuse) * vec3(texColor) + specular;
    
    // Define a cor final do fragmento
    color = vec4(result, 1.0f);
}
