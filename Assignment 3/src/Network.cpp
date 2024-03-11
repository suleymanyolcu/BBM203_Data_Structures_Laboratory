#include "Network.h"
#include <chrono>
#include <ctime>
#include <sstream>
#include <algorithm>
#include <fstream>

Network::Network() {

}

void Network::process_commands(vector<Client> &clients, vector<string> &commands, int message_limit,
                               const string &sender_port, const string &receiver_port) {
    // TODO: Execute the commands given as a vector of strings while utilizing the remaining arguments.
    /* Don't use any static variables, assume this method will be called over and over during testing.
     Don't forget to update the necessary member variables after processing each command. For example,
     after the MESSAGE command, the outgoing queue of the sender must have the expected frames ready to send. */
    for (const string &command: commands) {
        istringstream iss(command);
        string cmd_type;
        iss >> cmd_type;
        print_command(command);
        if (cmd_type == "MESSAGE") {
            string sender_ID, receiver_ID, message;
            iss >> sender_ID >> receiver_ID;
            char pound;
            iss >> pound;  // Read the '#' character
            getline(iss, message, '#');  // Read characters between '#' symbols
            cout << "Message to be sent: "<<'"'<<message<<'"'<<"\n"<<endl;
            Client& sender = findClient(clients,sender_ID);
            Client& receiver = findClient(clients,receiver_ID);
            vector<string> message_chunks = fragment_message(message, message_limit);
            for (size_t i = 0; i < message_chunks.size(); ++i) {
                // Create packets for each layer
                Packet *app_packet(new ApplicationLayerPacket(0, sender.client_id, receiver.client_id, message_chunks[i]));
                Packet *transport_packet(new TransportLayerPacket(1, sender_port, receiver_port));
                Packet *network_packet(new NetworkLayerPacket(2, sender.client_ip, receiver.client_ip));
                string next_hop_mac = findClient(clients, sender.routing_table[receiver.client_id]).client_mac;
                Packet *physical_packet(new PhysicalLayerPacket(3, sender.client_mac, next_hop_mac));

                // Create a stack of packets representing the frame
                stack<Packet *> frame_stack;
                frame_stack.push(app_packet);
                frame_stack.push(transport_packet);
                frame_stack.push(network_packet);
                frame_stack.push(physical_packet);

                // Add the frame to the sender's outgoing queue
                sender.outgoing_queue.push(frame_stack);
                cout<<"Frame #"<<i+1<<endl;
                physical_packet->print();
                network_packet->print();
                transport_packet->print();
                app_packet->print();
                cout<<"Message chunk carried: "<<'"'<<message_chunks[i]<<'"'<<endl;
                cout<<"Number of hops so far: 0"<<endl;
                cout<<"--------"<<endl;
                // Log the message and frame information

            }

            string timestamp = getCurrentTimestamp();
            Log log_entry(timestamp, message, message_chunks.size(), 0, sender_ID, receiver_ID, true, ActivityType::MESSAGE_SENT);
            sender.log_entries.push_back(log_entry);
        }

        else if(cmd_type == "SHOW_FRAME_INFO"){
            string client_ID, queue, frame_number_str;
            iss >> client_ID >> queue >> frame_number_str;
            int frame_number = stoi(frame_number_str);
            Client& client = findClient(clients, client_ID);
            if(queue=="out"){
                if (!(frame_number >= 1 && frame_number <= client.outgoing_queue.size())){
                    cout<<"No such frame."<<endl;
                    continue;
                }
                cout<<"Current Frame #"<<frame_number<< " on the outgoing queue of client "<<client_ID<<endl;
                std::queue<stack<Packet*>> outgoing_queue = client.outgoing_queue;
                for (int i = 1; i < frame_number; ++i) {
                    outgoing_queue.pop();
                }
                stack<Packet*> frame = outgoing_queue.front();
                PhysicalLayerPacket *physical_packet = dynamic_cast<PhysicalLayerPacket *>(frame.top());
                frame.pop();
                NetworkLayerPacket *network_packet = dynamic_cast<NetworkLayerPacket *>(frame.top());
                frame.pop();
                TransportLayerPacket *transport_packet = dynamic_cast<TransportLayerPacket *>(frame.top());
                frame.pop();
                ApplicationLayerPacket *app_packet = dynamic_cast<ApplicationLayerPacket *>(frame.top());
                frame.pop();
                string carried_message = app_packet->message_data;
                int number_of_hops = physical_packet->number_of_hops;
                cout<<"Carried Message: "<<'"'<<carried_message<<'"'<<endl;
                stack<Packet*> custom_frame;
                custom_frame.push(physical_packet);
                custom_frame.push(network_packet);
                custom_frame.push(transport_packet);
                custom_frame.push(app_packet);
                for(int i =0; i<4;i++){
                    cout<<"Layer "<<i<<" info: ";
                    custom_frame.top()->print();
                    custom_frame.pop();
                }
                cout<<"Number of hops so far: "<<number_of_hops<<endl;
            }
            if(queue=="in"){
                if (!(frame_number >= 1 && frame_number <= client.incoming_queue.size())){
                    cout<<"No such frame."<<endl;
                    continue;
                }
                cout<<"Current Frame #"<<frame_number<< " on the incoming queue of client "<<client_ID<<endl;
                std::queue<stack<Packet*>> incomingQueue = client.incoming_queue;
                for (int i = 1; i < frame_number; ++i) {
                    incomingQueue.pop();
                }
                stack<Packet*> frame = incomingQueue.front();
                PhysicalLayerPacket *physical_packet = dynamic_cast<PhysicalLayerPacket *>(frame.top());
                frame.pop();
                NetworkLayerPacket *network_packet = dynamic_cast<NetworkLayerPacket *>(frame.top());
                frame.pop();
                TransportLayerPacket *transport_packet = dynamic_cast<TransportLayerPacket *>(frame.top());
                frame.pop();
                ApplicationLayerPacket *app_packet = dynamic_cast<ApplicationLayerPacket *>(frame.top());
                frame.pop();
                string carried_message = app_packet->message_data;
                int number_of_hops = physical_packet->number_of_hops;
                cout<<"Carried Message: "<<'"'<<carried_message<<'"'<<endl;
                stack<Packet*> custom_frame;
                custom_frame.push(physical_packet);
                custom_frame.push(network_packet);
                custom_frame.push(transport_packet);
                custom_frame.push(app_packet);
                for(int i =0; i<4;i++){
                    cout<<"Layer "<<i<<" info: ";
                    custom_frame.top()->print();
                    custom_frame.pop();
                }
                cout<<"Number of hops so far: "<<number_of_hops<<endl;
            }
        }

        else if(cmd_type=="SHOW_Q_INFO"){
            string client_ID, queue;
            iss >> client_ID >> queue;
            Client& client = findClient(clients, client_ID);
            if(queue=="out"){
                cout<<"Client "<<client_ID<<" Outgoing Queue Status"<<endl;
                cout<<"Current total number of frames: "<<client.outgoing_queue.size()<<endl;
            }
            if(queue=="in"){
                cout<<"Client "<<client_ID<<" Incoming Queue Status"<<endl;
                cout<<"Current total number of frames: "<<client.incoming_queue.size()<<endl;
            }
        }

        else if(cmd_type=="SEND"){
            int frame_number=1;
            for (Client &sender : clients) {
                string complete_message;
                string sender_ID;
                string receiver_ID;
                int hops;
                while (!sender.outgoing_queue.empty()) {
                        stack<Packet *> frame = sender.outgoing_queue.front();
                        stack<Packet*> app = sender.outgoing_queue.front();
                        string receiver_mac = dynamic_cast<PhysicalLayerPacket*>(frame.top())->receiver_MAC_address;
                        Client &receiver = findClientbyMAC(clients,receiver_mac);
                        sender.outgoing_queue.pop();
                        // Update the hop count
                        dynamic_cast<PhysicalLayerPacket *>(frame.top())->number_of_hops += 1;
                        hops = dynamic_cast<PhysicalLayerPacket *>(frame.top())->number_of_hops;
                        // Add the frame to the receiver's incoming queue
                        receiver.incoming_queue.push(frame);

                        // Print the network trace
                        cout << "Client " << sender.client_id << " sending frame #" << frame_number << " to client " << receiver.client_id << endl;
                        while (!app.empty() && dynamic_cast<ApplicationLayerPacket*>(app.top()) == nullptr) {
                            app.pop();
                        }
                        for (int i = 0; i < 4; ++i) {
                            frame.top()->print();
                            frame.pop();
                        }
                        string message = dynamic_cast<ApplicationLayerPacket *>(app.top())->message_data;
                        complete_message += message;
                        sender_ID = dynamic_cast<ApplicationLayerPacket *>(app.top())->sender_ID;
                        receiver_ID = dynamic_cast<ApplicationLayerPacket *>(app.top())->receiver_ID;
                        // Print message chunk
                        cout << "Message chunk carried: \"" << message << "\"" << endl;
                        cout << "Number of hops so far: " << hops << endl;
                        cout<<"--------"<<endl;
                        if(isMessageLastChunk(message)){
                            frame_number=0;
                        }
                        frame_number += 1;
                    }
            }
        }

        else if(cmd_type=="RECEIVE"){
            int frame_number=1;
            for(Client &receiver : clients){
                string complete_message;
                string sender_ID;
                string receiver_ID;
                bool isReceived;
                int hops;
                if(receiver.incoming_queue.empty()){
                    continue;
                }
                bool isPrinted=false;
                while (!receiver.incoming_queue.empty()){
                    stack<Packet*> app_id = receiver.incoming_queue.front();
                    string sender_mac = dynamic_cast<PhysicalLayerPacket*>(app_id.top())->sender_MAC_address;
                    while (!app_id.empty() && dynamic_cast<ApplicationLayerPacket*>(app_id.top()) == nullptr) {
                        app_id.pop();
                    }
                    string real_sender_id = dynamic_cast<ApplicationLayerPacket *>(app_id.top())->sender_ID;
                    string real_receiver_id = dynamic_cast<ApplicationLayerPacket *>(app_id.top())->receiver_ID;
                    string sender_id = findClientbyMAC(clients,sender_mac).client_id;
                    string next_hop_id = receiver.routing_table[real_receiver_id];
                    bool client_exist = doesClientExist(clients,next_hop_id);
                    if(receiver.client_mac!= findClient(clients,real_receiver_id).client_mac&&client_exist){
                        if(!isPrinted){
                            cout<<"Client "<<receiver.client_id<<" receiving a message from client "<<sender_id<<", but intended for client "<<real_receiver_id<<". Forwarding... "<<endl;
                            isPrinted= true;
                        }
                        stack<Packet *> frame =receiver.incoming_queue.front();
                        hops= dynamic_cast<PhysicalLayerPacket*>(frame.top())->number_of_hops;
                        stack<Packet *> app =receiver.incoming_queue.front();
                        while (!app.empty() && dynamic_cast<ApplicationLayerPacket*>(app.top()) == nullptr) {
                            app.pop();
                        }
                        string message = dynamic_cast<ApplicationLayerPacket *>(app.top())->message_data;
                        complete_message += message;
                        string new_sender_mac = receiver.client_mac;
                        string next_hop_mac = findClient(clients,receiver.routing_table[real_receiver_id]).client_mac;
                        dynamic_cast<PhysicalLayerPacket*>(frame.top())->sender_MAC_address = new_sender_mac;
                        dynamic_cast<PhysicalLayerPacket*>(frame.top())->receiver_MAC_address = next_hop_mac;
                        receiver.incoming_queue.pop();
                        receiver.outgoing_queue.push(frame);
                        cout<<"Frame #"<<frame_number<<" MAC address change: New sender MAC "<<new_sender_mac<<", new receiver MAC "<<next_hop_mac<<endl;
                        frame_number ++;
                        if(isMessageLastChunk(message)){
                            frame_number=1;
                            cout<<"--------"<<endl;
                            isPrinted= false;
                            vector<string> message_chunks = fragment_message(complete_message, message_limit);
                            string timestamp = getCurrentTimestamp();
                            Log log_entry(timestamp, "", message_chunks.size(), hops, real_sender_id, real_receiver_id, true, ActivityType::MESSAGE_FORWARDED);
                            receiver.log_entries.push_back(log_entry);
                            complete_message="";
                        }
                    }
                    if(receiver.client_mac!= findClient(clients,real_receiver_id).client_mac&&!client_exist){
                        stack<Packet *> frame =receiver.incoming_queue.front();
                        int hops = dynamic_cast<PhysicalLayerPacket *>(frame.top())->number_of_hops;
                        stack<Packet *> app =receiver.incoming_queue.front();
                        while (!app.empty() && dynamic_cast<ApplicationLayerPacket*>(app.top()) == nullptr) {
                            app.pop();
                        }
                        string message = dynamic_cast<ApplicationLayerPacket *>(app.top())->message_data;
                        app.pop();
                        complete_message += message;
                        cout<<"Client "<<receiver.client_id<<" receiving frame #"<<frame_number<<" from client "<<sender_id<<", but intended for client "<<real_receiver_id<<". Forwarding... "<<endl;
                        cout<<"Error: Unreachable destination. Packets are dropped after "<<hops<<" hops!"<<endl;
                        frame_number++;
                        if(isMessageLastChunk(message)){
                            frame_number=1;
                            cout<<"--------"<<endl;
                            vector<string> message_chunks = fragment_message(complete_message, message_limit);
                            string timestamp = getCurrentTimestamp();
                            Log log_entry(timestamp, "", message_chunks.size(), hops, real_sender_id, real_receiver_id, false, ActivityType::MESSAGE_DROPPED);
                            receiver.log_entries.push_back(log_entry);
                            complete_message="";
                        }
                        for(int i = 0 ; i<4; i++){
                            delete frame.top();
                            frame.pop();
                        }

                        receiver.incoming_queue.pop();
                    }
                    if(receiver.client_mac == findClient(clients,real_receiver_id).client_mac){
                        cout<<"Client "<<receiver.client_id<<" receiving frame #"<<frame_number<<" from client "<<sender_id<<", originating from client "<<real_sender_id<<endl;
                        stack<Packet *> frame =receiver.incoming_queue.front();
                        stack<Packet *> app =receiver.incoming_queue.front();
                        int hops = dynamic_cast<PhysicalLayerPacket *>(frame.top())->number_of_hops;
                        while (!app.empty() && dynamic_cast<ApplicationLayerPacket*>(app.top()) == nullptr) {
                            app.pop();
                        }
                        string message = dynamic_cast<ApplicationLayerPacket *>(app.top())->message_data;
                        app.pop();
                        complete_message+=message;
                        vector<string> message_chunks = fragment_message(complete_message, message_limit);
                        for (int i = 0; i < 4; ++i) {
                            frame.top()->print();
                            delete frame.top();
                            frame.pop();
                        }
                        cout<<"Message chunk carried: "<<'"'<<message<<'"'<<endl;
                        cout<<"Number of hops so far: "<<hops<<endl;
                        cout<<"--------"<<endl;

                        receiver.incoming_queue.pop();
                        frame_number++;
                        if(isMessageLastChunk(message)){
                            frame_number=1;
                            cout<<"Client "<<receiver.client_id<<" received the message "<<'"'<<complete_message<<'"'<<" from client "<<real_sender_id<<"."<<endl;
                            cout<<"--------"<<endl;
                            vector<string> message_chunks = fragment_message(complete_message, message_limit);
                            string timestamp = getCurrentTimestamp();
                            Log log_entry(timestamp, complete_message, message_chunks.size(), hops, real_sender_id, real_receiver_id,
                                          true, ActivityType::MESSAGE_RECEIVED);
                            receiver.log_entries.push_back(log_entry);
                            complete_message="";
                        }
                    }
                }

            }
        }

        else if(cmd_type=="PRINT_LOG"){
            string client_ID;
            iss>>client_ID;
            Client& client = findClient(clients,client_ID);
            string success;
            if(!client.log_entries.empty()){
                int index = 1;
                cout<<"Client "<<client_ID<<" Logs:"<<endl;
                for(const Log & log_entry : client.log_entries){
                    if(log_entry.success_status){
                        success="Yes";
                    } else{
                        success="No";
                    }
                    cout<<"--------------"<<endl;
                    cout<<"Log Entry #"<<index<<":"<<endl;
                    cout<<"Activity: "<<activityTypeToString(log_entry.activity_type)<<endl;
                    cout<<"Timestamp: "<<log_entry.timestamp<<endl;
                    cout<<"Number of frames: "<<log_entry.number_of_frames<<endl;
                    cout<<"Number of hops: "<<log_entry.number_of_hops<<endl;
                    cout<<"Sender ID: "<<log_entry.sender_id<<endl;
                    cout<<"Receiver ID: "<<log_entry.receiver_id<<endl;
                    cout<<"Success: "<<success<<endl;
                    if(!log_entry.message_content.empty()){
                        cout<<"Message: "<<'"'<<log_entry.message_content<<'"'<<endl;
                    }
                    index++;
                }
            }
        }

        else{
            cout<<"Invalid command."<<endl;
        }

    }

}

