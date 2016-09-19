#include "CommandDispatch.h"
#include "ApplicationFactory.h"
#include "IOEvent.h"

#include <iostream>

Nova::CommandDispatch::CommandDispatch( Nova::ApplicationFactory& app) : _app(app)
{


}

Nova::CommandDispatch::~CommandDispatch()
{

}

void
Nova::CommandDispatch::Input( const Nova::IOEvent& event )
{
    
    if( _commandstarted ){
        if( event.key_data.key == IOEvent::KEY_GRAVE_ACCENT ){
            _commandstarted = false;
            _buffer.str("");
            return;
        }
        
        if( event.key_data.key >= IOEvent::KEY_SPACE && event.key_data.key <= IOEvent::KEY_GRAVE_ACCENT )
            _buffer << char( event.key_data.key );
        
        
        if( event.key_data.key == IOEvent::KEY_ENTER ||
            event.key_data.key == IOEvent::KEY_KP_ENTER ){
            _buffer.str("");
            _commandstarted = false;
            return;
        }

    }
    else{
        if( event.key_data.key == IOEvent::KEY_GRAVE_ACCENT )
            _commandstarted = true;
    }
}

void
Nova::CommandDispatch::Render( const Nova::IOEvent& event )
{
    _app.GetTextRenderingService().Render( _buffer.str(), 1.0, 0, 0 );

}
