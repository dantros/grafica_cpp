/**
 * @file easy_shaders.cpp
 * @brief Many simple shader programs for quick prototyping
 * 
 * @author Daniel Calderón
 * @license MIT
*/

#include "easy_shaders.h"
#include "root_directory.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace Grafica
{

GLuint textureSimpleSetup(
    const std::filesystem::path& imgPath,
    GLuint sWrapMode,
    GLuint tWrapMode,
    GLuint minFilterMode,
    GLuint maxFilterMode)
{
    // wrapMode: GL_REPEAT, GL_CLAMP_TO_EDGE
    // filterMode: GL_LINEAR, GL_NEAREST

    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    // texture wrapping params
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, sWrapMode);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, tWrapMode);

    // texture filtering params
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilterMode);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, maxFilterMode);

    // load image, create texture and generate mipmaps
	int width, height, nrChannels;

    std::cout << "std::filesystem::path " << imgPath << std::endl;
    std::cout << " imgPath.string() " << imgPath.string() << std::endl;
    std::cout << " imgPath.string().c_str() " << imgPath.string().c_str() << std::endl;

    unsigned char *data = stbi_load(imgPath.string().c_str(), &width, &height, &nrChannels, 0);
    if (data == nullptr)
	{
        std::cout << "Failed to load texture" << std::endl;
        throw;
	}

    GLuint format;
    switch (nrChannels)
    {
    case 3:
        format = GL_RGB;
        break;
    case 4:
        format = GL_RGBA;
        break;
    default:
        std::cout << "nrChannels: " << nrChannels << std::endl;
        throw;
    };

    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(data);

    return texture;
}

void PositionColorVAO::setupVAO(GPUShape& gpuShape) const
{
    // Binding VAO to setup
    glBindVertexArray(gpuShape.vao);

    // Binding buffers to the current VAO
    glBindBuffer(GL_ARRAY_BUFFER, gpuShape.vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gpuShape.ebo);

    // position attribute
    auto position = glGetAttribLocation(shaderProgram, "position");
    glVertexAttribPointer(position, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)0);
    glEnableVertexAttribArray(position);

    // color attribute
    auto color = glGetAttribLocation(shaderProgram, "color");
    glVertexAttribPointer(color, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(color);

    // Unbinding current VAO
    glBindVertexArray(0);
}

void PositionColorVAO::drawCall(const GPUShape& gpuShape, GLuint mode) const
{
    // Binding the VAO and executing the draw call
    glBindVertexArray(gpuShape.vao);
    glDrawElements(mode, gpuShape.size, GL_UNSIGNED_INT, nullptr);

    // Unbind the current VAO
    glBindVertexArray(0);
}

SimpleShaderProgram::SimpleShaderProgram()
{
    const std::string vertexShaderCode = R"(
        #version 130
        in vec3 position;
        in vec3 color;
        
        out vec3 fragColor;
        
        void main()
        {
            fragColor = color;
            gl_Position = vec4(position, 1.0f);
        }
    )";

    const std::string fragmentShaderCode = R"(
        #version 130
        
        in vec3 fragColor;
        out vec4 outColor;
        
        void main()
        {
            outColor = vec4(fragColor, 1.0f);
        }
    )";

    shaderProgram = createShaderProgramFromCode({
        {GL_VERTEX_SHADER, vertexShaderCode.c_str()},
        {GL_FRAGMENT_SHADER, fragmentShaderCode.c_str()}
    });
}

TransformShaderProgram::TransformShaderProgram()
{
    const std::string vertexShaderCode = R"(
        #version 130                                        
                                                            
        in vec3 position;                                   
        in vec3 color;                                      
        out vec3 fragColor;                                 
        uniform mat4 transform;                             
                                                            
        void main()                                         
        {                                                   
            fragColor = color;                              
            gl_Position = transform * vec4(position, 1.0f); 
        }
    )";

    const std::string fragmentShaderCode = R"(
        #version 130                         
                                             
        in vec3 fragColor;                   
        out vec4 outColor;                   
                                             
        void main()                          
        {                                    
            outColor = vec4(fragColor, 1.0f);
        }
    )";

    shaderProgram = createShaderProgramFromCode({
        {GL_VERTEX_SHADER, vertexShaderCode.c_str()},
        {GL_FRAGMENT_SHADER, fragmentShaderCode.c_str()}
    });
}

