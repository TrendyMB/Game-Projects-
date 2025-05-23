// BattleShip.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <list>
#include <cstdlib>
#include <ctime> 
#include <random>

// AI modes for the Enemy
#define MODE_HUNTING 0
#define MODE_SINKING 1

// Sinking directions 
#define SINK_NORTH 1
#define SINK_EAST 2
#define SINK_SOUTH 3
#define SINK_WEST 4

// Ship Types
#define SHIP_BATTLESHIP 1
#define SHIP_CARRIER 2
#define SHIP_FRIGATE 3
#define SHIP_SUB 4
#define SHIP_GUNBOAT 5

// Difficulty
#define Difficulty_Easy 1
#define Difficulty_Normal 2

// Win Value
#define Player_Win 1
#define Enemy_Win 2

class SquareObject {
private:
    bool misAShip , mhasChecked , mwasShip = false;
    std::string boolString;
    int shipType = 0;

public:
    SquareObject(bool isShip) { 
        setShip(isShip); 
    }

    void setShip(bool isShip) {
        misAShip = isShip;
        if (misAShip == true) 
        { 
            boolString = "<>"; 
            mwasShip = true;
        }
        else{ boolString = "[]"; }
    }
    bool checked(bool check) 
    { 
        mhasChecked = check; 
        return mhasChecked;
    }
    bool wasShip(bool ws) {
        mwasShip = ws;
        return mwasShip;
    }
    bool hasChecked()
    { 
        return mhasChecked; 
    }
    bool GetAShip() { return misAShip; }
    bool getWasShip(){ return mwasShip; }
    int setShipType(int sT)
    { 
        shipType = sT;
        return shipType;
    }
    int getShipType() { return shipType; }
    std::string getShipString() { return boolString; }
};

class Ship{
private:
    int shipLength, shipType;
    std::string shipName;
public:
    Ship(int len, std::string Name, int type) { 
        shipLength = len;
        shipName = Name;
        shipType = type;
    }
    int getLength() { return shipLength; }
    int getType() { return shipType;  }
    std::string getShipName() { return shipName; }
    
};

class XYpos {
private:
    int xpos = 0, ypos = 0;
public:
    int getX() { return xpos; }
    int getY() { return ypos; }
    int setX(int X) { xpos = X; return xpos; }
    int setY(int Y) { ypos = Y; return ypos; }
};

//Default class within grid
SquareObject slot(false);
std::vector<std::vector<SquareObject>> Grid(10, std::vector<SquareObject>(10, slot));
std::vector<std::vector<SquareObject>> enemyGrid = Grid;
std::vector<std::vector<SquareObject>> playerGrid = Grid;

//Set Ship Structs and list
Ship battleShip(4, "Battleship", SHIP_BATTLESHIP), carrier(5, "Carrier", SHIP_CARRIER), frigate(3, "Frigate", SHIP_FRIGATE), sub(3, "submarine", SHIP_SUB), gunBoat(2, "Gunboat", SHIP_GUNBOAT);
std::list<Ship> ships = { battleShip, carrier, frigate, sub, gunBoat };

//prevous hit data
std::list<SquareObject> playerHits;
std::list<SquareObject> enemyHits;

//Saved Two int Values
XYpos xyPos, savedPos, dirAndOff;

void setColor(int textColor)
{
    //set text colour 
    std::cout << "\033[" << textColor << "m";
}

void resetColour() {
    //return text colour to white
    std::cout << "\033[0m";
}

bool checkSpot(std::vector<std::vector<SquareObject>> checkGrid, int xpos, int ypos, bool hasShip) {

    //Check if position is overlapping grid, if so return false
    if (xpos < 0 || ypos < 0 || xpos > 10 || ypos > 10) {     
        hasShip = false;
    }
    else {

        //if grid slot exists
        if (checkGrid[ypos][xpos].GetAShip() == true) {
            hasShip = true;
        }
        else {
            hasShip = false;
        }
    }
    
    return hasShip;
}

