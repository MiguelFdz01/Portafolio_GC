#include <glad/glad.h>      //Loader de opengl
#include <GLFW/glfw3.h>     //herramienta para manejo de opengl

#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;


const char* vertexShaderSource = "#version 330 core\n"          //aqui se define color
"layout (location = 0) in vec3 aPos;\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"}\0";

const char* fragmentShaderSource = "#version 330 core\n"        //aqui se define color
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"   FragColor = vec4(0.796f, 0.125f, 0.270f, 1.0f);\n"        //aqui se define color (se puede cambiar color aqui, deben ser los primeros 3) (RGB normalized decimal)
"}\n\0";


//1) Modifica el shader (agrega informacion de color vertice por vertice)
const char* vertexShaderSourceWithColor =
"#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"layout (location = 1) in vec3 aColor;\n"                   //se define un nueva linea de codigo que manda a llamar el location de la posicion #1

"out vec3 ourColor;"                                        //se crea una variable de salida de cualquier nombre 
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos, 1.0);\n"
"   ourColor = aColor;"                                     //esta variable de salida le asignamos el nombre del atributo
"}\0";


const char* fragmentShaderSourceWithColor = "#version 330 core\n"
"out vec4 FragColor;\n"
"in vec3 ourColor;"                                         //se tiene una variable de entrada que debe tener el mismo nombre que la variable de salida
"void main()\n"
"{\n"
"   FragColor = vec4(ourColor.x, ourColor.y, ourColor.z, 1.0f);\n"      //se manda a poner ese ourColor y sus componentes x, y ,z (se carga con el color del atributo correspondiente)
"}\n\0";




