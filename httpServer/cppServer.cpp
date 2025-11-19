//
// Created by risame on 11/19/25.
//

#include "cppServer.h"

void cppServer::setupRoutes() {
    svr.Post("/identify", [this](const auto& req, auto& res) {
            this->recognizeSong(req, res);
        });

    svr.Post("/addSong", [this](const auto& req, auto& res) {
        this->addSong(req, res);
    });
}
void cppServer::start(int port) {
    setupRoutes();
    std::cout << "Starting server on port " << port << std::endl;
    running = true;
    PORT = port;
    svr.listen("localhost", port); 
}

void cppServer::addSong(const httplib::Request &req, httplib::Response &res) {
    res.set_header("Access-Control-Allow-Origin", "*");
    if (!req.form.has_file("audio")) {
        res.status = 400;
        res.set_content("No audio file", "text/plain");
        return;
    }
    this->recogniser.insert_one_song("test");
}

void cppServer::recognizeSong(const httplib::Request &req, httplib::Response &res) {
    res.set_header("Access-Control-Allow-Origin", "*");

    if (!req.form.has_file("audio")) {
        res.status = 400;
        res.set_content("No audio file", "text/plain");
        return;
    }
    this->recogniser.recognize_audio("test");
}

