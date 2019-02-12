#include <iostream>
//#include <memory>
//#include <thread>

#include <vector>
#include <string>
#include <sstream>

#include <unistd.h>

#include <grpc/grpc.h>
#include <grpc++/channel.h>
#include <grpc++/client_context.h>
#include <grpc++/create_channel.h>
#include <grpc++/security/credentials.h>

//#include "helper.h"

#include "timeline_client.h"
#include "timeline.grpc.pb.h"

#include <chrono>
#include <ctime>

using grpc::Channel;
using grpc::ClientContext;
using grpc::ClientReader;
using grpc::ClientReaderWriter;
using grpc::ClientWriter;
using grpc::Status;

class Client : public IClient
{
    public:
        Client(const std::string& hname,
            const std::string& uname,
            const std::string& p)
            :hostname(hname), myUsername(uname), port(p){}
    protected:
        virtual int connectTo();
        virtual IReply processCommand(std::string& input);
        virtual void processTimeline();
    private:
        std::string hostname;
        std::string myUsername;
        std::string port;
        
        // You can have an instance of the client stub
        // as a member variable.
        std::unique_ptr<timeline::Stub> stub_;
};

int main(int argc, char** argv) {

    std::string hostname = "localhost";
    std::string myUsername = "default";
    std::string port = "3010";
    int opt = 0;
    while ((opt = getopt(argc, argv, "h:u:p:")) != -1){
        switch(opt) {
            case 'h':
                hostname = optarg;break;
            case 'u':
                myUsername = optarg;break;
            case 'p':
                port = optarg;break;
            default:
                std::cerr << "Invalid Command Line Argument\n";
        }
    }

    Client myc(hostname, myUsername, port);
    // You MUST invoke "run_client" function to start business logic
    myc.run_client();

    return 0;
}

int Client::connectTo()
{
    std::string hosting = this->hostname + ":" + this->port;
    stub_ = timeline::NewStub(grpc::CreateChannel(hosting, grpc::InsecureChannelCredentials()));

    ClientContext context;
    username usr;
    Reply reply;

    usr.set_user(this->myUsername);
    grpc::Status status = stub_->acceptConnections(&context, usr, &reply);

    //Set up default message
    //init_post.set_user(myUsername);	
    //init_post.set_time(std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count());
    //init_post.set_post("timeline created");

    //Writes the post there
    //std::shared_ptr<ClientReaderWriter<::posts, ::posts>> swr(stub_->timeline(&context));
    //swr->Write(init_post);
    //swr->WritesDone();

    return 1; // return 1 if success, otherwise return -1
}

IReply Client::processCommand(std::string& input)
{
    //Split tokens into the vector
    std::string buf;
    std::stringstream ss(input);
    std::vector<std::string> tokens;
    while (ss >> buf){
        tokens.push_back(buf);
    }

    //Set up variables here
    ClientContext context;    //Client that's being sent replies
    grpc::Status status;      //Success or Fail	
    Reply reply;              //Some possible reply
    fufArgs usrs;             //Username1 and Username2
    IReply ire;               //Their custom reply
	
    if(strstr(tokens[0].c_str(),"FOLLOW")) {
        usrs.set_clientname(this->myUsername);
        usrs.set_argname(tokens[1]);
        status = stub_->follow(&context, usrs, &reply);
        //std::cout<<"reply:"<<reply.status();
    }
    else if(strstr(tokens[0].c_str(),"UNFOLLOW")) {
        usrs.set_clientname(this->myUsername);
        usrs.set_argname(tokens[1]);
        status = stub_->follow(&context, usrs, &reply);
        //std::cout<<"reply:"<<reply.status();
    }
    else if(strstr(tokens[0].c_str(),"LIST")) {
        ListUsers list;
        Empty empty;
        status = stub_->list(&context, empty, &list);
        //std::cout<<"list:"<<std::endl;
        //std::unique_ptr<ClientReader<ListUsers> > reader(stub_->list(&context,empty));
        //while (reader->Read(&usr)) {           
        //    std::cout <<usr.user()<< std::endl;
        //    ire.all_users.push_back(usr.user());
        //}
        //Status status = reader->Finish();
    }
    else if(strstr(tokens[0].c_str(),"TIMELINE")) {
        //processtimeline()
    }
    else {}
    // ------------------------------------------------------------
    // GUIDE 2:
    // Then, you should create a variable of IReply structure
    // provided by the client.h and initialize it according to
    // the result. Finally you can finish this function by returning
    // the IReply.
    // ------------------------------------------------------------
    
    // ------------------------------------------------------------
    // HINT: How to set the IReply?
    // Suppose you have "Join" service method for JOIN command,
    // IReply can be set as follow:
    // 
    //     // some codes for creating/initializing parameters for
    //     // service method
    //     IReply ire;
    //     grpc::Status status = stub_->Join(&context, /* some parameters */);
    //     ire.grpc_status = status;
    //     if (status.ok()) {
    //         ire.comm_status = SUCCESS;
    //     } else {
    //         ire.comm_status = FAILURE_NOT_EXISTS;
    //     }
    //      
    //      return ire;
    // 
    // IMPORTANT: 
    // For the command "LIST", you should set both "all_users" and 
    // "following_users" member variable of IReply.
    // ------------------------------------------------------------
    
   
    ire.grpc_status = status;
    if (status.ok())
        ire.comm_status = SUCCESS;
    else
        ire.comm_status = FAILURE_NOT_EXISTS;

    return ire;
}

void Client::processTimeline()
{
	// ------------------------------------------------------------
    // In this function, you are supposed to get into timeline mode.
    // You may need to call a service method to communicate with
    // the server. Use getPostMessage/displayPostMessage functions
    // for both getting and displaying messages in timeline mode.
    // You should use them as you did in hw1.
	// ------------------------------------------------------------

    // ------------------------------------------------------------
    // IMPORTANT NOTICE:
    //
    // Once a user enter to timeline mode , there is no way
    // to command mode. You don't have to worry about this situation,
    // and you can terminate the client program by pressing
    // CTRL-C (SIGINT)
	// ------------------------------------------------------------
}
