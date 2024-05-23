#include "TranscodingService.h"
#include <iostream>
#include <string>
#include "crow.h"
#include "datetime.h"
#include "CustomLogger.h" // CustomLogger 헤더 파일 포함

void unrefGstClient(GstRTSPClient* client);

std::shared_ptr<TranscodingService> service;  // (수정) 자동 정리를 위해 unique_ptr 사용
std::recursive_mutex  processMutex;  // 클라이언트 목록에 대한 뮤텍스


void startApiServer() {
    crow::SimpleApp app;

    // // 루트 엔드포인트 정의
    // CROW_ROUTE(app, "/")([](){
    //     return "Hello world";
    // });

    // // JSON 응답을 반환하는 엔드포인트 정의
    // CROW_ROUTE(app, "/json")([]{
    //     crow::json::wvalue x({{"message", "Hello, World!"}});
    //     x["message2"] = "Hello, World.. Again!";
    //     return x;
    // });

    // // 경로 파라미터를 사용하는 엔드포인트 정의
    // CROW_ROUTE(app, "/hello/<string>")([](const std::string& name){
    //     std::ostringstream os;
    //     os << "Hello, " << name << "!";
    //     return crow::response(os.str());
    // });

    // /status 경로에 대한 GET 요청을 처리하는 엔드포인트 정의
    CROW_ROUTE(app, "/status").methods(crow::HTTPMethod::Get)([](){
        crow::json::wvalue result;
        result["name"] = "Rtsp Server";
        result["version"] = "v1.0";
        result["status"] = "Active";
        return crow::response(result);
    });

    // POST 요청을 처리하는 엔드포인트 정의
    CROW_ROUTE(app, "/add").methods(crow::HTTPMethod::Post)([](const crow::request& req){
        auto body = crow::json::load(req.body);
        if (!body || body.t() != crow::json::type::Object || !body.has("rtsp_uris") || body["rtsp_uris"].t() != crow::json::type::List) {
            return crow::response(400);
        }

        std::vector<std::string> failedUrls;
        crow::json::wvalue result;
        crow::json::wvalue::list response_list;

         for (const auto& item : body["rtsp_uris"]) {
            if (item.t() != crow::json::type::Object || !item.has("id") || !item.has("url")) {
                return crow::response(400);
            }
            std::string id = item["id"].s();
            std::string url = item["url"].s();

            auto uriParts = service->AddRtsp(id, url);
            if (uriParts) {
                crow::json::wvalue uri_info;
                uri_info["id"] = id;
                uri_info["full_path"] = uriParts->full_path;
                uri_info["user"] = uriParts->username;
                uri_info["pass"] = uriParts->password;
                uri_info["ip"] = uriParts->ip;
                uri_info["port"] = uriParts->port;
                uri_info["path"] = uriParts->path;
                uri_info["ready_time"] = uriParts->ready_time;

                crow::json::wvalue mount_points;
                mount_points["uri_first"] = uriParts->mount_points.uri_first;
                mount_points["uri_second"] = uriParts->mount_points.uri_second;
                mount_points["uri_third"] = uriParts->mount_points.uri_third;
                mount_points["uri_fourth"] = uriParts->mount_points.uri_fourth;

                uri_info["mount_points"] = std::move(mount_points);
                response_list.push_back(std::move(uri_info));
            } else {
                failedUrls.push_back(url);
            }
        }

        result["list"] = std::move(response_list);
        result["count"] = static_cast<int>(response_list.size());

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
                if (!body || body.t() != crow::json::type::Object || !body.has("rtsp_ids") || body["rtsp_ids"].t() != crow::json::type::List) {
            return crow::response(400);
        }

        std::vector<std::string> failedUrls;
        for (const auto& item : body["rtsp_ids"]) {
            if (item.t() != crow::json::type::Object || !item.has("id")) {
                return crow::response(400);
            }
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

    // 클라이언트 세션 정보를 반환하는 엔드포인트 정의
    CROW_ROUTE(app, "/get-sessions").methods(crow::HTTPMethod::Get)([] {
        return service->GetClientSessions();
    });

    // 미디어 정보를 반환하는 엔드포인트 정의
    CROW_ROUTE(app, "/get-medias").methods(crow::HTTPMethod::Get)([] {
        return service->GetMedias();
    });

    // API 서버 시작
    app.port(8080).multithreaded().run();
}

int main(int argc, char *argv[]) {

    gst_init(nullptr, nullptr);

    // 디버그 레벨 설정
    gst_debug_set_default_threshold(GST_LEVEL_WARNING);

    // 커스텀 로거 설정
    auto logger = std::make_shared<CustomLogger>();
    crow::logger::setHandler(logger.get());

    // API 서버를 별도의 스레드에서 시작
    std::thread apiServerThread(startApiServer);
    apiServerThread.detach();

    service = std::make_shared<TranscodingService>();

    // 수신 및 송신 서버 파이프라인을 각각의 스레드에서 시작
    std::thread serverThread(&TranscodingService::StartServer, service);
    serverThread.detach();

    //// Server connects event for "client-connected"
    //g_signal_connect(service->Server, "client-connected", G_CALLBACK(client_connected_callback), service.get());

    // GstRTSPSessionPool *session_pool = gst_rtsp_server_get_session_pool(service->Server);
    // g_object_set(session_pool, "max-sessions", 100, NULL);

    // GMainContext *context = g_main_context_default();
    // GSource *cleanup_source = gst_rtsp_session_pool_create_watch(session_pool);
    // g_source_set_callback(cleanup_source, (GSourceFunc)session_cleanup, session_pool, NULL);
    // g_source_attach(cleanup_source, context);

    GMainLoop *mainLoop = g_main_loop_new(nullptr, FALSE);
    g_main_loop_run(mainLoop);
    
    service->StopServer();
    return 0;
}



void unrefGstClient(GstRTSPClient* client) {
    //std::lock_guard<std::recursive_mutex> lock(processMutex);  // (수정) 동기화 추가
    if (client != nullptr && G_IS_OBJECT(client)) {
        gst_object_unref(client);
    }
}
