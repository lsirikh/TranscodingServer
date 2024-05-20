#include <glib.h>
#include "headers/UriParts.h"
#include "headers/TranscodingService.h"
#include "datetime.h"

TranscodingService::TranscodingService() {
    Server = gst_rtsp_server_new();
    mainLoop = g_main_loop_new(nullptr, FALSE);
}

TranscodingService::~TranscodingService() {
    if (serverThread.joinable())
        serverThread.join();
    g_object_unref(Server);
    g_main_loop_unref(mainLoop);
}

void TranscodingService::StartServer() {

    //g_object_set(Server, "service", "8555", NULL);
    gst_rtsp_server_set_address(Server, "0.0.0.0"); // 서버 주소 설정
    gst_rtsp_server_set_service(Server, "8555"); // 서버 포트 설정

    //// Server connects event for "client-connected"
    //g_signal_connect(Server, "client-connected", G_CALLBACK(TranscodingService::client_connected_callback), NULL);

    // std::bind를 사용하여 멤버 함수와 this 포인터를 함께 연결
    g_signal_connect(Server, "client-connected", G_CALLBACK(+[](GstRTSPServer* server, GstRTSPClient* client, gpointer user_data) {
        auto* self = static_cast<TranscodingService*>(user_data);
        self->client_connected_callback(server, client, nullptr);
    }), this);

    // 세션 풀 생성
    GstRTSPSessionPool *pool = gst_rtsp_session_pool_new();
    // 서버에 세션 풀 설정
    gst_rtsp_server_set_session_pool(Server, pool);

    g_object_set(pool, "max-sessions", 200, NULL);

    // 세션 풀 클린업을 위한 GSource 설정
    GMainContext *context = g_main_context_default();
    GSource *cleanup_source = gst_rtsp_session_pool_create_watch(pool);
    g_source_set_callback(cleanup_source, (GSourceFunc) +[](gpointer user_data) -> gboolean {
        auto* self = static_cast<TranscodingService*>(user_data);
        self->session_cleanup(static_cast<GstRTSPSessionPool*>(user_data));
        return G_SOURCE_CONTINUE;
    }, pool, NULL);
    g_source_attach(cleanup_source, context);

    // 세션 풀의 참조 카운트를 줄여 메모리 누수를 방지
    g_object_unref(pool);

    gst_rtsp_server_attach(Server, nullptr);
    g_print("RTSP Server set IP:8555\n");
    g_main_loop_run(mainLoop);
}

void TranscodingService::StopServer() {
    g_main_loop_quit(mainLoop);
}

// 클라이언트가 연결될 때 호출될 콜백 함수
void TranscodingService::client_connected_callback(GstRTSPServer* server, GstRTSPClient* client, gpointer user_data) {
    try
    {
        std::lock_guard<std::recursive_mutex> lock(processMutex);
        if (client == nullptr) {
            gst_println("Received a null client pointer.\n");
            return;
        }
        // 현재 시간 가져오기
        std::ostringstream oss = getCurrentTime();

        // 클라이언트의 메모리 주소와 타입 크기 출력
        g_print("[%s]Client(%p) was connected to RTSP server!\n", oss.str().c_str(), client);

        // 클라이언트 연결 시 처리할 로직 추가
        g_signal_connect(client, "closed", G_CALLBACK(+[](GstRTSPClient* client, gpointer user_data) {
            static_cast<TranscodingService*>(user_data)->closed_callback(client, nullptr);
        }), this);

        g_signal_connect(client, "describe-request", G_CALLBACK(+[](GstRTSPClient* client, GstRTSPContext* context, gpointer user_data) {
            static_cast<TranscodingService*>(user_data)->describe_request_callback(client, context, nullptr);
        }), this);

        g_signal_connect(client, "setup-request", G_CALLBACK(+[](GstRTSPClient* client, GstRTSPContext* context, gpointer user_data) {
            static_cast<TranscodingService*>(user_data)->setup_request_callback(client, context);
        }), this);

        g_signal_connect(client, "teardown-request", G_CALLBACK(+[](GstRTSPClient* client, GstRTSPContext* context, gpointer user_data) {
            static_cast<TranscodingService*>(user_data)->teardown_request_callback(client, context, nullptr);
        }), this);
    }
    catch (const std::exception& e)
    {
        g_print("Exception in onClientConnected: %s\n", e.what());
    }
}

