#ifndef IMAGE360_IMAGE360_H
#define IMAGE360_IMAGE360_H

#include <coreEngine/IRenderer.h>
#include <coreEngine/factory/ISceneFactory.h>
#include <coreEngine/factory/IModelFactory.h>
#include <coreEngine/factory/ICameraFactory.h>
#include <coreEngine/events/EventKeyPressListener.h>
#include <coreEngine/events/EventPassiveMouseMotionListener.h>
#include <coreEngine/events/IEventQueue.h>
#include <coreEngine/factory/IDiffuseTextureFactory.h>
#include <coreEngine/factory/IDiffuseTextureCubeMapFactory.h>

#include <coreEngine/modifier/Image.h>
#include <coreEngine/ui/UIFactory.h>
#include <coreEngine/components/transformTree/ITransformTreeFactory.h>
#include <coreEngine/components/gazeDetector/GazeDetectorFactory.h>

#include <coreEngine/factory/IEventGazeListenerFactory.h>

namespace cl
{
enum TEXTURE_MAP_MODE
{
    CUBE_MAP_MODE_SIX_IMAGES,
    CUBE_MAP_MODE_SINGLE_IMAGE,
    EQUIRECTANGULAR_MAP_MODE
};
enum IMAGE_MODE
{
    STEREO,
    MONO
};

class Image360 : public EventKeyPressListener, public EventPassiveMouseMotionListener
{
  public:
    Image360(std::unique_ptr<IRenderer> renderer,
             std::unique_ptr<ISceneFactory> sceneFactory,
             std::unique_ptr<IModelFactory> modelFactory,
             std::unique_ptr<IDiffuseTextureFactory> diffuseTextureFactory,
             std::unique_ptr<IDiffuseTextureCubeMapFactory> diffuseTextureCubeMapFactory,
             std::unique_ptr<ITransformTreeFactory> transformTreeFactory,
             std::unique_ptr<ICameraFactory> cameraFactory,
             IEventQueue *eventQueue,
             ILoggerFactory *loggerFactory,
             std::unique_ptr<UIFactory> uiFactory,
             std::unique_ptr<GazeDetectorFactory> gazeDetectorFactory,
             std::unique_ptr<IEventGazeListenerFactory> gazeEventListenerFactory,
             std::string fontFolderPath)
    {
        assert(renderer != nullptr);
        assert(sceneFactory != nullptr);
        assert(modelFactory != nullptr);
        assert(diffuseTextureFactory != nullptr);
        assert(diffuseTextureCubeMapFactory != nullptr);
        assert(transformTreeFactory != nullptr);
        assert(eventQueue != nullptr);
        assert(cameraFactory != nullptr);
        assert(uiFactory != nullptr);
        assert(gazeDetectorFactory != nullptr);
        assert(gazeEventListenerFactory != nullptr);

        this->renderer = std::move(renderer);
        this->sceneFactory = std::move(sceneFactory);
        this->modelFactory = std::move(modelFactory);
        this->diffuseTextureFactory = std::move(diffuseTextureFactory);
        this->diffuseTextureCubeMapFactory = std::move(diffuseTextureCubeMapFactory);
        this->transformTreeFactory = std::move(transformTreeFactory);
        this->cameraFactory = std::move(cameraFactory);
        this->eventQueue = eventQueue;
        this->logger = loggerFactory->createLogger("Image360::");
        this->uiFactory = std::move(uiFactory);
        this->gazeDetectorFactory = std::move(gazeDetectorFactory);
        this->eventGazeListenerFactory = std::move(gazeEventListenerFactory);
        this->fontFolderPath = fontFolderPath;
    }

    //virtual ~Image360() = 0;
    //IApplication implementation
    virtual void start() = 0;
    /**
		* @arg mode: One of the values of enum TEXTURE_MAP_MODE - CUBE_MAP_MODE_SIX_IMAGES, CUBE_MAP_MODE_SINGLE_IMAGE, EQUIRECTANGULAR_MAP_MODE
		* @arg textureImages: Images required to generate textures. In case of CUBE_MAP_MODE_SIX_IMAGES order of images should be FRONT, LEFT, BACK, RIGHT, TOP AND BOTTOM. In other cases just one image is required.
		*/
    virtual void initialize(TEXTURE_MAP_MODE mode, std::vector<std::unique_ptr<Image>> &textureImages) = 0;
    virtual void update() = 0;
    virtual void drawInit() = 0;    // draw common stuff
    virtual void draw(EYE eye) = 0; // draw eye specific stuff - camera, models etc.
    virtual void drawComplete() = 0;
    virtual void deinitialize() = 0; // todo - this has to be a virtual function as well
    virtual void stop() = 0;
    virtual void pause() = 0;
    virtual void resume() = 0;
    virtual void onKeyPress(char key, int x, int y) = 0;
    virtual void onPassiveMouseMotion(int x, int y) = 0;
    virtual IRenderer *getRenderer() = 0;

    std::unique_ptr<EventGazeListener> actionButtonListener;
    std::unique_ptr<EventGazeListener> closeButtonListener;

    void setIsControllerPresent(bool isControllerPresent)
    {
        this->isControllerPresent = isControllerPresent;
    }

    void setCloseButtonText(std::string newCloseButtonText)
    {
        this->closeButtonText = newCloseButtonText;
    }

    void setActionButtonText(std::string newActionButtonText)
    {
        this->actionButtonText = newActionButtonText;
    }

    std::string getActionButtonText()
    {
        return this->actionButtonText;
    }

  protected:
    std::unique_ptr<IRenderer> renderer;
    IEventQueue *eventQueue;
    std::unique_ptr<ILogger> logger;

    std::unique_ptr<ISceneFactory> sceneFactory;
    std::unique_ptr<IModelFactory> modelFactory;
    std::unique_ptr<IDiffuseTextureFactory> diffuseTextureFactory;
    std::unique_ptr<IDiffuseTextureCubeMapFactory> diffuseTextureCubeMapFactory;
    std::unique_ptr<ITransformTreeFactory> transformTreeFactory;
    std::unique_ptr<ICameraFactory> cameraFactory;
    std::unique_ptr<UIFactory> uiFactory;
    std::unique_ptr<GazeDetectorFactory> gazeDetectorFactory;
    std::unique_ptr<IEventGazeListenerFactory> eventGazeListenerFactory;

    std::string fontFolderPath = "";

    std::string closeButtonText = "Close";
    std::string actionButtonText = "Notify Me";
    std::unique_ptr<PlanarBackground> actionButtonBackground;
    std::unique_ptr<PlanarBackground> closeButtonBackground;

    bool isControllerPresent = false;
};
}

#endif //IMAGE360_IMAGE360_H