int setDifficulty(int diff) {
    std::string select;
    diff = 0;
    //display difficulty options
    std::cout << "Select difficulty:  " << "\n";
    std::cout << "(Easy) targets randomly regardless of prevous hits  " << "\n";
    std::cout << "(Normal) will focus on surrounding area on hit  " << "\n";
    //Input difficulty
    while (diff == 0) {
        std::cout << "Select: " << "\n";
        std::cin >> select;
        std::cout << "\n";
        //Upper Case
        std::transform(select.begin(), select.end(), select.begin(), ::toupper);
        if (select == "EASY") {
            diff = Difficulty_Easy;
        }
        else if (select == "NORMAL") {
            diff = Difficulty_Normal;
        }
    }
    //Return the selected difficulty to the method
    return diff;
}

void printHits(std::vector<std::vector<SquareObject>> printGrid, int xpos, int ypos, bool seeShips) {
    //tell user which grid has been printed

    if (seeShips) {
        std::cout << "\n" << "Your Board" << "\n";
    }
    else {
        std::cout << "\n" << "Enemy Board" << "\n";
    }
    
    std::cout << "  0 1 2 3 4 5 6 7 8 9";
    for (int i = 0; i < printGrid.size(); i++) {
        //print each slot string in a 10 by 10 grid
        std::cout << "\n";
        std::cout << i;
        for (int j = 0; j < printGrid.size(); j++) {

            if (printGrid[i][j].hasChecked() == true) {
                resetColour();
                //if player has targeted slot
                if (printGrid[i][j].getWasShip() == true) {
                    //set colour to green to indicate hit
                    setColor(32);
                }
                else if (printGrid[i][j].GetAShip() == false) {
                    //set colour to red to indicate miss
                    setColor(31);
                }
               
                //Colour grid to show player targeting
                if (xpos < 10 && j == xpos) { setColor(33); }
                if (ypos < 10 && i == ypos) { setColor(33); }
                if (ypos < 10 && i == ypos && xpos < 10 && j == xpos) { setColor(31); } 
                
                //if player has targeted slot
                if (printGrid[i][j].getWasShip() == true) {
                    //set colour to green to indicate hit
                    std::cout << "<>";
                }
                else if (printGrid[i][j].GetAShip() == false) {
                    //set colour to red to indicate miss
                    std::cout << "[]";
                }
                resetColour();
            }      
            else
            {
                //if player has not targeted slot 
                //Colour grid to show player targeting
                if (xpos < 10 && j == xpos) { setColor(33); }
                if (ypos < 10 && i == ypos) { setColor(33); }
                if (ypos < 10 && i == ypos && xpos < 10 && j == xpos) { setColor(31); }         
                
                //change grid symbol if ship
                if (seeShips == true && printGrid[i][j].GetAShip()){
                    std::cout << "<>";
                }
                else{
                    std::cout << "[]";
                }                
                resetColour(); 
            }
        }
    }

    std::cout << "\n";
}

void aim(std::vector<std::vector<SquareObject>> printGrid, bool isPlayer) {

    std::string moveStr;
    bool cont = false;        
    int xpos = 0;
    int ypos = 0;
   

    while (cont == false) {

        std::string c;
        std::cout << "Type A and D to move on X axis, Type W and S to move on Y axis, Type C to continue: ";
        std::cin >> moveStr;
        //Upper Case
        std::transform(moveStr.begin(), moveStr.end(), moveStr.begin(), ::toupper);

        // add/minsus x y position on the current grid
        if (moveStr == "D") { xpos += 1; }
        if (moveStr == "A") { xpos -= 1; }
        if (xpos > 9) { xpos = 0; }
        if (xpos < 0) { xpos = 9; }  
        if (moveStr == "W") { ypos -= 1; }
        if (moveStr == "S") { ypos += 1; }  
        if (xpos > 9) { xpos = 0; }
        if (xpos < 0) { xpos = 9; }
        if (moveStr == "C") { cont = true; break; }

        //Change Grid depending on requirement
        if (isPlayer) {
            printHits(printGrid, xpos, ypos, true);
        }
        else {
            printHits(printGrid, xpos, ypos, false);
        }
        
        //save current target position to target class 
        xyPos.setX(xpos);
        xyPos.setY(ypos);
    }  
}

