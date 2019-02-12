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
#include "timeline_server.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::ServerReader;
using grpc::ServerReaderWriter;
using grpc::ServerWriter;
using grpc::Status;

using timeline::username;
using timeline::fufArgs;
using timeline:Reply;
using timeline::posts;
using timeline::ListUsers;
using timeline::Empty;
using std::chrono::system_clock;


class clientEntry {

    public:

    std::string clientName;     //clients name
    std::vector<std::string> followedList;      //who theyre following
    std::vector<posts> client_posts;            //list of their posts
    std::vector<posts> timeline;      //timeline

}

class timelineServiceImpl final : public timelineService:service {

    
    std::vector<clientEntry> clients;

    public:

        Status acceptConnections(username* username, Empty* response) override {       //great, we have a new user. So lets add it to clients. 

            new clientEntry *newClient; 

            newClient->clientName = username->user();
            clients.push_back(*newClient);

            return Status::OK;

        }

        Status follow(fufArgs* args, Empty * empty) override {          //allow user to follow another user

            if(args->clientName() == args->argName()) {  //someone is trying to follow themself... lets not waste time here...

                return Status::INVALID_ARGUMENT;

            }

            std::string clientName = args->clientName();
            std::string followName = args->argName();

            int j = 0;
            int client = -1;
            int follow = -1;


            for(j = 0; j < clients.size(); j++) {
                
                if(clientName == clients.at(j).clientName) {    //found index for client

                    client = j;

                }

                if(followName == clients.at(j).clientName) {
                //we found a user with the specified username. Now we need to link that username to user's followed list, and update timeline accordingly
                    follow = j;

                }

            }

            if(client == -1) {  //shouldn't happen, but means that we couldn't find the client making request in our database

                return Status::INVALID_ARGUMENT;

            }

            else if(follow == -1) { //we couldn't find the username of the user we're trying to follow in our database

                return Status::NOT_FOUND;

            }

            else {          //good, we found the client making the argument, and we found the user we're trying to follow

                clients.at(client).followedList.push_back(clients.at(follow).clientName); //a little messy looking, but we just add follow username to client followed users vector
                return Status::OK;

            }
            


        }

        Status unfollow(fufArgs * args, Empty * empty) override {

            if(args->clientName() == args->argName()) {       //someone is trying to unfollow themself...again, lets not waste time here...

                return Status::INVALID_ARGUMENT;

            }

            std::string clientName = args->clientName();
            std::string followName = args->argName();
            int j = 0;
            int client = -1;
            int unfollow = -1;

            for(j = 0; j < clients.size(); j++) {
                
                if(clientName == clients.at(j).clientName) {    //found index for client

                    client = j;

                }

                if(followName == clients.at(j).clientName) {
                //we found a user with the specified username. Now we need to remove username from followed list, and update timeline accordingly
                    unfollow = j;

                }

            }

            if(client == -1) {  //shouldn't happen, but means that we couldn't find the client making request in our database

                return Status::INVALID_ARGUMENT;

            }

            else if(unfollow == -1) { //we couldn't find the username of the user we're trying to unfollow in our database

                return Status::NOT_FOUND;

            }

            else {          //good, we found the client making the argument, and we found the user we're trying to unfollow ////////////////////////////////////////////////////////////////////

                clients.at(client).followedList.erase(unfollow); 
                
                return Status::OK;

            }



        }

        Status List(Empty * empty, ServerWriter<ListUsers> * writer) override {   //list out all the names of the users. Easy

            int j = 0;

            for(j = 0; j < clients.size(); j++) {

                ListUsers * newUser = new ListUsers;
                *newUser->set_user(clients.at(j));
                writer->Write(*user);
                delete user;

            }

            return Status::OK;

        }

        Status timeline(ServerReaderWriter<posts, posts> * stream) override {     //start the timeline loop. Accept new posts from user, and update their timeline with new posts from followed users
        
            std::vector<posts> received_posts;  //vector of received posts, might not end up needing
            std::string connected_client;       //name of our connected client running on timeline. Only need to get once from first received post
            int client_place = -1;              //place where our client is in our clients vector. Only need to get once from name on first post
            posts post;                         //posts objects for receiving the posts sent from client

            while(stream->Read(&post)) {

                if(client_place == -1) {    //This is our first loop, I.e. timeline connection was just established. Lets find the name and place of the client within our clients vector

                    connected_client = post.user(); //got our name, now lets find in our clients vector

                    int j = 0;

                    for(j = 0; j < clients.size(); j++) {   //basically just reused this search function from earlier
                
                        if(connected_client == clients.at(j).clientName) {    //found index for client

                            client_place = j;

                        }

                    }

                    if(client_place == -1) {    //error, we couldn't find our client in our client vector. Shouldn't happen but here we are

                        return Status::FAILED_PRECONDITION;

                    }

                }

                //Ok, now I think we need to check the post, and update all clients timelines. First lets start with connected client. Really hope this is threadsafe...

                clients.at(client_place).client_posts.push_back(post);
                clients.at(client_place).timeline.push_back(post);

                int j = 0;
                for(j = 0; j < clients.size(); j++){

                    int k = 0;
                    for(k = 0; k < clients.at(j).followedList.size(); k ++){

                        if(clients.at(j).followedList.at(k) == connected_client){

                            clients.at(j).timeline.push_back(post);

                        }

                    }

                }

            }

        }

}

void runServer(const std::string host, const std::string port) {

    std::string serverAddress = host + port;
    timelineServiceImpl service;

    ServerBuilder builder;
    builder.AddListeningPort(serverAddress, grpc::InsecureServerCredentials);
    builder.RegisterService(&service);
    std::unique_ptr<Server> server(builder.BuildAndStart());
    std::cout << "Server listening on " << serverAddress << std::endl;
    server->wait();

}