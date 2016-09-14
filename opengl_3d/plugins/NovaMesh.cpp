#include "../Renderable.h"
#include <iostream>
#include <fstream>

namespace Nova {
    
    class NovaMesh : public Renderable {        

        virtual void load(std::string path){
            std::cout << "Calling load from NovaMesh plugin." << std::endl;

        }


        virtual void draw(){


        }


    };

}

extern "C" Nova::Renderable* Nova_CreateRenderable() {
    return new Nova::NovaMesh();
}

extern "C" void Nova_DestroyRenderable( Nova::Renderable* r) {
    delete r;
}
