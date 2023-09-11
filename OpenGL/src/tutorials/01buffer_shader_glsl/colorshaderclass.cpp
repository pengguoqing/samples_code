////////////////////////////////////////////////////////////////////////////////
// Filename: colorshaderclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "colorshaderclass.h"
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

ColorShaderClass::ColorShaderClass()
{
    m_OpenGLPtr = 0;
}


ColorShaderClass::ColorShaderClass(const ColorShaderClass& other)
{
}


ColorShaderClass::~ColorShaderClass()
{
}


bool ColorShaderClass::Initialize(const shared_ptr<OpenGLClass>& opengl)
{
    // Store the pointer to the OpenGL object.
    m_OpenGLPtr = opengl;

    // Initialize the vertex and pixel shaders.
    bool result = InitializeShader();
    if(!result){
        return false;
    }

    return true;
}


void ColorShaderClass::Shutdown()
{
    // Shutdown the shader.
    ShutdownShader();

    return;
}


bool ColorShaderClass::InitializeShader()
{
    // Load the vertex shader and fragment shader source file
    string vs_filename{ "color.vs" }, ps_filename{"color.ps"};
    std::ifstream vfile(vs_filename, std::ios::binary);
    std::ifstream pfile(ps_filename, std::ios::binary);
    std::stringstream vstream, pstream;

    vstream << vfile.rdbuf();
    pstream << pfile.rdbuf();

    string vs_text {vstream.str()};
    string ps_text {pstream.str()};
    // Create a vertex and fragment shader object.
    m_vertexShader      = glCreateShader(GL_VERTEX_SHADER);
    m_fragmentShader    = glCreateShader(GL_FRAGMENT_SHADER);

	const char* vbuffer = vs_text.c_str();
	const char* pbuffer = ps_text.c_str();

    glShaderSource(m_vertexShader, 1, &vbuffer, nullptr);
    glShaderSource(m_fragmentShader, 1, &pbuffer, nullptr);

    // Release the vertex and fragment shader buffers.
    
    // Compile the shaders.
    glCompileShader(m_vertexShader);
    glCompileShader(m_fragmentShader);

    // Check to see if the vertex shader compiled successfully.
    int status;
    glGetShaderiv(m_vertexShader, GL_COMPILE_STATUS, &status);
    if(status != 1){
        return false;
    }

    // Check to see if the fragment shader compiled successfully.
    glGetShaderiv(m_fragmentShader, GL_COMPILE_STATUS, &status);
    if(status != 1){ 
        return false;
    }

    // Create a shader program object.
    m_shaderProgram = glCreateProgram();

    // Attach the vertex and fragment shader to the program object.
    glAttachShader(m_shaderProgram, m_vertexShader);
    glAttachShader(m_shaderProgram, m_fragmentShader);

    // Bind the shader input variables.
    glBindAttribLocation(m_shaderProgram, 0, "inputPosition");
    glBindAttribLocation(m_shaderProgram, 1, "inputColor");

    // Link the shader program.
    glLinkProgram(m_shaderProgram);

    // Check the status of the link.
    glGetProgramiv(m_shaderProgram, GL_LINK_STATUS, &status);
    if(status != 1)
    {
        // If it did not link then write the syntax error message out to a text file for review.
        OutputLinkerErrorMessage(m_shaderProgram);
        return false;
    }

    return true;
}


void ColorShaderClass::ShutdownShader()
{
    // Detach the vertex and fragment shaders from the program.
    glDetachShader(m_shaderProgram, m_vertexShader);
    glDetachShader(m_shaderProgram, m_fragmentShader);

    // Delete the vertex and fragment shaders.
    glDeleteShader(m_vertexShader);
    glDeleteShader(m_fragmentShader);

    // Delete the shader program.
    glDeleteProgram(m_shaderProgram);

    return;
}


void ColorShaderClass::OutputShaderErrorMessage(unsigned int shaderId, char* shaderFilename)
{
    long count;
    int logSize, error;
    char* infoLog;
    FILE* filePtr;


    // Get the size of the string containing the information log for the failed shader compilation message.
    glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &logSize);

    // Increment the size by one to handle also the null terminator.
    logSize++;

    // Create a char buffer to hold the info log.
    infoLog = new char[logSize];

    // Now retrieve the info log.
    glGetShaderInfoLog(shaderId, logSize, NULL, infoLog);

    // Open a text file to write the error message to.
    filePtr = fopen("shader-error.txt", "w");
	if(filePtr == NULL)
	{
        cout << "Error opening shader error message output file." << endl;
		return;
	}

    // Write out the error message.
    count = fwrite(infoLog, sizeof(char), logSize, filePtr);
    if(count != logSize)
	{
        cout << "Error writing shader error message output file." << endl;
		return;
	}

    // Close the file.
    error = fclose(filePtr);
    if(error != 0)
	{
        cout << "Error closing shader error message output file." << endl;
		return;
	}

    // Notify the user to check the text file for compile errors.
    cout << "Error compiling shader.  Check shader-error.txt for error message.  Shader filename: " << shaderFilename << endl;

    return;
}


void ColorShaderClass::OutputLinkerErrorMessage(unsigned int programId)
{
    long count;
    FILE* filePtr;
    int logSize, error;
    char* infoLog;


    // Get the size of the string containing the information log for the failed shader compilation message.
    glGetProgramiv(programId, GL_INFO_LOG_LENGTH, &logSize);

    // Increment the size by one to handle also the null terminator.
    logSize++;

    // Create a char buffer to hold the info log.
    infoLog = new char[logSize];

    // Now retrieve the info log.
    glGetProgramInfoLog(programId, logSize, NULL, infoLog);

    // Open a file to write the error message to.
    filePtr = fopen("linker-error.txt", "w");
	if(filePtr == NULL)
	{
        cout << "Error opening linker error message output file." << endl;
		return;
	}

    // Write out the error message.
    count = fwrite(infoLog, sizeof(char), logSize, filePtr);
    if(count != logSize)
	{
        cout << "Error writing linker error message output file." << endl;
		return;
	}

    // Close the file.
    error = fclose(filePtr);
    if(error != 0)
	{
        cout << "Error closing linker error message output file." << endl;
		return;
	}

    // Pop a message up on the screen to notify the user to check the text file for linker errors.
    cout << "Error linking shader program.  Check linker-error.txt for message." << endl;

    return;
}


bool ColorShaderClass::SetShaderParameters(const glm::mat4& world, const glm::mat4& view, const glm::mat4& pro)
{
    // Install the shader program as part of the current rendering state.
    glUseProgram(m_shaderProgram);

    // Set the world matrix in the vertex shader.
    int location  = glGetUniformLocation(m_shaderProgram, "worldMatrix");
    if(location == -1)
    {
        return false;
    }
    glUniformMatrix4fv(location, 1, false, &world[0][0]);

    // Set the view matrix in the vertex shader.
    location = glGetUniformLocation(m_shaderProgram, "viewMatrix");
    if(location == -1)
    {
        return false;
    }
    glUniformMatrix4fv(location, 1, false, &view[0][0]);

    // Set the projection matrix in the vertex shader.
    location = glGetUniformLocation(m_shaderProgram, "projectionMatrix");
    if(location == -1)
    {
        return false;
    }
    glUniformMatrix4fv(location, 1, false, &pro[0][0]);

    return true;
}
