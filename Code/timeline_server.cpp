#include <algorithm>
#include <chrono>
#include <cmath>
#include <iostream>
#include <memory>
#include <string>

#include <grpc/grpc.h>
#include <grpc++/server.h>
#include <grpc++/server_builder.h>
#include <grpc++/server_context.h>
#include <grpc++/security/server_credentials.h>
//#include "helper.h"
#include "timeline.grpc.pb.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::ServerReader;
using grpc::ServerReaderWriter;
using grpc::ServerWriter;
using grpc::Status;

using timeline::username;
using timeline::fufArgs;
using timeline::Reply;
using timeline::posts;
using timeline::ListUsers;
using timeline::Empty;


using std::chrono::system_clock;



class clientEntry {
    public:
    std::string clientName;                     //clients name
    std::vector<std::string> followedList;      //who theyre following
    std::vector<posts> client_posts;            //list of their posts
    std::vector<posts> timeline;                //timeline
};

class timelineImpl final : public timeline::Service {
    public:
        std::vector<clientEntry> clients;

        //FOLLOW <username>: Follows the named user's timeline
        Status follow(ServerContext* context, const fufArgs* args, Reply* response) override {
            //Prepping variables
            std::string clientName = args->get_clientName();
            std::string followName = args->argName();

            //Someone is trying to follow themself??
            if(clientName == followName)
                return Status::INVALID_ARGUMENT;
            
            //Find indexes
            int j = 0, client = -1, follow = -1;
            for(j = 0; j < clients.size(); j++) {
                if(clientName == clients.at(j).clientName)    
                    client = j; //found index for client
                if(followName == clients.at(j).clientName)
                    follow = j; //found index for follower
            }

            //Do the follow here
            if(client == -1) // couldn't find the client in database
                return Status::INVALID_ARGUMENT; 
            else if(follow == -1) // couldn't find the follower in database
                return Status::NOT_FOUND; 
            else //add follow username to client followed users vector
                clients.at(client).followedList.push_back(clients.at(follow).clientName); 
            return Status::OK;

        }

        ::grpc::Status unfollow(::grpc::ServerContext* context, const ::fufArgs* request, ::Reply* response) override {
           /* std::string requestedUsername = request->user();
            if(std::find(userNameList.begin(), userNameList.end(), request->user()) != userNameList.end()) {
                //TODO: disconnect from the user timeline
                for (int i = 0; i < userTimeline.size(); i++){
                    for (int j = 0; j < userTimeline.size(); j++){
                        if(userTimeline[i][j] == requestedUsername){
                            userTimeline[i][j].erase();
                            break;
                        }    
                    }
                }
                response->set_status(1);
            }
            else{
                //not found
                response->set_status(0);
            }*/
            return ::grpc::Status::OK;
        }

        ::grpc::Status list(::grpc::ServerContext* context, const ::Empty* request, ::grpc::ServerWriter< ::ListUsers>* writer) override {
            ListUsers usr;
            std::cout<<"usersend:"<<userNameList[0]<<std::endl;
            usr.set_user(userNameList[0]);
            writer->Write(usr);
            return ::grpc::Status::OK;
        }

        ::grpc::Status timeline(::grpc::ServerContext* context, ::grpc::ServerReaderWriter< ::posts, ::posts>* stream) override {
            //find if the user is in the list, if not add

            std::cout<<"I'm in timeline"<<std::endl;
            ::posts po;
            stream->Read(&po);
            std::cout<<po.user()<<std::endl;
            std::cout<<po.time()<<std::endl;
            std::cout<<po.post()<<std::endl;
            userNameList.push_back(po.user());

            /*
	    if(std::find(userNameList.begin(), userNameList.end(), po->user()) != userNameList.end()) {
                //response->set_status(1);
            }
            else{
		//create a timeline instance for the user
                //response->set_status(0);
		userNameList.push_back(po.user());
            }*/
	   

            //save post to file
            return ::grpc::Status::OK;
        }
};

void runServer(const std::string host, const std::string port) {

    std::string serverAddress = host + ":" + port;
    timelineImpl service;

    //Read jason file
    ServerBuilder builder;
    builder.AddListeningPort(serverAddress, grpc::InsecureServerCredentials());
    builder.RegisterService(&service);
    std::unique_ptr<Server> server(builder.BuildAndStart());
    std::cout << "Server listening on " << serverAddress << std::endl;
    server->Wait();

}

int main(int argc, char** argv) {
	runServer("localhost","3010");
return 0;
}