ModelViewProjectionShaderProgram::ModelViewProjectionShaderProgram()
{
    const std::string vertexShaderCode = R"(
        #version 130                                                       
                                                                           
        uniform mat4 projection;                                           
        uniform mat4 view;                                                 
        uniform mat4 model;                                                
        in vec3 position;                                                  
        in vec3 color;                                                     
        out vec3 newColor;                                                 
                                                                           
        void main()                                                        
        {                                                                  
            gl_Position = projection * view * model * vec4(position, 1.0f);
            newColor = color;                                              
        }
    )";

    const std::string fragmentShaderCode = R"(
        #version 130                        
                                            
        in vec3 newColor;                   
        out vec4 outColor;                  
                                            
        void main()                         
        {                                   
            outColor = vec4(newColor, 1.0f);
        }
    )";

    shaderProgram = createShaderProgramFromCode({
        {GL_VERTEX_SHADER, vertexShaderCode.c_str()},
        {GL_FRAGMENT_SHADER, fragmentShaderCode.c_str()}
    });
}

void PositionTextureVAO::setupVAO(GPUShape& gpuShape) const
{
    // Binding VAO to setup
    glBindVertexArray(gpuShape.vao);

    // Binding buffers to the current VAO
    glBindBuffer(GL_ARRAY_BUFFER, gpuShape.vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gpuShape.ebo);

    // position attribute
    auto position = glGetAttribLocation(shaderProgram, "position");
    glVertexAttribPointer(position, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)0);
    glEnableVertexAttribArray(position);

    // texCoords attribute
    auto texCoords = glGetAttribLocation(shaderProgram, "texCoords");
    glVertexAttribPointer(texCoords, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(texCoords);

    // Unbinding current VAO
    glBindVertexArray(0);
}

void PositionTextureVAO::drawCall(const GPUShape& gpuShape, GLuint mode) const
{
    // Binding the VAO and texture
    glBindVertexArray(gpuShape.vao);
    glBindTexture(GL_TEXTURE_2D, gpuShape.texture);

    // Executing the draw call
    glDrawElements(mode, gpuShape.size, GL_UNSIGNED_INT, nullptr);

    // Unbind the current VAO
    glBindVertexArray(0);
}

TextureTransformShaderProgram::TextureTransformShaderProgram()
{
    const std::string vertexShaderCode = R"(
        #version 130                                       
                                                           
        uniform mat4 transform;                            
        in vec3 position;                                  
        in vec2 texCoords;                                 
        out vec2 outTexCoords;                             
                                                           
        void main()                                        
        {                                                  
            gl_Position = transform * vec4(position, 1.0f);
            outTexCoords = texCoords;                      
        }                                                  
    )";

    const std::string fragmentShaderCode = R"(
        #version 130                                     
                                                         
        uniform sampler2D samplerTex;                    
        in vec2 outTexCoords;                            
        out vec4 outColor;                               
                                                         
        void main()                                      
        {                                                
            outColor = texture(samplerTex, outTexCoords);
        }                                                
    )";

    shaderProgram = createShaderProgramFromCode({
        {GL_VERTEX_SHADER, vertexShaderCode.c_str()},
        {GL_FRAGMENT_SHADER, fragmentShaderCode.c_str()}
    });
}

