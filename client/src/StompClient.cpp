#include <iostream>
#include <string>
#include "../include/ConnectionHandler.h"
#include <thread>
#include "../include/dataBase.h"
#include <boost/lexical_cast.hpp>
#include <map>

using namespace std;


static string host = "";
static short port = 0;
static ConnectionHandler* handler;
static dataBase db;
static int subscriptionId = 1;

void keyboard(){
    map<string, names_and_events> nameToGameUpdate;
    string currLoggedInName;
	bool checkIfConnected = true;
	while(true){
		const short bufsize = 1024;
		char buf[bufsize];
		std::cin.getline(buf, bufsize);
		std::string line(buf);
		string firstCmd = "";
		vector<string> dividedLine = db.divideByChar(line, ' ');
		firstCmd = dividedLine[0];

		if(firstCmd != "login" && !db.loggedIn){
			cout << "You must login first" << endl; 
		}
		else if(firstCmd == "login" && db.connected){
			cout << "The client is already logged in, log out before trying again" << endl;
		}
		else{
			if(firstCmd == "login"){
				string portAsString = "";
				vector<string> hostAndPort = db.divideByChar(dividedLine[1], ':');
				host = hostAndPort[0];
				portAsString = hostAndPort[1];
				port = boost::lexical_cast<short>(portAsString);
				ConnectionHandler *temp_handler = new ConnectionHandler(host,port);
				handler = temp_handler;
				if (!db.connected && !handler->connect()) {
					cout << "Could not connect to server" << endl;
				}
				else{	
					string frameToSend = "CONNECT\nreceipt:" + to_string(db.receiptId) + "\naccept-version:1.2\nhost:stomp.cs.bgu.ac.il\nlogin:" + dividedLine[2] +"\npasscode:" + dividedLine[3] +"\n\n";
					db.receiptIdToCmd[to_string(db.receiptId)] = "CONNECT";
					db.receiptId++;
					currLoggedInName = dividedLine[2];
					if(handler->sendLine(frameToSend)){
						db.connected = true;
					}
					else{
						checkIfConnected = false;
					}
				}
			}
			else if(firstCmd == "join"){
				string frameToSend = "SUBSCRIBE\nreceipt:" + to_string(db.receiptId) + "\ndestination:/" + dividedLine[1] + "\nid:" + to_string(subscriptionId) + "\n\n";
				db.receiptIdToCmd[to_string(db.receiptId)] = "SUBSCRIBE";
				db.receiptIdToGame[to_string(db.receiptId)] = dividedLine[1];
				db.receiptId++;
				checkIfConnected = handler->sendLine(frameToSend);
			}
			else if(firstCmd == "exit"){
				string frameToSend = "UNSUBSCRIBE\nreceipt:" + to_string(db.receiptId) + "\nid:" + db.gameToSubId[dividedLine[1]] + "\n\n";
				db.receiptIdToCmd[to_string(db.receiptId)] = "UNSUBSCRIBE";
				db.receiptIdToGame[to_string(db.receiptId)] = dividedLine[1];
				db.receiptId++;
				checkIfConnected = handler->sendLine(frameToSend);
			}
			else if(firstCmd == "report"){
				names_and_events eventsToSend = parseEventsFile(dividedLine[1]);
				vector<string> eventsAsString;
				string nameOfGame = eventsToSend.team_a_name + "_" + eventsToSend.team_b_name;
				if(find(db.games.begin(), db.games.end(), nameOfGame) != db.games.end()){
					db.games.push_back(nameOfGame);
				}
				string frameToSend = "";
				string startFrameToSend = "SEND\ndestination:/" + eventsToSend.team_a_name + "_" + eventsToSend.team_b_name + "\n\nuser: " + 
									currLoggedInName + "\nteam a: " + eventsToSend.team_a_name + "\nteam b: " + eventsToSend.team_b_name;
				for(auto eve : eventsToSend.events){
					string temp = "";
					temp += "\nevent name: " + eve.get_name();
					temp += "\ntime: " + to_string(eve.get_time()) + "\ngeneral game updates:";
					for (auto up : eve.get_game_updates()){
						temp += "\n    " + up.first + ": " + up.second;
					}
					temp += "\nteam a updates:";
					for (auto up : eve.get_team_a_updates()){
						temp += "\n    " + up.first + ": " + up.second;
					}
					temp += "\nteam b updates:";
					for (auto up : eve.get_team_b_updates()){
						temp += "\n    " + up.first + ": " + up.second;
					}
					temp += "\ndescription:\n" + eve.get_discription() + "\n";

					frameToSend = startFrameToSend + temp;
					checkIfConnected = handler->sendLine(frameToSend);
					frameToSend = "";
				}
				
				
			}
			else if(firstCmd == "summary"){
				string gameName = dividedLine[1];
				string userName = dividedLine[2];
				string fileToWrite = dividedLine[3];
				vector<game*> games = db.gameNameToGames[gameName];
				bool foundGame = false;
				for(auto Game : games){
					if((*Game).user == userName){
						Game->summary(fileToWrite);
						foundGame = true;
					}
				}
				if(!foundGame){
					cout << "This user never updated about this game." << endl;
				}
			}

			else if(firstCmd == "logout"){
				string frameToSend = "DISCONNECT\nreceipt:" + to_string(db.receiptId) + "\n\n";
				db.receiptIdToCmd[to_string(db.receiptId)] = "DISCONNECT";
				db.receiptId++;
				if(handler->sendLine(frameToSend)){
					db.connected = false;
				}

			}
			else{
				cout << "Illegal command, try again" << endl;
			}
		} 
		if(!checkIfConnected){
			for(auto &v : db.gameNameToGames){
				for(auto &Game : v.second){
					delete(Game);
				}
			}
			delete(handler);
			db.connected = false;
			db.loggedIn = false;
			db.gameNameToGames.clear();
			db.receiptIdToCmd.clear();
			db.receiptIdToGame.clear();
			db.gameToSubId.clear();
			db.games.clear();
			cout << "Cannot connect to server..." << endl;
		}
    }
}

