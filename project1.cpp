//B99292359FFD910ED13A7E6C7F9705B8742F0D79
//  project1.cpp
//  project1
//
//  Created by Nathan Katzman on 1/25/21.
//

#include <iostream>
#include <deque>
#include <vector>
#include <algorithm>
#include <cmath>
#include <fstream>
#include <getopt.h>
#include <cstring>
#include <string>
#include "xcode_redirect.hpp"
using namespace std;

class location {
public:
    size_t room_num;
    size_t row;
    size_t col;
    
    location() {
        room_num = (size_t) -1;
        row = (size_t) -1;
        col = (size_t) -1;
    }
    location(size_t room_num_in, size_t row_in, size_t col_in){
        room_num = room_num_in;
        row = row_in;
        col = col_in;
    }
    bool operator ==(const location &rhs) {
        return room_num == rhs.room_num && row == rhs.row && col == rhs.col;
    }
};

class Room{
public:
    vector < vector <char> > room;
    vector < vector <location> > backtracker;
    
    Room(size_t room_size) {
        room = vector < vector <char> >
            (room_size, vector<char>(room_size, '.'));
        backtracker = vector < vector<location> >(room_size, vector <location>(room_size, location()));
    }
    
    // vector contructor:
    //  1) size of the vector we want to create
    //  2) value of each element in the vector that we're creating.
};

class tracker{
public:
    size_t num_rooms;//stores the amount of rooms we can use
    size_t room_size;//Stores how big the room is
    
    vector <Room> rooms;
    deque <location> dq;
    
    location countess_location;
    location starting_location;
    
    tracker(size_t num_rooms_in, size_t room_size_in)
    : num_rooms(num_rooms_in), room_size(room_size_in)  {
        rooms = vector<Room>(num_rooms, Room(room_size_in));
    }
};

tracker *read_variables(){
    char character;
    size_t num_rooms, room_size;
    
    cin >> character;
    cin >> num_rooms;
    cin >> room_size; // Our overall room will be of size: room_size x room_size
    
    tracker *map =  new tracker(num_rooms, room_size);
    
//    x->rooms[0].room[0][0] = '1';
//    Room &curr_room = x->rooms[0];
//    curr_room.room[0][1] = '2';
    string line;
    getline(cin, line, '\n'); // Reads in garbage input of newline character
    if(character == 'M'){
        for (size_t room_num = 0; room_num < num_rooms; ++room_num) {
            Room &curr_room = map->rooms[room_num];
            for (size_t room_row = 0; room_row < room_size; ++room_row) {
                getline(cin, line, '\n');
                while (line[0] == '/' && line[1] == '/') {
                    getline(cin, line, '\n');
                }
                for(size_t room_col = 0; room_col < room_size; ++room_col){
                    char c = line[room_col];
                    curr_room.room[room_row][room_col] = c;
                    // Countess represented by 'C'
                    if (c == 'C') {
                        map->countess_location.room_num = room_num;
                        map->countess_location.row = room_row;
                        map->countess_location.col = room_col;
                        
                        cerr << "Countess found at " << room_num << room_row << room_col << endl;
                    }
                    if(c == 'S') {
                        map->starting_location = location(room_num, room_row, room_col);
                    }
                }
            }
        }
    }
    else if(character == 'L'){
        while (cin) {
            getline(cin, line, '\n');
            if (line[0] == '/' && line[1] == '/') {
                continue;
            }
            if (line == "") {
                break;
            }
            line.pop_back();
            char c = line.back();
            size_t room_num = ((size_t) line[1]) - 48;
                        
            size_t second_comma_index = line.find(',', 3);
            string row_in_string = line.substr(3,second_comma_index - 3);
            string col_in_string = line.substr(second_comma_index + 1,line.length()
                                               - second_comma_index - 3);
            size_t room_row = (size_t) stoi(row_in_string);
            size_t room_col = (size_t) stoi(col_in_string);
            map->rooms[room_num].room[room_row][room_col] = c;
            
            if (c == 'C') {
                map->countess_location.room_num = room_num;
                map->countess_location.row = room_row;
                map->countess_location.col = room_col;
                
                cerr << "Countess found at " << room_num << room_row << room_col << endl;
            }
            if(c == 'S') {
                map->starting_location = location(room_num, room_row, room_col);
            }
            
            cout << "line: " << line <<
            " and row: " << row_in_string << " and col: " << col_in_string << endl;
        }
    }
    else{
        cerr << "DID NOT CHOOSE M OR L ERROR!!!" << endl;
        exit(1);
    }
    return map;
}

