#version 420 core
layout (location = 0) in vec3 aPos;

out vec3 FragPos;


uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat3 normalMatrix;

void main()
{
    FragPos = vec3(model * vec4(aPos, 1.0));
    gl_Position = projection * view * model * vec4(aPos, 1.0f);
}

