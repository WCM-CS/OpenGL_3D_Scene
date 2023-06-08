#include <iostream>             // cout, cerr
#include <cstdlib>              // EXIT_FAILURE
#include <GL/glew.h>            // GLEW library: Used instead of glad, replaced glad in the cylinder files
#include <GLFW\glfw3.h>         // GLFW library

//Texture Includes
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// GLM Math Header inclusions
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "camera.h" // Camera class 
#include "cylinder.h"
#include "shader.h"

using namespace std; // Standard namespace

/*Shader program Macro*/
#ifndef GLSL
#define GLSL(Version, Source) "#version " #Version " core \n" #Source
#endif

// Unnamed namespace
namespace
{
    const char* const WINDOW_TITLE = "Walker's Final Project"; // Macro for window title

    // Variables for window width and height
    const int WINDOW_WIDTH = 800;
    const int WINDOW_HEIGHT = 600;
    bool p = true; //projection type variable 

    // Stores the GL data relative to a given mesh
    struct GLMesh
    {
        //Obelisk Data
        GLuint vao;   //For the Obelisk Data      // Handle for the vertex array object
        GLuint vboO;  //For the obelisk dta
        GLuint nVerticesO; // Oelisk indices
  
        //Plane Data
        GLuint vao2;    //For the plane data
        GLuint vbo;    //for the plane data     // Handle for the vertex buffer object
        GLuint nVertices;    // plane vertices
 
       //Rubiks Cube Data, Front, Back, Bottom sides
        GLuint vaoRC;   
        GLuint vboRC;  
        GLuint nVerticesRC;

        //Rubiks B Data, Right, Top sides
        GLuint vaoRC2;    // Handle for the vertex array object
        GLuint vboRC2;  
        GLuint nVerticesRC2; 
        //for third texture
        GLuint vaoRC3;    // Handle for the vertex array object
        GLuint vboRC3;
        GLuint nVerticesRC3;

        //Mesh for the Book
        GLuint vaoB; //Pages tex
        GLuint vboB;
        GLuint nVerticesB;

        GLuint vaoB2; //Front TEx
        GLuint vboB2;
        GLuint nVerticesB2;

        GLuint vaoB3; //Side Tex
        GLuint vboB3;
        GLuint nVerticesB3;

        GLuint vaoCy1; //Side Tex
        GLuint vboCy1;


    };

   
    // Main GLFW window
    GLFWwindow* gWindow = nullptr;
    // Triangle mesh data
    GLMesh gMesh;
    // Texture id
    GLuint gTextureWood;
    GLuint gTextureMarble;
    GLuint gTextureRubikA;
    GLuint gTextureRubikB;
    GLuint gTextureRubikC;
    GLuint gTexturePages;
    GLuint gTexturePages2;
    GLuint gTexturePages3;
    GLuint gTextureMark;
    GLuint gTextureLamp;
    GLuint gTextureBase;




    glm::vec2 gUVScaleWood(1.0f, 1.5f);
    glm::vec2 gUVScaleMarb(1.0f, 1.0f);
    glm::vec2 gUVScaleRubik(1.0f, 1.0f);
    glm::vec2 gUVScalePages(1.5f, 1.0f);
    glm::vec2 gUVScalePages2(1.0f, 1.0f);
    glm::vec2 gUVScaleMark(0.5f, 0.50f);

    // Shader program
    GLuint gProgramId;
    GLuint gProgramIdL;
    GLuint gProgramIdL2;
    GLuint gProgramIdL3;
    GLuint ourShader;




    // camera
    Camera gCamera(glm::vec3(3.0f, 0.0f, 20.0f));
    float gLastX = WINDOW_WIDTH / 2.0f;
    float gLastY = WINDOW_HEIGHT / 2.0f;
    bool gFirstMouse = true;

    // timing
    float gDeltaTime = 0.0f; // time between current frame and last frame
    float gLastFrame = 0.0f;


    // Subject position and scale
  //  glm::vec3 gPlanePosition(0.0f, 0.0f, 0.0f);
    //glm::vec3 gPlaneScale(2.0f);

   

    // Light position and scale
    glm::vec3 gLightColor(1.0f, 1.0f, 0.7f);
    glm::vec3 gLightPosition(20.0f, 10.0f, -5.0f);
    glm::vec3 gLightScale(0.3f);
    // Light position and scale
    glm::vec3 gLightColor2(0.2f, 0.20f, 0.20f);
    glm::vec3 gLightPosition2(-7.0, 20.0f, -2.0f);
    glm::vec3 gLightScale2(0.3f);


}

/* User-defined Function prototypes to:
 * initialize the program, set the window size,
 * redraw graphics on the window when resized,
 * and render graphics on the screen
 */
bool UInitialize(int, char* [], GLFWwindow** window);
void UResizeWindow(GLFWwindow* window, int width, int height);
void UProcessInput(GLFWwindow* window);
void UMousePositionCallback(GLFWwindow* window, double xpos, double ypos);
void UMouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
void UMouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
void UCreateMesh(GLMesh& mesh);
void UDestroyMesh(GLMesh& mesh);
bool UCreateTexture(const char* filename, GLuint& textureId);
void UDestroyTexture(GLuint textureId);
//void URender();
void UMakePlane();
void UMakeObelisk();
void UMakeRubik();
void UMakeBook();
void UMakeBookMark();
//void UMakeCylinder();

bool UCreateShaderProgram(const char* vtxShaderSource, const char* fragShaderSource, GLuint& programId);
void UDestroyShaderProgram(GLuint programId);

const GLchar* verShader = GLSL(440,
    layout(location = 0) in vec3 aPos;
    layout(location = 1) in vec3 aNormal;

    out vec3 FragPos;
    out vec3 Normal;

    uniform mat4 model;
    uniform mat4 view;
    uniform mat4 projection;

void main()
{
    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = aNormal;
    gl_Position = projection * view * vec4(FragPos, 1.0);
});

/* Fragment Shader Source Code*/
const GLchar* fragShader = GLSL(440,
    out vec4 FragColor;

    in vec3 Normal;
    in vec3 FragPos;

    uniform vec3 lightPos;
    uniform vec3 lightColor;
    uniform vec3 objectColor;

void main()
{
    // ambient
    float ambientStrength = 0.4;
    vec3 ambient = ambientStrength * lightColor;

    // diffuse 
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    vec3 result = (ambient + diffuse) * objectColor;
    FragColor = vec4(result, 1.0);
});



/* Vertex Shader Source Code*/
const GLchar* vertexShaderSource = GLSL(440,
    layout(location = 0) in vec3 position; // VAP position 0 for vertex position data
    layout(location = 1) in vec3 normal; // VAP position 1 for normals
    layout(location = 2) in vec2 textureCoordinate;


    out vec3 vertexNormal; // For outgoing normals to fragment shader
    out vec3 vertexFragmentPos; // For outgoing color / pixels to fragment shader
    out vec2 vertexTextureCoordinate;
    
    //Uniform / Global variables for the  transform matrices
    uniform mat4 model;
    uniform mat4 view;
    uniform mat4 projection;
    uniform mat4 lightSpaceMatrix;


void main()
{
    gl_Position = projection * view * model * vec4(position, 1.0f); // Transforms vertices into clip coordinates

    vertexFragmentPos = vec3(model * vec4(position, 1.0f)); // Gets fragment / pixel position in world space only (exclude view and projection)

    vertexNormal = mat3(transpose(inverse(model))) * normal; // get normal vectors in world space only and exclude normal translation properties
    vertexTextureCoordinate = textureCoordinate;

    //vec4 shadowCoords = lightSpaceMatrix * vec4(vertexFragmentPos, 1.0f);
 

    // Pass shadow coordinates to fragment shader
    //gl_Position = shadowCoords;
}
);

/* Fragment Shader Source Code*/
const GLchar* fragmentShaderSource = GLSL(440,
    in vec3 vertexNormal; // For incoming normals
    in vec3 vertexFragmentPos; // For incoming fragment position
    in vec2 vertexTextureCoordinate;

    out vec4 fragmentColor; // For outgoing cube color to the GPU
    
    // Uniform / Global variables for object color, light color, light position, and camera/view position
    uniform vec3 objectColor;
    uniform vec3 lightColor;
    uniform vec3 lightPos;

    uniform vec3 lightColor2;
    uniform vec3 lightPos2;


    uniform vec3 lightDir;
    uniform vec3 viewPosition;

   uniform float specularIntensity = 0.5f; 
   uniform float specularIntensity2 = 0.8f;
    //uniform float specularIntensity2 = 1.0f;
    uniform float highlightSize = 5.0f;
    uniform float highlightSize2 = 8.0f;
    uniform sampler2D uTexture; // Useful when working with multiple textures
    uniform vec2 uvScale;

  //  uniform sampler2DShador shadowMap;
    //uniform float shadowBias = 0.0005f;

void main()
{   
    /*Phong lighting model calculations to generate ambient, diffuse, and specular components*/

    //float bias = max(0.05 * (1.0 - dot(vertexNormal, lightDir)), shadowBias);

      //Calculate Ambient lighting*/
    float ambientStrength1 = 0.2f; // Set ambient or global lighting strength
    float ambientStrength2 = 0.4f;
    vec3 ambient = (ambientStrength1 * lightColor) + (ambientStrength2 * lightColor2); // Generate ambient light color

    //Calculate Diffuse lighting*/
    vec3 norm = normalize(vertexNormal); // Normalize vectors to 1 unit
    vec3 lightDirection = normalize(lightPos - vertexFragmentPos); // Calculate distance (light direction) between light source and fragments/pixels on cube
    vec3 lightDirection2 = normalize(lightDir);
    

    float impact = max(dot(norm, lightDirection), 0.0);// Calculate diffuse impact by generating dot product of normal and light
    float impact2 = max(dot(norm, lightDirection2), 0.0);
    vec3 diffuse = (impact * lightColor) + (impact2 * lightColor2);; // Generate diffuse light color

    //Calculate Specular lighting*/
    vec3 viewDir = normalize(viewPosition - vertexFragmentPos); // Calculate view direction
    vec3 reflectDir = reflect(-lightDirection, norm);// Calculate reflection vector
    vec3 reflectDir2 = reflect(-lightDirection2, norm);
    //Calculate specular component
    float specularComponent = pow(max(dot(viewDir, reflectDir), 0.0), highlightSize);
    float specularComponent2 = pow(max(dot(viewDir, reflectDir2), 0.0), highlightSize2);

    vec3 specular = (specularIntensity * specularComponent * lightColor) + (specularIntensity2 * specularComponent2 * lightColor2);

    // Texture holds the color to be used for all three components
    vec4 textureColor = texture(uTexture, vertexTextureCoordinate * uvScale);
    // Calculate phong result
    vec3 phong = (ambient + diffuse + specular) * textureColor.xyz;
    fragmentColor = vec4(phong,  1.0f); // Send lighting results to GPU
}
);


