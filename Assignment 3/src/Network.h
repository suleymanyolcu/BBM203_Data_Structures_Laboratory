#ifndef NETWORK_H
#define NETWORK_H

#include <vector>
#include <iostream>
#include "Packet.h"
#include "Client.h"

using namespace std;

class Network {
public:
    Network();
    ~Network();

    // Executes commands given as a vector of strings while utilizing the remaining arguments.
    void process_commands(vector<Client> &clients, vector<string> &commands, int message_limit, const string &sender_port,
                     const string &receiver_port);

    // Initialize the network from the input files.
    vector<Client> read_clients(string const &filename);
    void read_routing_tables(vector<Client> & clients, string const &filename);
    vector<string> read_commands(const string &filename);

    vector<string> fragment_message(const string &message, int chunk_size);

    string getCurrentTimestamp();

    Client & findClient(vector<Client> &clients, const string &_id);

    void print_command(const string& command);

    Client &findClientbyMAC(vector<Client> &clients, const string &_mac);

    bool isMessageLastChunk(const string &str);

    bool doesClientExist(vector<Client> &clients, const string &_id);

    string activityTypeToString(ActivityType type);


    void deleteFrame(stack<Packet *> &frame);
};

#endif  // NETWORK_H
