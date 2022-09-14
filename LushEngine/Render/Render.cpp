#include "Render.hpp"

using namespace Lush;

Render::Render(std::shared_ptr<MessageBus> messageBus) : Node(messageBus)
{
    this->_functionList.push_back(std::bind(&Render::receiveLoadIcon, this, std::placeholders::_1));
    this->_functionList.push_back(std::bind(&Render::receiveLoadShaders, this, std::placeholders::_1));
    this->_functionList.push_back(std::bind(&Render::receiveLoadTextures, this, std::placeholders::_1));
    this->_functionList.push_back(std::bind(&Render::receiveLoadSkyBox, this, std::placeholders::_1));
    this->_functionList.push_back(std::bind(&Render::receiveLoadModels, this, std::placeholders::_1));
    this->_functionList.push_back(std::bind(&Render::receiveAddObject, this, std::placeholders::_1));
    this->_functionList.push_back(std::bind(&Render::receiveClearObject, this, std::placeholders::_1));
    this->_functionList.push_back(std::bind(&Render::receiveScenesName, this, std::placeholders::_1));

    if (!glfwInit())
        throw std::runtime_error("GLFW failed to initialize");
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    this->_windowWidth = 1920.0f;
    this->_windowHeight = 1080.0f;
    this->_window = glfwCreateWindow(this->_windowWidth, this->_windowHeight, "Lush Engine", nullptr, nullptr);
    this->_lastFrame = 0.0f;
    this->_deltaTime = 0.0f;
    this->_mouseX = this->_windowWidth / 2.0f;
    this->_mouseY = this->_windowHeight / 2.0f;
    this->_lastMouseX = this->_mouseX;
    this->_lastMouseY = this->_mouseY;
    this->_freeCamera = false;
    this->_dirLightAngle = glm::pi<float>() / 2.0f;

    if (this->_window == NULL)
        throw std::runtime_error("Failed to create GLFW window");

    glfwMakeContextCurrent(this->_window);
    glfwSetWindowUserPointer(this->_window, this);
    auto keyboardPressCallback = [](GLFWwindow *w, int key, int scan, int action, int mods) {
        static_cast<Render *>(glfwGetWindowUserPointer(w))->handleKeyboardPress(key, scan, action, mods);
    };
    glfwSetKeyCallback(this->_window, keyboardPressCallback);

    if (glewInit() != GLEW_OK)
        throw std::runtime_error("GLEW failed to initialize");
    if (!GLEW_VERSION_2_1)
        throw std::runtime_error("GLEW does not support OpenGL 2.1");
    this->_showCameraImGui = true;
    this->_showWindowImGui = true;
    this->_selectionImGui = 0;
    glGenFramebuffers(1, &this->_hoverBuffer.framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, this->_hoverBuffer.framebuffer);
    glGenTextures(1, &this->_hoverBuffer.texture);
    glBindTexture(GL_TEXTURE_2D, this->_hoverBuffer.texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, this->_windowWidth, this->_windowHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->_hoverBuffer.texture, 0);
    glGenRenderbuffers(1, &this->_hoverBuffer.depthbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, this->_hoverBuffer.depthbuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, this->_windowWidth, this->_windowHeight);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, this->_hoverBuffer.depthbuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    float quadVertices[] = {-1.0f, 1.0f, 0.0f, 1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, -1.0f, 1.0f, 0.0f,

                            -1.0f, 1.0f, 0.0f, 1.0f, 1.0f,  -1.0f, 1.0f, 0.0f, 1.0f, 1.0f,  1.0f, 1.0f};
    glGenVertexArrays(1, &this->_planeVAO);
    glGenBuffers(1, &this->_planeVBO);
    glBindVertexArray(this->_planeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, this->_planeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)(2 * sizeof(float)));
    float skyboxVertices[] = {// positions
                              -1.0f, 1.0f,  -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  -1.0f, -1.0f, 1.0f,  -1.0f, -1.0f, 1.0f,  1.0f,  -1.0f, -1.0f, 1.0f,  -1.0f,

                              -1.0f, -1.0f, 1.0f,  -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  -1.0f, -1.0f, 1.0f,  -1.0f, -1.0f, 1.0f,  1.0f,  -1.0f, -1.0f, 1.0f,

                              1.0f,  -1.0f, -1.0f, 1.0f,  -1.0f, 1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  -1.0f, 1.0f,  -1.0f, -1.0f,

                              -1.0f, -1.0f, 1.0f,  -1.0f, 1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  -1.0f, 1.0f,  -1.0f, -1.0f, 1.0f,

                              -1.0f, 1.0f,  -1.0f, 1.0f,  1.0f,  -1.0f, 1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  -1.0f, 1.0f,  1.0f,  -1.0f, 1.0f,  -1.0f,

                              -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  1.0f,  -1.0f, -1.0f, 1.0f,  -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  1.0f,  -1.0f, 1.0f};
    glGenVertexArrays(1, &this->_skyBoxVAO);
    glGenBuffers(1, &this->_skyBoxVBO);
    glBindVertexArray(this->_skyBoxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, this->_skyBoxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);

    GLint maxTessLevel;
    glGetIntegerv(GL_MAX_TESS_GEN_LEVEL, &maxTessLevel);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    if (!IMGUI_CHECKVERSION())
        throw std::runtime_error("IMGUI version is invalid");
    ImGui::CreateContext();

    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(this->_window, true);
    ImGui_ImplOpenGL3_Init("#version 130");

    this->_hover = 0;
    this->_map = std::make_unique<Map>(2624, 1756);
}

