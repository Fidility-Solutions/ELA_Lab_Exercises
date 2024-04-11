#include <stdio.h>
#include <stdlib.h>
#include <dbus/dbus.h>

#define SERVICE_NAME "com.example.SampleService"
#define OBJECT_PATH "/com/example/SampleObject"
#define INTERFACE_NAME "com.example.SampleInterface"

DBusConnection *connection;

static void on_method_call(DBusConnection *conn, DBusMessage *msg, void *user_data) {
    DBusMessage *reply;
    const char *method_name = dbus_message_get_member(msg);
    
    printf("Method call: %s\n", method_name);
    
    /* Handle method calls based on method name */
    if (strcmp(method_name, "Echo") == 0) {
        const char *message;
        dbus_message_get_args(msg, NULL, DBUS_TYPE_STRING, &message, DBUS_TYPE_INVALID);
        
        printf("Received message: %s\n", message);
        
        /* Send a reply back to the client */
        reply = dbus_message_new_method_return(msg);
        dbus_message_append_args(reply, DBUS_TYPE_STRING, &message, DBUS_TYPE_INVALID);
        dbus_connection_send(connection, reply, NULL);
        dbus_message_unref(reply);
    } else {
        printf("Unknown method call\n");
    }
}

int main() {
    DBusError err;
    dbus_error_init(&err);
    
    /* Connect to the D-Bus session bus */
    connection = dbus_bus_get(DBUS_BUS_SESSION, &err);
    if (dbus_error_is_set(&err)) {
        fprintf(stderr, "Connection Error (%s)\n", err.message);
        dbus_error_free(&err);
        exit(1);
    }
    
    /* Register a D-Bus object */
    dbus_connection_register_object_path(connection, OBJECT_PATH, &interface_vtable, NULL);
    
    /* Request bus name and advertise service */
    dbus_bus_request_name(connection, SERVICE_NAME, 0, &err);
    if (dbus_error_is_set(&err)) {
        fprintf(stderr, "Name Error (%s)\n", err.message);
        dbus_error_free(&err);
        exit(1);
    }
    
    printf("Server is running...\n");
    
    /* Handle D-Bus method calls */
    dbus_connection_add_filter(connection, on_method_call, NULL, NULL);
    
    /* Loop and handle incoming messages */
    while (dbus_connection_read_write_dispatch(connection, -1)) {}
    
    return 0;
}