vector<string> Network::fragment_message(const string &message, int chunk_size) {
    vector<string> fragments;

    // Calculate the number of chunks needed
    size_t num_chunks = (message.length() + chunk_size - 1) / chunk_size;

    // Fragment the message
    for (size_t i = 0; i < num_chunks; ++i) {
        size_t start_pos = i * chunk_size;
        size_t end_pos = min((i + 1) * chunk_size, message.length());
        fragments.push_back(message.substr(start_pos, end_pos - start_pos));
    }

    return fragments;
}

vector<Client> Network::read_clients(const string &filename) {
    vector<Client> clients;

    // Open the input file
    ifstream inputFile(filename);

    if (!inputFile.is_open()) {
        cerr << "Error opening file: " << filename << endl;
        return clients; // Return an empty vector in case of an error
    }

    // Read the number of clients from the first line
    int number_of_clients;
    inputFile >> number_of_clients;

    // Read client information from the file
    for (int i = 0; i < number_of_clients; ++i) {
        string id, ip, mac;
        inputFile >> id >> ip >> mac;

        // Create a Client instance and add it to the vector
        clients.emplace_back(id, ip, mac);
    }

    // Close the file
    inputFile.close();

    return clients;
    // TODO: Read clients from the given input file and return a vector of Client instances.
}

