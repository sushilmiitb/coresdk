//
// Created by chimeralabs on 10/19/2016.
//

#ifndef ANDROIDSDK_MATERIALGLUNIFORMCOLOR_H
#define ANDROIDSDK_MATERIALGLUNIFORMCOLOR_H

#include <coreEngine/conf/Types.h>
#include <coreEngine/model/MaterialUniformColor.h>

namespace cl{
    class MaterialGLUniformColor : public MaterialUniformColor{
        CL_GLuint colorUniformObject;

    public:
        MaterialGLUniformColor(Shader *shaderPtr, IObjectRenderer *objectRendererPtr,
                               const std::string &tag) : MaterialUniformColor(shaderPtr, objectRendererPtr, tag) { }

        CL_GLuint getColorUniformObject(){
            return colorUniformObject;
        }
        void setColorUniformObject(CL_GLuint colorUniformObject){
            this->colorUniformObject = colorUniformObject;
        }
    };
}

#endif //ANDROIDSDK_MATERIALGLUNIFORMCOLOR_H
