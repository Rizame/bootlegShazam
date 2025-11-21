//
// Created by risame on 11/19/25.
//

#include "cppServer.h"
#include <fstream>
#include<filesystem>

void cppServer::setupRoutes() {
    svr.Post("/api/recognise", [this](const auto& req, auto& res) {
            this->recognizeSong(req, res);
        });

    svr.Post("/api/addSong", [this](const auto& req, auto& res) {
        this->addSong(req, res);
    });
}
void cppServer::start(int port) {
    setupRoutes();
    std::cout << "Starting server on port " << port << std::endl;
    PORT = port;
    svr.listen("localhost", port); 
}

void cppServer::addSong(const httplib::Request &req, httplib::Response &res) {
    res.set_header("Access-Control-Allow-Origin", "*");
    if (!req.form.has_file("audio")) {
        res.status = 400;
        res.set_content("No audio file A", "text/plain");
        return;
    }
    //this->recogniser.insert_one_song("test");
    std::cout<<"Adding song"<<std::endl;
}

void cppServer::recognizeSong(const httplib::Request &req, httplib::Response &res) {
    res.set_header("Access-Control-Allow-Origin", "*");

    if (!req.form.has_file("audio")) {
        res.status = 400;
        res.set_content("No audio file R", "text/plain");
        return;
    }
    std::cout<<"Recognising song"<<std::endl;
    const auto& audioFile = req.form.get_file("audio");
    std::cout << "Uploaded file: " << audioFile.filename
              << " (" << audioFile.content_type << ") - "
              << audioFile.content.size() << " bytes" << std::endl;

    std::string path = "frontTempAudio/";
    std::string fileName = audioFile.filename;
    std::filesystem::create_directories(path);

    std::ofstream ofs(path+fileName, std::ios::binary);
    ofs << audioFile.content;

    if (!ofs) {
        std::cout<<"Failed to open file"<<std::endl;
        res.status = 500;
        res.set_content("Failed to write file", "text/plain");
        return;
    }
    ofs.close();

    wav::Score result = this->recogniser.recognize_audio(fileName);

    std::cout<<"Recognized audio: "<<result.songName<<std::endl;
    std::string json_response;
    if (result.songId != -1) {
        json_response =
            "{\"status\":\"success\","
            "\"songId\":" + std::to_string(result.songId) + ","
            "\"songName\":\"" + result.songName + "\","
            "\"confidence\":" + std::to_string(result.score) + ","
            "\"offset\":" + std::to_string(result.offset) + ","
            "\"percentageMatch\":" + std::to_string(result.percentageMatch) + ","
            "\"message\":\"Song identified successfully\"}";
    } else {
        json_response =
            "{\"status\":\"no_match\","
            "\"songId\":-1,"
            "\"songName\":\"\","
            "\"confidence\":0,"
            "\"offset\":0,"
            "\"percentageMatch\":0,"
            "\"message\":\"No matching song found\"}";
    }


    if (remove((path+fileName).c_str()) == 0) {
        std::cout << "File deleted successfully." << std::endl;
    } else {
        std::cerr << "Error deleting file '" << fileName << "'." << std::endl;
    }

    res.set_content(json_response, "application/json");
}

