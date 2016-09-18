#ifndef __OPENGL_3D_TEXT_RENDERING_SERVICE_H__
#define __OPENGL_3D_TEXT_RENDERING_SERVICE_H__

#include <string>
#include <memory>

namespace Nova {

    class ApplicationFactory;
    class IOEvent;
    
    class TextRenderer {
    public:
        TextRenderer() { };
        virtual ~TextRenderer() { };

        virtual void RenderText( std::string text, float scaling, float x, float y, float width, float height) = 0;
    };
    
    class TextRenderingService{

    public:
        TextRenderingService( ApplicationFactory& app);
        virtual ~TextRenderingService();

        void UpdateScreenSize(const IOEvent& event);
        void RegisterProvider( std::unique_ptr< TextRenderer > provider ); 
        void Render(  std::string text, float scaling, float x, float y );
    private:
        int width;
        int height;
        ApplicationFactory& _app;
        std::unique_ptr< TextRenderer > _provider;
    };
}


#endif
