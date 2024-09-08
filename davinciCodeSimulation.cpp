// project1 for the Programming Methodology course @SNU

#include <iostream>
#include <fstream>

using namespace std;

enum class TileColor { WHITE, BLACK };
enum class TileStatus { HIDDEN, REVEALED };

struct Tile {
    int number;
    TileColor color;
    TileStatus status;
};

class Player {
private:
    Tile tiles[13];
    int numTiles = 0;
public:
    // stores the information of the previous guesses
    // initialize prevTargetNumber, prevTargetColor to any random value
    int prevTargetNumber = INT_MAX;
    TileColor prevTargetColor = TileColor::WHITE; 
    int possibleNumbers[12] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};

    // setter- update prevTargetNumber & prevTargetColor
    void updatePrevValues(int number, TileColor color) {
        prevTargetNumber = number;
        prevTargetColor = color;
    }

    // add new Tile to tiles array
    void addTile(Tile tile) {
        tiles[numTiles] = tile;
        numTiles++;
    }

    // getter for Tile at index, as tiles array is private
    Tile& getTile(int index) {
        if (index < numTiles) return tiles[index];
        else {
            cerr << "Out of tile index!" << endl;
            exit(1);
        } 
    }

    // getter for private variable numTiles
    int getNumTiles() {
        return numTiles;
    }

    // print current tiles status
    void displayTiles(ofstream& ofs) {
        
        // the output format

        sortTiles();
        for (size_t i = 0; i < numTiles; i++) {
            ofs << tiles[i].number;
            ofs << (tiles[i].color == TileColor::WHITE ? "w" : "b") << "(";
            ofs << (tiles[i].status == TileStatus::HIDDEN ? "H" : "R") << ")";
            if (i != numTiles - 1) ofs << ", ";
        }
    }

    // if the Tile at index is revealed return true, else return false
    bool isTileRevealed(int index) {
        if (index < numTiles) return tiles[index].status == TileStatus::REVEALED;
        else {
            cerr << "Out of tile index!" << endl;
            exit(1);
        }
    }

    // sort tiles in ascending order & in "black < white" rule
    void sortTiles() {
        for(int i = 0; i < numTiles-1; i++) {
            // I will apply the selection sort method (with minor changes)
            Tile min = tiles[i];
            int min_idx = i;
            for(int j = i+1; j < numTiles; j++) {
                if(tiles[min_idx].number > tiles[j].number) {
                    min = tiles[j];
                    min_idx = j;
                }
                if(tiles[min_idx].number == tiles[j].number) {
                    // same number - then the black one should be "min"
                    if(tiles[j].color == TileColor::BLACK) {
                        min = tiles[j];
                        min_idx = j;
                    }

                    // if tiles[i].color == TileColor::BLACK, then do nothing
                    // because tiles[i] is already min
                }                
            }
            //swap tiles[i] & tiles[min_idx]
            Tile temp;
            temp = tiles[i];
            tiles[i] = tiles[min_idx];
            tiles[min_idx] = temp;
        }
    }

    // true if all tiles are revealed, false if not
    bool allTilesRevealed() {
        for(int i = 0; i < numTiles; i++) {
            if(tiles[i].status == TileStatus::HIDDEN)
                return false;
        }

        return true;

    }

    // true if no tiles are revealed, false if not
    bool noTilesRevealed() {
        for(int i = 0; i < numTiles; i++) {
            if(tiles[i].status == TileStatus::REVEALED){
                return false;
            }
        }

        return true;
    }

    // if Tile with the number "tileNum" and with the color "tileColor" exists in the player's deck
    bool ifTileExists(int tileNum, TileColor tileColor) {
        for(int i = 0; i < numTiles; i++) {
            if(tiles[i].number == tileNum && tiles[i].color == tileColor) {
                return true;
            }
        }
        
        // doesn't exist
        return false;
    }

    // if Tile with the number "tileNum" and with the color "tileColor" is revealed now
    bool ifTileIsRevealed(int tileNum, TileColor tileColor) {
        for(int i = 0; i < numTiles; i++) {
            if(tiles[i].number == tileNum && tiles[i].color == tileColor && tiles[i].status == TileStatus::REVEALED)
                return true;
        }

        // doesn't exist or isn't revealed
        return false;
    }

    // reveal Tile with the number "tileNum", and with the color "tileColor"
    void revealTile(int tileNum, TileColor tileColor) {
        for(int i = 0; i < numTiles; i++) {
            if(tiles[i].number == tileNum && tiles[i].color == tileColor) {
                // reveal tiles[i]
                tiles[i].status = TileStatus::REVEALED;
            }
        }
    }

    void revealTile(int index) {
        tiles[index].status = TileStatus::REVEALED;
    }

};

