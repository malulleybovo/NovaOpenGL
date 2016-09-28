#ifndef __OPENGL_3D_IO_SERVICE_H__
#define __OPENGL_3D_IO_SERVICE_H__

#include "IOEvent.h"
#include <functional>
#include <map>
#include <vector>

namespace Nova{

    class ApplicationFactory;
    
    class IOService {
    public:
        IOService( ApplicationFactory& app);
        virtual ~IOService();

        enum EventType { MOUSE_DOWN, MOUSE_UP, MOUSE_MOVE, KEY_DOWN, KEY_UP, KEY_HOLD, REDRAW, TIME, SCROLL,
                         DROP, COMMAND };
        
        typedef std::function<void(IOEvent&)> IOEvent_Callback;

        void Trigger( IOEvent& event );

        void On( std::string commandName, IOEvent_Callback callback);               
        void On( EventType type, IOEvent_Callback callback);               

        void PriorityOn( std::string commandName, IOEvent_Callback callback);               
        void PriorityOn( EventType type, IOEvent_Callback callback);

        void PriorityClear( std::string commandName );
        void PriorityClear( EventType type );

    private:

        EventType translateEventType( const IOEvent& ) const;
        
        ApplicationFactory& _app;
        std::map< EventType , std::vector<IOEvent_Callback> > _callbacks;
        std::map< EventType , IOEvent_Callback > _priority_callback; 

        std::map< std::string , std::vector<IOEvent_Callback> > _command_callbacks;
        std::map< std::string , IOEvent_Callback > _command_priority_callback;
    };

    struct Binding {
        // Must match the following ...
        int trigger;
        int action;
        int modifiers;

        // ... to produce the following:
        std::string command;
        std::vector< std::string > args;

        bool match( Binding other){
            return other.trigger == trigger &&
                other.action == action &&
                other.modifiers == modifiers;
        }
    };
    
    class KeyBinder {
    public:
        KeyBinder( ApplicationFactory& app );
        virtual ~KeyBinder();
        
        void ListKeyBindings() const;
        bool Translate( std::string raw_binding, Binding& binding ) const ;
        void Bind( Binding b);
        void UnBind( Binding b);
        
        void Dispatch( const IOEvent& event ) const;

    private:
        bool FindBinding( Binding& binding_template ) const;
        ApplicationFactory& _app;
        std::map<int, std::vector< Binding > > _boundActions;        
    };

}

#endif
