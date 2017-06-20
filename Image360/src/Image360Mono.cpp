#include <assert.h>
#include <coreEngine/components/transformTree/TransformTreeFactory.h>
#include <coreEngine/modelBuilder/CubeBuilder.h>
#include <coreEngine/modelBuilder/ModelLoader.h>
#include <coreEngine/modelBuilder/UVSphereBuilder.h>
#include <coreEngine/modifier/ImageModifier.h>
#include <coreEngine/modifier/ModelModifier.h>
#include <coreEngine/util/SimpleOBJLoader.h>
#include <image360/Image360Mono.h>

namespace cl {
Image360Mono::Image360Mono(
    std::unique_ptr<IRenderer> renderer,
    std::unique_ptr<ISceneFactory> sceneFactory,
    std::unique_ptr<IModelFactory> modelFactory,
    std::unique_ptr<IDiffuseTextureFactory> diffuseTextureFactory,
    std::unique_ptr<IDiffuseTextureCubeMapFactory> diffuseTextureCubeMapFactory,
    std::unique_ptr<ITransformTreeFactory> transformTreeFactory,
    std::unique_ptr<ICameraFactory> cameraFactory, IEventQueue *eventQueue,
    ILoggerFactory *loggerFactory, std::unique_ptr<UIFactory> uiFactory,
    std::unique_ptr<GazeDetectorFactory> gazeDetectorFactory,
    std::unique_ptr<IEventGazeListenerFactory> eventGazeListenerFactory,
    std::string fontFolderPath)
    : Image360(std::move(renderer), std::move(sceneFactory),
               std::move(modelFactory), std::move(diffuseTextureFactory),
               std::move(diffuseTextureCubeMapFactory),
               std::move(transformTreeFactory), std::move(cameraFactory),
               std::move(eventQueue), std::move(loggerFactory),
               std::move(uiFactory), std::move(gazeDetectorFactory),
               std::move(eventGazeListenerFactory), std::move(fontFolderPath)) {
  this->logger->setTag("Image360Mono::");
}

// IApplication implementation
void Image360Mono::start() {
  this->renderer->start();
  this->logger->log(LOG_INFO, "Application started.");
}
void Image360Mono::initialize(
    TEXTURE_MAP_MODE mapMode,
    std::vector<std::unique_ptr<Image>> &textureImages) {
  std::unique_ptr<Camera> camera;

  scene = sceneFactory->create("testScene");
  assert(scene != nullptr);
  scene->setBackgroundColor(CL_Vec4(0.0f, 0.0f, 0.4f, 0.0f));
  scene->setDepthTest(true);
  scene->setBlending(true);
  camera = cameraFactory->create("camera", scene.get());
  assert(camera != nullptr);
  camera->setAspect(1.5f);
  camera->setFarPlane(1000.0f);
  camera->setFov(1.5f);
  camera->setNearPlane(1.0f);
  this->camera = camera.get();
  scene->addToScene(std::move(camera));

  std::unique_ptr<Model> imageContainer =
      modelFactory->create("imageContainer");
  assert(imageContainer != nullptr);
  this->imageContainer = imageContainer.get();
  scene->addToScene(std::move(imageContainer));
  TransformTreeCamera *transformTreeCamera = nullptr;

  if (mapMode == EQUIRECTANGULAR_MAP_MODE) {
    std::unique_ptr<ShaderDiffuseTexture> shaderDiffuseTexture;
    std::unique_ptr<MaterialDiffuseTexture> materialDiffuseTexture;
    std::unique_ptr<Texture> imageTexture;
    shaderDiffuseTexture =
        diffuseTextureFactory->createShader("shader", scene.get());
    assert(shaderDiffuseTexture != nullptr);
    this->shader = shaderDiffuseTexture.get();
    scene->addToScene(std::move(shaderDiffuseTexture));

    materialDiffuseTexture = diffuseTextureFactory->createMaterial(
        "material", (ShaderDiffuseTexture *)this->shader);
    assert(materialDiffuseTexture != nullptr);
    this->material = materialDiffuseTexture.get();
    scene->addToScene(std::move(materialDiffuseTexture));

    imageTexture = diffuseTextureFactory->createTexture("imageTexture");
    assert(imageTexture != nullptr);
    assert(textureImages.size() == 1);
    assert(textureImages[0] != nullptr);
    std::unique_ptr<Image> image = std::move(textureImages[0]);
    imageTexture->setTextureData(std::move(image->data));
    imageTexture->setHeight(image->height);
    imageTexture->setWidth(image->width);
    imageTexture->setTextureDataSize(image->dataSize);
    this->imageTexture = imageTexture.get();
    scene->addToScene(std::move(imageTexture));
    ((MaterialDiffuseTexture *)this->material)
        ->setDiffuseTexture(this->imageTexture);
    std::unique_ptr<TransformTreeCamera> transformTreeCameraUptr =
        transformTreeFactory->createTransformTreeCamera(this->camera);
    this->camera->getComponentList().addComponent(
        std::move(transformTreeCameraUptr));

    std::unique_ptr<TransformTreeModel> transformSphereUptr =
        transformTreeFactory->createTransformTreeModel(this->imageContainer);
    this->imageContainer->getComponentList().addComponent(
        std::move(transformSphereUptr));

    transformTreeCamera =
        (TransformTreeCamera *)this->camera->getComponentList().getComponent(
            "transformTree");
    transformTreeCamera->setLocalPosition(CL_Vec3(0.0f, 0.0f, 0.0f));
    transformTreeCamera->setLocalRotation(CL_Vec3(0.0f, 0.0f, 0.0f));

    TransformTreeModel *transformSphere =
        (TransformTreeModel *)this->imageContainer->getComponentList()
            .getComponent("transformTree");
    transformSphere->setLocalPosition(CL_Vec3(0.0f, 0.0f, 0.0f));
    transformSphere->setLocalScale(CL_Vec3(100.0f, 100.0f, 100.0f));

    this->imageContainer->createBiRelation(this->material);

    std::unique_ptr<ModelModifier> modelModifier(new ModelModifier);
    UVSphereBuilder uvSphereBuilder(modelModifier.get());
    uvSphereBuilder.buildUnitSphere(this->imageContainer, 5);
  } else {
    std::unique_ptr<ShaderDiffuseTextureCubeMap> shaderUPtr =
        diffuseTextureCubeMapFactory->createShader("shader", scene.get());
    assert(shaderUPtr != nullptr);
    this->shader = shaderUPtr.get();
    scene->addToScene(std::move(shaderUPtr));

    std::unique_ptr<MaterialDiffuseTextureCubeMap> materialUPtr =
        diffuseTextureCubeMapFactory->createMaterial(
            "material", (ShaderDiffuseTextureCubeMap *)this->shader);
    assert(materialUPtr != nullptr);
    this->material = materialUPtr.get();
    scene->addToScene(std::move(materialUPtr));

    std::unique_ptr<TextureCubeMap> textureUPtr =
        diffuseTextureCubeMapFactory->createTexture("imageTexture");
    assert(textureUPtr != nullptr);
    std::unique_ptr<Image> frontImage(new Image);
    std::unique_ptr<Image> leftImage(new Image);
    std::unique_ptr<Image> backImage(new Image);
    std::unique_ptr<Image> rightImage(new Image);
    std::unique_ptr<Image> topImage(new Image);
    std::unique_ptr<Image> bottomImage(new Image);
    if (mapMode == CUBE_MAP_MODE_SIX_IMAGES) {
      assert(textureImages.size() == 6);
      frontImage = std::move(textureImages[0]);
      leftImage = std::move(textureImages[1]);
      backImage = std::move(textureImages[2]);
      rightImage = std::move(textureImages[3]);
      topImage = std::move(textureImages[4]);
      bottomImage = std::move(textureImages[5]);
    } else if (mapMode == CUBE_MAP_MODE_SINGLE_IMAGE) {
      assert(textureImages.size() == 1);
      assert(textureImages[0] != nullptr);
      std::unique_ptr<Image> originalImage = std::move(textureImages[0]);
      ImageModifier imageModifier(logger.get());
      imageModifier.convertSingleCubicImageIntoSix(
          originalImage.get(), frontImage.get(), leftImage.get(),
          backImage.get(), rightImage.get(), bottomImage.get(), topImage.get());
    }
    assert(rightImage != nullptr);
    textureUPtr->setTextureData(TEXTURE_CUBE_MAP_FACE_RIGHT,
                                std::move(rightImage->data));
    textureUPtr->setHeight(TEXTURE_CUBE_MAP_FACE_RIGHT, rightImage->height);
    textureUPtr->setWidth(TEXTURE_CUBE_MAP_FACE_RIGHT, rightImage->width);
    textureUPtr->setTextureDataSize(TEXTURE_CUBE_MAP_FACE_RIGHT,
                                    rightImage->dataSize);

    assert(leftImage != nullptr);
    textureUPtr->setTextureData(TEXTURE_CUBE_MAP_FACE_LEFT,
                                std::move(leftImage->data));
    textureUPtr->setHeight(TEXTURE_CUBE_MAP_FACE_LEFT, leftImage->height);
    textureUPtr->setWidth(TEXTURE_CUBE_MAP_FACE_LEFT, leftImage->width);
    textureUPtr->setTextureDataSize(TEXTURE_CUBE_MAP_FACE_LEFT,
                                    leftImage->dataSize);

    assert(topImage != nullptr);
    textureUPtr->setTextureData(TEXTURE_CUBE_MAP_FACE_TOP,
                                std::move(topImage->data));
    textureUPtr->setHeight(TEXTURE_CUBE_MAP_FACE_TOP, topImage->height);
    textureUPtr->setWidth(TEXTURE_CUBE_MAP_FACE_TOP, topImage->width);
    textureUPtr->setTextureDataSize(TEXTURE_CUBE_MAP_FACE_TOP,
                                    topImage->dataSize);

    assert(bottomImage != nullptr);
    textureUPtr->setTextureData(TEXTURE_CUBE_MAP_FACE_BOTTOM,
                                std::move(bottomImage->data));
    textureUPtr->setHeight(TEXTURE_CUBE_MAP_FACE_BOTTOM, bottomImage->height);
    textureUPtr->setWidth(TEXTURE_CUBE_MAP_FACE_BOTTOM, bottomImage->width);
    textureUPtr->setTextureDataSize(TEXTURE_CUBE_MAP_FACE_BOTTOM,
                                    bottomImage->dataSize);

    assert(backImage != nullptr);
    textureUPtr->setTextureData(TEXTURE_CUBE_MAP_FACE_BACK,
                                std::move(backImage->data));
    textureUPtr->setHeight(TEXTURE_CUBE_MAP_FACE_BACK, backImage->height);
    textureUPtr->setWidth(TEXTURE_CUBE_MAP_FACE_BACK, backImage->width);
    textureUPtr->setTextureDataSize(TEXTURE_CUBE_MAP_FACE_BACK,
                                    backImage->dataSize);

    assert(frontImage != nullptr);
    textureUPtr->setTextureData(TEXTURE_CUBE_MAP_FACE_FRONT,
                                std::move(frontImage->data));
    textureUPtr->setHeight(TEXTURE_CUBE_MAP_FACE_FRONT, frontImage->height);
    textureUPtr->setWidth(TEXTURE_CUBE_MAP_FACE_FRONT, frontImage->width);
    textureUPtr->setTextureDataSize(TEXTURE_CUBE_MAP_FACE_FRONT,
                                    frontImage->dataSize);

    this->imageTexture = textureUPtr.get();
    scene->addToScene(std::move(textureUPtr));
    ((MaterialDiffuseTextureCubeMap *)this->material)
        ->setDiffuseTexture(this->imageTexture);

    std::unique_ptr<TransformTreeCamera> transformTreeCameraUptr =
        transformTreeFactory->createTransformTreeCamera(this->camera);
    this->camera->getComponentList().addComponent(
        std::move(transformTreeCameraUptr));

    std::unique_ptr<TransformTreeModel> transformSphereUptr =
        transformTreeFactory->createTransformTreeModel(this->imageContainer);
    this->imageContainer->getComponentList().addComponent(
        std::move(transformSphereUptr));

    transformTreeCamera =
        (TransformTreeCamera *)this->camera->getComponentList().getComponent(
            "transformTree");
    transformTreeCamera->setLocalPosition(CL_Vec3(0.0f, 0.0f, 0.0f));
    transformTreeCamera->setLocalRotation(CL_Vec3(0.0f, 0.0f, 0.0f));

    TransformTreeModel *transformSphere =
        (TransformTreeModel *)this->imageContainer->getComponentList()
            .getComponent("transformTree");
    transformSphere->setLocalPosition(CL_Vec3(0.0f, 0.0f, 0.0f));
    transformSphere->setLocalScale(CL_Vec3(100.0f, 100.0f, 100.0f));

    this->imageContainer->createBiRelation(this->material);

    std::unique_ptr<ModelModifier> modelModifier(new ModelModifier);
    CubeBuilder cubeBuilder(modelModifier.get());
    cubeBuilder.buildInwardCube(this->imageContainer);
  }

  // Font store Intialization
  std::unique_ptr<FontStore> fontStore =
      uiFactory->createFontStore(scene.get(), this->fontFolderPath.c_str());

  // Text style info
  TextStyle textStyle;
  textStyle.fontSize = 20;
  textStyle.scale = 0.025f;
  textStyle.color = CL_Vec4(1.0, 1.0, 1.0, 1.0);

  // Action Button Initialization
  auto actionButtonPosition = CL_Vec3(-5.1, 0.0, -15.5);
  auto actionButtonRotation = CL_Vec3(0.0, 0.0, 0.0);
  auto actionButtonColor = CL_Vec4(0.0, 0.0, 0.0, 0.7);
  auto actionButtonWidth = 3.0f;
  auto actionButtonHeight = 1.0f;
  this->actionButtonBackground = uiFactory->createPlanarBackground(
      "actionButton", scene.get(), actionButtonColor, actionButtonPosition,
      actionButtonRotation, actionButtonWidth, actionButtonHeight);

  // action button text position & orientation is relative to the actionButton
  // planar background
  auto actionTextPosition = CL_Vec3(-1.0, -0.2, 0.001);
  auto actionTextRotation = CL_Vec3(0.0, 0.0, 0.0);
  std::unique_ptr<TextElement> actionButtonElement =
      uiFactory->createTextElement("actionButtonElement", fontStore.get(),
                                   &textStyle, this->actionButtonText,
                                   actionTextPosition, actionTextRotation,
                                   scene.get());
  this->actionButtonBackground->addChild("child1",
                                         std::move(actionButtonElement));

  // Close Button Intialization
  auto closeButtonPosition = CL_Vec3(5.1, 0.0, -15.5);
  auto closeButtonRotation = CL_Vec3(0.0, 0.0, 0.0);
  auto closeButtonColor = CL_Vec4(0.0, 0.0, 0.0, 0.7);
  auto closeButtonWidth = 3.0f;
  auto closeButtonHeight = 1.0f;
  this->closeButtonBackground = uiFactory->createPlanarBackground(
      "closeButton", scene.get(), closeButtonColor, closeButtonPosition,
      closeButtonRotation, closeButtonWidth, closeButtonHeight);

  // close button text position & orientation is relative to the actionButton
  // planar background
  auto closeTextPosition = CL_Vec3(-1.0, -0.2, 1.0);
  auto closeTextRotation = CL_Vec3(0.0, 0.0, 0.0);
  std::unique_ptr<TextElement> closeButtonElement =
      uiFactory->createTextElement("closeButtonElement", fontStore.get(),
                                   &textStyle, this->closeButtonText,
                                   closeTextPosition, closeTextRotation,
                                   scene.get());
  this->closeButtonBackground->addChild("child2",
                                        std::move(closeButtonElement));

  gazeDetectorContainer = gazeDetectorFactory->createGazeDetectorContainer();

  // initializing action button model
  Model *actionButtonModel = (Model *)scene->getFromScene("actionButton");
  TransformTreeModel *transformActionButton =
      (TransformTreeModel *)actionButtonModel->getComponentList().getComponent(
          "transformTree");
  this->actionButtonListener =
      eventGazeListenerFactory->createActionButtonListener();

  // initializing close button model
  Model *closeButtonModel = (Model *)scene->getFromScene("closeButton");
  TransformTreeModel *transformCloseButton =
      (TransformTreeModel *)closeButtonModel->getComponentList().getComponent(
          "transformTree");
  this->closeButtonListener =
      eventGazeListenerFactory->createCloseButtonListener();

  TransformTree *gazeTransformTarget = nullptr;
  auto reticleColor = CL_Vec4(0.0, 1.0, 0.0, 1.0);
  if (isControllerPresent) {
    reticleBase = uiFactory->createReticle("reticleBase", scene.get(), nullptr,
                                           reticleColor);
    TransformTreeModel *transform = reticleBase->getTransformTreeModel();
    reticle = uiFactory->createReticle("reticle", scene.get(), transform,
                                       reticleColor);
    gazeTransformTarget = transform;
  } else {
    reticle = uiFactory->createReticle("reticle", scene.get(),
                                       transformTreeCamera, reticleColor);
    gazeTransformTarget = transformTreeCamera;
  }

  auto actionButtonGazeDetectorOrigin = CL_Vec3(0.0f, 0.0f, 0.0f);
  auto actionButtonGazeDetectorLookAt = CL_Vec3(0.0f, 0.0f, -1.0f);
  auto actionButtonGazeDetectorLengthX = 3.0f;
  auto actionButtonGazeDetectorLengthY = 1.0f;
  auto actionButtonGazeDetectorLengthZ = 0.00001f;
  std::unique_ptr<IComponent> gazeDetectorNotifyMe =
      gazeDetectorFactory->createGazeDetectorBox(
          this->actionButtonText, gazeTransformTarget, transformActionButton,
          actionButtonListener.get(), gazeDetectorContainer.get(),
          actionButtonGazeDetectorOrigin, actionButtonGazeDetectorLookAt,
          actionButtonGazeDetectorLengthX, actionButtonGazeDetectorLengthY,
          actionButtonGazeDetectorLengthZ);
  actionButtonModel->getComponentList().addComponent(
      std::move(gazeDetectorNotifyMe));

  auto closeButtonGazeDetectorOrigin = CL_Vec3(0.0f, 0.0f, 0.0f);
  auto closeButtonGazeDetectorLookAt = CL_Vec3(0.0f, 0.0f, -1.0f);
  auto closeButtonGazeDetectorLengthX = 3.0f;
  auto closeButtonGazeDetectorLengthY = 1.0f;
  auto closeButtonGazeDetectorLengthZ = 0.00001f;
  std::unique_ptr<IComponent> gazeDetectorCloseMe =
      gazeDetectorFactory->createGazeDetectorBox(
          this->closeButtonText, gazeTransformTarget, transformCloseButton,
          closeButtonListener.get(), gazeDetectorContainer.get(),
          closeButtonGazeDetectorOrigin, closeButtonGazeDetectorLookAt,
          closeButtonGazeDetectorLengthX, closeButtonGazeDetectorLengthY,
          closeButtonGazeDetectorLengthZ);
  closeButtonModel->getComponentList().addComponent(
      std::move(gazeDetectorCloseMe));

  // controller model load
  // const char* ddControllerFilename = "ddController.obj";
  // std::vector<CL_Vec3> vertices;
  // std::vector<CL_Vec2> uvs;
  // std::vector<CL_Vec3> normals;
  // std::vector<CL_GLushort> elements;

  // ModelLoader::load_obj(ddControllerFilename, vertices, uvs, normals,
  // elements);
  // std::string vertexMsg = "Number of Vertices " +
  // std::to_string(vertices.size());
  // logger->log(cl::LOG_DEBUG, vertexMsg);

  // fade screen initialization
  auto fadeScreenPosition = CL_Vec3(0.0, 0.0, -2.0);
  auto fadeScreenRotation = CL_Vec3(0.0, 0.0, 0.0);
  auto fadeScreenColor = CL_Vec4(0.0, 0.0, 0.0, 0.0);

  this->fadeScreen = uiFactory->createFadeScreen(
      "fadeScreen", scene.get(), fadeScreenColor, fadeScreenPosition,
      fadeScreenRotation, 20.0, 20.0);
  transformTreeCamera->addChild(this->fadeScreen->getTransformTree());

  renderer->initialize(scene.get());
}

void Image360Mono::update() { renderer->update(); }

void Image360Mono::drawInit() {
  if (this->fadeStarted) {
    this->fadeScreen->setColor(CL_Vec4(0.0, 0.0, 0.0, this->alphaFade));
    if (this->alphaFade < 1.0) {
      this->alphaFade += this->fadeSpeed;
    } else {
      this->fadeComplete = true;
    }
  }
  while (!eventQueue->empty()) {
    std::unique_ptr<IEvent> event = eventQueue->pop();
    event->callListener();
  }
  renderer->drawInit(scene.get());
}

void Image360Mono::draw(EYE eye) { renderer->draw(scene.get(), eye); }

void Image360Mono::drawComplete() { renderer->drawComplete(); }

void Image360Mono::deinitialize() { renderer->deinitialize(scene.get()); }
void Image360Mono::stop() { renderer->stop(); }

void Image360Mono::pause() { renderer->pause(); }

void Image360Mono::resume() { renderer->resume(); }

void Image360Mono::onKeyPress(char key, int x, int y) {
  // logger->log(LOG_DEBUG, "Key pressed:" + std::string(1, key));
  logger->log(LOG_DEBUG, "Key pressed:" + std::string(1, key));
  TransformTreeModel *transform = reticle->getTransformTreeModel();
  CL_Vec3 rot = transform->getParent()->getLocalRotation();

  if (key == 'W') {
    rot[1] -= 0.6f;
  } else if (key == 'S') {
    rot[1] += 0.6f;
  } else if (key == 'A') {
    rot[0] -= 0.6f;
  } else if (key == 'D') {
    rot[0] += 0.6f;
  } else {
    return;
  }
  transform->getParent()->setLocalRotation(rot);
}

void Image360Mono::onPassiveMouseMotion(int x, int y) {
  // logger->log(LOG_DEBUG, "Mouse move:" + std::to_string(x) + "," +
  // std::to_string(y));
  if (lastPassiveMousePositionX != -1) {
    float xoff =
        (x - lastPassiveMousePositionX) * passiveMouseMotionSensitivity;
    float yoff =
        (y - lastPassiveMousePositionY) * passiveMouseMotionSensitivity;

    TransformTreeCamera *transform =
        (TransformTreeCamera *)camera->getComponentList().getComponent(
            "transformTree");
    CL_Vec3 rotation = transform->getLocalRotation();
    transform->setLocalRotation(
        CL_Vec3(rotation.x - yoff, rotation.y - xoff, rotation.z));
  }
  lastPassiveMousePositionX = x;
  lastPassiveMousePositionY = y;
}

IRenderer *Image360Mono::getRenderer() { return this->renderer.get(); }
}