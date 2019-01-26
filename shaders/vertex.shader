
#version 330 core
  
layout (location = 0) in vec3 position;
//Used the logic from learnopengl.com
uniform mat4 camera_transformation_matrix;

void main()
{
    gl_Position = camera_transformation_matrix * vec4(position.x, position.y, position.z, 1.0);
}