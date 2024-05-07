#include "iceoryx_client.h"

Napi::Object IceoryxClient::Init(Napi::Env env, Napi::Object exports)
{
    Napi::Function func =
        DefineClass(env,
                    "IceoryxClient",
                    {InstanceMethod("sendMessage", &IceoryxClient::SendMessage)});

    Napi::FunctionReference *constructor = new Napi::FunctionReference();
    *constructor = Napi::Persistent(func);
    env.SetInstanceData(constructor);

    exports.Set("IceoryxClient", func);

    return exports;
}

IceoryxClient::IceoryxClient(const Napi::CallbackInfo &info) : Napi::ObjectWrap<IceoryxClient>(info)
{
    Napi::Env env = info.Env();

    int length = info.Length();

    // if (length <= 0 || !info[0].IsArray())
    // {
    //     Napi::TypeError::New(env, "Wrong paramemters").ThrowAsJavaScriptException();
    //     return;
    // }

    // Napi::Array lIoxConfStrings = info[0].As<Napi::Array>();

    iox::popo::ClientOptions options;
    options.responseQueueCapacity = 1U;

    m_client = new iox::popo::Client<ApiData, ApiData>({"Api", "Request-Response", "Handle"},
                                                       options);

    _waitset.attachState(*m_client, iox::popo::ClientState::HAS_RESPONSE).or_else([](auto)
                                                                                  { printf("failed to attach client"); });
}

IceoryxClient::~IceoryxClient()
{
    delete m_client;
}

Napi::Value IceoryxClient::SendMessage(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();

    // Check if the correct number of arguments are passed and if the first argument is a string
    if (info.Length() != 1 || !info[0].IsString())
    {
        Napi::TypeError::New(env, "Wrong parameters, must be a string").ThrowAsJavaScriptException();
        Napi::String response = Napi::String::New(env, "error");

        return response;
    }

    // Get the string from the first argument
    Napi::String message = info[0].As<Napi::String>();
    std::string messageStr = message.Utf8Value();

    std::string req{messageStr + "\n"};
    std::string resp{
        R"({"req": "{ "name": "unknown" }", "returnCode": "error", "returnCodeError": "message send failed"})"};

    m_client->loan()
        .and_then([&](auto &request)
                  {
            request.getRequestHeader().setSequenceId(_ctx.requestSequenceId);
            _ctx.expectedResponseSequenceId = _ctx.requestSequenceId;
            _ctx.requestSequenceId += 1;
            std::copy(req.begin(), req.end(), request->data);
            printf( "Send Request: %s", req.c_str());

            request.send().or_else([&](auto &error) { printf( "Could not send Request! %s", error); }); })
        .or_else([](auto &error)
                 { printf("Could not allocate Request! %s", error); });

    auto notificationVector = _waitset.timedWait(iox::units::Duration::fromMinutes(25));

    for (auto &notification : notificationVector)
    {
        if (notification->doesOriginateFrom(m_client))
        {

            while (m_client->take().and_then([&](const auto &response)
                                             {
                auto receivedSequenceId = response.getResponseHeader().getSequenceId();
                if (receivedSequenceId == _ctx.expectedResponseSequenceId)
                {
                    resp = std::string(response->data);
                    printf(  "Got Response : %s\n", resp.c_str());
                }
                else
                {
                    printf(  "Got Response with outdated sequence ID!");
                    resp =
                        R"({"req": "{ "name": "unknown" }", "returnCode": "error", "returnCodeError": "Got Response with outdated sequence ID!"})";
                } }))
            {
            }
        }
    }

    Napi::String response = Napi::String::New(env, resp);

    return response;
}