int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }


    // build and compile our shader program
    // ------------------------------------
    // vertex shader
    int vertexShader = glCreateShader(GL_VERTEX_SHADER);    //ID
    glShaderSource(vertexShader, 1, &vertexShaderSourceWithColor, NULL);
    glCompileShader(vertexShader);
    // check for shader compile errors
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    // fragment shader
    int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSourceWithColor, NULL);
    glCompileShader(fragmentShader);
    // check for shader compile errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    // link shaders
    int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    // check for linking errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    /*float vertices[] = {       //los vertices son informacion de las coordenadas x, y, z de los puntos que va a conformar la figura
        1.0f,  -0.5f, 0.0f, // punto 0
        0.5f,  -0.5f, 0.0f,  // punto 1
        0.5f,  0.5f, 0.0f,  // punto 2

        0.0f,  0.5f, 0.0f,  // punto 3
        0.0f, -0.5f, 0.0f,  // punto 4
        -0.5f, -0.5f, 0.0f, // punto 5

        -0.5f, 0.5f, 0.0f,  // punto 6
        -1.0f,  -0.5f, 0.0f,    // punto 7
        //-1.0f,   0.5f, 0.0f,  // bottom right

    }; */


    unsigned int indices[] = {  // note that we start from 0!   //es un arreglo que dice en que orden se van a ir dibujando la figura
        //este es el orden en que se dibujaran los vertices

        0, 2, 1,  // first Triangle
        3, 5, 4,   // second Triangle

        8, 6, 7,   // third Triangle    
        10, 9, 11,   // fourth Triangle

        12, 14, 13,
        15, 17, 16,
        19, 20, 18,
        21, 22, 23
    }; 


    //Modifica el arreglo de vertices
    float verticesTriangulosColores[] = {
        
        /*
        -1.0f, 1.0f, 0.0f,    0.976, 1, 0.101,  // 0
        0.0f, 0.0f, 0.0f,   0.976, 1, 0.101,  // 1      amarillo
        -1.0f, -1.0f, 0.0f, 0.976, 1, 0.101,  // 2 
        
        0.0f, 0.0f, 0.0f,   0.901, 0.403, 0.956,  // 3
        -1.0f, -1.0f, 0.0f, 0.901, 0.403, 0.956,  // 4      rosa
        1.0f,  -1.0f, 0.0f, 0.901, 0.403, 0.956,   // 5

        1.0f,  0.0f, 0.0f, 1, 0, 0,        // 6
        0.5f,  -0.5f, 0.0f,  1, 0, 0,        // 7     rojo 1
        1.0f,  -1.0f, 0.0f, 1, 0, 0,        // 8

        0.5f,  0.5f, 0.0f, 1, 0, 0,        // 9
        0.5f,  -0.5f, 0.0f,  1, 0, 0,        // 10      rojo 2
        1.0f,  0.0f, 0.0f, 1, 0, 0,        // 11

        0.0f,  0.0f, 0.0f,  0, 0.921, 0.901,    //12
        0.5f,  0.5f, 0.0f,  0, 0.921, 0.901,    //13      celeste
        0.5f,  -0.5f, 0.0f,  0, 0.921, 0.901,    //14

        0.0f,  0.0f, 0.0f,  0.050, 0.772, 0.368,    //15
        -0.5f,  0.5f, 0.0f,  0.050, 0.772, 0.368,    //16     verde claro 1
        0.5f,  0.5f, 0.0f,  0.050, 0.772, 0.368,    //17

        0.0f,  1.0f, 0.0f,  0.050, 0.772, 0.368,    //18
        -0.5f,  0.5f, 0.0f,  0.050, 0.772, 0.368,    //19   verde claro 2
        0.5f,  0.5f, 0.0f,  0.050, 0.772, 0.368,    //20

        0.0f,  1.0f, 0.0f,  0.627, 0.011, 0.482,    //21
        -0.5f,  0.5f, 0.0f,  0.627, 0.011, 0.482,    //22   morado
        -1.0f,  1.0f, 0.0f,  0.627, 0.011, 0.482,    //23

        0.0f,  1.0f, 0.0f,  0.980, 0.454, 0.039,    //27
        1.0f,  1.0f, 0.0f,  0.980, 0.454, 0.039,    //28    naranja
        1.0f,  0.0f, 0.0f,  0.980, 0.454, 0.039,    //29
        */


        0.0f, 1.0f, 0.0f,    1, 0, 0,  // 0
        0.0f, 0.0f, 0.0f,   1, 0, 0,  // 1      rojo
        0.5f, 0.5f, 0.0f, 1, 0, 0,  // 2

        0.0f, 0.0f, 0.0f,   0.901, 0.403, 0.956,  // 3
        0.5f, 0.5f, 0.0f, 0.901, 0.403, 0.956,  // 4      rosa
        1.0f,  0.0f, 0.0f, 0.901, 0.403, 0.956,   // 5

        -0.25f,  0.25f, 0.0f, 0.050, 0.772, 0.368,        // 6
        -0.25f,  0.0f, 0.0f,  0.050, 0.772, 0.368,        // 7     verde 1
        -0.5f,  0.0f, 0.0f, 0.050, 0.772, 0.368,        // 8

        -0.5f,  0.25f, 0.0f, 0.050, 0.772, 0.368,        // 9
        -0.5f,  0.0f, 0.0f,  0.050, 0.772, 0.368,        // 10     verde 2
        -0.25f,  0.25f, 0.0f, 0.050, 0.772, 0.368,        // 11

        -0.5f,  0.0f, 0.0f,  0.356, 0.050, 0.850,    //12
        -0.5f,  0.25f, 0.0f,  0.356, 0.050, 0.850,    //13      modado
        -0.75f,  0.0f, 0.0f,  0.356, 0.050, 0.850,    //14

        -0.5f,  0.25f, 0.0f,  0, 0.921, 0.901,    //15
        -0.25f,  0.25f, 0.0f,  0, 0.921, 0.901,    //16     celeste
        -0.25f,  0.5f, 0.0f,  0, 0.921, 0.901,    //17

        0.0f,  -0.25f, 0.0f,  0.976, 1, 0.101,    //18
        0.25f,  -0.75f, 0.0f,  0.976, 1, 0.101,    //19   amarillo
        0.5f,  -0.25f, 0.0f,  0.976, 1, 0.101,    //20

        0.0f,  -0.25f, 0.0f,  0.980, 0.454, 0.039,    //21
        0.25f,  -0.75f, 0.0f,  0.980, 0.454, 0.039,    //22    naranja
        -0.25f,  -0.75f, 0.0f,  0.980, 0.454, 0.039,    //23

        0.0f,  -0.25f, 0.0f,  0.980, 0.454, 0.039,    //24
        -0.5f,  -0.25f, 0.0f,  0.980, 0.454, 0.039,    //25    naranja 2
        -0.25f,  -0.75f, 0.0f,  0.980, 0.454, 0.039,    //26

    };


    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);


    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticesTriangulosColores), verticesTriangulosColores, GL_STATIC_DRAW);

    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    //glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);


    
    //para 2 atributos
    //catalogamos/categorizamos la informacion

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0); //Column Major
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    

    //para un atributo
    //glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0); //Column Major
    //glEnableVertexAttribArray(0);


    // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // remember: do NOT unbind the EBO while a VAO is active as the bound element buffer object IS stored in the VAO; keep the EBO bound.
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
    // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    glBindVertexArray(0);


    // uncomment this call to draw in wireframe polygons.
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // draw our first triangle
        glUseProgram(shaderProgram);
        glLineWidth(20.0);
        glPointSize(20.0);

        glBindVertexArray(VAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
        glDrawArrays(GL_TRIANGLES, 0, 72);
        //glDrawElements(GL_TRIANGLES, 18, GL_UNSIGNED_INT, 0);
        //glBindVertexArray(0); // no need to unbind it every time 

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}