void TranscodingService::closed_callback(GstRTSPClient* client, gpointer user_data) {
    std::lock_guard<std::recursive_mutex> lock(processMutex);  // (수정) 스레드 안전성 확보

    // 현재 시간 가져오기
    std::ostringstream oss = getCurrentTime();

    g_print("[%s]Client(%p) has closed the connection!\n", oss.str().c_str(), client);
    RemoveClientWithClient(client);
}

void TranscodingService::describe_request_callback(GstRTSPClient* client, GstRTSPContext* context, gpointer user_data) {
    g_print("Describe request: %p\n", client);
}

void TranscodingService::setup_request_callback(GstRTSPClient* client, GstRTSPContext* context) {
    try
    {
        std::lock_guard<std::recursive_mutex> lock(processMutex);
        // RTSUrl 객체 얻기
        const GstRTSPUrl* url = context->uri;
         // 현재 시간 가져오기
        std::ostringstream oss = getCurrentTime();
        // RTSSession 객체 얻기
        GstRTSPSession* session = context->session;
        const gchar* session_id = "";
        if (session) {
            session_id = gst_rtsp_session_get_sessionid (session);
            g_print("[%s]Session id: %s\n", oss.str().c_str(), session_id);
            
            // 세션의 타임아웃 확인
            guint timeout = gst_rtsp_session_get_timeout(session);
            g_print("[%s]Session timeout: %u seconds\n", oss.str().c_str(), timeout);

            // 현재 monotonic 시간 얻기
            gint64 now = g_get_monotonic_time();
            // 세션 만료까지 남은 시간 계산
            gint timeout_msec = gst_rtsp_session_next_timeout_usec(session, now) / 1000;
            g_print("[%s]Session will timeout in: %d seconds\n", oss.str().c_str(), timeout_msec);
            // Prevent session from expiring.
            gst_rtsp_session_prevent_expire(session);
            
        } else {
            g_print("[%s]No session associated with the context.\n", oss.str().c_str());
        }
        if (url && url->abspath) {
            g_print("[%s]Client(%s) was connected to URL: %s\n", oss.str().c_str(), session_id, url->abspath);
            AddClientInfo(client, session_id, url->abspath);
        }
        else {
            g_print("[%s]Client(%s) can't be connected to URL: %s\n", oss.str().c_str(), session_id, url->abspath);
        }
    }
    catch (const std::exception& e) {
        g_print("Exception in onClientSetup: %s\n", e.what());
    }
}

void TranscodingService::teardown_request_callback(GstRTSPClient* client, GstRTSPContext* context, gpointer user_data) {
    try
    {
        std::lock_guard<std::recursive_mutex> lock(processMutex);
        // 현재 시간 가져오기
        std::ostringstream oss = getCurrentTime();
        g_print("[%s] Session will be tear-downed.\n", oss.str().c_str());
        GstRTSPSessionPool* session_pool = gst_rtsp_client_get_session_pool (client);
        session_cleanup(session_pool);
    }
    catch (const std::exception& e) {
        g_print("Exception in onClientSetup: %s\n", e.what());
    }
}

void TranscodingService::session_cleanup(GstRTSPSessionPool* pool) {
    if(pool != nullptr){
        guint removed_sessions = gst_rtsp_session_pool_cleanup(pool);
        g_print("Cleaned up %u sessions\n", removed_sessions);
    }
}