class DaVinciCodeGame {
private:
    Tile centerTiles[24];
    Player players[4];
    int numCenter = 24;
    int numPlayers;
    int currentPlayerIndex;

    //every player gets 4 tiles at the start
    const int INITIAL_TILE_NUM = 4;
    
public:
    DaVinciCodeGame(int num) {
        numPlayers = num;
        currentPlayerIndex = 0;
        initialize();
        initializeTiles();
        initializePlayerTiles();
    }

    void initialize() {
        // no need to use this function
    }

    // Get card from the centerTiles deck
    Tile popCenter() {
        if (numCenter < 0) {
            cerr << "Out of center tile index!" << endl;
            exit(1);
        }

        Tile clueTile;

        // the clue tile is the very first tile at the center tiles pile
        clueTile = centerTiles[0];

        // delete one tile from centerTiles
        numCenter--;

        // shift centerTiles
        for(int i=0; i<numCenter-1; i++) {
            centerTiles[i] = centerTiles[i+1];
        }

        return clueTile;
    }   

    // Put all cards in the input.txt file into centerTiles array
    void initializeTiles() {

        ifstream inputStream;
        inputStream.open("input.txt");

        string inputTileInfo;
        int currentCardNum = 0; //tracks the number of cards extracted from input.txt
        while(inputStream >> inputTileInfo) {
            if(currentCardNum == 24) { break; }
            Tile inputTile = makeTileFromInfo(inputTileInfo);
            centerTiles[currentCardNum] = inputTile;
            currentCardNum ++;
        }

        inputStream.close();

    }

    // initialize Tiles for every player - 4 Tiles each
    void initializePlayerTiles() {

        // we assume there are only 2 players

        // initialize for player1
        for(int i = 0; i < INITIAL_TILE_NUM; i++) { players[0].addTile(centerTiles[i]); }

        // initialize for player2
        for(int i = INITIAL_TILE_NUM; i < INITIAL_TILE_NUM*2; i++) { players[1].addTile(centerTiles[i]); }

        // delete taken cards from centerTiles by shifting
        for(int i = INITIAL_TILE_NUM*2; i < numCenter; i++) { centerTiles[i-INITIAL_TILE_NUM*2] = centerTiles[i]; }

        // reset current numCenter
        numCenter = 16; // because 8 cards are taken away from players 1 & 2

    }

    // make Tile object based on the information of input string
    Tile makeTileFromInfo (string inputTileInfo) {
        Tile tile;

        if(inputTileInfo.length() == 2) {

            // extract tile number info
            char number = inputTileInfo[0];
            int tileNumber = number - '0';
            
            tile.number = tileNumber;

            // extract tile color info
            char color = inputTileInfo[1];
            if (color == 'w') {
                tile.color = TileColor::WHITE;
            }
            else { //color == 'b'
                tile.color = TileColor::BLACK;
            }
        }

        else if (inputTileInfo.length () == 3) {
            char number_digit2 = inputTileInfo[0];
            char number_digit1 = inputTileInfo[1];
            int tileNumber = (number_digit2 - '0') * 10 + (number_digit1 - '0');

            tile.number = tileNumber;

            // extract tile color info
            char color = inputTileInfo[2];
            if (color == 'w') {
                tile.color = TileColor::WHITE;
            }
            else { //color == 'b'
                tile.color = TileColor::BLACK;
            }
        }

        else {
            // invalid input
            cerr << "Invalid input- check input.txt file" << endl;
            exit(1);
        }

        // initial tiles are all hidden
        tile.status = TileStatus::HIDDEN;

        return tile;
    }

