#include "IOService.h"

Nova::IOService::IOService( ApplicationFactory& app) : _app(app)
{


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
        switch( event.mousebutton_data.action ){
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
        switch( event.key_data.action ){
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
    case IOEvent::DRAW:
        type=REDRAW;
        break;
    case IOEvent::DROP:
        type=DROP;
        break;
    case IOEvent::SCROLL:
        type=SCROLL;
        break;
    }

    return type;
}

void
Nova::IOService::Trigger( Nova::IOEvent event )
{
    // First check and call the priority callback if it is locked.
    {
        auto res = _priority_callback.find( translateEventType(event) );
        if( res != _priority_callback.end() ){
            res->second( event );
            return; 
        }
    }

    // Otherwise, proceed through the list of callbacks and call each one.
    {
        auto res = _callbacks.find( translateEventType(event) );
        if( res != _callbacks.end() ){
            for( auto callback : res->second ){
                callback( event );
            }
        }
    }
    
}

void
Nova::IOService::On( Nova::IOService::EventType type, Nova::IOService::IOEvent_Callback callback){
    auto res = _callbacks.find( type );
    if( res == _callbacks.end() ){
        _callbacks.insert( std::make_pair( type, std::vector< IOEvent_Callback >() ) );
    }
    _callbacks.at( type ).push_back( callback );
}

void
Nova::IOService::PriorityOn( Nova::IOService::EventType type, Nova::IOService::IOEvent_Callback callback){
    auto res = _priority_callback.find( type );
    if( res == _priority_callback.end() ){
        _priority_callback.insert( std::make_pair( type, callback ) );
    }
    else
        throw std::runtime_error( "Could not lock priority callback. Already locked by another client." ); 
}

void
Nova::IOService::PriorityClear( Nova::IOService::EventType type ){
    auto res = _priority_callback.find( type );
    if( res != _priority_callback.end() ){
        _priority_callback.erase( res );
    }
    

}