/// <summary>
/// 
/// </summary>
/// <param name="uri"></param>
bool TranscodingService::AddRtsp(const std::string& uri) {
    try
    {
        std::lock_guard<std::recursive_mutex> lock(processMutex);

        if(!checkUri(uri)) {
            return false;
        }

        UriParts uriParts;
        uriParts.parseUri(uri);

        std::string firstMount = "/" + uriParts.id + "/first";
        std::string secondMount = "/" + uriParts.id + "/second";
        std::string thirdMount = "/" + uriParts.id + "/third";
        std::string fourthMount = "/" + uriParts.id + "/fourth";
        bool isRegistered = false;
        if (mediaFactories.find(firstMount) != mediaFactories.end()) {
            std::cerr << "Mount point already exists: " << firstMount << std::endl;
            isRegistered = true;
        }
        if (mediaFactories.find(secondMount) != mediaFactories.end()) {
            std::cerr << "Mount point already exists: " << secondMount << std::endl;
            isRegistered = true;
        }
        if (mediaFactories.find(thirdMount) != mediaFactories.end()) {
            std::cerr << "Mount point already exists: " << thirdMount << std::endl;
            isRegistered = true;
        }
        if (mediaFactories.find(fourthMount) != mediaFactories.end()) {
            std::cerr << "Mount point already exists: " << fourthMount << std::endl;
            isRegistered = true;
        }

        if(isRegistered) return false;

        GstRTSPMountPoints* mounts = gst_rtsp_server_get_mount_points(Server);
        if (!mounts) {
            std::cerr << "Failed to get mount points." << std::endl;
            return false;
        }

        GstRTSPMediaFactory* firstFactory = gst_rtsp_media_factory_new();
        //latency=0 ! rtph264depay ! rtph264pay name=pay0 pt=96
        //latency=500 ! rtph264depay ! h264parse ! nvh264dec ! videoconvert ! queue ! x264enc ! rtph264pay config-interval=1 name=pay0 pt=96
        gst_rtsp_media_factory_set_launch(firstFactory, ("( rtspsrc location=" + uri + " latency=500 ! rtph264depay ! rtph264pay name=pay0 pt=96 )").c_str());
        gst_rtsp_media_factory_set_shared(firstFactory, TRUE);

        GstRTSPMediaFactory* secondFactory = gst_rtsp_media_factory_new();
        gst_rtsp_media_factory_set_launch(secondFactory, ("( rtspsrc location=" + uri + " latency=500 ! rtph264depay ! h264parse ! nvh264dec ! videoconvert ! queue ! videoscale ! videorate ! video/x-raw,framerate=15/1,width=1280,height=960 ! nvh264enc bitrate=2000 speed-preset=ultrafast tune=zerolatency cabac=true ! rtph264pay config-interval=1 name=pay0 pt=96 )").c_str());
        gst_rtsp_media_factory_set_shared(secondFactory, TRUE);

        GstRTSPMediaFactory* thirdFactory = gst_rtsp_media_factory_new();
        gst_rtsp_media_factory_set_launch(thirdFactory, ("( rtspsrc location=" + uri + " latency=500 ! rtph264depay ! h264parse ! nvh264dec ! videoconvert ! queue ! videoscale ! videorate ! video/x-raw,framerate=10/1,width=640,height=480 ! nvh264enc bitrate=800 speed-preset=ultrafast tune=zerolatency cabac=true ! rtph264pay config-interval=1 name=pay0 pt=96 )").c_str());
        gst_rtsp_media_factory_set_shared(thirdFactory, TRUE);

        GstRTSPMediaFactory* forthFactory = gst_rtsp_media_factory_new();
        gst_rtsp_media_factory_set_launch(forthFactory, ("( rtspsrc location=" + uri + " latency=500 ! rtph264depay ! h264parse ! nvh264dec ! videoconvert ! queue ! videoscale ! videorate ! video/x-raw,framerate=10/1,width=320,height=240 ! nvh264enc bitrate=400 speed-preset=ultrafast tune=zerolatency cabac=true ! rtph264pay config-interval=1 name=pay0 pt=96 )").c_str());
        gst_rtsp_media_factory_set_shared(forthFactory, TRUE);

        // 현재 시간 가져오기
        std::ostringstream oss = getCurrentTime();

        g_print("[%s]RTSP mount points : %s\n", oss.str().c_str(), firstMount.c_str());
        g_print("[%s]RTSP mount points : %s\n", oss.str().c_str(), secondMount.c_str());
        g_print("[%s]RTSP mount points : %s\n", oss.str().c_str(), thirdMount.c_str());
        g_print("[%s]RTSP mount points : %s\n", oss.str().c_str(), fourthMount.c_str());

        gst_rtsp_mount_points_add_factory(mounts, firstMount.c_str(), firstFactory);
        mediaFactories[firstMount] = firstFactory;
        gst_rtsp_mount_points_add_factory(mounts, secondMount.c_str(), secondFactory);
        mediaFactories[secondMount] = secondFactory;
        gst_rtsp_mount_points_add_factory(mounts, thirdMount.c_str(), thirdFactory);
        mediaFactories[thirdMount] = thirdFactory;
        gst_rtsp_mount_points_add_factory(mounts, fourthMount.c_str(), forthFactory);
        mediaFactories[fourthMount] = forthFactory;
        g_object_unref(mounts);

        oss = getCurrentTime();
        g_print("[%s]Added RTSP stream: %s\n", oss.str().c_str(), uri.c_str());

        return true;
    }
    catch (const std::exception & e) {
        g_print("Exception in AddRtsp: %s\n", e.what());
        return false;
    }
}

