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
/*
using timeline::username;
using timeline::fufArgs;
using timeline::Reply;
using timeline::posts;
using timeline::ListUsers;
using timeline::Empty;
*/

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

        //Used for connectto, begins a timeline for all users
        ::grpc::Status acceptConnections(::grpc::ClientContext* context, const ::username& request, ::Reply* response) override {       
            //great, we have a new user. So lets add it to clients. 
            new clientEntry *newClient; 
            newClient->clientName = username->user();
            clients.push_back(*newClient);
            response->set_status(0);
            return Status::OK;

        }

        //FOLLOW <username>: Follows the named user's timeline
        ::grpc::Status follow(::grpc::ServerContext* context, const ::fufArgs* args, ::Reply* response) override {
            //Prepping variables
            std::string clientName = args->clientname();
            std::string followName = args->argname();

            //Someone is trying to follow themself??
            if(clientName == followName){
                response->set_status(-3);
                return Status::OK;
            }
            
            //Find indexes
            int j = 0, client = -1, follow = -1;
            for(j = 0; j < clients.size(); j++) {
                if(clientName == clients.at(j).clientName)    
                    client = j; //found index for client
                if(followName == clients.at(j).clientName)
                    follow = j; //found index for follower
            }

            //Do the follow here
            if(client == -1){ // couldn't find the client in database
                response->set_status(-1);
                return Status::OK;
            }
            else if(follow == -1){ // couldn't find the follower in database
                response->set_status(-1);
                return Status::OK;
            }
            else //add follow username to client followed users vector
                clients.at(client).followedList.push_back(clients.at(follow).clientName); 

            //Success!
            response->set_status(0);
            return Status::OK;

        }

        //UNFOLLOW <username>: Unfollows the named user's timeline
        ::grpc::Status unfollow(::grpc::ServerContext* context, const ::fufArgs* args, ::Reply* response) override {
            //Prepping variables
            std::string clientName = args->clientname();
            std::string followName = args->argname();

            //Someone is trying to follow themself??
            if(clientName == followName){
                response->set_status(-3);
                return Status::OK;
            }

            //Find indexes
            int j = 0, client = -1, follow = -1;
            for(j = 0; j < clients.size(); j++) {
                if(clientName == clients.at(j).clientName)    
                    client = j; //found index for client
                if(followName == clients.at(j).clientName)
                    follow = j; //found index for follower
            }

            //Do the unfollow here
            if(client == -1){ // couldn't find the client in database
                response->set_status(-1);
                return Status::OK;
            }
            else if(follow == -1){ // couldn't find the follower in database
                response->set_status(-1); 
                return Status::OK;
            }
            else //delete from followed users vector
                clients.at(client).followedList.erase(unfollow); 

            //Success!
            response->set_status(0);
            return Status::OK;

        }

        //::grpc::Status list(::grpc::ServerContext* context, const ::Empty* request, ::grpc::ServerWriter< ::ListUsers>* writer) override {
        ::grpc::Status list(::grpc::ServerContext* context, const ::Empty* request, ::ListUsers* response) override {
            //TODO need to display all the followers as well
            std::vector<std::vector<std::string>> followedList;

            for(int j = 0; j < clients.size(); j++) {
                //followedList[j].push_back(clients[j]);
            }
            return Status::OK;

        }

        ::grpc::Status timeline(::grpc::ServerContext* context, ::grpc::ServerReaderWriter< ::posts, ::posts>* stream) override {
            //TODO make this shit

            std::cout<<"I'm in timeline"<<std::endl;
            ::posts po;
            stream->Read(&po);
            std::cout<<po.user()<<std::endl;
            std::cout<<po.time()<<std::endl;
            std::cout<<po.post()<<std::endl;
            //userNameList.push_back(po.user());

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