    void displayPlayersTiles(ofstream& ofs) {

        // the output format

        for (int i = 0; i < numPlayers; ++i) {
            ofs << "Player " << i + 1 << "'s Tile : [ ";
            players[i].displayTiles(ofs);
            ofs << " ]";
            ofs << endl;
        }
    }

    // decide the Tile index which the player would make a guess
    int setGuessTarget(int targetPlayerIndex) {
        // get the first HIDDEN Tile index from the left
        for(int i = 0; i < players[targetPlayerIndex].getNumTiles(); i++) {
            // if target player's i-th Tile is HIDDEN, return that index
            if(players[targetPlayerIndex].getTile(i).status == TileStatus::HIDDEN) {
                return i;
            }
        }

        // if all Tiles are revealed, the game should end
        exit(1);
    }

    // decide the number to guess for the target Tile
    int setGuessNumber(int targetTileIndex, Player& targetPlayer, Player& currentPlayer, ofstream& ofs) {
        // basic infos
        int targetTileNumber = targetPlayer.getTile(targetTileIndex).number;
        TileColor targetTileColor = targetPlayer.getTile(targetTileIndex).color;

        // make the guess number

        // get the Tile of targetTileColor with the smallest number 
        // that is (1) not in the previous guesses, (2) un-REVEALED, (3) not in current player's deck

        // check if the player has previous guessess for the current target
        
        int possibleNumbers[12];

        // (1) handle previous guesses
        // has previous guessess
        if(targetTileNumber == currentPlayer.prevTargetNumber && targetTileColor == currentPlayer.prevTargetColor) {
            // copy the previous guessess
            copy(currentPlayer.possibleNumbers, currentPlayer.possibleNumbers + 12, possibleNumbers);
        }

        // doesn't have previous guessess
        else {
            // initialize possibleNumbers array
            int initialPossibleNumbers[12] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};
            copy(initialPossibleNumbers, initialPossibleNumbers + 12, possibleNumbers);
        }

        // (2) delete numbers that are revealed, and the numbers smaller than them        
        // if at least one tile is revealed
        if(!targetPlayer.noTilesRevealed() && targetTileIndex > 0) {
            // get the max number, color of revealed Tiles (= the right-est Tile) of target player
            Tile lastRevealedTile = targetPlayer.getTile(targetTileIndex-1);
            int lastRevealedNum = lastRevealedTile.number;
            TileColor lastRevealedColor = lastRevealedTile.color;

            if(targetTileColor == lastRevealedColor) {
                // delete numbers 0 ~ lastRevealedNum
                for(int i = 0; i <= lastRevealedNum; i++) {
                    possibleNumbers[i] = INT_MAX;
                }
            }

            else if(targetTileColor == TileColor::WHITE && lastRevealedColor == TileColor::BLACK) {
                // delete numbers 0 ~ (lastRevealedNum-1)
                for(int i = 0; i <= (lastRevealedNum-1); i++) {
                    possibleNumbers[i] = INT_MAX;
                }
            }

            else if(targetTileColor == TileColor::BLACK && lastRevealedColor == TileColor::WHITE) {
                // delete numbers 0 ~ lastRevealedNum
                for(int i = 0; i <= lastRevealedNum; i++) {
                    possibleNumbers[i] = INT_MAX;
                }
            }
        
        }

        // (3) delete numbers in the current player's deck
        // clue tile is already included in the current player's deck

        for(int i = 0; i <= 11; i++) {
            if(currentPlayer.ifTileExists(i, targetTileColor)) {
                // switch the non-possible number to INT_MAX
                possibleNumbers[i] = INT_MAX;
            }
        }

        // find min value from the possibleNumbers array
        int min = possibleNumbers[0];
        int min_idx = 0;
        for(int i = 1; i <= 11; i++) {
            if(min > possibleNumbers[i]) {
                min = possibleNumbers[i];
                min_idx = i;
            }
        }

        // make the guess using the min value of possible numbers
        int guess = min;

