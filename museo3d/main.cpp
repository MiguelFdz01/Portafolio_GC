
#include <GLFW/glfw3.h>
#include <engine/Billboard.h>
#include <engine/CollisionBox.h>
#include <engine/Objectives.h>
#include <engine/Particles.h>
#include <engine/Plane.h>
#include <engine/QuadTexture.h>
#include <engine/RigidModel.h>
#include <engine/Terrain.h>
#include <engine/functions.h>
#include <engine/shader_m.h>
#include <engine/skybox.h>
#include <engine/textrenderer.h>
#include <glad/glad.h>
#include <iostream>

int main()
{
    //:::: INICIALIZAMOS GLFW CON LA VERSIÓN 3.3 :::://
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    //:::: CREAMOS LA VENTANA:::://
    GLFWwindow *window = glfwCreateWindow(800, 600, "SuperMarket Rescue", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    //:::: OBTENEMOS INFORMACIÓN DE TODOS LOS EVENTOS DE LA VENTANA:::://
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetJoystickCallback(joystick_callback);

    //:::: DESHABILITAMOS EL CURSOR VISUALMENTE EN LA PANTALLA :::://
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    //:::: INICIALIZAMOS GLAD CON LAS CARACTERÍSTICAS DE OPENGL ESCENCIALES :::://
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    //INICIALIZAMOS LA ESCENA
    Shader ourShader("shaders/multiple_lighting.vs", "shaders/multiple_lighting.fs");
    Shader selectShader("shaders/selectedShader.vs", "shaders/lighting_maps.fs");
    initScene(ourShader);
    Terrain terrain("textures/heightmap.jpg", texturePaths);
    SkyBox sky(1.0f, "1");
    cb = isCollBoxModel ? &models[indexCollBox].collbox : &collboxes.at(indexCollBox).second;

    //:::: RENDER:::://
    while (!glfwWindowShouldClose(window))
    {

        //::::TIMING:::://Ayuda a crear animaciones fluidas
        float currentFrame = glfwGetTime();
        deltaTime = (currentFrame - lastFrame);
        lastFrame = currentFrame;
        respawnCount += 0.1;

        //::::ENTRADA CONTROL:::://
        processInput(window);
        //:::: LIMPIAMOS BUFFERS:::://
        glClearColor(0.933f, 0.811f, 0.647f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        //:::: PASAMOS INFORMACIÓN AL SHADER:::://
        ourShader.use();


        //ourShader lightingShader("multiple_lighting.vs", "multiple_lighting.fs");
        //ourShader lightCubeShader("lighting_maps.vs", "lighting_maps.fs");


        //:::: DEFINICIÓN DE MATRICES::::// La multiplicaciónd e model*view*projection crea nuestro entorno 3D
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom),
                                                (float) 250 / (float) 250, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        ourShader.setMat4("projection", projection);
        ourShader.setMat4("view", view);

        //Se llaman los Voids:
        //:::: RENDER DE MODELOS:::://
        drawModels(&ourShader, view, projection);
        //:::: SKYBOX Y TERRENO:::://
        loadEnviroment(&terrain, &sky, view, projection);
        //:::: COLISIONES :::://
        collisions();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    //:::: LIBERACIÓN DE MEMORIA:::://   
    delete[] texturePaths;
    sky.Release();
    terrain.Release();
    for (int i = 0; i < lightcubes.size(); i++)
        lightcubes[i].second.Release();
    for (int i = 0; i < collboxes.size(); i++)
        collboxes[i].second.Release();
    for (int i = 0; i < models.size(); i++)
        models[i].Release();
    for (int i = 0; i < rigidModels.size(); i++)
    {
        physicsEnviroment.Unregister(rigidModels[i].getRigidbox());
    }
    physicsEnviroment.Unregister(&piso);
    physicsEnviroment.Unregister(&pared);
    glfwTerminate();

    return 0;
}

void initScene(Shader ourShader)
{

    //AGUA
    //:::: DEFINIMOS LAS TEXTURAS DE LA MULTITEXTURA DEL TERRENO :::://
    texturePaths = new const char *[4];
    texturePaths[0] = "textures/multitexture_colors.jpg";
    texturePaths[1] = "textures/terrain1.png";
    texturePaths[2] = "textures/terrain2.png";
    texturePaths[3] = "textures/terrain3.png";

    //:::: POSICIONES DE TODAS LAS LUCES :::://
    pointLightPositions.push_back(glm::vec3(3.0f, 1.0f, 5.0f));         //posicion dirlight

    pointLightPositions.push_back(glm::vec3(0.5f, 5.0f, -2.2f));       //posiciones de pointlights
    pointLightPositions.push_back(glm::vec3(1.0f, 5.0f, -2.2f));       
    pointLightPositions.push_back(glm::vec3(1.5f, 5.0f, -2.2f));

    pointLightPositions.push_back(glm::vec3(2.5f, 5.0f, -2.2f));
    pointLightPositions.push_back(glm::vec3(3.0f, 5.0f, -2.2f));
    pointLightPositions.push_back(glm::vec3(3.5f, 5.0f, -2.2f));

    pointLightPositions.push_back(glm::vec3(4.5f, 5.0f, -2.2f));
    pointLightPositions.push_back(glm::vec3(5.0f, 5.0f, -2.2f));
    pointLightPositions.push_back(glm::vec3(5.5f, 5.0f, -2.2f));



    // Posiciones de las luces del ejercicio
    
    ejercicioIlumPositions.push_back(glm::vec3(3.0, 12.0, -2.0));        // posicion donde se origina el spotlight
    ejercicioIlumPositions.push_back(glm::vec3(0.0, -5.0, 0.0));        // direccion en la que apunta el spotlight

    ejercicioIlumPositions.push_back(glm::vec3(0.5f, 5.0f, -2.2f));       //posiciones de pointlights
    ejercicioIlumPositions.push_back(glm::vec3(1.0f, 5.0f, -2.2f));
    ejercicioIlumPositions.push_back(glm::vec3(1.5f, 5.0f, -2.2f));

    ejercicioIlumPositions.push_back(glm::vec3(2.5f, 5.0f, -2.2f));
    ejercicioIlumPositions.push_back(glm::vec3(3.0f, 5.0f, -2.2f));
    ejercicioIlumPositions.push_back(glm::vec3(3.5f, 5.0f, -2.2f));

    ejercicioIlumPositions.push_back(glm::vec3(4.5f, 5.0f, -2.2f));
    ejercicioIlumPositions.push_back(glm::vec3(5.0f, 5.0f, -2.2f));
    ejercicioIlumPositions.push_back(glm::vec3(5.5f, 5.0f, -2.2f));




    //:::: POSICIONES DE TODOS LOS OBJETOS CON FISICAS :::://
    physicsObjectsPositions.push_back(glm::vec3(0.0, 10.0, 0.0));
    physicsObjectsPositions.push_back(glm::vec3(2.0, 10.0, 0.0));
    physicsObjectsPositions.push_back(glm::vec3(1.0, 10.0, 0.0));
    physicsObjectsPositions.push_back(glm::vec3(-2.0, 10.0, -2.0));
    physicsObjectsPositions.push_back(glm::vec3(-2.0, 10.0, -2.0));
    physicsObjectsPositions.push_back(glm::vec3(15.0, 1.0, -2.0));
    physicsObjectsPositions.push_back(glm::vec3(15.0, 1.0, -2.0));
    physicsObjectsPositions.push_back(glm::vec3(25.0, 10.0, -2.0));


    //:::: ESTADO GLOBAL DE OPENGL :::://
    glEnable(GL_DEPTH_TEST);

    //Definimos los collbox de la camara
    camera.setCollBox();

    //:::: CARGAMOS LOS SHADERS :::://
    ourShader.use();
       

    //:::: INICIALIZAMOS NUESTROS MODELOS :::://        
    models.push_back(Model("estatua2", "modelos portafolio/estatua2nueva.obj", glm::vec3(1, -0.15, -2.2), glm::vec3(0, 90, 0), 0.0f, initScale));
    models.push_back(Model("estatua3", "modelos portafolio/estatua3nueva.obj", glm::vec3(3, -0.15, -2.2), glm::vec3(0, 270, 0), 0.0f, initScale));
    models.push_back(Model("estatua1", "modelos portafolio/estatua1nueva.obj", glm::vec3(5, -0.15, -2.2), glm::vec3(0, 270, 0), 0.0f, initScale));
    
    models.push_back(Model("pedestal2", "modelos portafolio/pedestal2.obj", glm::vec3(1, -0.15, -2.2), glm::vec3(0, 90, 0), 0.0f, initScale));
    models.push_back(Model("estatua3", "modelos portafolio/pedestal3.obj", glm::vec3(3, -0.15, -2.2), glm::vec3(0, 270, 0), 0.0f, initScale));
    models.push_back(Model("estatua3", "modelos portafolio/pedestal1.obj", glm::vec3(5, -0.15, -2.2), glm::vec3(0, 270, 0), 0.0f, initScale));   

    //models.push_back(Model("cuadro1", "modelos portafolio/cuadro1.obj", glm::vec3(7, -0.1, -2.67), glm::vec3(0, 270, 0), 0.0f, initScale));
    //models.push_back(Model("cuadro2", "modelos portafolio/cuadro2.obj", glm::vec3(9, -0.1, -2.67), glm::vec3(0, 270, 0), 0.0f, initScale));
    //models.push_back(Model("cuadro3", "modelos portafolio/cuadro3.obj", glm::vec3(11, -0.1, -2.67), glm::vec3(0, 270, 0), 0.0f, initScale));

    models.push_back(Model("muro", "modelos portafolio/muro.obj", glm::vec3(6, -0.1, -3), glm::vec3(0, 270, 0), 0.0f, initScale));

   
    //CREAMOS TODAS  LAS CAJAS DE COLISION INDIVIDUALES
    CollisionBox collbox;
    glm::vec4 colorCollbox(0.41f, 0.2f, 0.737f, 0.06f);
    collbox = CollisionBox(glm::vec3(25.97, 2.4, 11), glm::vec3(0.3, 5, 12.4), colorCollbox);
    collboxes.insert(pair<int, pair<string, CollisionBox>>(0, pair<string, CollisionBox>("pared_atras", collbox)));
    collbox = CollisionBox(glm::vec3(9.88, 2.6, 7.45999), glm::vec3(0.3, 4.6, 7.6), colorCollbox);
    collboxes.insert(pair<int, pair<string, CollisionBox>>(1, pair<string, CollisionBox>("pared_frente_izq", collbox)));
    collbox = CollisionBox(glm::vec3(10.37, 2.8, 18.87), glm::vec3(0.3, 5.4, 1.2), colorCollbox);
    collboxes.insert(pair<int, pair<string, CollisionBox>>(2, pair<string, CollisionBox>("pared_frente_der", collbox)));
    collbox = CollisionBox(glm::vec3(10.35, 5.41, 14.85), glm::vec3(0.3, 1, 3.6), colorCollbox);
    collboxes.insert(pair<int, pair<string, CollisionBox>>(3, pair<string, CollisionBox>("pared_frente_arriba", collbox)));

    //CREAMOS LOS LIGHTCUBES QUE ENREALIDAD SON COLLISION BOXES QUE NOS AYUDARAN A IDENTIFICAR LA POSICIÓN DE DONDE ESTAN
    glm::vec3 lScale(0.5);
    colorCollbox = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);

    collbox = CollisionBox(pointLightPositions[0], lScale, colorCollbox);
    lightcubes.insert(pair<int, pair<string, CollisionBox>>(0, pair<string, CollisionBox>("LUZDIR", collbox)));

    collbox = CollisionBox(pointLightPositions[1], lScale, colorCollbox);
    lightcubes.insert(pair<int, pair<string, CollisionBox>>(1, pair<string, CollisionBox>("LUZ1", collbox)));
    collbox = CollisionBox(pointLightPositions[2], lScale, colorCollbox);
    lightcubes.insert(pair<int, pair<string, CollisionBox>>(2, pair<string, CollisionBox>("LUZ2", collbox)));
    collbox = CollisionBox(pointLightPositions[3], lScale, colorCollbox);
    lightcubes.insert(pair<int, pair<string, CollisionBox>>(3, pair<string, CollisionBox>("LUZ3", collbox)));

    collbox = CollisionBox(pointLightPositions[4], lScale, colorCollbox);
    lightcubes.insert(pair<int, pair<string, CollisionBox>>(4, pair<string, CollisionBox>("LUZ4", collbox)));
    collbox = CollisionBox(pointLightPositions[5], lScale, colorCollbox);
    lightcubes.insert(pair<int, pair<string, CollisionBox>>(5, pair<string, CollisionBox>("LUZ5", collbox)));
    collbox = CollisionBox(pointLightPositions[6], lScale, colorCollbox);
    lightcubes.insert(pair<int, pair<string, CollisionBox>>(6, pair<string, CollisionBox>("LUZ6", collbox)));

    collbox = CollisionBox(pointLightPositions[7], lScale, colorCollbox);
    lightcubes.insert(pair<int, pair<string, CollisionBox>>(7, pair<string, CollisionBox>("LUZ7", collbox)));
    collbox = CollisionBox(pointLightPositions[8], lScale, colorCollbox);
    lightcubes.insert(pair<int, pair<string, CollisionBox>>(8, pair<string, CollisionBox>("LUZ8", collbox)));
    collbox = CollisionBox(pointLightPositions[9], lScale, colorCollbox);
    lightcubes.insert(pair<int, pair<string, CollisionBox>>(9, pair<string, CollisionBox>("LUZ9", collbox)));



   
     
}
//:::: CONFIGURACIONES :::://

void loadEnviroment(Terrain *terrain, SkyBox *sky, glm::mat4 view, glm::mat4 projection)
{
    glm::mat4 model = mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0, -2.5f, 0.0f));   // translate it down so it's at the center of the scene
    model = glm::scale(model, glm::vec3(100.5f, 100.0f, 100.5f)); // it's a bit too big for our scene, so scale it down

    terrain->draw(model, view, projection);    
    setMultipleLight(terrain->getShader(), pointLightPositions);
    // terrain->getShader()->setFloat("shininess", 100.0f);  // paso a void setmultiplelight

    glm::mat4 skyModel = mat4(1.0f);
    skyModel = glm::translate(skyModel, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
    skyModel = glm::scale(skyModel, glm::vec3(40.0f, 40.0f, 40.0f));  // it's a bit too big for our scene, so scale it down
    sky->draw(skyModel, view, projection, skyPos);
    sky->getShader()->setFloat("shininess", 10.0f);
    setMultipleLight(sky->getShader(), pointLightPositions);

    //RENDER DE LIGHT CUBES
    if (renderLightingCubes)
        for (pair<int, pair<string, CollisionBox>> lights : lightcubes)
            lights.second.second.draw(view, projection);

}