Render::~Render()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    if (this->_window != NULL)
        glfwDestroyWindow(this->_window);
    glfwTerminate();
}

void Render::run()
{
    while (this->_running) {
        this->_messageBus->notify(Module::RENDER, this->_functionList);
        if (glfwWindowShouldClose(this->_window)) {
            this->_running = false; // because it can sometimes send the quit message twice
            this->sendMessage(Message(Packet(), BaseCommand::QUIT, Module::BROADCAST));
        }
        if (!this->_launched) {
            std::this_thread::sleep_for(std::chrono::milliseconds(16));
            continue;
        }
        this->handleTime();
        this->handleMouseMovement();
        this->handleMouseClick();
        this->handleKeyboard();
        this->update();
        this->draw();
        glfwPollEvents();
    }
}

void Render::update()
{
    this->_camera->update();
    this->_pointLights.clear();
    for (auto &[key, object] : this->_objects)
        if (object->isLightSource())
            this->_pointLights.push_back(object->getPosition());
    for (auto &[key, object] : this->_objects)
        object->setHovered(this->_hover - 1 == static_cast<int>(key));
}

void Render::draw()
{
    this->drawImGui();
    this->drawScene();
    this->drawMap();
    this->drawSkyBox();
    this->drawPicking();
    this->drawOutline();

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    glfwSwapBuffers(this->_window);
}

void Render::handleTime()
{
    float currentFrame = static_cast<float>(glfwGetTime());

    this->_deltaTime = currentFrame - this->_lastFrame;
    this->_lastFrame = currentFrame;
}