void PositionColorNormalVAO::setupVAO(GPUShape& gpuShape) const
{
    // Binding VAO to setup
    glBindVertexArray(gpuShape.vao);

    // Binding buffers to the current VAO
    glBindBuffer(GL_ARRAY_BUFFER, gpuShape.vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gpuShape.ebo);

    // position attribute
    auto position = glGetAttribLocation(shaderProgram, "position");
    glVertexAttribPointer(position, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (void*)0);
    glEnableVertexAttribArray(position);

    // color attribute
    auto color = glGetAttribLocation(shaderProgram, "color");
    glVertexAttribPointer(color, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(color);

    // normal attribute
    auto normal = glGetAttribLocation(shaderProgram, "normal");
    glVertexAttribPointer(normal, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (void*)(6 * sizeof(GLfloat)));
    glEnableVertexAttribArray(normal);

    // Unbinding current VAO
    glBindVertexArray(0);
}

void PositionColorNormalVAO::drawCall(const GPUShape& gpuShape, GLuint mode) const
{
    // Binding the VAO and texture
    glBindVertexArray(gpuShape.vao);

    // Executing the draw call
    glDrawElements(mode, gpuShape.size, GL_UNSIGNED_INT, nullptr);

    // Unbind the current VAO
    glBindVertexArray(0);
}

PhongColorShaderProgram::PhongColorShaderProgram()
{
    const std::string vertexShaderCode = R"(
        #version 330 core                                             
                                                                      
        layout (location = 0) in vec3 position;                       
        layout (location = 1) in vec3 color;                          
        layout (location = 2) in vec3 normal;                         
        out vec3 fragPosition;                                        
        out vec3 fragOriginalColor;                                   
        out vec3 fragNormal;                                          
        uniform mat4 model;                                           
        uniform mat4 view;                                            
        uniform mat4 projection;                                      
                                                                      
        void main()                                                   
        {                                                             
            fragPosition = vec3(model * vec4(position, 1.0));         
            fragOriginalColor = color;                                
            fragNormal = mat3(transpose(inverse(model))) * normal;    
            gl_Position = projection * view * vec4(fragPosition, 1.0);
        }                                                             
    )";

    const std::string fragmentShaderCode = R"(
        #version 330 core                                                                            
                                                                                                     
        out vec4 fragColor;                                                                          
                                                                                                     
        in vec3 fragNormal;                                                                          
        in vec3 fragPosition;                                                                        
        in vec3 fragOriginalColor;                                                                   
                                                                                                     
        uniform vec3 lightPosition;                                                                  
        uniform vec3 viewPosition;                                                                   
        uniform vec3 La;                                                                             
        uniform vec3 Ld;                                                                             
        uniform vec3 Ls;                                                                             
        uniform vec3 Ka;                                                                             
        uniform vec3 Kd;                                                                             
        uniform vec3 Ks;                                                                             
        uniform uint shininess;                                                                      
        uniform float constantAttenuation;                                                           
        uniform float linearAttenuation;                                                             
        uniform float quadraticAttenuation;                                                          
                                                                                                     
        void main()                                                                                  
        {                                                                                            
            // ambient                                                                               
            vec3 ambient = Ka * La;                                                                  
                                                                                                     
            // diffuse                                                                               
            // fragment normal has been interpolated, so it does not necessarily have norm equal to 1
            vec3 normalizedNormal = normalize(fragNormal);                                           
            vec3 toLight = lightPosition - fragPosition;                                             
            vec3 lightDir = normalize(toLight);                                                      
            float diff = max(dot(normalizedNormal, lightDir), 0.0);                                  
            vec3 diffuse = Kd * Ld * diff;                                                           
                                                                                                     
            // specular                                                                              
            vec3 viewDir = normalize(viewPosition - fragPosition);                                   
            vec3 reflectDir = reflect(-lightDir, normalizedNormal);                                  
            float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);                         
            vec3 specular = Ks * Ls * spec;                                                          
                                                                                                     
            // attenuation                                                                           
            float distToLight = length(toLight);
            float attenuation = constantAttenuation                                                  
                + linearAttenuation * distToLight                                                    
                + quadraticAttenuation * distToLight * distToLight;                                  
                                                                                                     
            vec3 result = (ambient + ((diffuse + specular) / attenuation)) * fragOriginalColor;      
            fragColor = vec4(result, 1.0);                                                           
        }                                                                                            
    )";

    shaderProgram = createShaderProgramFromCode({
        {GL_VERTEX_SHADER, vertexShaderCode.c_str()},
        {GL_FRAGMENT_SHADER, fragmentShaderCode.c_str()}
    });
}


void PositionTextureNormalVAO::setupVAO(GPUShape& gpuShape) const
{
    // Binding VAO to setup
    glBindVertexArray(gpuShape.vao);

    // Binding buffers to the current VAO
    glBindBuffer(GL_ARRAY_BUFFER, gpuShape.vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gpuShape.ebo);

    // position attribute
    auto position = glGetAttribLocation(shaderProgram, "position");
    glVertexAttribPointer(position, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)0);
    glEnableVertexAttribArray(position);

    // texCoords attribute
    auto texCoords = glGetAttribLocation(shaderProgram, "texCoords");
    glVertexAttribPointer(texCoords, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(texCoords);

    // normal attribute
    auto normal = glGetAttribLocation(shaderProgram, "normal");
    glVertexAttribPointer(normal, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(5 * sizeof(GLfloat)));
    glEnableVertexAttribArray(normal);

    // Unbinding current VAO
    glBindVertexArray(0);
}

