
#include "TranscodingService.h"
#include <iostream>
#include <string>
#include "crow.h"
#include "datetime.h"


#define DEFAULT_RTSP_PORT "8555"
#define DEFAULT_DISABLE_RTCP FALSE

static char *port = (char *)DEFAULT_RTSP_PORT;
static gboolean disable_rtcp = DEFAULT_DISABLE_RTCP;

static GOptionEntry entries[] = {
    {"port", 'p', 0, G_OPTION_ARG_STRING, &port,
     "Port to listen on (default: " DEFAULT_RTSP_PORT ")", "PORT"},
    {"disable-rtcp", '\0', 0, G_OPTION_ARG_NONE, &disable_rtcp,
     "Whether RTCP should be disabled (default false)", NULL},
    {NULL}};

void unrefGstClient(GstRTSPClient* client);
void client_connected_callback(GstRTSPServer* server, GstRTSPClient* client, gpointer user_data);
void closed_callback(GstRTSPClient* client, gpointer user_data);
void describe_request_callback(GstRTSPClient* client, GstRTSPContext* context, gpointer user_data);
void setup_request_callback(GstRTSPClient* client, GstRTSPContext* context);
void teardown_request_callback(GstRTSPClient* client, GstRTSPContext* context, gpointer user_data);
void session_cleanup(GstRTSPSessionPool* pool);

std::shared_ptr<TranscodingService> service;  // (수정) 자동 정리를 위해 unique_ptr 사용
std::recursive_mutex  processMutex;  // 클라이언트 목록에 대한 뮤텍스


void startApiServer() {
    crow::SimpleApp app;

    // 루트 엔드포인트 정의
    CROW_ROUTE(app, "/")([](){
        return "Hello world";
    });

    // JSON 응답을 반환하는 엔드포인트 정의
    CROW_ROUTE(app, "/json")([]{
        crow::json::wvalue x({{"message", "Hello, World!"}});
        x["message2"] = "Hello, World.. Again!";
        return x;
    });

    // 경로 파라미터를 사용하는 엔드포인트 정의
    CROW_ROUTE(app, "/hello/<string>")([](const std::string& name){
        std::ostringstream os;
        os << "Hello, " << name << "!";
        return crow::response(os.str());
    });

    // POST 요청을 처리하는 엔드포인트 정의
    CROW_ROUTE(app, "/add").methods(crow::HTTPMethod::Post)([](const crow::request& req){
        auto body = crow::json::load(req.body);
        if (!body || body.t() != crow::json::type::List) {
            return crow::response(400);
        }

        std::vector<std::string> failedUrls;
        for (const auto& item : body) {
            std::string url = item["url"].s();
            bool ret = service->AddRtsp(url);
            if (!ret) {
                failedUrls.push_back(url);
            }
        }

        crow::json::wvalue result;
        if (failedUrls.empty()) {
            result["result"] = "SUCCESS";
            result["message"] = "All url was added successfully";
        } else {
            result["result"] = "FAIL";
            std::ostringstream os;
            for (size_t i = 0; i < failedUrls.size(); ++i) {
                if (i > 0) os << ", ";
                os << "'" << failedUrls[i] << "'";
            }
            if (failedUrls.size() == 1) {
                result["message"] = os.str() + " was not added.";
            } else {
                result["message"] = os.str() + " were not added.";
            }
        }

        return crow::response(result);
    });

    CROW_ROUTE(app, "/remove").methods(crow::HTTPMethod::Post)([](const crow::request& req){
        auto body = crow::json::load(req.body);
        if (!body || body.t() != crow::json::type::List) {
            return crow::response(400);
        }

        std::vector<std::string> failedUrls;
        for (const auto& item : body) {
            std::string id = item["id"].s();
            bool ret = service->RemoveRtsp(id);
            if (!ret) {
                failedUrls.push_back(id);
            }
        }

        crow::json::wvalue result;
        if (failedUrls.empty()) {
            result["result"] = "SUCCESS";
            result["message"] = "All id was removed successfully";
        } else {
            result["result"] = "FAIL";
            std::ostringstream os;
            for (size_t i = 0; i < failedUrls.size(); ++i) {
                if (i > 0) os << ", ";
                os << "'" << failedUrls[i] << "'";
            }
            if (failedUrls.size() == 1) {
                result["message"] = os.str() + " was not added.";
            } else {
                result["message"] = os.str() + " were not added.";
            }
        }

        return crow::response(result);
    });

    // API 서버 시작
    app.port(8080).multithreaded().run();
}

