#include "../include/game.h"
#include <string>
#include <fstream>

using namespace std;

game::game(string team_a_name, string team_b_name, string user_) : team_a(team_a_name), team_b(team_b_name), time(0), team_a_updates(), team_b_updates(), full_game_desc(), user(user_){}

game& game::updateGame(Event eve){
    time = eve.get_time();
    map<string, string> team_a_up = eve.get_team_a_updates();
    map<string,string> team_b_up = eve.get_team_b_updates();

    for(auto &up : team_a_up){
        team_a_updates[up.first] = up.second;
    }

    for(auto &up : team_b_up){
        team_b_updates[up.first] = up.second;
    }

    string desc = "";
    desc += to_string(time) + " - " + eve.get_name() + ":\n" + eve.get_discription()+"\n\n";
    full_game_desc.push_back(desc);

    return *this;
}

void game::summary(string file){
    string toWrite = team_a + " vs " + team_b + "\nGame stats:\n" + team_a + " stats:";

    for(auto &up : team_a_updates){
        toWrite += "\n    " + up.first + ": " + up.second;
    }

    toWrite += "\n" + team_b + " stats:";

    for(auto &up : team_b_updates){
        toWrite += "\n    " + up.first + ": " + up.second;
    }

    toWrite += "\nGame event report:\n";

    for(auto &str : full_game_desc){
        toWrite += str;
    }
    ofstream outfile(file);
    outfile << toWrite;
    outfile.close();
}
game& game::operator=(const game &other){
    team_a = other.team_a;
    team_b = other.team_b;
    team_a_updates = other.team_a_updates;
    team_b_updates = other.team_b_updates;
    time = other.time;
    user = other.user;
    full_game_desc = other.full_game_desc;
    return *this;
}

bool game::operator==(const game &other){
    if(user == other.user){
        return true;
    }
    return false;
}