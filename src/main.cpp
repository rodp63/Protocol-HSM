#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>

#include <iostream>
#include <list>
#include <vector>
#include <map>
#include <memory>

#include "HSMP.hpp"

typedef void (*function_ptr)();
std::map<std::string, function_ptr> hsmp_commands;

// first initial user to test
auto firsts_users = {User("127.0.0.1","Joaquin","gaa"),User("127.0.0.1","Mateo","gee"),User("127.0.0.1","Miguel","gii"),};
auto users = std::make_shared<std::list<User>>(firsts_users);

// TODO
// Way to identify who is send request because some of response is necessary

void SendRequest(char *buffer) {
  int sock;
  char send_data[1024];
  struct sockaddr_in server_addr;
  struct hostent *host;
  int n;

  //host = (struct hostent *)gethostbyname((char *)"34.74.78.79"); // Mateo's host
  host = (struct hostent *)gethostbyname((char *)"127.0.0.1");
  if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
  {
    perror("socket");
    exit(1);
  }

  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(5000);
  server_addr.sin_addr = *((struct in_addr *)host->h_addr);
  bzero(&(server_addr.sin_zero), 8);

  n = sendto(sock, buffer, strlen(buffer), 0, (struct sockaddr *)&server_addr, sizeof(struct sockaddr));

}

void client(){

  // One thread client -> server
  auto req = std::shared_ptr<HSMP::ClientRequest>();
  req = HSMP::CreateRequest();
  char *buffer_ready_to_send = req->ParseToCharBuffer();
  SendRequest(buffer_ready_to_send);
  
  // Other thread async server -> client
  //std::shared_ptr<HSMP::ServerResponse> res;
  //res = HSMP::ProcessResponse("B01003HolaATodosLee");
  //res->PrintStructure();

}

void server() {

  int sock;
  ssize_t bytes_read, len = 1024;
  socklen_t *addr_len;
  struct sockaddr_in server_addr, client_addr;

  if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
    perror("Socket");
    exit(1);
  }

  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(5000);
  server_addr.sin_addr.s_addr = INADDR_ANY;
  bzero(&(server_addr.sin_zero), 8);

  if (bind(sock, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) == -1) {
    perror("Bind");
    exit(1);
  }

  char *recv_data = new char[9999];

  printf("\n HSMP Waiting for client");
  fflush(stdout);

  std::shared_ptr<HSMP::ClientRequest> req;

  while (1) {
    bytes_read = recvfrom(sock, recv_data, 1024, 0, (struct sockaddr *)&client_addr, addr_len);
    recv_data[bytes_read] = '\0';
    printf("\n(%s , %d) said : ", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
    
    // HSMP SERVER working
    req = HSMP::ProcessRequest(sock);
    req->PrintStructure();
    // TODO
    // We need something that prevent wrong request
    // req->ProcessThisRequest(users); // maybe is necessary pass who is the request doing, do you need it? me(Miguel) yes.
    // HSMP SERVER working

    bzero(recv_data,9999);
    fflush(stdout);
  }
}

void PrintHelp() {
  std::cout << "USE ./hsmp server or ./hsmp client\n";
}

int main(int argc, char *argv[])
{
  hsmp_commands["client"] = &client;
  hsmp_commands["server"] = &server;

  if(argc == 1) {
    PrintHelp();
  }
  else if(argc == 2){
    std::string command = argv[1];
    if (hsmp_commands.count(command)) {
      (*hsmp_commands[command])();
    }
    else {
      PrintHelp();
    }
  }
  else {
    PrintHelp();
  }
  return 0;

  // previous main
  /*
  std::shared_ptr<HSMP::ClientRequest> req;

  req = HSMP::ProcessRequest("l1104santistebanucsp");
  req->PrintStructure();

  req = HSMP::ProcessRequest("i");
  req->PrintStructure();

  req = HSMP::ProcessRequest("m00505hellomateo");
  req->PrintStructure();

  req = HSMP::ProcessRequest("f05mateo");
  req->PrintStructure();

  req = HSMP::ProcessRequest("x");
  req->PrintStructure();

  req = HSMP::ProcessRequest("b010HolaATodos");
  req->PrintStructure();

  req = HSMP::ProcessRequest("u006000000001005MiFotoDataDeFotoPeter");
  req->PrintStructure();

  std::shared_ptr<HSMP::ServerResponse> res;

  res = HSMP::ProcessResponse("Lok");
  res->PrintStructure();

  res = HSMP::ProcessResponse("I03110305SantistebanLeePeter");
  res->PrintStructure();

  res = HSMP::ProcessResponse("M00305byemateo");
  res->PrintStructure();

  res = HSMP::ProcessResponse("B01003HolaATodosLee"); //Broadcast
  res->PrintStructure();

  res = HSMP::ProcessResponse("U006000000001005MiFotoDataDeFotoPeter"); // Upload
  res->PrintStructure();

  res = HSMP::ProcessResponse("F05mateo");
  res->PrintStructure();

  res = HSMP::ProcessResponse("X");
  res->PrintStructure();

  res = HSMP::ProcessResponse("EThis is a Error Message");
  res->PrintStructure();
  */

  return 0;
}