void manualSetShips() {

    //Reset Grid to 0
    for (int i = 0; i < playerGrid.size(); i++) {
        for (int j = 0; j < playerGrid.size(); j++) {
            playerGrid[i][j].setShip(false);
            playerGrid[i][j].wasShip(false);
        }
    }

    for (Ship s : ships) {
        // x and y pos values, and direction value
        int xPos = 0, yPos = 0, ori = 0;
        std::string orient;
        
        //access aim method to edit target postion
        aim(playerGrid, true);

        //set current position to the a and y pos on the 
        xPos = xyPos.getX();
        yPos = xyPos.getY();


        //decide the direction of the current ship facee from the back 
        //grid will reset if direction input sets ship beyond grid
        std::cout << "Set ship Orientation (Ship spawns from back) (W): up (A): left (S): Down (D): Right ";
        std::cin >> orient;

        //Upper Case
        std::transform(orient.begin(), orient.end(), orient.begin(), ::toupper);

        for (int i = 0; i < s.getLength(); i++) {

            //Orient ship
            if (orient == "W") {
                //Repeat method if overlap
                if (yPos - i > 9 || xPos > 9 || yPos - i < 0  || xPos < 0 || playerGrid[yPos - i][xPos].GetAShip() == true) {
                    std::cout << "Grid Overlap \n"; manualSetShips();
                }
                playerGrid[yPos - i][xPos].setShip(true);
                playerGrid[yPos - i][xPos].setShipType(s.getType());
            }
            else if (orient == "A") {
                //Repeat method if overlap
                if (yPos > 9 || xPos - i > 9 || yPos < 0 || xPos - i < 0 || playerGrid[yPos][xPos - i].GetAShip() == true) {
                    std::cout << "Grid Overlap \n"; manualSetShips();
                }
                playerGrid[yPos][xPos + i].setShip(true);
                playerGrid[yPos][xPos + i].setShipType(s.getType());
            }    
            else if (orient == "S") {
                //Repeat method if overlap
                if (yPos + i > 9 || xPos > 9 || yPos + i < 0  || xPos < 0 || playerGrid[yPos + i][xPos].GetAShip() == true) {
                    std::cout << "Grid Overlap \n"; manualSetShips();
                }
                playerGrid[yPos + i][xPos].setShip(true);
                playerGrid[yPos + i][xPos].setShipType(s.getType());
            }
            else {
                //Repeat method if overlap
                if (yPos > 9 || xPos + i > 9 || yPos < 0 || xPos + i < 0 || playerGrid[yPos][xPos + i].GetAShip() == true) {
                    std::cout << "Grid Overlap \n"; manualSetShips();
                }
                playerGrid[yPos][xPos + i].setShip(true);
                playerGrid[yPos][xPos + i].setShipType(s.getType());
            }

            
        }
        //update grid with additions 
        printHits(playerGrid, 10, 10, true);
    }    


    //declare ships set
    std::cout << "Ships Set \n";
    return;
}

