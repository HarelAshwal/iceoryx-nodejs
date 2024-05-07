#ifndef ICEORYX_SERVER_H
#define ICEORYX_SERVER_H

#include "iceoryx_hoofs/posix_wrapper/signal_handler.hpp"
#include "iceoryx_posh/iceoryx_posh_types.hpp"
#include "iceoryx_posh/popo/user_trigger.hpp"
#include "iceoryx_posh/popo/wait_set.hpp"
#include "iceoryx_posh/runtime/posh_runtime.hpp"
#include "iceoryx_posh/popo/server.hpp"
#include "iceoryx_posh/popo/listener.hpp"
#include "iceoryx_hoofs/posix_wrapper/signal_watcher.hpp"

#include <napi.h>

#include "common.h"

class IceoryxServer : public Napi::ObjectWrap<IceoryxServer>
{
public:
    static Napi::Object Init(Napi::Env env, Napi::Object exports);

    static void onRequestReceived(iox::popo::Server<ApiData, ApiData> *server,
                                  IceoryxServer *self);

    IceoryxServer(const Napi::CallbackInfo &info);

    ~IceoryxServer();

    // Add methods for server functionality here

    void Listen(const Napi::CallbackInfo &info);

private:
    iox::popo::Server<ApiData, ApiData> *m_server;

    // @brief context data structure
    ContextData _ctx;

    // Add any private members as needed
};

#endif // ICEORYX_SERVER_H