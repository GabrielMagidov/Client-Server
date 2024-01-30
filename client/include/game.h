#pragma once

#include <iostream>
#include <map>
#include <vector>
#include <string>
#include "../include/event.h"

using namespace std;

/*
*This class saves the game when ever the user gets a message from server about this game.
*The fields of the class:
*   team_a, team_b, time, user: this fields are the name of the first team, the name of the second team, the time of the last event reported about this game, the user that reported about this game.
*   team_a_updates, team_b_updates:  this fields are maps that get a name of a stat and returns the last update of this stat.
*   full_game_desc: this field saves the full game description, every time there is a report for this game, the description of the report is added to this field with the time and name of the event.
*The method update game gets a new event and update the time, the full game description and the stats of this game.
*The method summary gets a path to file and summarizes the game as we were asked to do.
*
*
*NOTICE: if a game has the same name(for example, Germany_Japan), but X different users reported about this game, X instances of this game will be saved(in our implementation), each one will have the same names for the team but a differrent user.
*/
class game{
    private:
        string team_a;
        string team_b;
        int time;
        map<string, string> team_a_updates;
        map<string, string> team_b_updates;

    public:
        vector<string> full_game_desc;
        game(string team_a_name, string team_b_name, string user_);
        game& updateGame(Event eve);
        string user;
        void summary(string file);
        game& operator=(const game &other);
        bool operator==(const game &other);
};