void PositionTextureNormalVAO::drawCall(const GPUShape& gpuShape, GLuint mode) const
{
    // Binding the VAO and texture
    glBindVertexArray(gpuShape.vao);
    glBindTexture(GL_TEXTURE_2D, gpuShape.texture);

    // Executing the draw call
    glDrawElements(mode, gpuShape.size, GL_UNSIGNED_INT, nullptr);

    // Unbind the current VAO
    glBindVertexArray(0);
}

PhongTextureShaderProgram::PhongTextureShaderProgram()
{
    const std::string vertexShaderCode = R"(
        #version 330 core                                             
                                                                      
        layout (location = 0) in vec3 position;                       
        layout (location = 1) in vec2 texCoords;                      
        layout (location = 2) in vec3 normal;                         
        out vec3 fragPosition;                                        
        out vec2 fragTexCoords;                                       
        out vec3 fragNormal;                                          
        uniform mat4 model;                                           
        uniform mat4 view;                                            
        uniform mat4 projection;                                      
                                                                      
        void main()                                                   
        {                                                             
            fragPosition = vec3(model * vec4(position, 1.0));         
            fragTexCoords = texCoords;                                
            fragNormal = mat3(transpose(inverse(model))) * normal;    
            gl_Position = projection * view * vec4(fragPosition, 1.0);
        }                                                             
    )";

    const std::string fragmentShaderCode = R"(
        #version 330 core                                                                            
                                                                                                     
        out vec4 fragColor;                                                                          
                                                                                                     
        in vec3 fragNormal;                                                                          
        in vec2 fragTexCoords;                                                                       
        in vec3 fragPosition;                                                                        
                                                                                                     
        uniform vec3 lightPosition;                                                                  
        uniform vec3 viewPosition;                                                                   
        uniform vec3 La;                                                                             
        uniform vec3 Ld;                                                                             
        uniform vec3 Ls;                                                                             
        uniform vec3 Ka;                                                                             
        uniform vec3 Kd;                                                                             
        uniform vec3 Ks;                                                                             
        uniform uint shininess;                                                                      
        uniform float constantAttenuation;                                                           
        uniform float linearAttenuation;                                                             
        uniform float quadraticAttenuation;                                                          
                                                                                                     
        uniform sampler2D samplerTex;                                                                
                                                                                                     
        void main()                                                                                  
        {                                                                                            
            // ambient                                                                               
            vec3 ambient = Ka * La;                                                                  
                                                                                                     
            // diffuse                                                                               
            // fragment normal has been interpolated, so it does not necessarily have norm equal to 1
            vec3 normalizedNormal = normalize(fragNormal);                                           
            vec3 toLight = lightPosition - fragPosition;                                             
            vec3 lightDir = normalize(toLight);                                                      
            float diff = max(dot(normalizedNormal, lightDir), 0.0);                                  
            vec3 diffuse = Kd * Ld * diff;                                                           
                                                                                                     
            // specular                                                                              
            vec3 viewDir = normalize(viewPosition - fragPosition);                                   
            vec3 reflectDir = reflect(-lightDir, normalizedNormal);                                  
            float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);                         
            vec3 specular = Ks * Ls * spec;                                                          
                                                                                                     
            // attenuation                                                                           
            float distToLight = length(toLight);                                                     
            float attenuation = constantAttenuation                                                  
                + linearAttenuation * distToLight                                                    
                + quadraticAttenuation * distToLight * distToLight;                                  
                                                                                                     
            vec4 fragOriginalColor = texture(samplerTex, fragTexCoords);                             
                                                                                                     
            vec3 result = (ambient + ((diffuse + specular) / attenuation)) * fragOriginalColor.rgb;  
            fragColor = vec4(result, 1.0);                                                           
        }                                                                                            
    )";

    shaderProgram = createShaderProgramFromCode({
        {GL_VERTEX_SHADER, vertexShaderCode.c_str()},
        {GL_FRAGMENT_SHADER, fragmentShaderCode.c_str()}
    });
}

    
} //Grafica