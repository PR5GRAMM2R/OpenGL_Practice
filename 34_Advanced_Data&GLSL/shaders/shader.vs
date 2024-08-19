#version 420 core       // 330 버전 말고, 420 버전을 사용해야 binding 기능 사용 가능
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

layout (std140, binding = 0) uniform Matrices{       // Uniform Block
    mat4 model;
    mat4 view;
    mat4 projection;
};

// out vec3 Normal;
// out vec3 Position;

out VS_OUT{                 // Interface Blocks
    vec3 Normal;
    vec3 Position;
} vs_out;

// uniform mat4 model;
// uniform mat4 view;
// uniform mat4 projection;

void main()
{
    vs_out.Normal = mat3(transpose(inverse(model))) * aNormal;
    vs_out.Position = vec3(model * vec4(aPos, 1.0));
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    //gl_PointSize = gl_Position.z;
}