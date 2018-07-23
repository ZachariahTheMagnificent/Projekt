/*!
 *
 */

#ifndef PROJEKT_EVENT_HANDLER_H
#define PROJEKT_EVENT_HANDLER_H

#include <vector>

#include "event.h"

class event_handler
{
public:
    /*
    static bool
    pop_event( event& e )
    {
        if( !event_queue_.empty() )
        {
            e = event_queue_.front();
            event_queue_.;

            return true;
        }

        return false;
    }
     */

    static std::vector<event> pull( )
    {
        return std::move( event_queue_ );
    }

    static void
    push_event( event& e )
    {
        event_queue_.emplace_back( e );
    }

private:
    static std::vector<event> event_queue_;
};

#endif //PROJEKT_EVENT_HANDLER_H
