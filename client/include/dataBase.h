#pragma once

#include <iostream>
#include <map>
#include <vector>
#include <string>
#include "../include/game.h"

using namespace std;

/*
*This is a class that saves a "data base" seperatly for each client.
*Every time a user connects to the client he creats the data base with the following fields:
    *loggedIn - a boolean that is true if there is a player currently logged in.
    *connected - a boolean that is true if the client is connected to server.
    *receiptId - an integer that every time the user sends a frame to the server, he sends it with the current receipt id and increases the value by one.
    *gameNameToGames - a map that gets a game name("teamA_teamB") and returns a vector of all of the updated version for this game, each version is the latest update of this game from each user
    *receiptIdTOCmd - a map that gets a receiptId(which was sent to server with a frame) and returns which command was sent to the server(login, join, etc...)
    *receiptIdToGame - a map that gets a receiptId(which was sent to server with a frame of join or exit) and returns the name of a game which was this frame's destination
    *gameToSubId - a map that gets a game name and returns the subscription if which was sent to the server when subscribing to that game.
*Every time there is a disconnection from the server all of those fields are cleared and return to default values - int=0, bool=false, map becomes empty.
*The method divideByChar gets a string and a character and retruns a vector of string which represents the string splitted by this character.
*/

class dataBase{
    public:
        dataBase();
        bool loggedIn;
        bool connected;
        int receiptId;
        map<string, vector<game*>> gameNameToGames;
        map<string, string> receiptIdToCmd;
        map<string, string> receiptIdToGame;
        map<string, string> gameToSubId;
        vector<string> games;
        vector<string> divideByChar(string &str, char c);
};