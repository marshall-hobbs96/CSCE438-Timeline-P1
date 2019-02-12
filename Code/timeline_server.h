#include <algorithm>
#include <chrono>
#include <cmath>
#include <iostream>
#include <memory>
#include <string>

#include <grpc/grpc.h>
#include <grpcpp/server.h>
#include <grpcpp/server_builder.h>
#include <grpcpp/server_context.h>
#include <grpcpp/security/server_credentials.h>
#include "helper.h"
#include "route_guide.grpc.pb.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::ServerReader;
using grpc::ServerReaderWriter;
using grpc::ServerWriter;
using grpc::Status;
using routeguide::Point;
using routeguide::Feature;
using routeguide::Rectangle;
using routeguide::RouteSummary;
using routeguide::RouteNote;
using routeguide::RouteGuide;
using std::chrono::system_clock;

class post {

    public:
    int time;
    std::string name;
    std::string content;

}

class clientEntry {

    public:

    std::string clientName;     //clients name
    std::vector<std::string> followedList;      //who theyre following
    std::vector<post> posts;            //list of their posts
    std::vector<std::string> timeline;      //timeline

}

class timelineServiceImpl final : public timelineService:service {

    std::vector<clientEntry> clients;

    public:

        Status acceptConnections(username* username, Empty* response) {



        }

        Status follow(fufArgs * args, Empty * empty) {

            

        }

        Status unfollow(fufArgs * args, Empty * empty) {



        }

        Status List(Empty * empty, ServerWriter<ListUsers> * writer) {



        }

        Status timeline(ServerReaderWriter<posts> * stream) {

            

        }


}

void runServer(const std::string host, const std::string port) {



}