/* Lamp Shader Source Code*/
const GLchar* lampVertexShaderSource = GLSL(440,

    layout(location = 0) in vec3 position; // VAP position 0 for vertex position data

    //Uniform / Global variables for the  transform matrices
    uniform mat4 model;
    uniform mat4 view;
    uniform mat4 projection;
void main()
{
    gl_Position = projection * view * model * vec4(position, 1.0f); // Transforms vertices into clip coordinates
}
);


/* Fragment Shader Source Code*/
const GLchar* lampFragmentShaderSource = GLSL(440,

    out vec4 fragmentColor; // For outgoing lamp color (smaller cube) to the GPU

void main()
{
    fragmentColor = vec4(1.0f, 1.0f, 0.8f, 1.0f); // Set color to white (1.0f,1.0f,1.0f) with alpha 1.0
}
);

/* Lamp Shader Source Code*/
const GLchar* lampVertexShaderSource2 = GLSL(440,

    layout(location = 0) in vec3 position; // VAP position 0 for vertex position data

//Uniform / Global variables for the  transform matrices
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
void main()
{
    gl_Position = projection * view * model * vec4(position, 1.0f); // Transforms vertices into clip coordinates
}
);


/* Fragment Shader Source Code*/
const GLchar* lampFragmentShaderSource2 = GLSL(440,

    out vec4 fragmentColor; // For outgoing lamp color (smaller cube) to the GPU

void main()
{
    fragmentColor = vec4(0.2f, 0.2f, 0.2f, 1.0f); // Set color to white (1.0f,1.0f,1.0f) with alpha 1.0
}
);


// Images are loaded with Y axis going down, but OpenGL's Y axis goes up, so let's flip it
void flipImageVertically(unsigned char* image, int width, int height, int channels)
{
    for (int j = 0; j < height / 2; ++j)
    {
        int index1 = j * width * channels;
        int index2 = (height - 1 - j) * width * channels;

        for (int i = width * channels; i > 0; --i)
        {
            unsigned char tmp = image[index1];
            image[index1] = image[index2];
            image[index2] = tmp;
            ++index1;
            ++index2;
        }
    }
}


int main(int argc, char* argv[])
{
    if (!UInitialize(argc, argv, &gWindow))
        return EXIT_FAILURE;

    // Create the mesh
    UCreateMesh(gMesh); // Calls the function to create the Vertex Buffer Object

    // Create the shader program
    if (!UCreateShaderProgram(vertexShaderSource, fragmentShaderSource, gProgramId))
        return EXIT_FAILURE;
    if (!UCreateShaderProgram(lampVertexShaderSource, lampFragmentShaderSource, gProgramIdL))
        return EXIT_FAILURE;
    if (!UCreateShaderProgram(lampVertexShaderSource2, lampFragmentShaderSource2, gProgramIdL2))
        return EXIT_FAILURE;
    if (!UCreateShaderProgram(verShader, fragShader, gProgramIdL3))
        return EXIT_FAILURE;

    Shader ourShader("shaderfiles/7.3.camera.vs", "shaderfiles/7.3.camera.fs");
    Shader lightShader("shaderfiles/6.light_cube.vs", "shaderfiles/6.light_cube.fs");
    // tell opengl for each sampler to which texture unit it belongs to (only has to be done once)
   
   
    // Load texture Wood
    const char* texFilename = "textures/wood3.jpg";
    if (!UCreateTexture(texFilename, gTextureWood))
    {
        cout << "Failed to load texture " << texFilename << endl;
        return EXIT_FAILURE;
    }
    glUseProgram(gProgramId);
    glUniform1i(glGetUniformLocation(gProgramId, "uTexture1"), 0);

    //--------------------------------------------------------------------

    texFilename = "textures/marb2.jpg";
    if (!UCreateTexture(texFilename, gTextureMarble))
    {
        cout << "Failed to load texture " << texFilename << endl;
        return EXIT_FAILURE;
    }
    glUniform1i(glGetUniformLocation(gProgramId, "uTexture2"), 1);

    //--------------------------------------------------------------------

    texFilename = "textures/Rub5.png";
    if (!UCreateTexture(texFilename, gTextureRubikA))
    {
        cout << "Failed to load texture " << texFilename << endl;
        return EXIT_FAILURE;
    }
    glUniform1i(glGetUniformLocation(gProgramId, "uTexture3"), 2);

    //--------------------------------------------------------------------
    
    texFilename = "textures/RubikA.jpg";
    if (!UCreateTexture(texFilename, gTextureRubikB))
    {
        cout << "Failed to load texture " << texFilename << endl;
        return EXIT_FAILURE;
    }
    glUniform1i(glGetUniformLocation(gProgramId, "uTexture4"), 3);

    //--------------------------------------------------------------------

    texFilename = "textures/rt3.png";
    if (!UCreateTexture(texFilename, gTextureRubikC))
    {
        cout << "Failed to load texture " << texFilename << endl;
        return EXIT_FAILURE;
    }
    glUniform1i(glGetUniformLocation(gProgramId, "uTexture5"), 4);

    texFilename = "textures/bPages.jpg";
    if (!UCreateTexture(texFilename, gTexturePages))
    {
        cout << "Failed to load texture " << texFilename << endl;
        return EXIT_FAILURE;
    }
    glUniform1i(glGetUniformLocation(gProgramId, "uTexture6"), 5);

    texFilename = "textures/skingb.jpg";
    if (!UCreateTexture(texFilename, gTexturePages2))
    {
        cout << "Failed to load texture " << texFilename << endl;
        return EXIT_FAILURE;
    }
    glUniform1i(glGetUniformLocation(gProgramId, "uTexture7"), 6);

    texFilename = "textures/nBlueTex.jpg";
    if (!UCreateTexture(texFilename, gTexturePages3))
    {
        cout << "Failed to load texture " << texFilename << endl;
        return EXIT_FAILURE;
    }
    glUniform1i(glGetUniformLocation(gProgramId, "uTexture8"), 7);

    texFilename = "textures/rMark.jpg";
    if (!UCreateTexture(texFilename, gTextureMark))
    {
        cout << "Failed to load texture " << texFilename << endl;
        return EXIT_FAILURE;
    }
    glUniform1i(glGetUniformLocation(gProgramId, "uTexture9"), 8);

    texFilename = "textures/linen2.png";
    if (!UCreateTexture(texFilename, gTextureLamp))
    {
        cout << "Failed to load texture " << texFilename << endl;
        return EXIT_FAILURE;
    }
    ourShader.use();
    glUniform1i(glGetUniformLocation(ourShader.ID, "uTexture10"), 9);

    texFilename = "textures/blackM.jpg";
    if (!UCreateTexture(texFilename, gTextureBase))
    {
        cout << "Failed to load texture " << texFilename << endl;
        return EXIT_FAILURE;
    }
    ourShader.use();
    glUniform1i(glGetUniformLocation(ourShader.ID, "uTexture11"), 10);

    glm::mat4 model;
    glm::mat4 model2;
    glm::mat4 model3;
    // Sets the background color of the window to black (it will be implicitely used by glClear)
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    // render loop
    // -----------
    while (!glfwWindowShouldClose(gWindow))
    {
        // per-frame timing
        // --------------------
        float currentFrame = glfwGetTime();
        gDeltaTime = currentFrame - gLastFrame;
        gLastFrame = currentFrame;

        // input
        // -----
        UProcessInput(gWindow);
        // Enable z-depth
        glEnable(GL_DEPTH_TEST);

        // Clear the frame and z buffers
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //Lamp Shade
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, gTextureLamp);
        ourShader.use();
        // glm::mat4 projection = glm::perspective(glm::radians(45.0f), (GLfloat)WINDOW_WIDTH / (GLfloat)WINDOW_HEIGHT, 0.1f, 100.0f);
        //glm::mat4 projection = glm::perspective(glm::radians(gCamera.Zoom), (GLfloat)WINDOW_WIDTH / (GLfloat)WINDOW_HEIGHT, 0.1f, 100.0f);
        

        glm::mat4 projection; //must declare projection before it's defined in the if statement
        if (p) {
            projection = glm::perspective(glm::radians(gCamera.Zoom), (GLfloat)WINDOW_WIDTH / (GLfloat)WINDOW_HEIGHT, 0.1f, 100.0f);
        }
        else {
            projection = glm::ortho(-5.0f, 5.0f, -5.0f, 5.0f, 0.1f, 100.0f);
        }
        ourShader.setMat4("projection", projection);

        // camera/view transformation
        glm::mat4 view = gCamera.GetViewMatrix();


        ourShader.setMat4("view", view);
        // Creates a perspective projection

        glBindVertexArray(gMesh.vaoCy1);


        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 1.0f, -7.0f));

        ourShader.setMat4("model", model);

        static_meshes_3D::Cylinder C(5, 30, 15, true, true, true);
        // glEnable(GL_CULL_FACE);
         //glCullFace(GL_FRONT);

         //glFrontFace(GL_CW);
        C.render();
        glBindVertexArray(0);
        glUseProgram(0);

        //-----------------------------------------------------------------------
        //Lamp Base
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, gTextureBase);
        ourShader.use();
        // glm::mat4 projection = glm::perspective(glm::radians(45.0f), (GLfloat)WINDOW_WIDTH / (GLfloat)WINDOW_HEIGHT, 0.1f, 100.0f);
        //glm::mat4 projection = glm::perspective(glm::radians(gCamera.Zoom), (GLfloat)WINDOW_WIDTH / (GLfloat)WINDOW_HEIGHT, 0.1f, 100.0f);
        ourShader.setMat4("projection", projection);

        // camera/view transformation
        //glm::mat4 view = gCamera.GetViewMatrix();


        ourShader.setMat4("view", view);
        // Creates a perspective projection

        glBindVertexArray(gMesh.vaoCy1);

        model2 = glm::mat4(1.0f);
        model2 = glm::translate(model2, glm::vec3(0.0f, -7.0f, -7.0f));

        ourShader.setMat4("model", model2);

        static_meshes_3D::Cylinder C2(4, 20, 2, true, true, true);
        // glEnable(GL_CULL_FACE);
         //glCullFace(GL_FRONT);

         //glFrontFace(GL_CW);
        C2.render();
        glBindVertexArray(0);
        glUseProgram(0);

        //----------------------------------------------------------------------------------------------------------

        lightShader.use();
        lightShader.setMat4("projection", projection);
        lightShader.setMat4("view", view);
        model3 = glm::mat4(1.0f);
        model3 = glm::translate(model3, glm::vec3(0.0f, -7.0f, -7.0f));
        model3 = glm::scale(model, glm::vec3(0.2f)); // a smaller cube
        lightShader.setMat4("model", model3);

        //call to obelisk mesh
        glBindVertexArray(gMesh.vao);
        glDrawArrays(GL_TRIANGLES, 0, gMesh.nVerticesO);

        // Deactivate the Vertex Array Object
        glBindVertexArray(0);
        glUseProgram(0);


        glUseProgram(gProgramIdL3);
        GLint objectColorLoc = glGetUniformLocation(gProgramIdL3, "objectColor");
        GLint lightColorLoc = glGetUniformLocation(gProgramIdL3, "lightColor");
        GLint lightPositionLoc = glGetUniformLocation(gProgramIdL3, "lightPos");
        glUniform3f(objectColorLoc, 0.0f, 0.0f, 0.0f);
        glUniform3f(lightColorLoc,0.0f, 0.0f, 0.0f);
        glUniform3f(lightPositionLoc,0.0f, -7.0f, -7.0f);

        GLint modelLoc = glGetUniformLocation(gProgramIdL3, "model");
        GLint viewLoc = glGetUniformLocation(gProgramIdL3, "view");
        GLint projLoc = glGetUniformLocation(gProgramIdL3, "projection");

        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model3));
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

        //call to obelisk mesh
        glBindVertexArray(gMesh.vao);
        glDrawArrays(GL_TRIANGLES, 0, gMesh.nVerticesO);

        // Deactivate the Vertex Array Object
        glBindVertexArray(0);
        glUseProgram(0);


        //-----------------------------------------------------
        UMakePlane();
        UMakeObelisk();
        UMakeRubik();
        UMakeBook();
        UMakeBookMark();


         // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        glfwSwapBuffers(gWindow);    // Flips the the back buffer with the front buffer every frame.
       
        glfwPollEvents();
    }

    // Release mesh data
    UDestroyMesh(gMesh);
    // Release texture
    UDestroyTexture(gTextureWood);
    UDestroyTexture(gTextureMarble);
    UDestroyTexture(gTextureRubikA);
    UDestroyTexture(gTextureRubikB);
    UDestroyTexture(gTextureRubikC);
    UDestroyTexture(gTexturePages);
    UDestroyTexture(gTexturePages2);
    UDestroyTexture(gTexturePages3);
    UDestroyTexture(gTextureMark);


    // Release shader program
    UDestroyShaderProgram(gProgramId);
    UDestroyShaderProgram(gProgramIdL);
    exit(EXIT_SUCCESS); // Terminates the program successfully
}


