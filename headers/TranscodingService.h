#ifndef TRANSCODINGSERVICE_H
#define TRANSCODINGSERVICE_H

#include <iostream>
#include <string>
#include <gst/gst.h>
#include <gst/rtsp-server/rtsp-server.h>
#include <gst/rtsp-server/rtsp-client.h>
#include <thread>
#include <list>
#include <mutex>
#include <memory>
#include <unordered_map>

struct ClientInfo {
    GstRTSPClient* client;
    std::string uri;

    ClientInfo(GstRTSPClient* c, const std::string& u) : client(c), uri(u) {
        gst_object_ref(client);
    }
    ~ClientInfo() {
        gst_object_unref(client);
    }
};

class TranscodingService {
public:
    TranscodingService();
    ~TranscodingService();

    void Initialize();
    void StartServer();
    void StopServer();

    void AddClientInfo(GstRTSPClient* client, const std::string& uri);
    void RemoveClientWithClient(GstRTSPClient* client);
    void RemoveClientWithId(const std::string& id);
    bool AddRtsp(const std::string& uri);
    bool RemoveRtsp(const std::string& id);
    GstRTSPServer* Server;

private:
    GMainLoop* mainLoop;
    std::list<std::shared_ptr<ClientInfo>> clientInfos;
    std::unordered_map<std::string, GstRTSPMediaFactory*> mediaFactories;
    std::string serverPort;
    std::recursive_mutex  processMutex;
    std::thread serverThread;
};

#endif // TRANSCODINGSERVICE_H