std::vector<std::vector<SquareObject>> randSetShips(std::vector<std::vector<SquareObject>> currentGrid) {

    //Set time to get random result every loop
    std::srand(time(0));

    //loop parameters
    bool layoutGood = false;
    bool thisLayoutGood = true;

    while (!layoutGood) {

        //bool to create while loop that will be broken if ship placements overlap or go off grid
        thisLayoutGood = true;

        //Reset Grid to 0
        for (int i = 0; i < currentGrid.size(); i++) {
            for (int j = 0; j < currentGrid.size(); j++) {
                currentGrid[i][j].setShip(false);
                currentGrid[i][j].wasShip(false);
                currentGrid[i][j].setShipType(0);
            }
        }  

        //Go through list of ship types 
        for (Ship s : ships) {

            //Set ship x and y position on grid
            //set one of the four directions the ship can face in (from the back)
            int xPos = std::rand() % 10;
            int yPos = std::rand() % 10;
            int orient = std::rand() % 4;

            //Get the length of the current ship type and extend to that length on the grid 
            for (int i = 0; i < s.getLength(); i++) {

                std::cout << s.getType() << "\n";
                //Orient ship
                if (orient == 0) {
                    //Repeat method if overlap
                    if (yPos + i > 9 || xPos > 9 || yPos + i < 0 || xPos < 0 || currentGrid[yPos + i][xPos].GetAShip() == true) {
                        std::cout << "Grid Overlap 1\n";
                        thisLayoutGood = false;
                        break;
                    }
                    currentGrid[yPos + i][xPos].setShip(true);
                    currentGrid[yPos + i][xPos].setShipType(s.getType());
                }
                else if (orient == 1) {
                    //Repeat method if overlap
                    if (yPos > 9 || xPos - i > 9 || yPos < 0 || xPos - i < 0 || currentGrid[yPos][xPos - i].GetAShip() == true) {
                        std::cout << "Grid Overlap 2\n";
                        thisLayoutGood = false;
                        break;
                    }
                    currentGrid[yPos][xPos - i].setShip(true);
                    currentGrid[yPos][xPos - i].setShipType(s.getType());
                }
                else if (orient == 2) {
                    //Repeat method if overlap
                    if (yPos - i > 9 || xPos > 9 || yPos - i < 0 || xPos < 0 || currentGrid[yPos - i][xPos].GetAShip() == true) {
                        std::cout << "Grid Overlap 3\n";
                        thisLayoutGood = false;
                        break;
                    }
                    currentGrid[yPos - i][xPos].setShip(true);
                    currentGrid[yPos - i][xPos].setShipType(s.getType());
                }
                else if (orient == 3) {
                    //Repeat method if overlap
                    if (yPos > 9 || xPos + i > 9 || yPos < 0 || xPos + i < 0 || currentGrid[yPos][xPos + i].GetAShip() == true) {
                        std::cout << "Grid Overlap 4\n";
                        thisLayoutGood = false;
                        break;
                    }
                    currentGrid[yPos][xPos + i].setShip(true);
                    currentGrid[yPos][xPos + i].setShipType(s.getType());
                }
            }

        }
        layoutGood = thisLayoutGood;
    }

    //
    return currentGrid;
}

bool focusedSearch = false;
void enemyAttackPlayer(int diff) {

    //Set direction and offset values
    //direction and offset ints are stored in an XYpos class
    //XYpos class is updated at the end of method with the altered direction and offset values, to keep values consistent on loop
    int direction = dirAndOff.getX();
    int offset = dirAndOff.getY();

    //overlap Parameter
    bool gridOverlap = false;

    //Set values
    std::srand(time(0));
    int hitXPos = 0;
    int hitYpos = 0;

    //revert direction to zero after 
    if (direction > 4) { 
        direction = 0; 
        offset = 1;
        savedPos.setX(0);
        savedPos.setY(0);
        focusedSearch = false;
    }

    //set direction
    if (direction == 0) { 
        offset = 1;
        //get random position
        hitXPos = std::rand() % 10;
        hitYpos = std::rand() % 10;
    }
    else if (direction == SINK_NORTH) {

        //Set targeted position to prevous succeccful hit plus north offset
        hitXPos = savedPos.getX();
        hitYpos = savedPos.getY() - offset;

        //automiss if grid overlap
        if (hitYpos < 0 || hitYpos > 9) {
            gridOverlap = true;
        }
    }
    else if (direction == SINK_EAST) {

        //Set targeted position to prevous succeccful hit plus east offset
        hitXPos = savedPos.getX() + offset;
        hitYpos = savedPos.getY();

        //automiss if grid overlap
        if (hitXPos < 0 || hitXPos > 9) {
            gridOverlap = true;
        }
    }
    else if (direction == SINK_SOUTH) {

        //Set targeted position to prevous successful hit plus south offset
        hitXPos = savedPos.getX();
        hitYpos = savedPos.getY() + offset;

        //automiss if grid overlap
        if (hitYpos < 0 || hitYpos > 9) {
            gridOverlap = true;
        }
    }
    else if (direction == SINK_WEST) {

        //Set targeted position to prevous successful hit plus west offset
        hitXPos = savedPos.getX() - offset;
        hitYpos = savedPos.getY();

        //automiss if grid overlap
        if (hitXPos < 0 || hitXPos > 9) {
            gridOverlap = true;
        }
    }
   
    //if the target postion overlaps grid auto miss
    bool hit = false;
    if (gridOverlap == false) {
        //set hit success
        hit = checkSpot(playerGrid, hitXPos, hitYpos, hit);
        
    }
    else {
        std::cout << "Enemy Overlap \n";
    }

    //If hit bool returns true
    if (hit) {
        std::cout << "Enemy hit player \n";

        //if enemy is not already in focused search mode and the difficulty is higher than easy begin focused search mode
        //when in focused search mode script will check clockwise around the successful hit
        // on each go will move up 1 space in each direction until miss
        // on miss will move one space around hit untill all sides checked
        // on all sides checked set focused search to false and return to random search
        if (focusedSearch == false && diff != Difficulty_Easy) {
            //starting focused search mode set saved position to current successful hit
            savedPos.setX(hitXPos);
            savedPos.setY(hitYpos);

            //set direction to north (int 1) to begin 4 sided check
            direction = SINK_NORTH;
            focusedSearch = true;
        }
        else {
            //on successful hit, if focused search mode increase target offset by 1 
            offset += 1;
        }                  
        playerGrid[hitYpos][hitXPos].setShip(false);
    }
    else {

        //on enemy miss if in focused search mode, check next direction and reset check distance to 1
        std::cout << "Enemy miss player \n";
        if (focusedSearch == true) {
            offset = 1;
            direction += 1;
        }   
    }

    //Set array ints for next round
    dirAndOff.setX(direction);
    dirAndOff.setY(offset);
}

