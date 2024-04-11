#include <stdio.h>
#include <stdlib.h>
#include <dbus/dbus.h>

#define SERVICE_NAME "com.example.SampleService"
#define OBJECT_PATH "/com/example/SampleObject"
#define INTERFACE_NAME "com.example.SampleInterface"

int main() {
    DBusError err;
    DBusConnection *connection;
    DBusMessage *msg;
    DBusMessageIter args;
    DBusPendingCall *pending;
    int ret;
    const char *reply;

    dbus_error_init(&err);

    /* Connect to the D-Bus session bus */
    connection = dbus_bus_get(DBUS_BUS_SESSION, &err);
    if (dbus_error_is_set(&err)) {
        fprintf(stderr, "Connection Error (%s)\n", err.message);
        dbus_error_free(&err);
        exit(1);
    }

    /* Create a new method call message */
    msg = dbus_message_new_method_call(SERVICE_NAME, OBJECT_PATH, INTERFACE_NAME, "Echo");

    /* Append arguments to the message */
    dbus_message_iter_init_append(msg, &args);
    dbus_message_iter_append_basic(&args, DBUS_TYPE_STRING, "Hello, D-Bus!");

    /* Send the message and get a handle for a reply */
    if (!dbus_connection_send_with_reply(connection, msg, &pending, -1)) {
        fprintf(stderr, "Out Of Memory!\n");
        exit(1);
    }
    if (pending == NULL) {
        fprintf(stderr, "Pending Call Null\n");
        exit(1);
    }
    dbus_connection_flush(connection);

    /* Block until we receive a reply */
    dbus_pending_call_block(pending);

    /* Get the reply message */
    msg = dbus_pending_call_steal_reply(pending);
    if (msg == NULL) {
        fprintf(stderr, "Reply Null\n");
        exit(1);
    }

    /* Read the message */
    if (!dbus_message_iter_init(msg, &args))
        fprintf(stderr, "Message has no arguments!\n");
    else if (dbus_message_iter_get_arg_type(&args) != DBUS_TYPE_STRING)
        fprintf(stderr, "Argument is not string!\n");
    else
        dbus_message_iter_get_basic(&args, &reply);

    printf("Reply: %s\n", reply);

    /* Free resources */
    dbus_message_unref(msg);
    dbus_pending_call_unref(pending);

    return 0;
}