vector<string> Network::read_commands(const string &filename) {
    vector<string> commands;
    // TODO: Read commands from the given input file and return them as a vector of strings.
    // Open the input file
    ifstream inputFile(filename);

    if (!inputFile.is_open()) {
        cerr << "Error opening file: " << filename << endl;
        return commands; // Return an empty vector in case of an error
    }

    // Read the number of commands from the first line
    int number_of_commands;
    inputFile >> number_of_commands;

    // Read commands from the file
    string line;
    getline(inputFile, line); // Consume the newline after the number_of_commands

    for (int i = 0; i < number_of_commands; ++i) {
        getline(inputFile, line);
        commands.push_back(line);
    }

    // Close the file
    inputFile.close();

    return commands;
}

void Network::read_routing_tables(vector<Client> &clients, const string &filename) {
    // TODO: Read the routing tables from the given input file and populate the clients' routing_table member variable.
    // Open the input file
    ifstream inputFile(filename);

    if (!inputFile.is_open()) {
        cerr << "Error opening file: " << filename << endl;
        return; // Return in case of an error
    }

    // Read routing tables for each client
    for (Client &client: clients) {
        // Read until a line with a single hyphen '-' is encountered
        while (true) {
            string line;
            getline(inputFile, line);

            // Check if the line contains a hyphen, indicating the end of the routing table
            if (line == "-") {
                break;
            }

            // Parse the line to get destination and next hop
            istringstream iss(line);
            string destination, next_hop;
            iss >> destination >> next_hop;

            // Populate the routing table for the current client
            client.routing_table[destination] = next_hop;
            if (inputFile.eof()) {
                break;
            }
        }
    }

    // Close the file
    inputFile.close();
}

