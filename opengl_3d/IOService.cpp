#include "IOService.h"
#include <iostream>

Nova::IOService::IOService( ApplicationFactory& app) : _app(app)
{
    PriorityOn( "LIST-COMMANDS", [&](IOEvent& event){ 
            std::cout << "Available Commands: " << std::endl;
            std::cout << "--------------------" << std::endl;
            for( auto entry : _command_callbacks ){
                std::cout << entry.first << std::endl;
            }
            for( auto entry : _command_priority_callback ){
                std::cout << entry.first << std::endl;
            }
        });
}

Nova::IOService::~IOService()
{

}

Nova::IOService::EventType
Nova::IOService::translateEventType( const Nova::IOEvent& event ) const {

    EventType type;
    switch( event.type ){
    case IOEvent::TIME:
        type=TIME;
        break;
    case IOEvent::MOUSEBUTTON:
        switch( event.mousebutton_data->action ){
        case IOEvent::M_DOWN:
            type=MOUSE_DOWN;
            break;
        case IOEvent::M_UP:
            type=MOUSE_UP;
            break;
        }
        break;
    case IOEvent::MOUSEMOVE:
        type=MOUSE_MOVE;
        break;
    case IOEvent::KEYBOARD:
        switch( event.key_data->action ){
        case IOEvent::K_DOWN:
            type=KEY_DOWN;
            break;
        case IOEvent::K_UP:
            type=KEY_UP;
            break;
        case IOEvent::K_HOLD:
            type=KEY_HOLD;
            break;
        }
        break;
    case IOEvent::DRAW:
        type=REDRAW;
        break;
    case IOEvent::DROP:
        type=DROP;
        break;
    case IOEvent::SCROLL:
        type=SCROLL;
        break;
    case IOEvent::COMMAND:
        type=COMMAND;
        break;
    }

    return type;
}

void
Nova::IOService::Trigger( Nova::IOEvent& event )
{
    // Command events are special as they produce labels for each distinct command
    if( translateEventType(event) == COMMAND ){
        std::string command = event.command_data->command;

        // First check and call the priority callback if it is locked.
        {
            auto res = _command_priority_callback.find( command );
            if( res != _command_priority_callback.end() ){
                res->second( event );
                return; 
            }
        }

        // Then check all attached events.
        {
            auto res = _command_callbacks.find( command );
            if( res != _command_callbacks.end() ){
                for( auto callback : res->second ){
                    callback( event );
                }
            }

        }

    }
    else { 
        // First check and call the priority callback if it is locked.
        {
            auto res = _priority_callback.find( translateEventType(event) );
            if( res != _priority_callback.end() ){
                //if( translateEventType(event) != TIME )
                //    std::cout << "Calling priority callback for event " <<  translateEventType(event) << " : " << res->second.target_type().name() << std::endl;
                res->second( event );
                return; 
            }
        }
        
        // Otherwise, proceed through the list of callbacks and call each one.
        {
            auto res = _callbacks.find( translateEventType(event) );
            if( res != _callbacks.end() ){
                for( auto callback : res->second ){
                    //if( translateEventType(event) != TIME )
                    //    std::cout << "Calling callback for event " <<  translateEventType(event) << " : " << callback.target_type().name()<< std::endl;
                    callback( event );
                }
            }
        }       
    }
}

void
Nova::IOService::On( std::string commandName, Nova::IOService::IOEvent_Callback callback){

    auto res = _command_callbacks.find( commandName );
    if( res == _command_callbacks.end() ){
        _command_callbacks.insert( std::make_pair( commandName, std::vector< IOEvent_Callback >() ) );
    }
    _command_callbacks.at( commandName ).push_back( callback );
}

void
Nova::IOService::On( Nova::IOService::EventType type, Nova::IOService::IOEvent_Callback callback){

    if( type == COMMAND )
        throw std::runtime_error( "Cannot listen to COMMAND events. Please listen to specific commands instead." );

    auto res = _callbacks.find( type );
    if( res == _callbacks.end() ){
        _callbacks.insert( std::make_pair( type, std::vector< IOEvent_Callback >() ) );
    }
    _callbacks.at( type ).push_back( callback );
}

void
Nova::IOService::PriorityOn( std::string commandName, Nova::IOService::IOEvent_Callback callback){

    auto res = _command_priority_callback.find( commandName );
    if( res == _command_priority_callback.end() ){
        _command_priority_callback.insert( std::make_pair( commandName, callback ) );
    }
    else
        throw std::runtime_error( "Could not lock priority callback. Already locked by another client." ); 
}

void
Nova::IOService::PriorityOn( Nova::IOService::EventType type, Nova::IOService::IOEvent_Callback callback){

    if( type == COMMAND )
        throw std::runtime_error( "Cannot listen to COMMAND events. Please listen to specific commands instead." );

    auto res = _priority_callback.find( type );
    if( res == _priority_callback.end() ){
        _priority_callback.insert( std::make_pair( type, callback ) );
    }
    else
        throw std::runtime_error( "Could not lock priority callback. Already locked by another client." ); 
}

void
Nova::IOService::PriorityClear( std::string commandName ){

    auto res = _command_priority_callback.find( commandName );
    if( res != _command_priority_callback.end() ){
        _command_priority_callback.erase( res );
    }  
}

void
Nova::IOService::PriorityClear( Nova::IOService::EventType type ){

    if( type == COMMAND )
        throw std::runtime_error( "Cannot clear priority on COMMAND events." );

    auto res = _priority_callback.find( type );
    if( res != _priority_callback.end() ){
        _priority_callback.erase( res );
    }  
}

//######################################################################33
//
//     class KeyBinder
//
//######################################################################33


Nova::KeyBinder::KeyBinder( ApplicationFactory& app ) : _app( app )
{


}


Nova::KeyBinder::~KeyBinder()
{


}


Nova::Binding
Nova::KeyBinder::Translate( std::string raw_binding ) const
{
    

    return Binding();
}

void
Nova::KeyBinder::Bind( Nova::Binding binding )
{
    auto res = _boundActions.find( binding.trigger );
    if( res != _boundActions.end() ){
        for( auto activeBinding : res->second ) {
            if( activeBinding.match( binding ) )
                throw std::runtime_error( "Cannot perform binding. Binding trigger already bound to command." );
        }
        // If we passed this section, our binding is unique!
        res->second.push_back( binding );
    }
    else{
        _boundActions.insert( std::make_pair( binding.trigger, std::vector< Binding >() ) );
        _boundActions.at( binding.trigger ).push_back( binding );
    }
}

void
Nova::KeyBinder::Dispatch( const IOEvent& event ) const
{


}