// Initialize GLFW, GLEW, and create a window
bool UInitialize(int argc, char* argv[], GLFWwindow** window)
{
    // GLFW: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // GLFW: window creation
    // ---------------------
    * window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE, NULL, NULL);
    if (*window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return false;
    }
    glfwMakeContextCurrent(*window);
    glfwSetFramebufferSizeCallback(*window, UResizeWindow);
    glfwSetCursorPosCallback(*window, UMousePositionCallback);
    glfwSetScrollCallback(*window, UMouseScrollCallback);
    glfwSetMouseButtonCallback(*window, UMouseButtonCallback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(*window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // GLEW: initialize
    // ----------------
    // Note: if using GLEW version 1.13 or earlier
    glewExperimental = GL_TRUE;
    GLenum GlewInitResult = glewInit();

    if (GLEW_OK != GlewInitResult)
    {
        std::cerr << glewGetErrorString(GlewInitResult) << std::endl;
        return false;
    }

    // Displays GPU OpenGL version
    cout << "INFO: OpenGL Version: " << glGetString(GL_VERSION) << endl;

    return true;
}


// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
void UProcessInput(GLFWwindow* window)
{
    static const float cameraSpeed = 2.5f;

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        gCamera.ProcessKeyboard(FORWARD, gDeltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        gCamera.ProcessKeyboard(BACKWARD, gDeltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        gCamera.ProcessKeyboard(LEFT, gDeltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        gCamera.ProcessKeyboard(RIGHT, gDeltaTime);
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        gCamera.ProcessKeyboard(UP, gDeltaTime);
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        gCamera.ProcessKeyboard(DOWN, gDeltaTime); // I had to add the variable down to the camera function
    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) //Goes from ortho to perspective by toggling p
        p = !p;
}


// glfw: whenever the window size changed (by OS or user resize) this callback function executes
void UResizeWindow(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}


// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void UMousePositionCallback(GLFWwindow* window, double xpos, double ypos)
{
    if (gFirstMouse)
    {
        gLastX = xpos;
        gLastY = ypos;
        gFirstMouse = false;
    }

    float xoffset = xpos - gLastX;
    float yoffset = gLastY - ypos; // reversed since y-coordinates go from bottom to top

    gLastX = xpos;
    gLastY = ypos;

    gCamera.ProcessMouseMovement(xoffset, yoffset);
}


// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void UMouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    //gCamera.ProcessMouseScroll(yoffset);
    //Note This function has an error because if you go below zero movement speed it inverts the key inputs
    //I tried to add if statements the while loops to prevent this but it cause the keys to get stuck in inverse after the condition was met

    //Note: I fixed it 
    //The method has to be above the condition instead of within, this works since open GL camera functions are in a loop not amde in the function which is continuously checking the conditions 
    gCamera.ProcessMouseScroll2(yoffset);
}

// glfw: handle mouse button events
// --------------------------------
void UMouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    switch (button)
    {
    case GLFW_MOUSE_BUTTON_LEFT:
    {
        if (action == GLFW_PRESS)
            cout << "Left mouse button pressed" << endl;
        else
            cout << "Left mouse button released" << endl;
    }
    break;

    case GLFW_MOUSE_BUTTON_MIDDLE:
    {
        if (action == GLFW_PRESS)
            cout << "Middle mouse button pressed" << endl;
        else
            cout << "Middle mouse button released" << endl;
    }
    break;

    case GLFW_MOUSE_BUTTON_RIGHT:
    {
        if (action == GLFW_PRESS)
            cout << "Right mouse button pressed" << endl;
        else
            cout << "Right mouse button released" << endl;
    }
    break;

    default:
        cout << "Unhandled mouse button event" << endl;
        break;
    }
}

void UMakeBook() {
    //Draw Plane
    glBindVertexArray(gMesh.vaoB);
    // Set the shader to be used
    glUseProgram(gProgramId);

    // 1. Scales the object by 2
    glm::mat4 scale = glm::scale(glm::vec3(9.0f, 1.5f, 4.5f));
    // 2. Rotates shape by 180 degrees in the z axis, upside down to make it special 
    glm::mat4 rotation = glm::rotate(glm::radians(90.0f), glm::vec3(0.0, 1.0f, 0.0f));
    // 3. Place object at the origin
    glm::mat4 translation = glm::translate(glm::vec3(-15.0f, -6.5f, 5.0f));
    // Model matrix: transformations are applied right-to-left order
    glm::mat4 model = translation * rotation * scale;

    // camera/view transformation
    glm::mat4 view = gCamera.GetViewMatrix();

    //glm::mat4 projection = glm::perspective(glm::radians(gCamera.Zoom), (GLfloat)WINDOW_WIDTH / (GLfloat)WINDOW_HEIGHT, 0.1f, 100.0f);
    glm::mat4 projection;
    if (p) {
        projection = glm::perspective(glm::radians(gCamera.Zoom), (GLfloat)WINDOW_WIDTH / (GLfloat)WINDOW_HEIGHT, 0.1f, 100.0f);
    }
    else {
        projection = glm::ortho(-5.0f, 5.0f, -5.0f, 5.0f, 0.1f, 100.0f);
    }

    // Retrieves and passes transform matrices to the Shader program
    GLint modelLoc = glGetUniformLocation(gProgramId, "model");
    GLint viewLoc = glGetUniformLocation(gProgramId, "view");
    GLint projLoc = glGetUniformLocation(gProgramId, "projection");

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));


    GLint objectColorLoc = glGetUniformLocation(gProgramId, "objectColor");
    GLint lightColorLoc = glGetUniformLocation(gProgramId, "lightColor");
    GLint lightPositionLoc = glGetUniformLocation(gProgramId, "lightPos");
    GLint viewPositionLoc = glGetUniformLocation(gProgramId, "viewPosition");
    GLint directionLoc = glGetUniformLocation(gProgramId, "lightDir");
    GLint lightColorLoc2 = glGetUniformLocation(gProgramId, "lightColor2");
    GLint lightPositionLoc2 = glGetUniformLocation(gProgramId, "lightPos2");


    //glUniform3f(objectColorLoc, gObjectColor.r, gObjectColor.g, gObjectColor.b);
    glUniform3f(lightColorLoc, gLightColor.r, gLightColor.g, gLightColor.b);
    glUniform3f(lightPositionLoc, gLightPosition.x, gLightPosition.y, gLightPosition.z);

    glUniform3f(lightColorLoc2, gLightColor2.r, gLightColor2.g, gLightColor2.b);
    glUniform3f(lightPositionLoc2, gLightPosition2.x, gLightPosition2.y, gLightPosition2.z);

    const glm::vec3 cameraPosition = gCamera.Position;
    glUniform3f(viewPositionLoc, cameraPosition.x, cameraPosition.y, cameraPosition.z);


    glBindTexture(GL_TEXTURE_2D, gTexturePages);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glBindTexture(GL_TEXTURE_2D, 0);

    GLint UVScaleLoc = glGetUniformLocation(gProgramId, "uvScale");
    glUniform2fv(UVScaleLoc, 1, glm::value_ptr(gUVScalePages));


    // bind textures on corresponding texture units
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gTexturePages);

    glDrawArrays(GL_TRIANGLES, 0, gMesh.nVerticesB);

    glBindVertexArray(0);
    glUseProgram(0);
    //--------------------------------------------------------------------------------------------
    glBindVertexArray(gMesh.vaoB2);
    // Set the shader to be used
    glUseProgram(gProgramId);
    glBindTexture(GL_TEXTURE_2D, gTexturePages2);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glBindTexture(GL_TEXTURE_2D, 0);

    GLint UVScaleLoc2 = glGetUniformLocation(gProgramId, "uvScale");
    glUniform2fv(UVScaleLoc2, 1, glm::value_ptr(gUVScalePages2));


    // bind textures on corresponding texture units
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gTexturePages2);

    glDrawArrays(GL_TRIANGLES, 0, gMesh.nVerticesB2);

    glBindVertexArray(0);
    glUseProgram(0);

    //--------------------------------------------------------------------------------------------
    glBindVertexArray(gMesh.vaoB3);
    // Set the shader to be used
    glUseProgram(gProgramId);
    glBindTexture(GL_TEXTURE_2D, gTexturePages3);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glBindTexture(GL_TEXTURE_2D, 0);

    GLint UVScaleLoc3 = glGetUniformLocation(gProgramId, "uvScale");
    glUniform2fv(UVScaleLoc3, 1, glm::value_ptr(gUVScalePages));


    // bind textures on corresponding texture units
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gTexturePages3);

    glDrawArrays(GL_TRIANGLES, 0, gMesh.nVerticesB3);

    glBindVertexArray(0);
    glUseProgram(0);

    //---------------------------------------------------------------



}