void Render::handleMouseMovement()
{
    glm::vec2 pos(0.0f);

    this->_lastMouseX = this->_mouseX;
    this->_lastMouseY = this->_mouseY;
    glfwGetCursorPos(this->_window, &this->_mouseX, &this->_mouseY);
    if (this->_freeCamera) {
        this->_camera->processMouseMovement(this->_mouseX - this->_lastMouseX, this->_lastMouseY - this->_mouseY);
        pos = {this->_windowWidth / 2.0f, this->_windowHeight / 2.0f};
    } else {
        if (ImGui::GetIO().WantCaptureMouse) {
            this->_hover = -1;
            return;
        }
        if (this->_mouseX < 0 || this->_mouseX > this->_windowWidth || this->_mouseY < 0 || this->_mouseY > this->_windowHeight)
            this->_hover = -1;
        pos = {this->_mouseX, this->_mouseY};
    }
    glBindFramebuffer(GL_FRAMEBUFFER, this->_hoverBuffer.framebuffer);
    unsigned char pixel[4];
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glReadPixels(pos.x, this->_windowHeight - pos.y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, &pixel);
    this->_hover = (pixel[0] << 16) + (pixel[1] << 8) + (pixel[2]);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Render::handleMouseClick()
{
    if (glfwGetMouseButton(this->_window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
        if (this->_hover > 0 && this->_objects.find(this->_hover - 1) != this->_objects.end())
            this->_objects[this->_hover - 1]->setSelected(true);
}

void Render::handleKeyboard()
{
    if (glfwGetKey(this->_window, GLFW_KEY_W) == GLFW_PRESS)
        this->_camera->processKeyboard(FRONT, this->_deltaTime);
    if (glfwGetKey(this->_window, GLFW_KEY_S) == GLFW_PRESS)
        this->_camera->processKeyboard(BACK, this->_deltaTime);
    if (glfwGetKey(this->_window, GLFW_KEY_A) == GLFW_PRESS)
        this->_camera->processKeyboard(LEFT, this->_deltaTime);
    if (glfwGetKey(this->_window, GLFW_KEY_D) == GLFW_PRESS)
        this->_camera->processKeyboard(RIGHT, this->_deltaTime);
    if (glfwGetKey(this->_window, GLFW_KEY_Q) == GLFW_PRESS)
        this->_camera->processKeyboard(UP, this->_deltaTime);
    if (glfwGetKey(this->_window, GLFW_KEY_E) == GLFW_PRESS)
        this->_camera->processKeyboard(DOWN, this->_deltaTime);
}

void Render::handleKeyboardPress(int key, [[maybe_unused]] int scancode, int action, [[maybe_unused]] int mods)
{
    if (key == GLFW_KEY_TAB && action == GLFW_PRESS) {
        this->_freeCamera = !this->_freeCamera;
        glfwSetInputMode(this->_window, GLFW_CURSOR, this->_freeCamera ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
    }
}

bool Render::showImGui(bool *open)
{
    bool changed = false;

    if (ImGui::Begin("Window", open)) {
        if (ImGui::BeginTabBar("First")) {
            if (ImGui::BeginTabItem("Scenes")) {
                if (ImGui::BeginListBox("")) {
                    for (int i = 0; i < static_cast<int>(this->_scenes.size()); i++) {
                        bool selected = (this->_selectionImGui == i);

                        if (ImGui::Selectable(this->_scenes[i].c_str(), selected)) {
                            this->_selectionImGui = i;
                            changed = true;
                        }
                        if (selected)
                            ImGui::SetItemDefaultFocus();
                    }
                    ImGui::EndListBox();
                }
                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Light")) {
                ImGui::SliderAngle("Angle", &this->_dirLightAngle, 0.0f, 180.0f);
                ImGui::EndTabItem();
            }
            ImGui::EndTabBar();
        }
        ImGui::Separator();
        ImGui::Text("Frames: %.1f FPS", 1.0f / this->_deltaTime);
    }
    ImGui::End();
    return changed;
}

void Render::drawImGui()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    if (this->_showWindowImGui)
        if (this->showImGui(&this->_showWindowImGui))
            this->sendMessage(Message(Packet(this->_scenes[this->_selectionImGui]), CoreCommand::LOAD_SCENE, Module::CORE));

    if (this->_showCameraImGui)
        this->_camera->showImGui(&this->_showCameraImGui);

    for (auto &[key, object] : this->_objects)
        if (object->isSelected()) {
            if (object->showImGui(key)) {
                Packet packet;
                packet << key << object->getPosition();
                this->sendMessage(Message(packet, CoreCommand::MOVE_OBJECT, Module::CORE));
            }
        }

    ImGui::Render();
}

void Render::drawScene()
{
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    this->_camera->use("Camera");
    this->_camera->setView(glfwGetTime());
    this->_camera->setDirLight(glm::vec3(0.0f, -glm::sin(this->_dirLightAngle), glm::cos(this->_dirLightAngle)));
    this->_camera->setPointLights(this->_pointLights);

    for (auto &[key, object] : this->_objects)
        if (!object->isSelected())
            object->draw(*this->_camera);

    this->_camera->use("Selection");
    this->_camera->setView(glfwGetTime());
    this->_camera->setDirLight(glm::vec3(0.0f, -glm::sin(this->_dirLightAngle), glm::cos(this->_dirLightAngle)));
    this->_camera->setPointLights(this->_pointLights);
    for (auto &[key, object] : this->_objects)
        if (object->isSelected())
            object->draw(*this->_camera);
}

void Render::drawMap()
{
    this->_camera->use("Map");
    this->_camera->setView(glfwGetTime());
    this->_camera->getShader()->setMat4("model", glm::mat4(1.0f));
    // this->_camera->setDirLight(glm::vec3(0.0f, -glm::sin(this->_dirLightAngle), glm::cos(this->_dirLightAngle)));
    // this->_camera->setPointLights(this->_pointLights);
    // this->_camera->getShader()->setInt("heightMap", 0);
    glBindTexture(GL_TEXTURE_2D, this->_textures["heightMap.png"]);

    if (this->_map != nullptr)
        this->_map->draw(*this->_camera->getShader());
}

void Render::drawPicking()
{
    glBindFramebuffer(GL_FRAMEBUFFER, this->_hoverBuffer.framebuffer);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    this->_camera->use("Picking");
    this->_camera->setView(glfwGetTime());
    for (auto &[key, object] : this->_objects) {
        glm::vec4 temp;
        temp.r = (((key + 1) & 0x00FF0000) >> 16) / 255.0f;
        temp.g = (((key + 1) & 0x0000FF00) >> 8) / 255.0f;
        temp.b = (((key + 1) & 0x000000FF) >> 0) / 255.0f;
        temp.a = 1.0f;
        this->_camera->getShader()->setVec4("id", temp);
        object->draw(*this->_camera);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Render::drawOutline()
{
    this->_camera->use("Outline");
    this->_camera->getShader()->setInt("id", this->_hover);

    glBindVertexArray(this->_planeVAO);
    glBindTexture(GL_TEXTURE_2D, this->_hoverBuffer.texture);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

void Render::drawSkyBox()
{
    glDepthFunc(GL_LEQUAL);
    this->_camera->use("SkyBox");
    this->_camera->setSkyBox();
    this->_camera->getShader()->setInt("skybox", 0);

    glBindVertexArray(this->_skyBoxVAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, this->_skyBoxTexture);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
    glDepthFunc(GL_LESS);
}

void Render::receiveLoadIcon(Packet packet)
{
    std::string texture;
    packet >> texture;

    GLFWimage images[1];
    images[0].pixels = stbi_load_from_memory((const stbi_uc *)texture.c_str(), static_cast<int>(texture.size()), &images[0].width, &images[0].height, 0, 4); // rgba channels
    glfwSetWindowIcon(this->_window, 1, images);
    stbi_image_free(images[0].pixels);
}

void Render::receiveLoadShaders(Packet packet)
{
    std::string name;
    std::string vertexCode;
    std::string fragmentCode;
    std::string geometryCode;
    std::string tessControlCode;
    std::string tessEvalCode;

    while (!packet.empty()) {
        packet >> name >> vertexCode >> fragmentCode >> geometryCode >> tessControlCode >> tessEvalCode;
        this->_shaders[name] = std::make_shared<Shader>(vertexCode, fragmentCode, geometryCode, tessControlCode, tessEvalCode);
    }
    this->_camera = std::make_unique<Camera>(this->_windowWidth, this->_windowHeight, this->_shaders);
}

void Render::receiveLoadTextures(Packet packet)
{
    std::string name;
    std::string texture;

    while (!packet.empty()) {
        packet >> name >> texture;
        unsigned int textureID;
        glGenTextures(1, &textureID);

        int width, height, nrComponents;
        unsigned char *data = stbi_load_from_memory((const stbi_uc *)texture.c_str(), static_cast<int>(texture.size()), &width, &height, &nrComponents, 0);
        if (data) {
            GLenum format;
            if (nrComponents == 1)
                format = GL_RED;
            else if (nrComponents == 3)
                format = GL_RGB;
            else if (nrComponents == 4)
                format = GL_RGBA;

            glBindTexture(GL_TEXTURE_2D, textureID);
            glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            stbi_image_free(data);
            this->_textures[name] = textureID;
        } else {
            stbi_image_free(data);
        }
        glBindTexture(GL_TEXTURE_2D, 0);
    }
}

void Render::receiveLoadSkyBox(Packet packet)
{
    std::vector<std::string> faces(6);
    for (int i = 0; i < 6; i++)
        packet >> faces[i];

    glGenTextures(1, &this->_skyBoxTexture);
    glBindTexture(GL_TEXTURE_CUBE_MAP, this->_skyBoxTexture);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++) {
        unsigned char *data = stbi_load_from_memory((const stbi_uc *)faces[i].c_str(), static_cast<int>(faces[i].size()), &width, &height, &nrChannels, 0);
        if (data) {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        } else {
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}

void Render::receiveLoadModels(Packet packet)
{
    std::string name;
    std::string modelContent;

    while (!packet.empty()) {
        packet >> name >> modelContent;
        this->_models[name] = std::make_shared<Model>(modelContent, this->_textures);
    }
}

void Render::receiveAddObject(Packet packet)
{
    while (!packet.empty()) {
        std::size_t key;
        GameObject object;

        packet >> key >> object;
        if (this->_models.find(object.getName()) == this->_models.end())
            throw std::runtime_error("Create object, model not loaded: " + object.getName());
        this->_objects[key] = std::make_unique<StaticModel>(object, this->_models[object.getName()]);
    }
}

void Render::receiveClearObject([[maybe_unused]] Packet packet)
{
    for (auto &[key, object] : this->_objects)
        object.reset();
    this->_objects.clear();
}

void Render::receiveScenesName(Packet packet)
{
    while (!packet.empty()) {
        std::string sceneName;

        packet >> sceneName;
        this->_scenes.push_back(sceneName);
    }
}