int main(int argc, char *argv[]) {

    GOptionContext *optctx;
    GError *error = NULL;

    optctx = g_option_context_new("<launch line> - Test RTSP Server, Launch\n\n"
                                  "Example: \"( videotestsrc ! x264enc ! rtph264pay name=pay0 pt=96 )\"");
    g_option_context_add_main_entries(optctx, entries, NULL);
    g_option_context_add_group(optctx, gst_init_get_option_group());
    if (!g_option_context_parse(optctx, &argc, &argv, &error))
    {
        g_printerr("Error parsing options: %s\n", error->message);
        g_option_context_free(optctx);
        g_clear_error(&error);
        return -1;
    }
    g_option_context_free(optctx);

    // API 서버를 별도의 스레드에서 시작
    std::thread apiServerThread(startApiServer);
    apiServerThread.detach();

    service = std::make_shared<TranscodingService>();
    service->Initialize();
    service->StartServer();

    //// Server connects event for "client-connected"
    g_signal_connect(service->Server, "client-connected", G_CALLBACK(client_connected_callback), service.get());

    GstRTSPSessionPool *session_pool = gst_rtsp_server_get_session_pool(service->Server);
    g_object_set(session_pool, "max-sessions", 100, NULL);

    GMainContext *context = g_main_context_default();
    GSource *cleanup_source = gst_rtsp_session_pool_create_watch(session_pool);
    g_source_set_callback(cleanup_source, (GSourceFunc)session_cleanup, session_pool, NULL);
    g_source_attach(cleanup_source, context);

    GMainLoop *mainLoop = g_main_loop_new(context, FALSE);
    g_main_loop_run(mainLoop);
    
    service->StopServer();
    return 0;
}

void session_cleanup(GstRTSPSessionPool* pool) {
    guint removed_sessions = gst_rtsp_session_pool_cleanup(pool);
    g_print("Cleaned up %u sessions\n", removed_sessions);
}

// 클라이언트가 연결될 때 호출될 콜백 함수
void client_connected_callback(GstRTSPServer* server, GstRTSPClient* client, gpointer user_data) {
    try
    {
        std::lock_guard<std::recursive_mutex> lock(processMutex);
        if (client == nullptr) {
            gst_println("Received a null client pointer.\n");
            return;
        }
        // 현재 시간 가져오기
        std::ostringstream oss = getCurrentTime();


        // client-close 이벤트에 onClientClosed 콜백 연결
        g_signal_connect(client, "closed", G_CALLBACK(closed_callback ), user_data);
        g_signal_connect(client, "describe-request", G_CALLBACK(describe_request_callback ), user_data);
        g_signal_connect(client, "setup-request", G_CALLBACK(setup_request_callback ), user_data);
        g_signal_connect(client, "teardown-request", G_CALLBACK(teardown_request_callback ), user_data);

        // 클라이언트의 메모리 주소와 타입 크기 출력
        g_print("[%s]Client(%p) was connected to RTSP server!\n", oss.str().c_str(), client);
    }
    catch (const std::exception& e)
    {
        g_print("Exception in onClientConnected: %s\n", e.what());
    }
}

// TranscodingService.cpp 파일에 추가
void closed_callback(GstRTSPClient* client, gpointer user_data) {
    try {
        std::lock_guard<std::recursive_mutex> lock(processMutex);  // (수정) 스레드 안전성 확보
        /*g_signal_handler_disconnect(client, closed_handler_id);
        g_signal_handler_disconnect(client, describe_handler_id);*/

        // 현재 시간 가져오기
        std::ostringstream oss = getCurrentTime();

        // 500ms 딜레이 추가
        //std::this_thread::sleep_for(std::chrono::milliseconds(500));
        g_print("[%s]Client(%p) has closed the connection!\n", oss.str().c_str(), client);

        service->RemoveClientWithClient(client);
    }
    catch (const std::exception& e) {
        g_print("Exception in onClientClosed: %s\n", e.what());
    }
}

void describe_request_callback(GstRTSPClient* client, GstRTSPContext* context, gpointer user_data) {
    try
    {
        std::lock_guard<std::recursive_mutex> lock(processMutex);
        // 클라이언트의 메모리 주소와 타입 크기 출력
        const GstRTSPUrl* url = context->uri;
        // 현재 시간 가져오기
        std::ostringstream oss = getCurrentTime();
        if (url && url->abspath) {
            g_print("[%s]Client was connected to URL: %s\n", oss.str().c_str(), url->abspath);
            service->AddClientInfo(client, url->abspath);
            
        }
        else {
            g_print("[%s]Client can't be connected to URL.\n", oss.str().c_str());
        }
    }
    catch (const std::exception& e) {
        g_print("Exception in onClientDescribe: %s\n", e.what());
    }
}

void setup_request_callback(GstRTSPClient* client, GstRTSPContext* context) {
    try
    {
        std::lock_guard<std::recursive_mutex> lock(processMutex);
        // 세션 객체 얻기
        GstRTSPSession* session = context->session;
         // 현재 시간 가져오기
        std::ostringstream oss = getCurrentTime();
        if (session) {
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
    }
    catch (const std::exception& e) {
        g_print("Exception in onClientSetup: %s\n", e.what());
    }
}
void teardown_request_callback(GstRTSPClient* client, GstRTSPContext* context, gpointer user_data) {
    
    try
    {
        std::lock_guard<std::recursive_mutex> lock(processMutex);
         // 현재 시간 가져오기
        std::ostringstream oss = getCurrentTime();
        g_print("[%s] Session will be tear-downed.\n", oss.str().c_str());
    }
    catch (const std::exception& e) {
        g_print("Exception in onClientSetup: %s\n", e.what());
    }
}

void unrefGstClient(GstRTSPClient* client) {
    //std::lock_guard<std::recursive_mutex> lock(processMutex);  // (수정) 동기화 추가
    if (client != nullptr && G_IS_OBJECT(client)) {
        gst_object_unref(client);
    }
}