void UMakeBookMark() {
    //Draw Plane
    glBindVertexArray(gMesh.vao2);
    // Set the shader to be used
    glUseProgram(gProgramId);

    // 1. Scales the object by 2
    glm::mat4 scale = glm::scale(glm::vec3(0.3f, 0.3f, 0.3f));
    // 2. Rotates shape by 180 degrees in the z axis, upside down to make it special 
    glm::mat4 rotation = glm::rotate(glm::radians(90.0f), glm::vec3(0.0, 1.0f, 0.0f));
    // 3. Place object at the origin
    glm::mat4 translation = glm::translate(glm::vec3(-17.5f, -3.5f, -4.5f));
    // Model matrix: transformations are applied right-to-left order
    glm::mat4 model = translation * rotation * scale;

    // camera/view transformation
    glm::mat4 view = gCamera.GetViewMatrix();

    //glm::mat4 projection = glm::perspective(glm::radians(gCamera.Zoom), (GLfloat)WINDOW_WIDTH / (GLfloat)WINDOW_HEIGHT, 0.1f, 100.0f);
    glm::mat4 projection;
    if (p) {
        projection = glm::perspective(glm::radians(gCamera.Zoom), (GLfloat)WINDOW_WIDTH / (GLfloat)WINDOW_HEIGHT, 0.1f, 100.0f);
    }
    else {
        projection = glm::ortho(-5.0f, 5.0f, -5.0f, 5.0f, 0.1f, 100.0f);
    }

    // Retrieves and passes transform matrices to the Shader program
    GLint modelLoc = glGetUniformLocation(gProgramId, "model");
    GLint viewLoc = glGetUniformLocation(gProgramId, "view");
    GLint projLoc = glGetUniformLocation(gProgramId, "projection");

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));


    GLint objectColorLoc = glGetUniformLocation(gProgramId, "objectColor");
    GLint lightColorLoc = glGetUniformLocation(gProgramId, "lightColor");
    GLint lightPositionLoc = glGetUniformLocation(gProgramId, "lightPos");
    GLint viewPositionLoc = glGetUniformLocation(gProgramId, "viewPosition");
    GLint directionLoc = glGetUniformLocation(gProgramId, "lightDir");
    GLint lightColorLoc2 = glGetUniformLocation(gProgramId, "lightColor2");
    GLint lightPositionLoc2 = glGetUniformLocation(gProgramId, "lightPos2");


    //glUniform3f(objectColorLoc, gObjectColor.r, gObjectColor.g, gObjectColor.b);
    glUniform3f(lightColorLoc, gLightColor.r, gLightColor.g, gLightColor.b);
    glUniform3f(lightPositionLoc, gLightPosition.x, gLightPosition.y, gLightPosition.z);

    glUniform3f(lightColorLoc2, gLightColor2.r, gLightColor2.g, gLightColor2.b);
    glUniform3f(lightPositionLoc2, gLightPosition2.x, gLightPosition2.y, gLightPosition2.z);

    const glm::vec3 cameraPosition = gCamera.Position;
    glUniform3f(viewPositionLoc, cameraPosition.x, cameraPosition.y, cameraPosition.z);


    glBindTexture(GL_TEXTURE_2D, gTextureMark);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    glBindTexture(GL_TEXTURE_2D, 0);

    GLint UVScaleLoc = glGetUniformLocation(gProgramId, "uvScale");
    glUniform2fv(UVScaleLoc, 1, glm::value_ptr(gUVScaleMark));


    // bind textures on corresponding texture units
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gTextureMark);

    glDrawArrays(GL_TRIANGLES, 0, gMesh.nVertices);

    glBindVertexArray(0);
    glUseProgram(0);

}

void UMakePlane() {
    //Draw Plane
    glBindVertexArray(gMesh.vao2);
    // Set the shader to be used
    glUseProgram(gProgramId);

    // 1. Scales the object by 2
    glm::mat4 scale = glm::scale(glm::vec3(3.0f, 0.75f, 5.0));
    // 2. Rotates shape by 180 degrees in the z axis, upside down to make it special 
    glm::mat4 rotation = glm::rotate(glm::radians(90.0f), glm::vec3(0.0, 1.0f, 0.0f));
    // 3. Place object at the origin
    glm::mat4 translation = glm::translate(glm::vec3(0.0f, -1.3f, 0.0f));
    // Model matrix: transformations are applied right-to-left order
    glm::mat4 model = translation * rotation * scale;

    // camera/view transformation
    glm::mat4 view = gCamera.GetViewMatrix();

    //glm::mat4 projection = glm::perspective(glm::radians(gCamera.Zoom), (GLfloat)WINDOW_WIDTH / (GLfloat)WINDOW_HEIGHT, 0.1f, 100.0f);
    glm::mat4 projection;
    if (p) {
        projection = glm::perspective(glm::radians(gCamera.Zoom), (GLfloat)WINDOW_WIDTH / (GLfloat)WINDOW_HEIGHT, 0.1f, 100.0f);
    }
    else {
        projection = glm::ortho(-5.0f, 5.0f, -5.0f, 5.0f, 0.1f, 100.0f);
    }

    // Retrieves and passes transform matrices to the Shader program
    GLint modelLoc = glGetUniformLocation(gProgramId, "model");
    GLint viewLoc = glGetUniformLocation(gProgramId, "view");
    GLint projLoc = glGetUniformLocation(gProgramId, "projection");

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));


    GLint objectColorLoc = glGetUniformLocation(gProgramId, "objectColor");
    GLint lightColorLoc = glGetUniformLocation(gProgramId, "lightColor");
    GLint lightPositionLoc = glGetUniformLocation(gProgramId, "lightPos");
    GLint viewPositionLoc = glGetUniformLocation(gProgramId, "viewPosition");
    GLint directionLoc = glGetUniformLocation(gProgramId, "lightDir");
    GLint lightColorLoc2 = glGetUniformLocation(gProgramId, "lightColor2");
    GLint lightPositionLoc2 = glGetUniformLocation(gProgramId, "lightPos2");


    //glUniform3f(objectColorLoc, gObjectColor.r, gObjectColor.g, gObjectColor.b);
    glUniform3f(lightColorLoc, gLightColor.r, gLightColor.g, gLightColor.b);
    glUniform3f(lightPositionLoc, gLightPosition.x, gLightPosition.y, gLightPosition.z);

    glUniform3f(lightColorLoc2, gLightColor2.r, gLightColor2.g, gLightColor2.b);
    glUniform3f(lightPositionLoc2, gLightPosition2.x, gLightPosition2.y, gLightPosition2.z);

    const glm::vec3 cameraPosition = gCamera.Position;
    glUniform3f(viewPositionLoc, cameraPosition.x, cameraPosition.y, cameraPosition.z);


    glBindTexture(GL_TEXTURE_2D, gTextureWood);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    glBindTexture(GL_TEXTURE_2D, 0);

    GLint UVScaleLoc = glGetUniformLocation(gProgramId, "uvScale");
    glUniform2fv(UVScaleLoc, 1, glm::value_ptr(gUVScaleWood));

    
    // bind textures on corresponding texture units
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gTextureWood);

    glDrawArrays(GL_TRIANGLES, 0, gMesh.nVertices);

    glBindVertexArray(0);
    glUseProgram(0);


    //Draw The Light

    //Draw Plane
    glBindVertexArray(gMesh.vao2);
    // Set the shader to be used
    glUseProgram(gProgramIdL);
   

    // 1. Scales the object 
    glm::mat4 scale2 = glm::scale(gLightScale);
    // 2. Rotates shape by 180 degrees in the z axis, upside down to make it special 
    glm::mat4 rotation2 = glm::rotate(glm::radians(45.0f), glm::vec3(0.0, 0.0f, -1.0f));
    // 3. Place object at the origin
    glm::mat4 translation2 = glm::translate(gLightPosition);
    // Model matrix: transformations are applied right-to-left order
    glm::mat4 model2 = translation2 * rotation2 * scale2;

   
    // Retrieves and passes transform matrices to the Shader program
    GLint modelLoc2 = glGetUniformLocation(gProgramIdL, "model");
    GLint viewLoc2 = glGetUniformLocation(gProgramIdL, "view");
    GLint projLoc2 = glGetUniformLocation(gProgramIdL, "projection");


    glUniformMatrix4fv(modelLoc2, 1, GL_FALSE, glm::value_ptr(model2));
    glUniformMatrix4fv(viewLoc2, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projLoc2, 1, GL_FALSE, glm::value_ptr(projection));

    glDrawArrays(GL_TRIANGLES, 0, gMesh.nVertices);
    glBindVertexArray(0);
    glUseProgram(0);

    //Draw The Light

    //Draw Plane
    glBindVertexArray(gMesh.vao2);
    // Set the shader to be used
    glUseProgram(gProgramIdL2);


    // 1. Scales the object 
    glm::mat4 scale3 = glm::scale(gLightScale2);
    // 2. Rotates shape by 180 degrees in the z axis, upside down to make it special 
    glm::mat4 rotation3 = glm::rotate(glm::radians(0.0f), glm::vec3(1.0, 0.0f, 0.0f));
    // 3. Place object at the origin
    glm::mat4 translation3 = glm::translate(gLightPosition2);
    // Model matrix: transformations are applied right-to-left order
    glm::mat4 model3 = translation3 * rotation3 * scale3;


    // Retrieves and passes transform matrices to the Shader program
    GLint modelLoc3 = glGetUniformLocation(gProgramIdL, "model");
    GLint viewLoc3 = glGetUniformLocation(gProgramIdL, "view");
    GLint projLoc3 = glGetUniformLocation(gProgramIdL, "projection");


    glUniformMatrix4fv(modelLoc3, 1, GL_FALSE, glm::value_ptr(model3));
    glUniformMatrix4fv(viewLoc3, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projLoc3, 1, GL_FALSE, glm::value_ptr(projection));

    glDrawArrays(GL_TRIANGLES, 0, gMesh.nVertices);
    glBindVertexArray(0);
    glUseProgram(0);

}