void drawModels(Shader *shader, glm::mat4 view, glm::mat4 projection)
{
    //DEFINIMOS EL BRILLO  DEL MATERIAL
    shader->setFloat("material.shininess", 60.0f);

    //setSimpleLight(shader);

    setMultipleLight(shader, pointLightPositions);
    for (int i = 0; i < models.size(); i++)    
    {
        //SI SE RECOGIO EL OBJETO
        shader->use();
        models[i].Draw(*shader);
        detectColls(&models[i].collbox, models[i].name, &camera, renderCollBox, collidedObject_callback);
    }
}

void setSimpleLight(Shader *shader)             // No esta activa, parece ser linea 230: setSimpleLight(shader);
{
    shader->setVec3("light.ambient", 0.2f, 0.2f, 0.2f);
    shader->setVec3("light.diffuse", 0.5f, 0.5f, 0.5f);
    shader->setVec3("light.specular", 1.0f, 1.0f, 1.0f);
    shader->setInt("lightType", (int)lightType);
    shader->setVec3("light.position", lightPos);
    shader->setVec3("light.direction", lightDir);
    shader->setFloat("light.cutOff", glm::cos(glm::radians(12.5f)));
    shader->setFloat("light.outerCutOff", glm::cos(glm::radians(17.5f)));
    shader->setVec3("viewPos", camera.Position);
    shader->setFloat("light.constant", 0.2f);
    shader->setFloat("light.linear", 0.02f);
    shader->setFloat("light.quadratic", 0.032f);
    shader->setFloat("material.shininess", 60.0f);
}


