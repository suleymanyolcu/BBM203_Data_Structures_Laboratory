#include <fstream>
#include <iostream>
#include "Leaderboard.h"

void Leaderboard::insert_new_entry(LeaderboardEntry * new_entry) {
    // TODO: Insert a new LeaderboardEntry instance into the leaderboard, such that the order of the high-scores
    //       is maintained, and the leaderboard size does not exceed 10 entries at any given time (only the
    //       top 10 all-time high-scores should be kept in descending order by the score).
    if (!head_leaderboard_entry || new_entry->score > head_leaderboard_entry->score) {
        // Insert at the beginning
        new_entry->next_leaderboard_entry = head_leaderboard_entry;
        head_leaderboard_entry = new_entry;
    } else {
        // Insert in the middle or at the end
        LeaderboardEntry *current = head_leaderboard_entry;
        while (current->next_leaderboard_entry && new_entry->score <= current->next_leaderboard_entry->score) {
            current = current->next_leaderboard_entry;
        }
        new_entry->next_leaderboard_entry = current->next_leaderboard_entry;
        current->next_leaderboard_entry = new_entry;
    }

    // Maintain the top 10 entries
    LeaderboardEntry *last = nullptr;
    LeaderboardEntry *current = head_leaderboard_entry;
    for (int i = 0; i < 10 && current; ++i) {
        last = current;
        current = current->next_leaderboard_entry;
    }

    if (last) {
        last->next_leaderboard_entry = nullptr;
        delete current;
    }
}

void Leaderboard::write_to_file(const string& filename) {
    ofstream outFile(filename);
    if (outFile.is_open()) {
        LeaderboardEntry *current = head_leaderboard_entry;
        while (current) {
            outFile << current->score << " " << current->last_played << " " << current->player_name << endl;
            current = current->next_leaderboard_entry;
        }
        outFile.close();
    } else {
        cerr << "Error opening the leaderboard file: " << filename << endl;
    }
}


void Leaderboard::read_from_file(const string& filename) {
    ifstream inFile(filename);
    if (inFile.is_open()) {
        head_leaderboard_entry = nullptr;
        unsigned long score;
        time_t lastPlayed;
        string playerName;
        while (inFile >> score) {
            inFile.ignore(); // Ignore the space
            inFile >> lastPlayed >> playerName;
            LeaderboardEntry *newEntry = new LeaderboardEntry(score, lastPlayed, playerName);
            insert_new_entry(newEntry);
        }
        inFile.close();
    } else {
        cerr << "Error opening the leaderboard file: " << filename << endl;
    }
}


void Leaderboard::print_leaderboard() {
    // TODO: Print the current leaderboard status to the standard output in the format specified in the PA instructions
    cout << "Leaderboard" << endl;
    cout << "-----------" << endl;
    int order = 1;
    LeaderboardEntry *current = head_leaderboard_entry;
    while (current) {
        cout << order << ". " << current->player_name << " " << current->score << " ";
        cout << formatTimestamp(current->last_played) << endl;
        current = current->next_leaderboard_entry;
        ++order;
    }
}

Leaderboard::~Leaderboard() {
    // TODO: Free dynamically allocated memory used for storing leaderboard entries
    LeaderboardEntry *current = head_leaderboard_entry;
    while (current) {
        LeaderboardEntry *next = current->next_leaderboard_entry;
        delete current;
        current = next;
    }
}

string Leaderboard::formatTimestamp(time_t timestamp) {
    struct tm *tm_info;
    char buffer[20]; // Updated buffer size
    tm_info = localtime(&timestamp);
    strftime(buffer, sizeof(buffer), "%H:%M:%S %d.%m.%Y", tm_info); // Updated format
    return buffer;
}