void UMakeObelisk() {
    
    //call to obelisk mesh
    glBindVertexArray(gMesh.vao);

    // 1. Scales the object by 2
    glm::mat4 scale = glm::scale(glm::vec3(1.4f, 1.4f, 1.4f));
    // 2. Rotates shape by 180 degrees in the z axis, upside down to make it special 
    glm::mat4 rotation = glm::rotate(glm::radians(0.0f), glm::vec3(0.0, 1.0f, 0.0f));
    // 3. Place object at the origin
    glm::mat4 translation = glm::translate(glm::vec3(5.0f, 4.6f, 8.0f));
    // Model matrix: transformations are applied right-to-left order
    glm::mat4 model = translation * rotation * scale;

    // camera/view transformation
    glm::mat4 view = gCamera.GetViewMatrix();

    //Creates both perspective and orthographic projections
    glm::mat4 projection; //must declare projection before it's defined in the if statement
    if (p) {
        projection = glm::perspective(glm::radians(gCamera.Zoom), (GLfloat)WINDOW_WIDTH / (GLfloat)WINDOW_HEIGHT, 0.1f, 100.0f);
    }
    else {
        projection = glm::ortho(-5.0f, 5.0f, -5.0f, 5.0f, 0.1f, 100.0f);
    }

    // Set the shader to be used
    glUseProgram(gProgramId);

    // Retrieves and passes transform matrices to the Shader program
    GLint modelLoc = glGetUniformLocation(gProgramId, "model");
    GLint viewLoc = glGetUniformLocation(gProgramId, "view");
    GLint projLoc = glGetUniformLocation(gProgramId, "projection");

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));


    GLint objectColorLoc = glGetUniformLocation(gProgramId, "objectColor");
    GLint lightColorLoc = glGetUniformLocation(gProgramId, "lightColor");
    GLint lightPositionLoc = glGetUniformLocation(gProgramId, "lightPos");
    GLint viewPositionLoc = glGetUniformLocation(gProgramId, "viewPosition");
    GLint directionLoc = glGetUniformLocation(gProgramId, "lightDir");
    GLint lightColorLoc2 = glGetUniformLocation(gProgramId, "lightColor2");
    GLint lightPositionLoc2 = glGetUniformLocation(gProgramId, "lightPos2");


    //glUniform3f(objectColorLoc, gObjectColor.r, gObjectColor.g, gObjectColor.b);
    glUniform3f(lightColorLoc, gLightColor.r, gLightColor.g, gLightColor.b);
    glUniform3f(lightPositionLoc, gLightPosition.x, gLightPosition.y, gLightPosition.z);

    glUniform3f(lightColorLoc2, gLightColor2.r, gLightColor2.g, gLightColor2.b);
    glUniform3f(lightPositionLoc2, gLightPosition2.x, gLightPosition2.y, gLightPosition2.z);

    const glm::vec3 cameraPosition = gCamera.Position;
    glUniform3f(viewPositionLoc, cameraPosition.x, cameraPosition.y, cameraPosition.z);


   glBindTexture(GL_TEXTURE_2D, gTextureMarble);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
   glBindTexture(GL_TEXTURE_2D, 0);

    GLint UVScaleLoc = glGetUniformLocation(gProgramId, "uvScale");
    glUniform2fv(UVScaleLoc, 1, glm::value_ptr(gUVScaleMarb));

   

    // bind textures on corresponding texture units
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gTextureMarble);

    // Draws the triangles
    glDrawArrays(GL_TRIANGLES, 0, gMesh.nVerticesO);

    // Deactivate the Vertex Array Object
    glBindVertexArray(0);
    glUseProgram(0);

}

void UMakeRubik() {

    // 1. Scales the object 
    glm::mat4 scale = glm::scale(glm::vec3(1.5f, 1.5f, 1.5f));
    // 2. Rotates shape by 180 degrees in the z axis, upside down to make it special 
    glm::mat4 rotation = glm::rotate(glm::radians(45.0f), glm::vec3(0.0, 1.0f, 0.0f));
    // 3. Place object at the origin
    glm::mat4 translation = glm::translate(glm::vec3(-13.0f, -3.5f, 5.0f));
    // Model matrix: transformations are applied right-to-left order
    glm::mat4 model = translation * rotation * scale;

    // camera/view transformation
    glm::mat4 view = gCamera.GetViewMatrix();

    //Creates both perspective and orthographic projections
    glm::mat4 projection; //must declare projection before it's defined in the if statement
    if (p) {
        projection = glm::perspective(glm::radians(gCamera.Zoom), (GLfloat)WINDOW_WIDTH / (GLfloat)WINDOW_HEIGHT, 0.1f, 100.0f);
    }
    else {
        projection = glm::ortho(-5.0f, 5.0f, -5.0f, 5.0f, 0.1f, 100.0f);
    }

    // Set the shader to be used
    glUseProgram(gProgramId);

    // Retrieves and passes transform matrices to the Shader program
    GLint modelLoc = glGetUniformLocation(gProgramId, "model");
    GLint viewLoc = glGetUniformLocation(gProgramId, "view");
    GLint projLoc = glGetUniformLocation(gProgramId, "projection");

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));


    GLint objectColorLoc = glGetUniformLocation(gProgramId, "objectColor");
    GLint lightColorLoc = glGetUniformLocation(gProgramId, "lightColor");
    GLint lightPositionLoc = glGetUniformLocation(gProgramId, "lightPos");
    GLint viewPositionLoc = glGetUniformLocation(gProgramId, "viewPosition");
    GLint directionLoc = glGetUniformLocation(gProgramId, "lightDir");
    GLint lightColorLoc2 = glGetUniformLocation(gProgramId, "lightColor2");
    GLint lightPositionLoc2 = glGetUniformLocation(gProgramId, "lightPos2");


    //glUniform3f(objectColorLoc, gObjectColor.r, gObjectColor.g, gObjectColor.b);
    glUniform3f(lightColorLoc, gLightColor.r, gLightColor.g, gLightColor.b);
    glUniform3f(lightPositionLoc, gLightPosition.x, gLightPosition.y, gLightPosition.z);

    glUniform3f(lightColorLoc2, gLightColor2.r, gLightColor2.g, gLightColor2.b);
    glUniform3f(lightPositionLoc2, gLightPosition2.x, gLightPosition2.y, gLightPosition2.z);

    const glm::vec3 cameraPosition = gCamera.Position;
    glUniform3f(viewPositionLoc, cameraPosition.x, cameraPosition.y, cameraPosition.z);



    glBindTexture(GL_TEXTURE_2D, gTextureRubikA);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glBindTexture(GL_TEXTURE_2D, 0);

    GLint UVScaleLoc = glGetUniformLocation(gProgramId, "uvScale");
    glUniform2fv(UVScaleLoc, 1, glm::value_ptr(gUVScaleRubik));
    

    //call to obelisk mesh
    glBindVertexArray(gMesh.vaoRC);

    // bind textures on corresponding texture units
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gTextureRubikA);

    // Draws the triangles
    //glDrawElements(GL_TRIANGLES, gMesh.nIndices, GL_UNSIGNED_SHORT, NULL); // Draws the triangle
    glDrawArrays(GL_TRIANGLES, 0, gMesh.nVerticesRC);
    glBindVertexArray(0);
    //------------------------------------------

    //call to obelisk mesh
    glBindVertexArray(gMesh.vaoRC2);
    // bind textures on corresponding texture units
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gTextureRubikB);

    // Draws the triangles
    //glDrawElements(GL_TRIANGLES, gMesh.nIndices, GL_UNSIGNED_SHORT, NULL); // Draws the triangle
    glDrawArrays(GL_TRIANGLES, 0, gMesh.nVerticesRC2);
    glBindVertexArray(0);
    //-------------------------------------------------------------

    //call to obelisk mesh
    glBindVertexArray(gMesh.vaoRC3);
    // bind textures on corresponding texture units
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gTextureRubikC);

    // Draws the triangles
    //glDrawElements(GL_TRIANGLES, gMesh.nIndices, GL_UNSIGNED_SHORT, NULL); // Draws the triangle
    glDrawArrays(GL_TRIANGLES, 0, gMesh.nVerticesRC3);
    glBindVertexArray(0);
    glUseProgram(0);
    //-------------------------------------------------------------
}


