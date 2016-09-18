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
                         DROP };
        
        typedef std::function<void(IOEvent&)> IOEvent_Callback;
        
        void Trigger( IOEvent event );
        void On( EventType type, IOEvent_Callback callback);               
        void PriorityOn( EventType type, IOEvent_Callback callback);
        void PriorityClear( EventType type );
        
    private:

        EventType translateEventType( const IOEvent& ) const;
        
        ApplicationFactory& _app;
        std::map< EventType , std::vector<IOEvent_Callback> > _callbacks;
        std::map< EventType , IOEvent_Callback > _priority_callback; 
    };
}

#endif
