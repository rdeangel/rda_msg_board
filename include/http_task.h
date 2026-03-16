#ifndef HTTP_TASK_H
#define HTTP_TASK_H

#ifdef ESP32

// Start the HTTP server task on Core 0.
// After calling this, handleHttpServer() becomes a no-op — the task owns
// all calls to serverHttp.handleClient() to avoid concurrent access.
// Call after httpWebDirDef() has registered all routes.
void initHttpTask();

#endif // ESP32

#endif // HTTP_TASK_H
