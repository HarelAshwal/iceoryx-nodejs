#include "iceoryx_server.h"

Napi::Object IceoryxServer::Init(Napi::Env env, Napi::Object exports)
{
    Napi::Function func = DefineClass(env, "IceoryxServer", {// Define your class methods here
                                                             InstanceMethod("listen", &IceoryxServer::Listen)});

    Napi::FunctionReference *constructor = new Napi::FunctionReference();
    *constructor = Napi::Persistent(func);
    env.SetInstanceData(constructor);

    exports.Set("IceoryxServer", func);
    return exports;
}

IceoryxServer::IceoryxServer(const Napi::CallbackInfo &info) : Napi::ObjectWrap<IceoryxServer>(info)
{
    // Initialize your server instance and any other members here
}

IceoryxServer::~IceoryxServer()
{
    // Clean up any resources or memory allocated here
    delete m_server;
}

// Implement your server methods here
void IceoryxServer::Listen(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();

    // // Check if the correct number of arguments are passed and if the first argument is a string
    // if (info.Length() != 1 || !info[0].IsString())
    // {
    //     Napi::TypeError::New(env, "Wrong parameters, must be a string").ThrowAsJavaScriptException();
    //     Napi::String response = Napi::String::New(env, "error");

    //     return response;
    // }

    // // Get the string from the first argument
    // Napi::String message = info[0].As<Napi::String>();
    // std::string messageStr = message.Utf8Value();

    iox::popo::Listener listener;
    iox::popo::ServerOptions options;

    options.requestQueueCapacity = 10U;
    iox::popo::Server<ApiData, ApiData> server({"Api", "Request-Response", "Handle"},
                                               options);

    printf("starting server...\n");
    listener
        .attachEvent(server, iox::popo::ServerEvent::REQUEST_RECEIVED,
                     iox::popo::createNotificationCallback(IceoryxServer::onRequestReceived, *this))
        .or_else([](auto)
                 { printf("unable to attach server"); });

    iox::posix::waitForTerminationRequest();
    listener.detachEvent(server, iox::popo::ServerEvent::REQUEST_RECEIVED);
}

void IceoryxServer::onRequestReceived(iox::popo::Server<ApiData, ApiData> *server, IceoryxServer *self)
{
    volatile bool keep_listening{true};

    printf("server start listening on message recieve...\n");

    while (keep_listening && server->take()
                                 .and_then([&](const auto &request)
                                           { server->loan(request)
                                                 .and_then([&](auto &response)
                                                           {
                                                                std::string err;
                                                                std::string client_req = std::string(request->data);
                                                                printf("%s->Request: %s", "server", client_req.c_str());              
                                                                std::string res{ "{some resp}"};
                                                                std::copy(res.begin(), res.end(), response->data);
                                                                printf( "%s->Response: %s", "server", res.c_str());
                                                                response.send().or_else(
                                                                    [&](auto &error) { printf( "Could not send Response! Error: %s", error); }); })
                                                 .or_else([](auto &error)
                                                          { printf("Could not allocate Response! Error: %s", error); }); }))
    {
    }
}