        // update possible guessess
        currentPlayer.prevTargetNumber = targetTileNumber;
        currentPlayer.prevTargetColor = targetTileColor;

        // delete guessed number in possibleNumbers
        possibleNumbers[min_idx] = INT_MAX;
        copy(possibleNumbers, possibleNumbers + 12, currentPlayer.possibleNumbers);

        return guess;
    }

    bool makeGuess(int targetTileIndex, Player& targetPlayer, int guessedNumber, ofstream& ofs) {

        // check and return if the guess was correct
        // if the guess was correct, target tile should be revealed

        // variable ifCorrect stores true if the guess is correct
        bool ifCorrect = targetPlayer.getTile(targetTileIndex).number == guessedNumber;

        if(ifCorrect) {
            //correct guess- reveal the target Tile
            targetPlayer.revealTile(targetTileIndex);
        }

        return ifCorrect;
    }

    void play() {
        ofstream ofs;
        TileColor clue_color;
        int clue_number;
        bool turnContinue = false;

        ofs.open("output.txt");
        if (!ofs.is_open()) {
            cerr << "Outout File error!" << endl;
            exit(1);
        }

        // the output format
        ofs << "Game Start!" << endl;

        while (true) {
            displayPlayersTiles(ofs);   

            // the output format
            ofs << "--------------------------------------------------------------------------------------------------" << endl;  
            ofs << "Player " << currentPlayerIndex + 1 << "'s Turn: " << endl;
            Player& currentPlayer = players[currentPlayerIndex];

            if (!currentPlayer.allTilesRevealed() && !turnContinue) { 

                // pick one clue tile
                Tile clueTile = popCenter();
                currentPlayer.addTile(clueTile);

                clue_number = clueTile.number;
                clue_color = clueTile.color;
                
                // sort the tiles pile with the clue tile included
                currentPlayer.sortTiles();

                // the output format

                ofs << "Clue tile received: " << clue_number << (clue_color == TileColor::WHITE ? "w" : "b" ) << endl;
            }

            int targetPlayerIndex, targetTileIndex, guessedNumber = 0;
            targetPlayerIndex = (currentPlayerIndex + 1) % numPlayers ;
            Player& targetPlayer = players[targetPlayerIndex];

            // set targetTileIndex
            targetTileIndex = setGuessTarget(targetPlayerIndex);

            // set guessedNumber
            guessedNumber = setGuessNumber(targetTileIndex, targetPlayer, currentPlayer, ofs);

            // the output format

            ofs << endl << "Target index is: " << targetTileIndex + 1 << ", Guessed number is: " << guessedNumber << endl;

            if (makeGuess(targetTileIndex, targetPlayer, guessedNumber, ofs)) { //guess true

                // targetPlayer.getTile(targetTileIndex).status = TileStatus::REVEALED;

                // the output format
                if (targetPlayer.allTilesRevealed()) {
                    ofs << "\nCorrect guess!\n" ;
                    ofs << "Player " << targetPlayerIndex + 1 << " has lost!" << endl;
                    ofs << "Player " << currentPlayerIndex + 1 << " is the winner!" << endl;
                    return;
                }
                ofs << "Correct guess! Player " << currentPlayerIndex + 1 << " gets another turn. \n" << endl;
                turnContinue = true;
            } else { //guess wrong

                 // the output format
                 
                ofs << "Incorrect guess! Player " << currentPlayerIndex + 1 << " reveals a clue tile. \n" << endl;
                if (!currentPlayer.allTilesRevealed()) {

                    // reveal the clue tile
                    currentPlayer.revealTile(clue_number, clue_color);

                }
                turnContinue = false;
            }

            if (targetPlayer.allTilesRevealed()) {

                // the output format

                ofs << "Player " << targetPlayerIndex + 1 << " has lost!" << endl;
                ofs << "Player " << currentPlayerIndex + 1 << " is the winner!" << endl;
                return;
            }

            if (!turnContinue) {
                currentPlayerIndex = (currentPlayerIndex + 1) % numPlayers;
            }
        }
        ofs.close();
    }
};

int main() {
    DaVinciCodeGame game(2);
    game.play();
    return 0;
}