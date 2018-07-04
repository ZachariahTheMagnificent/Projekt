/*!
 *
 */

#ifndef PROJEKT_EVENT_HANDLER_H
#define PROJEKT_EVENT_HANDLER_H

#include <queue>

#include "event.h"

class event_handler
{
public:
    static bool
    pop_event( event& e )
    {
        if( !event_queue_.empty() )
        {
            e = event_queue_.front();
            event_queue_.pop();

            return true;
        }

        return false;
    }

    static void
    push_event( event& e )
    {
        event_queue_.emplace( e );
    }

private:
    static std::queue<event> event_queue_;
};

#endif //PROJEKT_EVENT_HANDLER_H