std::string Network::getCurrentTimestamp() {
    // Get the current time
    auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

    // Prepare a buffer for the formatted time
    const int bufferSize = 20;  // Adjust the buffer size as needed
    char buffer[bufferSize];

    // Format the time using std::strftime
    if (std::strftime(buffer, bufferSize, "%Y-%m-%d %H:%M:%S", std::localtime(&now))) {
        return std::string(buffer);
    } else {
        // Handle the error (e.g., by returning an empty string)
        return "";
    }
}

Client & Network::findClient(vector<Client> &clients, string const& _id){
    for (auto & client : clients) {
        if(client.client_id==_id){
            return client;
        }
    }
}

bool Network::doesClientExist(vector<Client> &clients, string const& _id){
    bool value = false;
    for (auto & client : clients){
        if(client.client_id==_id){
            value = true;
        }
    }
    return value;
}

Client & Network::findClientbyMAC(vector<Client> &clients, string const& _mac){
    for (auto & client : clients) {
        if(client.client_mac==_mac){
            return client;
        }
    }
}

void Network::print_command(const string& command){
    int numberOfDashes = command.length();
    numberOfDashes = numberOfDashes+9;
    for (int i = 0; i < numberOfDashes; ++i) {
        std::cout << '-';
    }
    std::cout<<std::endl;
    std::cout<<"Command: "<<command<<std::endl;
    for (int i = 0; i < numberOfDashes; ++i) {
        std::cout << '-';
    }
    std::cout<<std::endl;
}

bool Network::isMessageLastChunk(const std::string& str) {
    for (char ch : str) {
        if (ch == '.' || ch == '?' || ch == '!') {
            return true;
        }
    }
    return false;
}

string Network::activityTypeToString(ActivityType type) {
    switch (type) {
        case ActivityType::MESSAGE_SENT:
            return "Message Sent";
        case ActivityType::MESSAGE_DROPPED:
            // Add other cases as needed
            return "Message Dropped";
        case ActivityType::MESSAGE_FORWARDED:
            return "Message Forwarded";
        case ActivityType::MESSAGE_RECEIVED:
            return "Message Received";
    }
}

Network::~Network() {

}
void Network::deleteFrame(stack<Packet*>& frame) {
    // Delete the packets in the frame
    while (!frame.empty()) {
        delete frame.top();
        frame.pop();
    }
}

