#include <glib.h>
#include "headers/UriParts.h"
#include "headers/TranscodingService.h"

TranscodingService::TranscodingService() {
    gst_init(nullptr, nullptr);
    Server = gst_rtsp_server_new();
    mainLoop = g_main_loop_new(nullptr, FALSE);
}

TranscodingService::~TranscodingService() {
    if (serverThread.joinable())
        serverThread.join();
    g_object_unref(Server);
    g_main_loop_unref(mainLoop);
}

void TranscodingService::Initialize() {

    g_object_set(Server, "service", "8555", NULL);
    gst_rtsp_server_attach(Server, nullptr);
    g_print("RTSP Server set IP:8555\n");
}

void TranscodingService::StartServer() {
    gst_println("RTSPServer StartServer");
    if (!serverThread.joinable()) {
        // Server Thread Run
        serverThread = std::thread([this]() { g_main_loop_run(mainLoop); });
    }
}

void TranscodingService::StopServer() {
    /*rtspServer->StopServer();
    delete rtspServer;*/

    g_main_loop_quit(mainLoop);
    if (serverThread.joinable()) {
        serverThread.join();
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
        UriParts uriParts;
        uriParts.parseUri(uri);

        GstRTSPMountPoints* mounts = gst_rtsp_server_get_mount_points(Server);
        if (!mounts) {
            std::cerr << "Failed to get mount points." << std::endl;
            return false;
        }

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


        GstRTSPMediaFactory* firstFactory = gst_rtsp_media_factory_new();
        //latency=0 ! rtph264depay ! rtph264pay name=pay0 pt=96
        //latency=500 ! rtph264depay ! h264parse ! nvh264dec ! videoconvert ! queue ! x264enc ! rtph264pay config-interval=1 name=pay0 pt=96
        gst_rtsp_media_factory_set_launch(firstFactory, ("( rtspsrc location=" + uri + " latency=500 ! rtph264depay ! rtph264pay name=pay0 pt=96 )").c_str());
        gst_rtsp_media_factory_set_shared(firstFactory, TRUE);

        GstRTSPMediaFactory* secondFactory = gst_rtsp_media_factory_new();
        gst_rtsp_media_factory_set_launch(secondFactory, ("( rtspsrc location=" + uri + " latency=500 ! rtph264depay ! h264parse ! avdec_h264 ! videoconvert ! queue ! videoscale ! videorate ! video/x-raw,framerate=15/1,width=1280,height=960 ! x264enc bitrate=2000 speed-preset=ultrafast tune=zerolatency cabac=true ! rtph264pay config-interval=1 name=pay0 pt=96 )").c_str());
        gst_rtsp_media_factory_set_shared(secondFactory, TRUE);

        GstRTSPMediaFactory* thirdFactory = gst_rtsp_media_factory_new();
        gst_rtsp_media_factory_set_launch(thirdFactory, ("( rtspsrc location=" + uri + " latency=500 ! rtph264depay ! h264parse ! avdec_h264 ! videoconvert ! queue ! videoscale ! videorate ! video/x-raw,framerate=10/1,width=640,height=480 ! x264enc bitrate=800 speed-preset=ultrafast tune=zerolatency cabac=true ! rtph264pay config-interval=1 name=pay0 pt=96 )").c_str());
        gst_rtsp_media_factory_set_shared(thirdFactory, TRUE);

        GstRTSPMediaFactory* forthFactory = gst_rtsp_media_factory_new();
        gst_rtsp_media_factory_set_launch(forthFactory, ("( rtspsrc location=" + uri + " latency=500 ! rtph264depay ! h264parse ! avdec_h264 ! videoconvert ! queue ! videoscale ! videorate ! video/x-raw,framerate=10/1,width=320,height=240 ! x264enc bitrate=400 speed-preset=ultrafast tune=zerolatency cabac=true ! rtph264pay config-interval=1 name=pay0 pt=96 )").c_str());
        gst_rtsp_media_factory_set_shared(forthFactory, TRUE);

        // g_print("RTSP mount points : %s\n", ("/" + uriParts.id + "/first").c_str());
        // g_print("RTSP mount points : %s\n", ("/" + uriParts.id + "/second").c_str());
        // g_print("RTSP mount points : %s\n", ("/" + uriParts.id + "/third").c_str());
        // g_print("RTSP mount points : %s\n", ("/" + uriParts.id + "/fourth").c_str());
        // gst_rtsp_mount_points_add_factory(mounts, ("/" + uriParts.id + "/first").c_str(), firstFactory);
        // gst_rtsp_mount_points_add_factory(mounts, ("/" + uriParts.id + "/second").c_str(), secondFactory);
        // gst_rtsp_mount_points_add_factory(mounts, ("/" + uriParts.id + "/third").c_str(), thirdFactory);
        // gst_rtsp_mount_points_add_factory(mounts, ("/" + uriParts.id + "/fourth").c_str(), forthFactory);

        g_print("RTSP mount points : %s\n", firstMount.c_str());
        g_print("RTSP mount points : %s\n", secondMount.c_str());
        g_print("RTSP mount points : %s\n", thirdMount.c_str());
        g_print("RTSP mount points : %s\n", fourthMount.c_str());

        gst_rtsp_mount_points_add_factory(mounts, firstMount.c_str(), firstFactory);
        mediaFactories[firstMount] = firstFactory;
        gst_rtsp_mount_points_add_factory(mounts, secondMount.c_str(), secondFactory);
        mediaFactories[secondMount] = secondFactory;
        gst_rtsp_mount_points_add_factory(mounts, thirdMount.c_str(), thirdFactory);
        mediaFactories[thirdMount] = thirdFactory;
        gst_rtsp_mount_points_add_factory(mounts, fourthMount.c_str(), forthFactory);
        mediaFactories[fourthMount] = forthFactory;
        g_object_unref(mounts);

        g_print("Added RTSP stream: %s\n", uri.c_str());

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

        // gst_rtsp_mount_points_remove_factory(mounts, firstMount.c_str());
        // gst_rtsp_mount_points_remove_factory(mounts, secondMount.c_str());
        // gst_rtsp_mount_points_remove_factory(mounts, thirdMount.c_str());
        // gst_rtsp_mount_points_remove_factory(mounts, fourthMount.c_str());

        g_object_unref(mounts);
        g_print("Removed RTSP stream: %s\n", id.c_str());

        RemoveClientWithId(id);
        return true;
    }
    catch (const std::exception& e)
    {
        g_print("Exception in RemoveRtsp: %s\n", e.what());
        return false;
    }
    
}

void TranscodingService::AddClientInfo(GstRTSPClient* client, const std::string& uri) {
    std::lock_guard<std::recursive_mutex> lock(processMutex);
    clientInfos.push_back(std::make_shared<ClientInfo>(client, uri));
    g_print("Current number of clients: %ld\n", clientInfos.size());
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
                //gst_object_unref(client);
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
                GstRTSPResult ret = gst_rtsp_connection_close(conn);

                if(ret == 0)
                    g_print("Client was disconnected : %s\n", "OK");
                
                //gst_object_unref((*it)->client);
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
