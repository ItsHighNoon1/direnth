#include "event.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct EventHandlerNode {
    int type;
    int(*event_callback)(Event*);
    int handler_id;
    int spare1;
    struct EventHandlerNode* prev;
    struct EventHandlerNode* next;
} EventHandler;

EventHandler* event_handlers[MAX_EVENT + 1];
int next_event_handler_id;

void setup_event_manager() {
    memset(event_handlers, 0, sizeof(event_handlers));
    next_event_handler_id = 0;
}

void raise_event(Event* evt) {
    if (evt->type <= 0 || evt->type > MAX_EVENT) {
        printf("Attempted to raise event with bad type %d\n", evt->type);
        return;
    }
    EventHandler* handler = event_handlers[evt->type];
    while (handler != NULL) {
        int consume = handler->event_callback(evt);
        if (consume) {
            break;
        }
        handler = handler->next;
    }
}

int register_event_handler(int type, int(*event_callback)(Event*)) {
    if (type <= 0 || type > MAX_EVENT) {
        printf("Attempted to register event handler with bad type %d\n", type);
        return -1;
    }

    EventHandler* new_handler = malloc(sizeof(EventHandler));
    new_handler->type = type;
    new_handler->event_callback = event_callback;
    new_handler->handler_id = next_event_handler_id++;
    new_handler->next = event_handlers[type];
    new_handler->prev = NULL;
    if (event_handlers[type] != NULL) {
        event_handlers[type]->prev = new_handler;
    }
    event_handlers[type] = new_handler;

    return new_handler->handler_id;
}

void teardown_event_manager() {
    for (int handler_chain = 1; handler_chain <= MAX_EVENT; handler_chain++) {
        EventHandler* handler = event_handlers[handler_chain];
        while (handler != NULL) {
            EventHandler* next_handler = handler->next;
            free(handler);
            handler = next_handler;
        }
    }
}