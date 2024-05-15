
#include "TranscodingService.h"
#include <iostream>
#include <string>
#include "crow.h"
#include "datetime.h"

void onClientConnected(GstRTSPServer* server, GstRTSPClient* client, gpointer user_data);
void onClientDescribe(GstRTSPClient* client, GstRTSPContext* context, gpointer user_data);
void onClientClosed(GstRTSPClient* client, gpointer user_data);
void unrefGstClient(GstRTSPClient* client);


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

int main() {
    // API 서버를 별도의 스레드에서 시작
    std::thread apiServerThread(startApiServer);
    apiServerThread.detach();

    service = std::make_shared<TranscodingService>();
    service->Initialize();
    service->StartServer();


    //// Server connects event for "client-connected"
    g_signal_connect(service->Server, "client-connected", G_CALLBACK(onClientConnected), service->Server);

    while (true) {
        std::cout << "Enter Menu to add, delete or quit: ";
        std::string input;
        std::getline(std::cin, input);

        if (input == "q") {
            std::cout << "Quitting..." << std::endl;
            break;
        }
        else if(input == "a") {
            try {
                std::cout << "Input Rtsp Url: ";
                std::string input;
                std::getline(std::cin, input);

                bool ret = service->AddRtsp(input);
                std::cout << "RTSP URL added: " << ret << std::endl;
            }
            catch (const std::exception& ex) {
                std::cerr << "Error adding RTSP URL: " << ex.what() << std::endl;
            }
        }
        else if (input == "d") {
            try {
                std::cout << "Remove Id: ";
                std::string input;
                std::getline(std::cin, input);

                bool ret = service->RemoveRtsp(input);
                std::cout << "RTSP URL removed: " << ret << std::endl;
            }
            catch (const std::exception& ex) {
                std::cerr << "Error removing RTSP URL: " << ex.what() << std::endl;
            }
        }
        else 
        {
            std::cout << "Input value was not valid." << std::endl;
        }
    }

    service->StopServer();
    return 0;
}

// 클라이언트가 연결될 때 호출될 콜백 함수
void onClientConnected(GstRTSPServer* server, GstRTSPClient* client, gpointer user_data) {
    try
    {
        std::lock_guard<std::recursive_mutex> lock(processMutex);
        if (client == nullptr) {
            gst_println("Received a null client pointer.\n");
            return;
        }
        // 500ms 딜레이 추가
        //std::this_thread::sleep_for(std::chrono::milliseconds(500));
        
        // 현재 시간 가져오기
        std::ostringstream oss = getCurrentTime();


        // client-close 이벤트에 onClientClosed 콜백 연결
        g_signal_connect(client, "closed", G_CALLBACK(onClientClosed), NULL);
        g_signal_connect(client, "describe-request", G_CALLBACK(onClientDescribe), NULL);

        // 클라이언트의 메모리 주소와 타입 크기 출력
        g_print("[%s]Client(%p) was connected to RTSP server!\n", oss.str().c_str(), client);

        // 클라이언트 연결을 별도의 스레드에서 처리
        std::thread([client]() {
            GMainContext *context = g_main_context_new();
            GMainLoop *clientLoop = g_main_loop_new(context, FALSE);
            g_main_context_push_thread_default(context);
            g_main_loop_run(clientLoop);
            g_main_context_pop_thread_default(context);
            g_main_loop_unref(clientLoop);
            g_main_context_unref(context);
        }).detach();
    }
    catch (const std::exception& e)
    {
        g_print("Exception in onClientConnected: %s\n", e.what());
    }
}

// TranscodingService.cpp 파일에 추가
void onClientClosed(GstRTSPClient* client, gpointer user_data) {
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

void onClientDescribe(GstRTSPClient* client, GstRTSPContext* context, gpointer user_data) {
    
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

void unrefGstClient(GstRTSPClient* client) {
    //std::lock_guard<std::recursive_mutex> lock(processMutex);  // (수정) 동기화 추가
    if (client != nullptr && G_IS_OBJECT(client)) {
        gst_object_unref(client);
    }
}
