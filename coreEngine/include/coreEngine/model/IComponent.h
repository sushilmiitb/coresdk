//
// Created by chimeralabs on 10/15/2016.
//

#ifndef ANDROIDSDK_ICOMPONENT_H
#define ANDROIDSDK_ICOMPONENT_H

#include <string>
#include <coreEngine/model/Object.h>

namespace cl{
    class IComponent{
    private:
        Object *objectPtr;
    public:
        IComponent(Object *objectPtr){
            this->objectPtr = objectPtr;
        }
        virtual std::string getComponentType() = 0;
        Object *getObject(){
            return objectPtr;
        }
        virtual void initialize() = 0;
        virtual void draw() = 0;
        virtual void deinitialize() = 0;
    };
}

#endif //ANDROIDSDK_ICOMPONENT_H
