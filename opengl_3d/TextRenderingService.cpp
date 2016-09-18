#include "TextRenderingService.h"
#include "IOEvent.h"

Nova::TextRenderingService::TextRenderingService(Nova::ApplicationFactory& app) : _app(app)
{

}

Nova::TextRenderingService::~TextRenderingService()
{

}

void
Nova::TextRenderingService::RegisterProvider(  std::unique_ptr< TextRenderer > provider )
{
    _provider = std::move( provider );
}


void
Nova::TextRenderingService::Render(  std::string text, float scaling, float x, float y )
{
    if( _provider )
        _provider->RenderText( text, scaling, x, y, width, height );
}


void
Nova::TextRenderingService::UpdateScreenSize(const Nova::IOEvent& event){
    width = event.draw_data.width;
    height = event.draw_data.height;
}
