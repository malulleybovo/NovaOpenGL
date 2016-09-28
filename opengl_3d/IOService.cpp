#include "IOService.h"
#include "ApplicationFactory.h"
#include <iostream>
#include <iomanip>
// C++11 support for regular expressions is still rather spotty.
// Fallback to boost if not supported.
#if defined(USE_C11_REGEX)
#define _rns std
#include <regex>
#else
#define _rns boost
#include <boost/regex.hpp>
#endif //USE_C11_REGEX


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
//std::cout << "Calling callback for event " <<  command  << " : " << callback.target_type().name()<< std::endl;
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
                        //std::cout << "Calling callback for event " <<  translateEventType(event) << " : " << callback.target_type().name()<< std::endl;
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
    _app.GetIOService().On("LIST-KEYBINDINGS", [&](Nova::IOEvent& event){ this->ListKeyBindings();});
}


Nova::KeyBinder::~KeyBinder()
{
    

}

void
Nova::KeyBinder::ListKeyBindings() const
{
    struct displayBinding {
        std::string trigger;
        std::string command;
    };

    std::vector<displayBinding> display;
    int max_trigger_len = 0;

    std::cout << "============================================" << std::endl;
    std::cout << "            Active Key Bindings             " << std::endl;
    std::cout << "============================================" << std::endl;
    for( auto& triggerGroup : _boundActions ){
        for( auto& binding : triggerGroup.second ){
            displayBinding db;
            if( binding.modifiers & IOEvent::mSHIFT )
                db.trigger += "SHIFT-";
            if( binding.modifiers & IOEvent::mALT )
                db.trigger += "ALT-";
            if( binding.modifiers & IOEvent::mCONTROL )
                db.trigger += "CTRL-";
            if( binding.modifiers & IOEvent::mSUPER )
                db.trigger += "SUPER-";
            db.trigger +=IOEvent::TranslateCode( binding.trigger );
            db.command = binding.command;
            for( auto& arg : binding.args )
                db.command += std::string(" ") + arg;
            max_trigger_len = std::max( max_trigger_len, (int)db.trigger.length() );
            display.push_back( db );
        }
    }

    for( auto db : display ){
        std::cout << std::left << std::setw(max_trigger_len) << db.trigger << "  =>  " << db.command << std::endl;
    }


}


bool
Nova::KeyBinder::Translate( std::string raw_binding, Nova::Binding& binding ) const
{    
    std::string r( R"((\w+)\s+(\w+)(?:(?:\s+(?:(MOD_SHIFT)|(MOD_ALT)|(MOD_CONTROL)|(MOD_SUPER)))*)\s+((?:[A-Z0-9-]+))\s*((?:[A-Z0-9-]+(?:)|\s+(?=[A-Z0-9-]+))*)\s*)");

    _rns::smatch m;
    bool results = _rns::regex_search(raw_binding, m, _rns::regex(r, _rns::regex::ECMAScript));
    if(
#if defined(USE_C11_REGEX)
       m.empty()
#else
       !results
#endif
       ) {
        std::cout << "Regex failed to match." << std::endl;
        return false;
    }
    else {
        // Parse out the match components
        int trigger = IOEvent::TranslateCode(m[1]);
        int action = IOEvent::TranslateAction(m[2]);
        int mods = (m[3]!="" ? IOEvent::mSHIFT : 0 ) |
            (m[4]!="" ? IOEvent::mALT : 0 ) |
            (m[5]!="" ? IOEvent::mCONTROL : 0 ) |
            (m[6]!="" ? IOEvent::mSUPER : 0 );
        std::string command_str = m[7];
        if( trigger == -1 || action == -1 ){
            std::cout << "No such key or action" << std::endl;
            return false;
        }
        binding.trigger = trigger;
        binding.action = action;
        binding.modifiers = mods;
        binding.command = command_str;

        std::stringstream arg_tokenizer;
        arg_tokenizer.str( m[8] );
        while( arg_tokenizer ){
            std::string arg;
            arg_tokenizer >> arg;
            binding.args.push_back( arg );
        }
        
        return true;
    }
    
}

void
Nova::KeyBinder::Bind( Nova::Binding binding )
{
    if( FindBinding( binding ) )
        throw std::runtime_error( "Cannot perform binding. Binding trigger already bound to command." );
    else{
        std::cout << "Binding " << binding.trigger << " " << binding.action << " " << binding.modifiers << "  to Command: " << binding.command << std::endl;
        _boundActions.insert( std::make_pair( binding.trigger, std::vector< Binding >() ) );
        _boundActions.at( binding.trigger ).push_back( binding );
    }
}

void
Nova::KeyBinder::UnBind( Nova::Binding binding )
{
    auto res = _boundActions.find( binding.trigger );
    if( res != _boundActions.end() ){
        for( auto iter = res->second.begin(); iter != res->second.end(); iter++){
            if( binding.match( (*iter) ) ){
                res->second.erase( iter );
                break;
            }
        }
    }
}

bool 
Nova::KeyBinder::FindBinding( Binding& binding_template ) const 
{
    auto res = _boundActions.find( binding_template.trigger );
    if( res != _boundActions.end() ){
        for( auto& binding : res->second ){
            if( binding_template.match( binding ) ){
                binding_template.command = binding.command;
                binding_template.args = binding.args;
                return true;
            }
        }
        return false;
    }
    return false;
}