// Implements the UCreateMesh function
void UCreateMesh(GLMesh& mesh)
{
    const GLuint floatsPerVertex = 3;
    const GLuint floatsPerNormal = 3;
    const GLuint floatsPerUV = 2;

    //Create plane Mesh
    float vertices2[] = {
        -5.0f, -9.0f, -5.0f,  0.0f, 0.0f, 1.0f,     0.0f, 0.0f,      //bottom left
        5.0f, -9.0f, -5.0f,   0.0f, 0.0f, 1.0f,     1.0f, 0.0f,     //bottom right
        5.0f, -9.0f, 5.0f,   0.0f, 0.0f, 1.0f,      1.0f, 1.0f,      //top right
        5.0f, -9.0f, 5.0f,   0.0f, 0.0f, 1.0f,      1.0f, 1.0f,      //top right
        -5.0f, -9.0f, 5.0f,   0.0f, 0.0f, 1.0f,     0.0f, 1.0f,      //top left
        -5.0f, -9.0f, -5.0f,   0.0f, 0.0f, 1.0f,     0.0f, 0.0f     //bottom left
    };

    //Set the vertices
    mesh.nVertices = sizeof(vertices2) / (sizeof(vertices2[0]) * (floatsPerVertex + floatsPerNormal + floatsPerUV));

    glGenVertexArrays(1, &mesh.vao2);//Generate vao
    glBindVertexArray(mesh.vao2);//bind vao

    glGenBuffers(1, &mesh.vbo); //create the buffer
    glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo); //bind buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices2), vertices2, GL_STATIC_DRAW); //sends gpu the buffer data

    // Strides between vertex coordinates
    GLint stride2 = sizeof(float) * (floatsPerVertex + floatsPerNormal+ floatsPerUV);
    // Create Vertex Attribute Pointers
    glVertexAttribPointer(0, floatsPerVertex, GL_FLOAT, GL_FALSE, stride2, 0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, floatsPerNormal, GL_FLOAT, GL_FALSE, stride2, (void*)(sizeof(float)* floatsPerVertex));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, floatsPerUV, GL_FLOAT, GL_FALSE, stride2, (void*)(sizeof(float) * (floatsPerVertex + floatsPerNormal)));
    glEnableVertexAttribArray(2);

    //------------------------------------------------------------------------------------------------------------------------
   //------------------------------------------------------------------------------------------------------------------------

    //Obelisk Data
    GLfloat verts[] = {
        //Top Pyramid
        0.0f, 1.5f, 0.0f,    0.0f, -1.0f, 1.0f,         0.50f, 0.50f,
        -0.75f, 0.0f, 0.75f, 0.0f, -1.0f, 1.0f,        0.0f, 0.0f,
        0.75f, 0.0f, 0.75f,  0.0f, -1.0f, 1.0f,        1.0f, 0.0f,

        0.0f, 1.5f, 0.0f,     1.0f, -1.0f,  0.0f,       0.50f, 0.50f,
        0.75f, 0.0f, 0.75f,    1.0f, -1.0f,  0.0f,      0.0f, 0.0f,
        0.75f, 0.0f, -0.75f,    1.0f, -1.0f,  0.0f,     1.0f, 0.0f,

        0.0f, 1.5f, 0.0f,      0.0f, -1.0f, -1.0f,      0.50f, 0.50f,
        0.75f, 0.0f, -0.75f,    0.0f, -1.0f, -1.0f,     0.0f, 0.0f,
        -0.75f, 0.0f, -0.75f,   0.0f, -1.0f, -1.0f,     1.0f, 0.0f,

        0.0f, 1.5f, 0.0f,        -1.0f, -1.0f, 0.0f,      0.50f, 0.50f,
        -0.75f, 0.0f, -0.75f,     -1.0f, -1.0f, 0.0f,     0.0f, 0.0f,
        -0.75f, 0.0f, 0.75f,      -1.0f, -1.0f, 0.0f,     1.0f, 0.0f,

        //Sides & faces
        0.75f, 0.0f, 0.75f,    0.0f, -1.0f, 1.0f,           1.0f, 1.0f,    //top right
        -0.75f, 0.0f, 0.75f,    0.0f, -1.0f, 1.0f,         0.0f, 1.0f,    //top left
         1.25f, -9.0f, 1.25f,   0.0f, -1.0f, 1.0f,        1.0f, 0.0f,   //bottom right
         1.25f, -9.0f, 1.25f,  0.0f, -1.0f, 1.0f,         1.0f, 0.0f,   //bottom right
         -0.75f, 0.0f, 0.75f,   0.0f, -1.0f, 1.0f,        0.0f, 1.0f,    //top left
         -1.25f, -9.0f, 1.25f,   0.0f, -1.0f, 1.0f,        0.0f, 0.0f,  //bottom left

         0.75f, 0.0f, -0.75f,    1.0f, -1.0f,  0.0f,     1.0f, 1.0f,    //top right
         0.75f, 0.0f, 0.75f,    1.0f, -1.0f,  0.0f,        0.0f, 1.0f,    //top left
         1.25f, -9.0f, -1.25f,  1.0f, -1.0f,  0.0f,         1.0f, 0.0f,   //bottom right
         1.25f, -9.0f, -1.25f,  1.0f, -1.0f,  0.0f,         1.0f, 0.0f,   //bottom right
         0.75f, 0.0f, 0.75f,   1.0f, -1.0f,  0.0f,          0.0f, 1.0f,    //top left
         1.25f, -9.0f, 1.25f,   1.0f, -1.0f,  0.0f,         0.0f, 0.0f,  //bottom left

         -0.75f, 0.0f, -0.75f,  0.0f, -1.0f, -1.0f,         1.0f, 1.0f,    //top right
        0.75f, 0.0f, -0.75f,   0.0f, -1.0f, -1.0f,            0.0f, 1.0f,    //top left
         -1.25f, -9.0f, -1.25f,  0.0f, -1.0f, -1.0f,          1.0f, 0.0f,   //bottom right
         -1.25f, -9.0f, -1.25f,  0.0f, -1.0f, -1.0f,          1.0f, 0.0f,   //bottom right
         0.75f, 0.0f, -0.75f,   0.0f, -1.0f, -1.0f,           0.0f, 1.0f,    //top left
         1.25f, -9.0f, -1.25f,   0.0f, -1.0f, -1.0f,         0.0f, 0.0f,  //bottom left

         -0.75f, 0.0f, 0.75f,   -1.0f, -1.0f, 0.0f,           1.0f, 1.0f,    //top right
        -0.75f, 0.0f, -0.75f,   -1.0f, -1.0f, 0.0f,            0.0f, 1.0f,    //top left
         -1.25f, -9.0f, 1.25f,  -1.0f, -1.0f, 0.0f,            1.0f, 0.0f,   //bottom right
         -1.25f, -9.0f, 1.25f,  -1.0f, -1.0f, 0.0f,           1.0f, 0.0f,   //bottom right
         -0.75f, 0.0f, -0.75f,  -1.0f, -1.0f, 0.0f,             0.0f, 1.0f,    //top left
         -1.25f, -9.0f, -1.25f,  -1.0f, -1.0f, 0.0f,           0.0f, 0.0f,  //bottom left

        //Base
        1.25f, -9.0f, 1.25f,     0.0f, -1.0f, 0.0f,             1.0f, 1.0f,
        -1.25f, -9.0f, 1.25f,     0.0f, -1.0f, 0.0f,            0.0f, 1.0f,
        1.25f, -9.0f, -1.25f,    0.0f, -1.0f, 0.0f,             1.0f, 0.0f,

        1.25f, -9.0f, -1.25f,   0.0f, -1.0f, 0.0f,             1.0f, 0.0f,
        -1.25f, -9.0f, 1.25f,  0.0f, -1.0f, 0.0f,              0.0f, 1.0f,
        -1.25f, -9.0f, -1.25f,   0.0f, -1.0f, 0.0f,             0.0f, 0.0f

    };


    mesh.nVerticesO = sizeof(verts) / (sizeof(verts[0]) * (floatsPerVertex + floatsPerUV + floatsPerNormal));

    glGenVertexArrays(1, &mesh.vao); // we can also generate multiple VAOs or buffers at the same time
    glBindVertexArray(mesh.vao);

    // Create 2 buffers: first one for the vertex data; second one for the indices
    glGenBuffers(1, &mesh.vboO);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.vboO); // Activates the buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW); // Sends vertex or coordinate data to the GPU
 
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.vboO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);

    // Strides between vertex coordinates
    GLint stride = sizeof(float) * (floatsPerVertex + floatsPerUV + floatsPerNormal);
    // Create Vertex Attribute Pointers
    glVertexAttribPointer(0, floatsPerVertex, GL_FLOAT, GL_FALSE, stride, 0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, floatsPerNormal, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float)* floatsPerVertex));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, floatsPerUV, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float)* (floatsPerVertex + floatsPerNormal)));
    glEnableVertexAttribArray(2);
    
    
    //------------------------------------------------------------------------------------------------------------------------
    //------------------------------------------------------------------------------------------------------------------------
    //Rubiks Cube

    GLfloat RCverts[] = {
        //Front face
        1.0f, 1.0f, 1.0f, 0.0f,  0.0f,  1.0f,     1.0f, 1.0f, //top right 0
        -1.0f, 1.0f, 1.0f, 0.0f,  0.0f,  1.0f,  0.0f, 1.0f, //top left 3
        1.0f, -1.0f, 1.0f, 0.0f,  0.0f,  1.0f,   1.0f, 0.0f, //bottom right 1
        1.0f, -1.0f, 1.0f,0.0f,  0.0f,  1.0f,   1.0f, 0.0f, //bottom right 1
        -1.0f, 1.0f, 1.0f,0.0f,  0.0f,  1.0f,    0.0f, 1.0f, //top left 3
        -1.0f, -1.0f, 1.0f,0.0f,  0.0f,  1.0f,   0.0f, 0.0f, //bottom left 2
        //Back Face
        -1.0f, -1.0f, -1.0f, 0.0f,  0.0f, -1.0f,   1.0f, 1.0f, //top right 0
        -1.0f, 1.0f, -1.0f,0.0f,  0.0f, -1.0f,    0.0f, 1.0f, //top left 3
        1.0f, -1.0f, -1.0f, 0.0f,  0.0f, -1.0f,   1.0f, 0.0f, //bottom right 1
        1.0f, -1.0f, -1.0f, 0.0f,  0.0f, -1.0f,   1.0f, 0.0f, //bottom right 1
        -1.0f, 1.0f, -1.0f, 0.0f,  0.0f, -1.0f,   0.0f, 1.0f, //top left 3
        1.0f, 1.0f, -1.0f, 0.0f,  0.0f, -1.0f,   0.0f, 0.0f, //bottom left 2
      
         //Bottom Face
         -1.0f, -1.0f, 1.0f, 0.0f, -1.0f,  0.0f,   1.0f, 1.0f, //top right 0
         -1.0f, -1.0f, -1.0f, 0.0f, -1.0f,  0.0f,   0.0f, 1.0f, //top left 3
         1.0f, -1.0f, 1.0f,  0.0f, -1.0f,  0.0f,    1.0f, 0.0f, //bottom right 1
         1.0f, -1.0f, 1.0f,  0.0f, -1.0f,  0.0f,   1.0f, 0.0f, //bottom right 1
         -1.0f, -1.0f, -1.0f, 0.0f, -1.0f,  0.0f,   0.0f, 1.0f, //top left 3
         1.0f, -1.0f, -1.0f, 0.0f, -1.0f,  0.0f,    0.0f, 0.0f, //bottom left 2

    };

    mesh.nVerticesRC = sizeof(RCverts) / (sizeof(RCverts[0]) * (floatsPerVertex + floatsPerUV + floatsPerNormal));

    glGenVertexArrays(1, &mesh.vaoRC); // we can also generate multiple VAOs or buffers at the same time
    glBindVertexArray(mesh.vaoRC);

    // Create 2 buffers: first one for the vertex data; second one for the indices
    glGenBuffers(1, &mesh.vboRC);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.vboRC); // Activates the buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(RCverts), RCverts, GL_STATIC_DRAW); // Sends vertex or coordinate data to the GPU

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.vboRC);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(RCverts), RCverts, GL_STATIC_DRAW);

    glVertexAttribPointer(0, floatsPerVertex, GL_FLOAT, GL_FALSE, stride, 0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, floatsPerNormal, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float)* floatsPerVertex));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, floatsPerUV, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float)* (floatsPerVertex + floatsPerNormal)));
    glEnableVertexAttribArray(2);
    

    //------------------------------------------------------------------------------------------------------------------------
    //------------------------------------------------------------------------------------------------------------------------

    GLfloat RC2verts[] = {
       
        //Right Face
        1.0f, 1.0f, -1.0f, 1.0f,  0.0f,  0.0f,    1.0f, 1.0f, //top right 0
        1.0f, 1.0f, 1.0f, 1.0f,  0.0f,  0.0f,   0.0f, 1.0f, //top left 3
        1.0f, -1.0f, -1.0f, 1.0f,  0.0f,  0.0f,  1.0f, 0.0f, //bottom right 1
        1.0f, -1.0f, -1.0f, 1.0f,  0.0f,  0.0f,   1.0f, 0.0f, //bottom right 1
        1.0f, 1.0f, 1.0f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f, //top left 3
        1.0f, -1.0f, 1.0f, 1.0f,  0.0f,  0.0f,   0.0f, 0.0f, //bottom left 2
     
        //Top Face
        1.0f, 1.0f, -1.0f, 0.0f,  1.0f,  0.0f,   1.0f, 1.0f, //top right 0
        -1.0f, 1.0f, -1.0f,0.0f,  1.0f,  0.0f,     0.0f, 1.0f, //top left 3
        1.0f, 1.0f, 1.0f, 0.0f,  1.0f,  0.0f,   1.0f, 0.0f, //bottom right 1
        1.0f, 1.0f, 1.0f, 0.0f,  1.0f,  0.0f,   1.0f, 0.0f, //bottom right 1
        -1.0f, 1.0f, -1.0f, 0.0f,  1.0f,  0.0f,    0.0f, 1.0f, //top left 3
        -1.0f, 1.0f, 1.0f, 0.0f,  1.0f,  0.0f,  0.0f, 0.0f, //bottom left 2
        
    };

    mesh.nVerticesRC2 = sizeof(RC2verts) / (sizeof(RC2verts[0]) * (floatsPerVertex + floatsPerUV + floatsPerNormal));

    glGenVertexArrays(1, &mesh.vaoRC2); // we can also generate multiple VAOs or buffers at the same time
    glBindVertexArray(mesh.vaoRC2);

    // Create 2 buffers: first one for the vertex data; second one for the indices
    glGenBuffers(1, &mesh.vboRC2);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.vboRC2); // Activates the buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(RC2verts), RC2verts, GL_STATIC_DRAW); // Sends vertex or coordinate data to the GPU

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.vboRC2);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(RC2verts), RC2verts, GL_STATIC_DRAW);

    // Create Vertex Attribute Pointers
    glVertexAttribPointer(0, floatsPerVertex, GL_FLOAT, GL_FALSE, stride, 0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, floatsPerNormal, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float)* floatsPerVertex));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, floatsPerUV, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float)* (floatsPerVertex + floatsPerNormal)));
    glEnableVertexAttribArray(2);

    //------------------------------------------------------------------------------------------------------------------------
    //------------------------------------------------------------------------------------------------------------------------
    
    GLfloat RC3verts[] = {

        //Left Face
         -1.0f, 1.0f, 1.0f,  -1.0f,  0.0f,  0.0f,    1.0f, 1.0f, //top right 0
         -1.0f, 1.0f, -1.0f,   -1.0f,  0.0f,  0.0f,  0.0f, 1.0f, //top left 3
         -1.0f, -1.0f, 1.0f,  -1.0f,  0.0f,  0.0f,   1.0f, 0.0f, //bottom right 1
         -1.0f, -1.0f, 1.0f,  -1.0f,  0.0f,  0.0f,    1.0f, 0.0f, //bottom right 1
         -1.0f, 1.0f, -1.0f,  -1.0f,  0.0f,  0.0f,   0.0f, 1.0f, //top left 3
         -1.0f, -1.0f, -1.0f,  -1.0f,  0.0f,  0.0f,   0.0f, 0.0f, //bottom left 2

    };

    mesh.nVerticesRC3 = sizeof(RC3verts) / (sizeof(RC3verts[0]) * (floatsPerVertex + floatsPerUV + floatsPerNormal));

    glGenVertexArrays(1, &mesh.vaoRC3); // we can also generate multiple VAOs or buffers at the same time
    glBindVertexArray(mesh.vaoRC3);

    // Create 2 buffers: first one for the vertex data; second one for the indices
    glGenBuffers(1, &mesh.vboRC3);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.vboRC3); // Activates the buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(RC3verts), RC3verts, GL_STATIC_DRAW); // Sends vertex or coordinate data to the GPU

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.vboRC3);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(RC3verts), RC3verts, GL_STATIC_DRAW);

    // Create Vertex Attribute Pointers
    glVertexAttribPointer(0, floatsPerVertex, GL_FLOAT, GL_FALSE, stride, 0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, floatsPerNormal, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float)* floatsPerVertex));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, floatsPerUV, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float)* (floatsPerVertex + floatsPerNormal)));
    glEnableVertexAttribArray(2);

    //--------------------------------------------------------------------------------------------------------------------------------------------------
    //Book Texture

    GLfloat Bverts[] = {

        //Front face
        1.0f, 1.0f, 1.0f, 0.0f,  0.0f,  1.0f,     1.0f, 1.0f, //top right 0
        -1.0f, 1.0f, 1.0f, 0.0f,  0.0f,  1.0f,  0.0f, 1.0f, //top left 3
        1.0f, -1.0f, 1.0f, 0.0f,  0.0f,  1.0f,   1.0f, 0.0f, //bottom right 1
        1.0f, -1.0f, 1.0f,0.0f,  0.0f,  1.0f,   1.0f, 0.0f, //bottom right 1
        -1.0f, 1.0f, 1.0f,0.0f,  0.0f,  1.0f,    0.0f, 1.0f, //top left 3
        -1.0f, -1.0f, 1.0f,0.0f,  0.0f,  1.0f,   0.0f, 0.0f, //bottom left 2

        //Left Face
        -1.0f, 1.0f, 1.0f,  -1.0f,  0.0f,  0.0f,    1.0f, 1.0f, //top right 0
        -1.0f, 1.0f, -1.0f,   -1.0f,  0.0f,  0.0f,  0.0f, 1.0f, //top left 3
        -1.0f, -1.0f, 1.0f,  -1.0f,  0.0f,  0.0f,   1.0f, 0.0f, //bottom right 1
        -1.0f, -1.0f, 1.0f,  -1.0f,  0.0f,  0.0f,    1.0f, 0.0f, //bottom right 1
        -1.0f, 1.0f, -1.0f,  -1.0f,  0.0f,  0.0f,   0.0f, 1.0f, //top left 3
        -1.0f, -1.0f, -1.0f,  -1.0f,  0.0f,  0.0f,   0.0f, 0.0f, //bottom left 2

        //Right Face
        1.0f, 1.0f, -1.0f, 1.0f,  0.0f,  0.0f,    1.0f, 1.0f, //top right 0
        1.0f, 1.0f, 1.0f, 1.0f,  0.0f,  0.0f,   0.0f, 1.0f, //top left 3
        1.0f, -1.0f, -1.0f, 1.0f,  0.0f,  0.0f,  1.0f, 0.0f, //bottom right 1
        1.0f, -1.0f, -1.0f, 1.0f,  0.0f,  0.0f,   1.0f, 0.0f, //bottom right 1
        1.0f, 1.0f, 1.0f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f, //top left 3
        1.0f, -1.0f, 1.0f, 1.0f,  0.0f,  0.0f,   0.0f, 0.0f //bottom left 2

    };

    mesh.nVerticesB = sizeof(Bverts) / (sizeof(Bverts[0]) * (floatsPerVertex + floatsPerUV + floatsPerNormal));

    glGenVertexArrays(1, &mesh.vaoB); // we can also generate multiple VAOs or buffers at the same time
    glBindVertexArray(mesh.vaoB);

    // Create 2 buffers: first one for the vertex data; second one for the indices
    glGenBuffers(1, &mesh.vboB);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.vboB); // Activates the buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(Bverts), Bverts, GL_STATIC_DRAW); // Sends vertex or coordinate data to the GPU

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.vboB);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Bverts), Bverts, GL_STATIC_DRAW);

    // Create Vertex Attribute Pointers
    glVertexAttribPointer(0, floatsPerVertex, GL_FLOAT, GL_FALSE, stride, 0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, floatsPerNormal, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float)* floatsPerVertex));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, floatsPerUV, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float)* (floatsPerVertex + floatsPerNormal)));
    glEnableVertexAttribArray(2);


    //----------------------

    GLfloat B2verts[] = {

        //Top Face
            1.0f, 1.0f, 1.0f,   0.0f, 1.0f,  0.0f,   1.0f, 1.0f, //top right 0
            1.0f, 1.0f, -1.0f,   0.0f,  1.0f,  0.0f,     0.0f, 1.0f, //top left 3
            -1.0f, 1.0f, 1.0f,     0.0f,  1.0f,  0.0f,   1.0f, 0.0f, //bottom right 1
            -1.0f, 1.0f, 1.0f,    0.0f,  1.0f,  0.0f,   1.0f, 0.0f, //bottom right 1
            1.0f, 1.0f, -1.0f,   0.0f,  1.0f,  0.0f,    0.0f, 1.0f, //top left 3
            -1.0f, 1.0f, -1.0f,    0.0f,  1.0f,  0.0f,  0.0f, 0.0f //bottom left 2

    };

    mesh.nVerticesB2 = sizeof(B2verts) / (sizeof(B2verts[0]) * (floatsPerVertex + floatsPerUV + floatsPerNormal));

    glGenVertexArrays(1, &mesh.vaoB2); // we can also generate multiple VAOs or buffers at the same time
    glBindVertexArray(mesh.vaoB2);

    // Create 2 buffers: first one for the vertex data; second one for the indices
    glGenBuffers(1, &mesh.vboB2);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.vboB2); // Activates the buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(B2verts), B2verts, GL_STATIC_DRAW); // Sends vertex or coordinate data to the GPU

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.vboB2);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(B2verts), B2verts, GL_STATIC_DRAW);

    // Create Vertex Attribute Pointers
    glVertexAttribPointer(0, floatsPerVertex, GL_FLOAT, GL_FALSE, stride, 0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, floatsPerNormal, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float)* floatsPerVertex));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, floatsPerUV, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float)* (floatsPerVertex + floatsPerNormal)));
    glEnableVertexAttribArray(2);

    //----------------------

    GLfloat B3verts[] = {

        //Back Face
       -1.0f, -1.0f, -1.0f, 0.0f,  0.0f, -1.0f,   1.0f, 1.0f, //top right 0
       -1.0f, 1.0f, -1.0f,0.0f,  0.0f, -1.0f,    0.0f, 1.0f, //top left 3
       1.0f, -1.0f, -1.0f, 0.0f,  0.0f, -1.0f,   1.0f, 0.0f, //bottom right 1
       1.0f, -1.0f, -1.0f, 0.0f,  0.0f, -1.0f,   1.0f, 0.0f, //bottom right 1
       -1.0f, 1.0f, -1.0f, 0.0f,  0.0f, -1.0f,   0.0f, 1.0f, //top left 3
       1.0f, 1.0f, -1.0f, 0.0f,  0.0f, -1.0f,   0.0f, 0.0f //bottom left 2

    };

    mesh.nVerticesB3 = sizeof(B3verts) / (sizeof(B3verts[0]) * (floatsPerVertex + floatsPerUV + floatsPerNormal));

    glGenVertexArrays(1, &mesh.vaoB3); // we can also generate multiple VAOs or buffers at the same time
    glBindVertexArray(mesh.vaoB3);

    // Create 2 buffers: first one for the vertex data; second one for the indices
    glGenBuffers(1, &mesh.vboB3);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.vboB3); // Activates the buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(B3verts), B3verts, GL_STATIC_DRAW); // Sends vertex or coordinate data to the GPU

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.vboB3);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(B3verts), B3verts, GL_STATIC_DRAW);

    // Create Vertex Attribute Pointers
    glVertexAttribPointer(0, floatsPerVertex, GL_FLOAT, GL_FALSE, stride, 0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, floatsPerNormal, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float)* floatsPerVertex));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, floatsPerUV, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float)* (floatsPerVertex + floatsPerNormal)));
    glEnableVertexAttribArray(2);

    //------------------------------------------------
    //Cylinder Mesh

    glGenVertexArrays(1, &mesh.vaoCy1); // we can also generate multiple VAOs or buffers at the same time
    glBindVertexArray(mesh.vaoCy1);

    // Create 2 buffers: first one for the vertex data; second one for the indices
    glGenBuffers(1, &mesh.vboCy1);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.vboCy1); // Activates the buffer
}