int main() {
	thread t1(keyboard);
	while(true){
		if(db.connected){
            string answer;
            bool checkIfConnected = handler->getLine(answer);
			if(!checkIfConnected){
				for(auto &v : db.gameNameToGames){
					for(auto &Game : v.second){
						delete(Game);
					}
				}
				delete(handler);
				db.connected = false;
				db.loggedIn = false;
				db.gameNameToGames.clear();
				db.receiptIdToCmd.clear();
				db.receiptIdToGame.clear();
				db.gameToSubId.clear();
				db.games.clear();
				cout << "Cannot connect to server..." << endl;
				break;
			}
			
            vector<string> dividedAnswer = db.divideByChar(answer, '\n'); 
            if(dividedAnswer[0] == "CONNECTED"){
                cout << "Login successful" << endl;
                db.loggedIn = true;
            }
            else if(dividedAnswer[0] == "MESSAGE"){
				string gameName = dividedAnswer[3].substr(13);
				string team_a_name = db.divideByChar(gameName, '_')[0];
				string team_b_name = db.divideByChar(gameName, '_')[1];
				string userName = db.divideByChar(dividedAnswer[4], ':')[1].substr(1);
				string desc = "";
				map<string, string> generalUpdates;
				map<string, string> team_a_upd;
				map<string, string> team_b_upd;
				bool foundGen = false;
				bool found_aUpd = false;
				bool found_bUpd = false;
				bool foundDesc = false;
				for(auto &str : dividedAnswer){
					vector<string> dividedStr = db.divideByChar(str, ':');
					if(dividedStr[0] == "general game updates"){
						foundGen = true;
						found_aUpd = false;
						found_bUpd = false;
						foundDesc = false;
					}
					else if(db.divideByChar(str, ':')[0] == "team a updates"){
						foundGen = false;
						found_aUpd = true;
						found_bUpd = false;
						foundDesc = false;
					}
					else if(dividedStr[0] == "team b updates"){
						foundGen = false;
						found_aUpd = false;
						found_bUpd = true;
						foundDesc = false;
					}
					else if(dividedStr[0] == "description"){
						foundGen = false;
						found_aUpd = false;
						found_bUpd = false;
						foundDesc = true;
					}
					if(foundGen && dividedStr[0] != "general game updates"){
						generalUpdates[dividedStr[0]] = dividedStr[1];
						
					}
					else if(found_aUpd && dividedStr[0] != "team a updates"){
						team_a_upd[dividedStr[0]] = dividedStr[1];
						
					}
					else if(found_bUpd && dividedStr[0] != "team b updates"){
						team_b_upd[dividedStr[0]] = dividedStr[1];
						
					}
					else if(foundDesc && dividedStr[0] != "description"){
						if(str != ""){	
							desc += str;
						}
						foundDesc = false;
					
					}
				}

				Event eve(team_a_name, team_b_name, db.divideByChar(dividedAnswer[7], ':')[1], stoi(db.divideByChar(dividedAnswer[8], ':')[1].substr(0)), generalUpdates,
						  team_a_upd, team_b_upd, desc);

				vector<game*> games = db.gameNameToGames[gameName];
				bool foundGame = false;
				int indexGame = 0;
				for(auto Game : games){
					if((*Game).user == userName){
						Game->updateGame(eve);
						foundGame = true;

					}
					indexGame++;
				}
				if(!foundGame){
					game *newGame = new game(team_a_name, team_b_name, userName);
					newGame->updateGame(eve);

					db.gameNameToGames[gameName].push_back(newGame);

				}				
            }
            else if(dividedAnswer[0] == "RECEIPT"){
                vector<string> dividedReceipt = db.divideByChar(dividedAnswer[1], ':');
                if(db.receiptIdToCmd[dividedReceipt[1]] == "SUBSCRIBE"){
					string joinedGame = db.receiptIdToGame[dividedReceipt[1]].substr(0);
                    cout << "Joined channel " +  joinedGame << endl;
					db.gameToSubId[joinedGame] = to_string(subscriptionId);
					subscriptionId++;
                }
                else if(db.receiptIdToCmd[dividedReceipt[1]] == "UNSUBSCRIBE"){
					string gameToExit = db.receiptIdToGame[dividedReceipt[1]];
                    cout << "Exited channel " + gameToExit << endl;
					db.gameToSubId.erase(gameToExit);
                }
				else if(db.receiptIdToCmd[dividedReceipt[1]] == "DISCONNECT"){
					for(auto &v : db.gameNameToGames){
						for(auto &Game : v.second){
							delete(Game);
						}
					}
                    delete(handler);
					db.gameNameToGames.clear();
					db.receiptIdToCmd.clear();
					db.receiptIdToGame.clear();
					db.gameToSubId.clear();
					db.games.clear();
					db.loggedIn = false;
					db.connected = false;
                }
            }
			else if(dividedAnswer[0] == "ERROR"){
				for(auto &v : db.gameNameToGames){
					for(auto &Game : v.second){
						delete(Game);
					}
				}
				delete(handler);
				db.gameNameToGames.clear();
				db.receiptIdToCmd.clear();
				db.receiptIdToGame.clear();
				db.gameToSubId.clear();
				db.games.clear();
				db.loggedIn = false;
				db.connected = false;
				cout << "ERROR: " + dividedAnswer[dividedAnswer.size()-2] << endl;
			}
            
        }
		
	}
	return 0;
}