void setMultipleLight(Shader *shader, vector<glm::vec3> pointLightPositions)        //Se llama desde linea 232:  setMultipleLight(shader, pointLightPositions);   
{
    shader->setFloat("material.shininess", 60.0f);
    shader->setVec3("viewPos", camera.Position);

    // iluminacion de ejercicio
    shader->setVec3("dl.direction", pointLightPositions[0]);
    shader->setVec3("dl.ambient", mainLight.x, mainLight.y, mainLight.z);
    shader->setVec3("dl.diffuse", mainLight.x, mainLight.y, mainLight.z);
    shader->setVec3("dl.specular", mainLight.x, mainLight.y, mainLight.z);

    
    // luz roja
    //shader->setVec3("plred.position", ejercicioIlumPositions[1]);
    shader->setVec3("plred.position", pointLightPositions[1]);
    shader->setVec3("plred.ambient", 0.3f, 0.0f, 0.0f);
    shader->setVec3("plred.diffuse", 0.3f, 0.0f, 0.0f);
    shader->setVec3("plred.specular", 0.3f, 0.0f, 0.0f);
    shader->setFloat("plred.constant", 1.0f);
    shader->setFloat("plred.linear", 0.09);
    shader->setFloat("plred.quadratic", 0.032);

    /*
    // luz verde
    //shader->setVec3("plgreen.position", ejercicioIlumPositions[2]);
    shader->setVec3("plgreen.position", pointLightPositions[2]);
    shader->setVec3("plgreen.ambient", 0.0f, 0.3f, 0.0f);
    shader->setVec3("plgreen.diffuse", 0.0f, 0.3f, 0.0f);
    shader->setVec3("plgreen.specular", 0.0f, 0.3f, 0.0f);
    shader->setFloat("plgreen.constant", 1.0f);
    shader->setFloat("plgreen.linear", 0.09);
    shader->setFloat("plgreen.quadratic", 0.032);


    // luz azul
    //shader->setVec3("plblue.position", ejercicioIlumPositions[3]);
    shader->setVec3("plblue.position", pointLightPositions[3]);
    shader->setVec3("plblue.ambient", 0.0f, 0.0f, 0.3f);
    shader->setVec3("plblue.diffuse", 0.0f, 0.0f, 0.3f);
    shader->setVec3("plblue.specular", 0.0f, 0.0f, 0.3f);
    shader->setFloat("plblue.constant", 1.0f);
    shader->setFloat("plblue.linear", 0.09);
    shader->setFloat("plblue.quadratic", 0.032);
    

    
    // luz roja
    //shader->setVec3("plr.position", ejercicioIlumPositions[4]);
    shader->setVec3("plr.position", pointLightPositions[4]);
    shader->setVec3("plr.ambient", 0.3f, 0.0f, 0.0f);
    shader->setVec3("plr.diffuse", 0.3f, 0.0f, 0.0f);
    shader->setVec3("plr.specular", 0.3f, 0.0f, 0.0f);
    shader->setFloat("plr.constant", 1.0f);
    shader->setFloat("plr.linear", 0.09);
    shader->setFloat("plr.quadratic", 0.032);
    */

    // luz verde
    //shader->setVec3("plg.position", ejercicioIlumPositions[5]);
    shader->setVec3("plg.position", pointLightPositions[5]);
    shader->setVec3("plg.ambient", 0.0f, 0.3f, 0.0f);
    shader->setVec3("plg.diffuse", 0.0f, 0.3f, 0.0f);
    shader->setVec3("plg.specular", 0.0f, 0.3f, 0.0f);
    shader->setFloat("plg.constant", 1.0f);
    shader->setFloat("plg.linear", 0.09);
    shader->setFloat("plg.quadratic", 0.032);

    /*
    // luz azul
    //shader->setVec3("plb.position", ejercicioIlumPositions[6]);
    shader->setVec3("plb.position", pointLightPositions[6]);
    shader->setVec3("plb.ambient", 0.0f, 0.0f, 0.3f);
    shader->setVec3("plb.diffuse", 0.0f, 0.0f, 0.3f);
    shader->setVec3("plb.specular", 0.0f, 0.0f, 0.3f);
    shader->setFloat("plb.constant", 1.0f);
    shader->setFloat("plb.linear", 0.09);
    shader->setFloat("plb.quadratic", 0.032);

    
    // luz roja
    //shader->setVec3("plroja.position", ejercicioIlumPositions[7]);
    shader->setVec3("plr.position", pointLightPositions[7]);
    shader->setVec3("plroja.ambient", 0.3f, 0.0f, 0.0f);
    shader->setVec3("plroja.diffuse", 0.3f, 0.0f, 0.0f);
    shader->setVec3("plroja.specular", 0.3f, 0.0f, 0.0f);
    shader->setFloat("plroja.constant", 1.0f);
    shader->setFloat("plroja.linear", 0.09);
    shader->setFloat("plroja.quadratic", 0.032);


    // luz verde
    //shader->setVec3("plverde.position", ejercicioIlumPositions[8]);
    shader->setVec3("plg.position", pointLightPositions[8]);
    shader->setVec3("plverde.ambient", 0.0f, 0.3f, 0.0f);
    shader->setVec3("plverde.diffuse", 0.0f, 0.3f, 0.0f);
    shader->setVec3("plverde.specular", 0.0f, 0.3f, 0.0f);
    shader->setFloat("plverde.constant", 1.0f);
    shader->setFloat("plverde.linear", 0.09);
    shader->setFloat("plverde.quadratic", 0.032);
    */

    // luz azul
    //shader->setVec3("plazul.position", ejercicioIlumPositions[9]);
    shader->setVec3("plb.position", pointLightPositions[9]);
    shader->setVec3("plazul.ambient", 0.0f, 0.0f, 0.3f);
    shader->setVec3("plazul.diffuse", 0.0f, 0.0f, 0.3f);
    shader->setVec3("plazul.specular", 0.0f, 0.0f, 0.3f);
    shader->setFloat("plazul.constant", 1.0f);
    shader->setFloat("plazul.linear", 0.09);
    shader->setFloat("plazul.quadratic", 0.032);


    // spot light
    shader->setVec3("sl.position", ejercicioIlumPositions[0]);
    shader->setVec3("sl.direction", ejercicioIlumPositions[1]);
    shader->setVec3("sl.ambient", 0.0f, 0.0f, 0.0f);
    shader->setVec3("sl.diffuse", 1.0f, 1.0f, 1.0f);
    shader->setVec3("sl.specular", 1.0f, 1.0f, 1.0f);
    shader->setFloat("sl.constant", 1.0f);
    shader->setFloat("sl.linear", 0.045);
    shader->setFloat("sl.quadratic", 0.0075);
    shader->setFloat("sl.cutOff", glm::cos(glm::radians(12.5f)));
    shader->setFloat("sl.outerCutOff", glm::cos(glm::radians(15.0f)));

    // switch de iluminacion
    shader->setBool("ejercicioIluminacion", ejercicioIluminacion);



    /*
    shader->setVec3("dirLights[0].direction", pointLightPositions[0]);
    shader->setVec3("dirLights[0].ambient", mainLight.x, mainLight.y, mainLight.z);
    shader->setVec3("dirLights[0].diffuse", mainLight.x, mainLight.y, mainLight.z);
    shader->setVec3("dirLights[0].specular", mainLight.x, mainLight.y, mainLight.z);

    shader->setVec3("dirLights[1].direction", pointLightPositions[1]);
    shader->setVec3("dirLights[1].ambient", 0.05f, 0.05f, 0.05f);
    shader->setVec3("dirLights[1].diffuse", 0.4f, 0.4f, 0.4f);
    shader->setVec3("dirLights[1].specular", 0.5f, 0.5f, 0.5f);

    shader->setVec3("dirLights[2].direction", pointLightPositions[2]);
    shader->setVec3("dirLights[2].ambient", 0.3f, 0.5f, 0.5f);
    shader->setVec3("dirLights[2].diffuse", 0.4f, 0.4f, 0.4f);
    shader->setVec3("dirLights[2].specular", 0.5f, 0.5f, 0.5f);

    shader->setVec3("dirLights[3].direction", pointLightPositions[3]);
    shader->setVec3("dirLights[3].ambient", 0.05f, 0.05f, 0.05f);
    shader->setVec3("dirLights[3].diffuse", 0.4f, 0.4f, 0.4f);
    shader->setVec3("dirLights[3].specular", 0.5f, 0.5f, 0.5f);


    //estatua 1
    shader->setVec3("dirLights[4].direction", pointLightPositions[4]);
    shader->setVec3("dirLights[4].ambient", 0.05f, 0.05f, 0.05f);
    shader->setVec3("dirLights[4].diffuse", 0.4f, 0.4f, 0.4f);
    shader->setVec3("dirLights[4].specular", 0.5f, 0.5f, 0.5f);
    */
    
    /*
    shader->setVec3("pointLights[0].position", pointLightPositions[1]);
    shader->setVec3("pointLights[0].ambient", 1.0f, 0.0f, 0.0f);
    shader->setVec3("pointLights[0].diffuse", 1.0f, 0.0f, 0.0f);
    shader->setVec3("pointLights[0].specular", 1.0f, 0.0f, 0.0f);
    shader->setFloat("pointLights[0].constant", 1.0f);
    shader->setFloat("pointLights[0].linear", 0.09);
    shader->setFloat("pointLights[0].quadratic", 0.032);

    shader->setVec3("pointLights[1].position", pointLightPositions[2]);
    shader->setVec3("pointLights[1].ambient", 0.0f, 1.0f, 0.0f);
    shader->setVec3("pointLights[1].diffuse", 0.0f, 1.0f, 0.0f);
    shader->setVec3("pointLights[1].specular", 0.0f, 1.0f, 0.0f);
    shader->setFloat("pointLights[1].constant", 1.0f);
    shader->setFloat("pointLights[1].linear", 0.09);
    shader->setFloat("pointLights[1].quadratic", 0.032);

    shader->setVec3("pointLights[2].position", pointLightPositions[3]);
    shader->setVec3("pointLights[2].ambient", 0.0f, 0.0f, 1.0f);
    shader->setVec3("pointLights[2].diffuse", 0.0f, 0.0f, 1.0f);
    shader->setVec3("pointLights[2].specular", 0.0f, 0.0f, 1.0f);
    shader->setFloat("pointLights[2].constant", 1.0f);
    shader->setFloat("pointLights[2].linear", 0.09);
    shader->setFloat("pointLights[2].quadratic", 0.032);

    shader->setVec3("pointLights[3].position", pointLightPositions[4]);
    shader->setVec3("pointLights[3].ambient", 1.0f, 0.0f, 0.0f);
    shader->setVec3("pointLights[3].diffuse", 1.0f, 0.0f, 0.0f);
    shader->setVec3("pointLights[3].specular", 1.0f, 0.0f, 0.0f);
    shader->setFloat("pointLights[3].constant", 1.0f);
    shader->setFloat("pointLights[3].linear", 0.09);
    shader->setFloat("pointLights[3].quadratic", 0.032);

    shader->setVec3("pointLights[4].position", pointLightPositions[5]);
    shader->setVec3("pointLights[4].ambient", 0.0f, 1.0f, 0.0f);
    shader->setVec3("pointLights[4].diffuse", 0.0f, 1.0f, 0.0f);
    shader->setVec3("pointLights[4].specular", 0.0f, 1.0f, 0.0f);
    shader->setFloat("pointLights[4].constant", 1.0f);
    shader->setFloat("pointLights[4].linear", 0.09);
    shader->setFloat("pointLights[4].quadratic", 0.032);

    shader->setVec3("pointLights[5].position", pointLightPositions[6]);
    shader->setVec3("pointLights[5].ambient", 0.0f, 0.0f, 1.0f);
    shader->setVec3("pointLights[5].diffuse", 0.0f, 0.0f, 1.0f);
    shader->setVec3("pointLights[5].specular", 0.0f, 0.0f, 1.0f);
    shader->setFloat("pointLights[5].constant", 1.0f);
    shader->setFloat("pointLights[5].linear", 0.09);
    shader->setFloat("pointLights[5].quadratic", 0.032);

    shader->setVec3("pointLights[6].position", pointLightPositions[7]);
    shader->setVec3("pointLights[6].ambient", 1.0f, 0.0f, 0.0f);
    shader->setVec3("pointLights[6].diffuse", 1.0f, 0.0f, 0.0f);
    shader->setVec3("pointLights[6].specular", 1.0f, 0.0f, 0.0f);
    shader->setFloat("pointLights[6].constant", 1.0f);
    shader->setFloat("pointLights[6].linear", 0.09);
    shader->setFloat("pointLights[6].quadratic", 0.032);

    shader->setVec3("pointLights[7].position", pointLightPositions[8]);
    shader->setVec3("pointLights[7].ambient", 0.0f, 1.0f, 0.0f);
    shader->setVec3("pointLights[7].diffuse", 0.0f, 1.0f, 0.0f);
    shader->setVec3("pointLights[7].specular", 0.0f, 1.0f, 0.0f);
    shader->setFloat("pointLights[7].constant", 1.0f);
    shader->setFloat("pointLights[7].linear", 0.09);
    shader->setFloat("pointLights[7].quadratic", 0.032);

    shader->setVec3("pointLights[8].position", pointLightPositions[9]);
    shader->setVec3("pointLights[8].ambient", 0.0f, 0.0f, 1.0f);
    shader->setVec3("pointLights[8].diffuse", 0.0f, 0.0f, 1.0f);
    shader->setVec3("pointLights[8].specular", 0.0f, 0.0f, 1.0f);
    shader->setFloat("pointLights[8].constant", 1.0f);
    shader->setFloat("pointLights[8].linear", 0.09);
    shader->setFloat("pointLights[8].quadratic", 0.032);
    */
    
    // spotLight
    /*
    shader->setVec3("spotLights[0].position", pointLightPositions[0]);
    shader->setVec3("spotLights[0].direction", glm::vec3(0.2, 0.8, 0.2));
    shader->setVec3("spotLights[0].ambient", 0.0f, 0.0f, 0.0f);
    shader->setVec3("spotLights[0].diffuse", 1.0f, 1.0f, 1.0f);
    shader->setVec3("spotLights[0].specular", 1.0f, 1.0f, 1.0f);
    shader->setFloat("spotLights[0].constant", 1.0f);
    shader->setFloat("spotLights[0].linear", 0.09);
    shader->setFloat("spotLights[0].quadratic", 0.032);
    shader->setFloat("spotLights[0].cutOff", glm::cos(glm::radians(12.5f)));
    shader->setFloat("spotLights[0].outerCutOff", glm::cos(glm::radians(15.0f)));
    
    shader->setVec3("spotLights[1].position", pointLightPositions[1]);
    shader->setVec3("spotLights[1].direction", camera.Front);
    shader->setVec3("spotLights[1].ambient", 0.0f, 0.0f, 0.0f);
    shader->setVec3("spotLights[1].diffuse", 1.0f, 1.0f, 1.0f);
    shader->setVec3("spotLights[1].specular", 1.0f, 1.0f, 1.0f);
    shader->setFloat("spotLights[1].constant", 1.0f);
    shader->setFloat("spotLights[1].linear", 0.09);
    shader->setFloat("spotLights[1].quadratic", 0.032);
    shader->setFloat("spotLights[1].cutOff", glm::cos(glm::radians(12.5f)));
    shader->setFloat("spotLights[1].outerCutOff", glm::cos(glm::radians(15.0f)));

    shader->setVec3("spotLights[2].position", pointLightPositions[2]);
    shader->setVec3("spotLights[2].direction", camera.Front);
    shader->setVec3("spotLights[2].ambient", 0.0f, 0.0f, 0.0f);
    shader->setVec3("spotLights[2].diffuse", 1.0f, 1.0f, 1.0f);
    shader->setVec3("spotLights[2].specular", 1.0f, 1.0f, 1.0f);
    shader->setFloat("spotLights[2].constant", 1.0f);
    shader->setFloat("spotLights[2].linear", 0.09);
    shader->setFloat("spotLights[2].quadratic", 0.032);
    shader->setFloat("spotLights[2].cutOff", glm::cos(glm::radians(12.5f)));
    shader->setFloat("spotLights[2].outerCutOff", glm::cos(glm::radians(15.0f)));

    shader->setVec3("spotLights[3].position", pointLightPositions[3]);
    shader->setVec3("spotLights[3].direction", camera.Front);
    shader->setVec3("spotLights[3].ambient", 0.0f, 0.0f, 0.0f);
    shader->setVec3("spotLights[3].diffuse", 1.0f, 1.0f, 1.0f);
    shader->setVec3("spotLights[3].specular", 1.0f, 1.0f, 1.0f);
    shader->setFloat("spotLights[3].constant", 1.0f);
    shader->setFloat("spotLights[3].linear", 0.09);
    shader->setFloat("spotLights[3].quadratic", 0.032);
    shader->setFloat("spotLights[3].cutOff", glm::cos(glm::radians(12.5f)));
    shader->setFloat("spotLights[3].outerCutOff", glm::cos(glm::radians(15.0f)));


    //estatua 1
    shader->setVec3("spotLights[4].position", pointLightPositions[4]);
    shader->setVec3("spotLights[4].direction", camera.Front);
    shader->setVec3("spotLights[4].ambient", 0.0f, 0.0f, 0.0f);
    shader->setVec3("spotLights[4].diffuse", 1.0f, 1.0f, 1.0f);
    shader->setVec3("spotLights[4].specular", 1.0f, 1.0f, 1.0f);
    shader->setFloat("spotLights[4].constant", 1.0f);
    shader->setFloat("spotLights[4].linear", 0.09);
    shader->setFloat("spotLights[4].quadratic", 0.032);
    shader->setFloat("spotLights[4].cutOff", glm::cos(glm::radians(12.5f)));
    shader->setFloat("spotLights[4].outerCutOff", glm::cos(glm::radians(15.0f)));
    */

    shader->setInt("lightType", (int)lightType);
    shader->setInt("maxRenderLights", 1);
}


void collisions()
{
    //TODO LO DE LAS COLISIONES VA AQUÍ
    detectColls(collboxes, &camera, renderCollBox, collidedObject_callback);
}