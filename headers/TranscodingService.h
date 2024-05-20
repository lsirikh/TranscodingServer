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
#include <crow/json.h> // Crow JSON 헤더 추가
#include <regex>

struct ClientInfo {
    GstRTSPClient* client;
    std::string uri;
    std::string session_id;

    ClientInfo(GstRTSPClient* c, const std::string& id, const std::string& u) : client(c), session_id(id), uri(u) {
        gst_object_ref(client);
    }
    ~ClientInfo() {
        gst_object_unref(client);
    }
};

class TranscodingService {
public:
    GstRTSPServer* Server;

    TranscodingService();
    ~TranscodingService();

    void StartServer();
    void StopServer();

    void AddClientInfo(GstRTSPClient* client, const std::string& id, const std::string& uri);
    void RemoveClientWithClient(GstRTSPClient* client);
    void RemoveClientWithId(const std::string& id);
    bool AddRtsp(const std::string& uri);
    bool RemoveRtsp(const std::string& id);
    crow::json::wvalue GetClientSessions(); // JSON 형식으로 세션 정보를 반환하는 메소드 추가
    crow::json::wvalue GetMedias(); // JSON 형식으로 미디어 정보를 반환하는 메소드 추가

private:
    GMainLoop* mainLoop;
    std::list<std::shared_ptr<ClientInfo>> clientInfos;
    std::unordered_map<std::string, GstRTSPMediaFactory*> mediaFactories;
    std::string serverPort;
    std::recursive_mutex  processMutex;
    std::thread serverThread;

    void client_connected_callback(GstRTSPServer* server, GstRTSPClient* client, gpointer user_data);
    void closed_callback(GstRTSPClient* client, gpointer user_data);
    void describe_request_callback(GstRTSPClient* client, GstRTSPContext* context, gpointer user_data);
    void setup_request_callback(GstRTSPClient* client, GstRTSPContext* context);
    void teardown_request_callback(GstRTSPClient* client, GstRTSPContext* context, gpointer user_data);
    void session_cleanup(GstRTSPSessionPool* pool);
    bool isValidUri(const std::string& uri);
    bool checkUri(const std::string& uri);
};

#endif // TRANSCODINGSERVICE_H
