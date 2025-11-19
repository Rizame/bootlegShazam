//
// Created by risame on 11/19/25.
//
#include "httplib.h"
#include "recogniser.h"

#ifndef SHAZAM_CPPSERVER_H
#define SHAZAM_CPPSERVER_H




class cppServer {
    private:
        Recogniser recogniser;
        httplib::Server svr;
        bool running = false;
        int PORT = 0;
        void recognizeSong(const httplib::Request& req, httplib::Response& res);
        void addSong(const httplib::Request& req, httplib::Response& res);

    public:
        void setupRoutes();
        void start(int port = 8080);
        ~cppServer() {
                running = false;
                svr.stop();
        }

};


#endif //SHAZAM_CPPSERVER_H