/// <summary>
/// 
/// </summary>
/// <param name="id"></param>
bool TranscodingService::RemoveRtsp(const std::string& id) {
    try
    {
        std::lock_guard<std::recursive_mutex> lock(processMutex);
        // Before Remove RTSP media factory, client session should be disconnected!
        RemoveClientWithId(id);

        //std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        // Logic to remove an RTSP URI from the server
        // This function needs actual GStreamer RTSP server details to be implemented
        GstRTSPMountPoints* mounts = gst_rtsp_server_get_mount_points(Server);
        if (!mounts) {
            std::cerr << "Failed to get mount points." << std::endl;
            return false;
        }

        std::string firstMount = "/" + id + "/first";
        std::string secondMount = "/" + id + "/second";
        std::string thirdMount = "/" + id + "/third";
        std::string fourthMount = "/" + id + "/fourth";

        auto it = mediaFactories.find(firstMount);
        if (it != mediaFactories.end()) {
            std::cerr << "Mount point was removed :" << firstMount.c_str() << std::endl;
            gst_rtsp_mount_points_remove_factory(mounts, firstMount.c_str());
            mediaFactories.erase(it);
        }

        it = mediaFactories.find(secondMount);
        if (it != mediaFactories.end()) {
            std::cerr << "Mount point was removed :" << secondMount.c_str() << std::endl;
            gst_rtsp_mount_points_remove_factory(mounts, secondMount.c_str());
            mediaFactories.erase(it);
        }

        it = mediaFactories.find(thirdMount);
        if (it != mediaFactories.end()) {
            std::cerr << "Mount point was removed :" << thirdMount.c_str() << std::endl;
            gst_rtsp_mount_points_remove_factory(mounts, thirdMount.c_str());
            mediaFactories.erase(it);
        }

        it = mediaFactories.find(fourthMount);
        if (it != mediaFactories.end()) {
            std::cerr << "Mount point was removed :" << fourthMount.c_str() << std::endl;
            gst_rtsp_mount_points_remove_factory(mounts, fourthMount.c_str());
            mediaFactories.erase(it);
        }
        // 현재 시간 가져오기
        std::ostringstream oss = getCurrentTime();
        g_print("[%s]Removed RTSP stream: %s\n", oss.str().c_str(), id.c_str());
        
        g_object_unref(mounts);
        return true;
    }
    catch (const std::exception& e)
    {
        g_print("Exception in RemoveRtsp: %s\n", e.what());
        return false;
    }
    
}

bool TranscodingService::isValidUri(const std::string& uri) {
    return gst_uri_is_valid(uri.c_str());
}

bool TranscodingService::checkUri(const std::string& uri) {
    std::ostringstream oss = getCurrentTime();
    if (!isValidUri(uri)) {
        // 현재 시간 가져오기
        g_print("[%s]Invalid URI: %s\n", oss.str().c_str(), uri.c_str());
        return false;
    }

    // if (canConnectToUri(uri)) {
    //     // 현재 시간 가져오기
    //     g_print("[%s]Can connect to URI: %s\n", oss.str().c_str(), uri.c_str());
    //     return true;
    // } else {
    //     g_print("[%s]Cannot connect to URI: %s\n", oss.str().c_str(), uri.c_str());
    //     return false;
    // }
    
    return true;
}

