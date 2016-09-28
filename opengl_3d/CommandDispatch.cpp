#include "CommandDispatch.h"
#include "ApplicationFactory.h"
#include "IOEvent.h"

#include <iostream>

Nova::CommandDispatch::CommandDispatch( Nova::ApplicationFactory& app) : _app(app), _commandstarted(false)
{


}

Nova::CommandDispatch::~CommandDispatch()
{

}

void
Nova::CommandDispatch::Input( const Nova::IOEvent& event )
{
    std::cout << "Console Input" << std::endl;
    std::cout << event.key_data->key << std::endl;

    if( _commandstarted ){
        if( event.key_data->key == IOEvent::KEY_GRAVE_ACCENT ){
            _app.GetIOService().PriorityClear( IOService::KEY_DOWN );
            _commandstarted = false;
            _buffer.str("");
            return;
        }
        
        if( event.key_data->key >= IOEvent::KEY_SPACE && event.key_data->key < IOEvent::KEY_GRAVE_ACCENT )
            _buffer << char( event.key_data->key );

        if( event.key_data->key == IOEvent::KEY_BACKSPACE ){
            std::string last_buffer = _buffer.str();
            last_buffer.pop_back();
            _buffer.str(last_buffer);
            _buffer.seekp (0, _buffer.end);
        }        
        
        if( event.key_data->key == IOEvent::KEY_ENTER ||
            event.key_data->key == IOEvent::KEY_KP_ENTER ){
            _buffer.seekp (0, _buffer.beg);
            std::string command;
            _buffer >> command;
            std::vector< std::string > args;
            while( !_buffer.eof() ){
                std::string arg;
                _buffer>>arg;
                args.push_back( arg );
            }
                
            _buffer.str("");
            _buffer.seekp (0, _buffer.beg);
            _commandstarted = false;
            _app.GetIOService().PriorityClear( IOService::KEY_DOWN );

            //std::cout << "Command Entered: " << command << std::endl;
            //std::cout << "\tArgs: ";
            //for( auto arg : args )
            //    std::cout << arg << ", ";
            //std::cout << std::endl;

            IOEvent event( IOEvent::COMMAND );
            event.command_data->command = command;
            event.command_data->args = args;
            _app.GetIOService().Trigger( event ); // Fire the command out!

            return;
        }

    }
    else{
        if( event.key_data->key == IOEvent::KEY_GRAVE_ACCENT ){
            try{
                _app.GetIOService().PriorityOn( IOService::KEY_DOWN,  [&](IOEvent& event){ this->Input( event ); });
                _commandstarted = true;
                _buffer.clear();
                _buffer.str("");
                _buffer.seekp (0, _buffer.beg);
            }
            catch( std::exception& e ){
                _commandstarted = false;                
            }
        }
    }
}

void
Nova::CommandDispatch::Render( const Nova::IOEvent& event )
{
    if( _commandstarted ){
        std::string displayText = "> ";
        displayText += _buffer.str();
        _app.GetTextRenderingService().Render( displayText, 1.0, 0, 0 );
    }

}
