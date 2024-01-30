#include <iostream>
#include <map>
#include <vector>
#include <string>
#include "../include/game.h"
#include "../include/dataBase.h"

dataBase::dataBase() : loggedIn(false), connected(false), receiptId(1), gameNameToGames(), receiptIdToCmd(), receiptIdToGame(), gameToSubId(), games() {}

vector<string> dataBase::divideByChar(string &str, char c){
    vector<string> ans;
    string temp = "";
    for(auto &ch : str){
        if(ch == c && temp != ""){
            ans.push_back(temp);
            temp = "";
        }
        else{
            temp += ch;
        }
    }
    ans.push_back(temp);

    return ans;
}