void playerAttackEnemy() {
    int xpos, ypos;
    std::string Orient;
    //Update Player on Game
    aim(enemyGrid, false);
    xpos = xyPos.getX();
    ypos = xyPos.getY();

    bool hit = false;
    //set current square as prevously checked
    enemyGrid[ypos][xpos].checked(true);
    hit = checkSpot(enemyGrid, xpos, ypos, hit);
    if (hit) {
        std::cout << "Player hit enemy \n";
        
        //remove ship from grid and as position to hit list
        enemyGrid[ypos][xpos].setShip(false);
        playerHits.insert(playerHits.end(), enemyGrid[ypos][xpos]);
    }
    else {
        //update player on game
        std::cout << "Player miss enemy\n";
    }
}

int countShip(int playerWins, int eniWins) {

    //integers for, the rounds squaues containing ships on each grid and winner int 1 is player 2 is enemy 
    int eShipCount = 0;
    int pShipCount = 0;
    int winner = 0;

    //int number of player ships 
    int pBattleShip = 0;
    int pCarrier = 0;
    int pFrigate = 0;
    int pSub = 0;
    int pGunBoat = 0;

    //int number of enemy ships 
    int eBattleShip = 0;
    int eCarrier = 0;
    int eFrigate = 0;
    int eSub = 0;
    int eGunBoat = 0;

    for (int i = 0; i < playerGrid.size(); i++) {
        for (int j = 0; j < playerGrid.size(); j++) {

            //if position on grid has ship set on it, at to the respective player value
            if (playerGrid[i][j].GetAShip()) { pShipCount++; }
            if (enemyGrid[i][j].GetAShip()) { eShipCount++; }

            //Add player ship num
            if (playerGrid[i][j].getShipType() == SHIP_BATTLESHIP) { pBattleShip++; }
            if (playerGrid[i][j].getShipType() == SHIP_CARRIER) { pCarrier++; }
            if (playerGrid[i][j].getShipType() == SHIP_FRIGATE) { pFrigate++; }
            if (playerGrid[i][j].getShipType() == SHIP_SUB) { pSub++; }
            if (playerGrid[i][j].getShipType() == SHIP_GUNBOAT) { pGunBoat++; }

            //Add enemy ship num
            if (enemyGrid[i][j].getShipType() == SHIP_BATTLESHIP) { eBattleShip++; }
            if (enemyGrid[i][j].getShipType() == SHIP_CARRIER) { eCarrier++; }
            if (enemyGrid[i][j].getShipType() == SHIP_FRIGATE) { eFrigate++; }
            if (enemyGrid[i][j].getShipType() == SHIP_SUB) { eSub++; }
            if (enemyGrid[i][j].getShipType() == SHIP_GUNBOAT) { eGunBoat++; }
        }
    }

    //tell user there own and enemies number of square set as ships
    std::cout << "Remaining player points: " << pShipCount << " \n";
    std::cout << "Remaining Enemy points: " << eShipCount << " \n";

    //Player ship squares
    std::cout << "Player Ships: \n";
    if (pBattleShip <= 0) {
        std::cout << "Battleship Sunk \n";
    }
    if (pCarrier <= 0) { 
        std::cout << "Carrier Sunk \n";
    }
    if (pFrigate <= 0) {  
        std::cout << "Frigate Sunk \n";
    }
    if (pSub <= 0) {
        std::cout << "Submarine Sunk \n";
    }
    if (pGunBoat <= 0) {
        std::cout << "Gunboat Sunk \n";
    }
    
    //Enemy ship squares
    std::cout << "Enemy Ships: \n";
    if (eBattleShip <= 0) {
        std::cout << "Battleship Sunk \n";
    }
    if (eCarrier <= 0) {
        std::cout << "Carrier Sunk \n";
    }
    if (eFrigate <= 0) {
        std::cout << "Frigate Sunk \n";
    }
    if (eSub <= 0) {
        std::cout << "Submarine Sunk \n";
    }
    if (eGunBoat <= 0) {
        std::cout << "Gunboat Sunk \n";
    }

    if (eShipCount <= 0) {   
        //add player win to game file
        std::ofstream winFile;
        winFile.open("playerWins.txt");
        playerWins += 1;
        winFile << playerWins;
        winFile.close();

        //set winner interger to trigger message
        winner = Player_Win;
    }
    if (pShipCount <= 0) { 
        //add enemy win to game file
        std::ofstream loseFile;
        loseFile.open("enemyWins.txt");
        eniWins += 1;
        loseFile << eniWins;
        loseFile.close();

        //set winner interger to trigger message
        winner = Enemy_Win;
    }

    //return winner int
    return winner;
}

