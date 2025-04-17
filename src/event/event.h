#pragma once

typedef struct {
    int type;
    int field1;
    int field2;
    int field3;
    char data[48];
} Event;

#define EVENT_TYPE_WINDOW_RESIZE 1
#define EVENT_TYPE_WINDOW_CLOSE 2
#define MAX_EVENT EVENT_TYPE_WINDOW_CLOSE

void setup_event_manager();
void raise_event(Event* evt);
int register_event_handler(int type, int(*event_callback)(Event*));
void teardown_event_manager();