void
Nova::KeyBinder::Dispatch( const IOEvent& event ) const
{

    switch( event.type ){
    case IOEvent::TIME:
        {
            // This event is special, as it is the render loop command
            IOEvent dispatched_event(IOEvent::COMMAND);
            dispatched_event.data_flags |= IOEvent::TIME;
            dispatched_event.command_data->command = "RENDER-FRAME";
            dispatched_event.currentTime = event.currentTime;
            _app.GetIOService().Trigger( dispatched_event );
        }
        break;
    case IOEvent::MOUSEBUTTON:
        {
            Binding bind_template;
            bind_template.trigger = event.mousebutton_data->button;
            bind_template.action = event.mousebutton_data->action;
            bind_template.modifiers = event.mousebutton_data->mods;
            if( FindBinding( bind_template ) ){ // Updates the template with the command string, if success
                IOEvent dispatched_event(IOEvent::COMMAND);
                dispatched_event.mousebutton_data = std::unique_ptr<IOEvent::_mousebutton_data>( new IOEvent::_mousebutton_data() );
                dispatched_event.data_flags |= (IOEvent::TIME | IOEvent::MOUSEBUTTON);
                dispatched_event.mousebutton_data->operator=( *(event.mousebutton_data) );
                dispatched_event.command_data->command = bind_template.command;
                dispatched_event.command_data->args = bind_template.args;
                dispatched_event.currentTime = event.currentTime;
                _app.GetIOService().Trigger( dispatched_event );
            }
        }
        break;
    case IOEvent::MOUSEMOVE:
        {
            Binding bind_template;
            bind_template.trigger = IOEvent::M_MOVE;
            bind_template.action = IOEvent::M_MOTION;
            bind_template.modifiers = event.mousemotion_data->mods;
            if( FindBinding( bind_template ) ){ // Updates the template with the command string, if success
                IOEvent dispatched_event(IOEvent::COMMAND);
                dispatched_event.mousemotion_data = std::unique_ptr<IOEvent::_mousemotion_data>( new IOEvent::_mousemotion_data() );
                dispatched_event.data_flags |= (IOEvent::TIME | IOEvent::MOUSEMOVE);
                dispatched_event.mousemotion_data->operator=( *(event.mousemotion_data) );
                dispatched_event.command_data->command = bind_template.command;
                dispatched_event.command_data->args = bind_template.args;
                dispatched_event.currentTime = event.currentTime;
                _app.GetIOService().Trigger( dispatched_event );
            }
        }
        break;
    case IOEvent::KEYBOARD:
        {
            Binding bind_template;
            bind_template.trigger = event.key_data->key;
            bind_template.action = event.key_data->action;
            bind_template.modifiers = event.key_data->mods;
            if( FindBinding( bind_template ) ){ // Updates the template with the command string, if success
                IOEvent dispatched_event(IOEvent::COMMAND);
                dispatched_event.key_data = std::unique_ptr<IOEvent::_key_data>( new IOEvent::_key_data() );
                dispatched_event.data_flags |= (IOEvent::TIME | IOEvent::KEYBOARD);
                dispatched_event.key_data->operator=( *(event.key_data) );
                dispatched_event.command_data->command = bind_template.command;
                dispatched_event.command_data->args = bind_template.args;
                dispatched_event.currentTime = event.currentTime;
                _app.GetIOService().Trigger( dispatched_event );
            }
        }
        break;
    case IOEvent::SCROLL:
        {
            Binding bind_template;
            bind_template.trigger = IOEvent::M_MIDDLE;
            bind_template.action = IOEvent::M_SCROLL;
            bind_template.modifiers = event.scroll_data->mods;
            if( FindBinding( bind_template ) ){ // Updates the template with the command string, if success
                IOEvent dispatched_event(IOEvent::COMMAND);
                dispatched_event.scroll_data = std::unique_ptr<IOEvent::_scroll_data>( new IOEvent::_scroll_data() );
                dispatched_event.data_flags |= (IOEvent::TIME | IOEvent::SCROLL);
                dispatched_event.scroll_data->operator=( *(event.scroll_data) );
                dispatched_event.command_data->command = bind_template.command;
                dispatched_event.command_data->args = bind_template.args;
                dispatched_event.currentTime = event.currentTime;
                _app.GetIOService().Trigger( dispatched_event );
            }
        }
        break;
    case IOEvent::DRAW:
        {
            // This event is special
            IOEvent dispatched_event(IOEvent::COMMAND);
            dispatched_event.data_flags |= IOEvent::TIME | IOEvent::DRAW;
            dispatched_event.draw_data = std::unique_ptr<IOEvent::_draw_data>( new IOEvent::_draw_data() );
            dispatched_event.draw_data->operator=( *(event.draw_data) );
            dispatched_event.command_data->command = "WINDOW-RESIZE";
            dispatched_event.currentTime = event.currentTime;
            _app.GetIOService().Trigger( dispatched_event );
        }
        break;
    case IOEvent::DROP:
        {
            // This event is special
            IOEvent dispatched_event(IOEvent::COMMAND);
            dispatched_event.data_flags |= IOEvent::TIME | IOEvent::DROP;
            dispatched_event.drop_data = std::unique_ptr<IOEvent::_drop_data>( new IOEvent::_drop_data() );
            dispatched_event.drop_data->operator=( *(event.drop_data) );
            dispatched_event.command_data->command = "FILE-DROP";
            dispatched_event.currentTime = event.currentTime;
            _app.GetIOService().Trigger( dispatched_event );
        }
        break;
    }

}