void TranscodingService::AddClientInfo(GstRTSPClient* client, const std::string& id, const std::string& uri) {
    std::lock_guard<std::recursive_mutex> lock(processMutex);
    // 현재 시간 가져오기
    std::ostringstream oss = getCurrentTime();
    clientInfos.push_back(std::shared_ptr<ClientInfo>(new ClientInfo(client, id, uri)));
    g_print("[%s]Current number of clients: %ld\n", oss.str().c_str(), clientInfos.size());
}

void TranscodingService::RemoveClientWithClient(GstRTSPClient* client) {
    try
    {
        std::lock_guard<std::recursive_mutex> lock(processMutex);
        if (client == nullptr) {
            std::cerr << "Error: Client pointer is null." << std::endl;
            return;
        }
        if (clientInfos.empty()) {
            std::cout << "Client list is empty. No clients to remove." << std::endl;
            return;
        }

        for (auto it = clientInfos.begin(); it != clientInfos.end(); ) {
            if ((*it)->client == client) {
                gst_rtsp_client_close(client); 
                it = clientInfos.erase(it); 
                std::cout << "Client removed." << std::endl;
            }
            else {
                ++it; 
            }
        }
        g_print("Current number of clients: %ld\n", clientInfos.size());
    }
    catch (const std::exception& e)
    {
        g_print("Exception in removeClient: %s\n", e.what());
    }
}

void TranscodingService::RemoveClientWithId(const std::string& id) {
    try
    {
        std::lock_guard<std::recursive_mutex> lock(processMutex);
        std::string mountPath = "/" + id;
        if (clientInfos.empty()) {
            std::cout << "Client list is empty. No clients to remove." << std::endl;
            return;
        }

        for (auto it = clientInfos.begin(); it != clientInfos.end(); ) {
            if ((*it)->uri.find(mountPath) != std::string::npos) {
                g_print("Removing client connected to: %s\n", (*it)->uri.c_str());
                GstRTSPConnection* conn = gst_rtsp_client_get_connection((*it)->client);
                if (conn) {
                    GstRTSPResult ret = gst_rtsp_connection_close(conn);
                    if(ret == GST_RTSP_OK) {
                        g_print("Client was disconnected : %s\n", "OK");
                    } else {
                        g_print("Client disconnection failed!(%d)\n", ret);
                    }
                    g_object_unref(conn); // Ensure connection object is unrefed properly
                } else {
                    g_print("Failed to get connection for client.\n");
                }
                
                it = clientInfos.erase(it); 
            }
            else {
                ++it;
            }
        }
        g_print("Current number of clients: %ld\n", clientInfos.size());
    }
    catch (const std::exception& e)
    {
        g_print("Exception in removeClient: %s\n", e.what());
    }
}

crow::json::wvalue TranscodingService::GetClientSessions() {
    std::lock_guard<std::recursive_mutex> lock(processMutex);
    crow::json::wvalue result;
    result["result"] = "SUCCESS";
    result["count"] = static_cast<int>(clientInfos.size());
    crow::json::wvalue::list list;
    for (const auto& clientInfo : clientInfos) {
        crow::json::wvalue session_info;
        session_info["session_id"] = clientInfo->session_id;
        session_info["uri"] = clientInfo->uri;
        list.push_back(std::move(session_info));
    }
    result["list"] = std::move(list);
    return result;
}

crow::json::wvalue TranscodingService::GetMedias() {
    std::lock_guard<std::recursive_mutex> lock(processMutex);
    std::unordered_map<std::string, std::vector<std::string>> groupedUris;
    for (const auto& pair : mediaFactories) {
        std::string key = pair.first.substr(0, pair.first.rfind('/'));
        groupedUris[key].push_back(pair.first);
    }

    crow::json::wvalue result;
    result["result"] = "SUCCESS";
    result["count"] = static_cast<int>(groupedUris.size());
    crow::json::wvalue::list list;

    for (const auto& group : groupedUris) {
        crow::json::wvalue media_info;
        for (const auto& uri : group.second) {
            std::string suffix = uri.substr(uri.rfind('/') + 1);
            media_info["uri" + suffix] = uri;
        }
        list.push_back(std::move(media_info));
    }

    result["list"] = std::move(list);
    return result;
}