int playerWinCount(int pWins) {
    //win counts
    std::ifstream winFile;
    winFile.open("playerWins.txt");

    //Update File data
    winFile >> pWins;

    //close files  
    winFile.close();

    //print 
    std::cout << "Number of past player wins: " << pWins << "\n";

    //return value
    return pWins;
}

int enemyWinCount(int eWins) {
    //win counts
    std::ifstream loseFile;
    loseFile.open("enemyWins.txt");

    //Update File data
    loseFile >> eWins;

    //close files
    loseFile.close();

    //print
    std::cout << "Number of past computer wins: " << eWins << "\n";

    //return value
    return eWins;
}

std::string gameMode(std::string mode) {    
    
    
    //repeat untill player gives acceptable input
    bool loop = true;
    while (loop) { 

        // check if user wants to auto add ships
        std::cout << "automatically place your ships yes/no? ";    
        std::cin >> mode;

        //change string to upper case
        std::transform(mode.begin(), mode.end(), mode.begin(), ::toupper);   

        //check input
        if (mode == "YES" || mode == "NO") {

            //end loop and proceed
            loop = false;
            break;
        }
    }
    return mode;
}

int main()
{        
    
    int playerWins = 0, enemyWins = 0, winner = 0;
    playerWins = playerWinCount(playerWins); 
    enemyWins = enemyWinCount(enemyWins);

    //Set Game
    int diffNum = 0;
    diffNum = setDifficulty(diffNum);

    //set the enemies grid
    enemyGrid = randSetShips(enemyGrid);

    //Allow player to manually/automatically place ships
    std::string automode;

    //Decide if the player will get there 
    automode = gameMode(automode);


    if (automode == "YES") {
        //randomly set player ships 
        playerGrid = randSetShips(playerGrid);
    }
    else {
        //player set ships
        manualSetShips();
    }
    
    //Core Game Loop
    bool gameInPlay = true;
    while (gameInPlay) {
        playerAttackEnemy();
        enemyAttackPlayer(diffNum);
        winner = countShip(playerWins, enemyWins);
        if (winner != 0) {
            //End loop if winner is found
            gameInPlay = false;
            break;
        }
        printHits(playerGrid, 10, 10, true);  
        printHits(enemyGrid, 10, 10, false);
    }

    if (winner == Player_Win) //winner int is 1
    {
        std::cout << "Player Wins! ";
    }
    else if(winner == Enemy_Win)//winner int is 2
    {
        std::cout << "Enemy Wins! ";
    }
}