void UDestroyMesh(GLMesh& mesh)
{
    glDeleteVertexArrays(1, &mesh.vao);
    glDeleteVertexArrays(1, &mesh.vao2);
    glDeleteBuffers(1, &mesh.vboO);
    glDeleteBuffers(1, &mesh.vbo);

    glDeleteVertexArrays(1, &mesh.vaoRC);
    glDeleteBuffers(1, &mesh.vboRC);
    glDeleteVertexArrays(1, &mesh.vaoRC2);
    glDeleteBuffers(1, &mesh.vboRC2);
    glDeleteVertexArrays(1, &mesh.vaoRC3);
    glDeleteBuffers(1, &mesh.vboRC3);
}

/*Generate and load the texture*/
bool UCreateTexture(const char* filename, GLuint& textureId)
{
    int width, height, channels;
    unsigned char* image = stbi_load(filename, &width, &height, &channels, 0);
    if (image)
    {
        flipImageVertically(image, width, height, channels);

        glGenTextures(1, &textureId);
        glBindTexture(GL_TEXTURE_2D, textureId);

        // set the texture wrapping parameters
        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT); //Swapped to Mirroed Repeat because it looks gooder U+1F44D
        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
        // set texture filtering parameters
        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        if (channels == 3)
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
        else if (channels == 4)
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
        else
        {
            cout << "Not implemented to handle image with " << channels << " channels" << endl;
            return false;
        }

        glGenerateMipmap(GL_TEXTURE_2D);

        stbi_image_free(image);
        glBindTexture(GL_TEXTURE_2D, 0); // Unbind the texture

        return true;
    }
    return false;
}

    void UDestroyTexture(GLuint textureId)
    {
        glGenTextures(1, &textureId);
    }


