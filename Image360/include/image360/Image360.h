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
#include <coreEngine/components/transform/ITransformCameraFactory.h>
#include <coreEngine/components/transform/ITransformModelFactory.h>
#include <coreEngine/modifier/Image.h>

namespace cl{
	enum TEXTURE_MAP_MODE{
		CUBE_MAP_MODE_SIX_IMAGES,
		CUBE_MAP_MODE_SINGLE_IMAGE,
		EQUIRECTANGULAR_MAP_MODE
	};
	class Image360 : public EventKeyPressListener, public EventPassiveMouseMotionListener{
	public:
		Image360(std::unique_ptr<IRenderer> renderer, 
				 std::unique_ptr<ISceneFactory> sceneFactory, 
				 std::unique_ptr<IModelFactory> modelFactory, 
				 std::unique_ptr<IDiffuseTextureFactory> diffuseTextureFactory, 
				 std::unique_ptr<IDiffuseTextureCubeMapFactory> diffuseTextureCubeMapFactory, 
				 std::unique_ptr<ITransformCameraFactory> transformCameraFactory, 
				 std::unique_ptr<ITransformModelFactory> transformModelFactory, 
				 std::unique_ptr<ICameraFactory> cameraFactory,
				 IEventQueue *eventQueue, 
				 ILoggerFactory *loggerFactory);

		//IApplication implementation
		void start();
		/**
		* @arg mode: One of the values of enum TEXTURE_MAP_MODE - CUBE_MAP_MODE_SIX_IMAGES, CUBE_MAP_MODE_SINGLE_IMAGE, EQUIRECTANGULAR_MAP_MODE
		* @arg textureImages: Images required to generate textures. In case of CUBE_MAP_MODE_SIX_IMAGES order of images should be FRONT, LEFT, BACK, RIGHT, TOP AND BOTTOM. In other cases just one image is required.
		*/
		void initialize(TEXTURE_MAP_MODE mode, std::vector<std::unique_ptr<Image>> &textureImages);
		void update();
		void draw();
		void deinitialize();
		void stop();
		void onKeyPress(char key, int x, int y);
		void onPassiveMouseMotion(int x, int y);
		IRenderer* getRenderer();
	private:
		std::unique_ptr<IRenderer> renderer;
		std::unique_ptr<ISceneFactory> sceneFactory;
		std::unique_ptr<IModelFactory> modelFactory;
		std::unique_ptr<IDiffuseTextureFactory> diffuseTextureFactory;
		std::unique_ptr<IDiffuseTextureCubeMapFactory> diffuseTextureCubeMapFactory;
		std::unique_ptr<ITransformCameraFactory> transformCameraFactory;
		std::unique_ptr<ITransformModelFactory> transformModelFactory;
		std::unique_ptr<ICameraFactory> cameraFactory;
		std::unique_ptr<ILogger> logger;
		std::unique_ptr<Scene> scene;
		Camera *camera;
		Shader *shader;
		Material *material;
		Texture *imageTexture;
		Model *imageContainer;
		IEventQueue *eventQueue;
		int lastPassiveMousePositionX = -1;
		int lastPassiveMousePositionY = -1;
		float passiveMouseMotionSensitivity = 0.2f;
	};
}

#endif //IMAGE360_IMAGE360_H