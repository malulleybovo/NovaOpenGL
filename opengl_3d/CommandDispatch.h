#ifndef __OPENGL_3D_COMMAND_DISPATCH_H__
#define __OPENGL_3D_COMMAND_DISPATCH_H__

#include <sstream>
#include <string>

namespace Nova {

    class ApplicationFactory;
    class IOEvent;
    
    class CommandDispatch {
    public:
        CommandDispatch( ApplicationFactory& app);
        virtual ~CommandDispatch();

        void Input( const IOEvent& event );
        void Render( const IOEvent& event );
        
    private:
        ApplicationFactory& _app;
        std::stringstream _buffer;
        bool _commandstarted;
        
    };

};


#endif
