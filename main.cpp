#include "httpServer/cppServer.h"
#include "include/recogniser.h"

int main() {
    //Test usage without a server.
    //
    // Recogniser test;
    // test.recognize_audio("chebaClip");

    /// records could not get printed to the console as a final valid match due to small matching %.
    /// though if you take a look at the statistics amongst all candidate song the actual song you are looking for will have highest match of them all and probably a valid offset.

    cppServer server;
    server.start();

    return 0;
}