// Implements the UCreateShaders function
bool UCreateShaderProgram(const char* vtxShaderSource, const char* fragShaderSource, GLuint& programId)
{
    // Compilation and linkage error reporting
    int success = 0;
    char infoLog[512];

    // Create a Shader program object.
    programId = glCreateProgram();

    // Create the vertex and fragment shader objects
    GLuint vertexShaderId = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);

    // Retrive the shader source
    glShaderSource(vertexShaderId, 1, &vtxShaderSource, NULL);
    glShaderSource(fragmentShaderId, 1, &fragShaderSource, NULL);

    // Compile the vertex shader, and print compilation errors (if any)
    glCompileShader(vertexShaderId); // compile the vertex shader
    // check for shader compile errors
    glGetShaderiv(vertexShaderId, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShaderId, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;

        return false;
    }

    glCompileShader(fragmentShaderId); // compile the fragment shader
    // check for shader compile errors
    glGetShaderiv(fragmentShaderId, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShaderId, sizeof(infoLog), NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;

        return false;
    }

    // Attached compiled shaders to the shader program
    glAttachShader(programId, vertexShaderId);
    glAttachShader(programId, fragmentShaderId);

    glLinkProgram(programId);   // links the shader program
    // check for linking errors
    glGetProgramiv(programId, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(programId, sizeof(infoLog), NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;

        return false;
    }

    glUseProgram(programId);    // Uses the shader program

    return true;
}


void UDestroyShaderProgram(GLuint programId)
{
    glDeleteProgram(programId);
}