bool runForwardAlgo(tracker *map, bool queue_mode) {
    map->dq.push_back(map->starting_location);
    while(!map->dq.empty()){
        location curr_location;
        if (queue_mode) {
            curr_location = map->dq.front();
            map->dq.pop_front();
        }
        else {
            curr_location = map->dq.back();
            map->dq.pop_back();
        }

        char curr_char = map->rooms[curr_location.room_num].room[curr_location.row][curr_location.col];
        // Pop from the deque (done)
        // if it's a warp pipe, then we add the location in the new room to the deque
        // if it's not a warp pipe, then we add the n,e,s,w locations to the deque
        // for each location we add to the deque:
        //     if it's out of bounds or it's not a . or a C, don't add it
        //     only add it if it's a . or a C, AND it's in bounds.
        //     if we did add it (meaning it's a . or a C), then also add
        //     the current location into the backtracking vectors
        //     if we find C, we stop.
        vector<location> next_locations;
        if (curr_char >= '0' && curr_char <= '9') {
            // curr_location is a warp pipe
            next_locations.push_back(location(((size_t) curr_char) - 48, curr_location.row, curr_location.col));
        }
        else {
            // curr_location is not a warp pipe
            next_locations.push_back(location(curr_location.room_num, curr_location.row - 1, curr_location.col));
            next_locations.push_back(location(curr_location.room_num, curr_location.row, curr_location.col + 1));
            next_locations.push_back(location(curr_location.room_num, curr_location.row + 1, curr_location.col));
            next_locations.push_back(location(curr_location.room_num, curr_location.row, curr_location.col - 1));
        }
        for (size_t i = 0; i < next_locations.size(); i++) {
            location next = next_locations[i];
            if (next.row >= map->room_size || next.col >= map->room_size) {
                continue;
            }
            char next_char = map->rooms[next.room_num].room[next.row][next.col];
            if((next_char == 'C' || next_char == '.' ||
                (next_char >= '0' && next_char <= '9')) &&
               map->rooms[next.room_num].backtracker[next.row][next.col] == location()) {
                map->dq.push_back(next);
                // TODO: add into backtracking vectors
                map->rooms[next.room_num].backtracker[next.row][next.col] = curr_location;
                if(next_char == 'C'){
                    return true;
                }
            }
        }
    }
    return false;
}

char direction(location &l1, location &l2) {
    if(l1.room_num != l2.room_num ){
        return 'p';
    }
    if(l1.row > l2.row){
        return 'n';
    }
    if(l1.row < l2.row){
        return 's';
    }
    if(l1.col > l2.col){
        return 'w';
    }
    return 'e';
}

void runBackAlgo(tracker *map, bool map_mode){
    vector <location> path;
    
    location current_location = map->countess_location;
    path.push_back(current_location);
    while (true) {
        location prev_location = map->rooms[current_location.room_num].backtracker[current_location.row][current_location.col];
        if(prev_location == location()){
            break;
        }
        
        path.push_back(prev_location);//push_back = add to the back
        current_location = prev_location;
    }
    // Print the path
    if (map_mode) {
        
    }
    else {
        cout << "Path Taken:" << endl;
        current_location = map->starting_location;
        location next_location;
        for(int i = (int) path.size() - 2; i >= 0; --i) {
            next_location = path[(size_t) i];
            cout << '(' << current_location.room_num << ','<< current_location.row << ',' << current_location.col << ',' << direction(current_location, next_location) << ')' << endl;
            current_location = next_location;
        }
    }
}

void getOptions(int argc, char** argv, bool &queue_mode, bool &map_mode){
    string mode;
    int option = 0;
    
    opterr = false;
    struct option Long_options[] = {
        {"stack", no_argument, nullptr, 's'},
        {"queue", no_argument, nullptr, 'q'},
        {"output", required_argument, nullptr, 'o'},
        {"help", no_argument, nullptr, 'h'},
    };
    int option_index = 0;
    while ((option = getopt_long(argc, argv, "sqo:h", Long_options,
                                 &option_index)) != -1) {
        switch(option){
            case 's':{
                queue_mode = false;
                break;
            }
            case 'q':{
                queue_mode = true;
                break;
            }
            case 'o':{
                if (!strcmp(optarg, "M")) {
                    map_mode = true;
                }
                else if (!strcmp(optarg, "L")) {
                    map_mode = false;
                }
                else {
                    cerr << "DID NOT USE M OR L FOR OUTPUT (-O)" << endl;
                    exit(1);
                }
                break;
            }
            case 'h':{
                cout << "Help!" << endl;
                cout << "This program reads data, creates a map, location" << endl;
                cout << ", reads the data, and creates a brand new map for" << endl;
                cout << "for Marco or Linguino to beat enemies, avoid obstacles" << endl;
                cout << "and save the countess from Louser" << endl;
                exit(0);
                //should give a printed message...
                break;
            }
            default:{
                cerr << "Error!" << endl;
                exit(1);
            }
        }
    }
}


int main(int argc, char* argv[]){
    xcode_redirect(argc, argv);
    bool queue_mode;
    bool map_mode;
    getOptions(argc, argv, queue_mode, map_mode);
    
    tracker *map = read_variables();
    bool foundCountess = runForwardAlgo(map, queue_mode);
    runBackAlgo(map, map_mode);
    
    if (!foundCountess) {
        cout << "No solution, " << 0 << " tiles discovered." << endl;
    }
    
    // Step 1: Think about what containers to hold information
    // Step 2: Read in all information into containers
    // Step 3: fix compile errors
    // Step 4: print it out
    
//    for (size_t i = 0; i < map->rooms.size(); ++i) {
//        cout << "Room number " << i << endl;
//        for(size_t j = 0; j < map->rooms[i].room.size(); ++j){
//            cout << "row: ";
//            for(size_t k = 0; k < map->rooms[i].room[j].size(); ++k){
//        // Print each room map.rooms[i]
//                cout << map->rooms[i].room[j][k];
//            }
//            cout << endl;
//        }
//    }
    
}






//(not final functions, still need work)
//How does the stack work, how to use it in practice, how to refer to class?

//Making maplayout a class will be a good idea, since it lets you
//see the map and it is stored with a class.

