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

class timelineServiceImpl final : public timelineService:service {

    public:

        Status follow(std::string username) {

            

        }

        Status unfollow(std::string username) {



        }

        Status List(empty arg) {



        }

        Status timeline(posts userPosts) {



        }

        private: 

        std::vector<std::string> userNameList;
        std::vector<std::vector<string>> userTimelines;


}

void runServer(const std::string host, const std::string port) {



}