#ifndef ICEORYX_CLIENT_H
#define ICEORYX_CLIENT_H

#include "common.h"

#include "iceoryx_posh/popo/client.hpp"
#include "iceoryx_posh/popo/wait_set.hpp"

#include <napi.h>

class IceoryxClient : public Napi::ObjectWrap<IceoryxClient>
{
public:
    static Napi::Object Init(Napi::Env env, Napi::Object exports);

    IceoryxClient(const Napi::CallbackInfo &info);

    ~IceoryxClient();

    // Add methods for client functionality here

    Napi::Value SendMessage(const Napi::CallbackInfo &info);

private:
    iox::popo::Client<ApiData, ApiData> *m_client;

    Napi::Buffer<uint8_t> m_loan_buffer;

    // @brief context data structure
    ContextData _ctx;

    // @brief wait set client
    iox::popo::WaitSet<> _waitset;

    // Add any private members as needed
};

#endif // ICEORYX_CLIENT_H
