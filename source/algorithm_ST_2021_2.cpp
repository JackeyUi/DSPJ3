#include <iostream>
#include <stdlib.h>
#include <algorithm>
#include <time.h>
#include "../include/algorithm.h"
#define INF 300
using namespace std;


#define D 4

/******************************************************
 * In your algorithm, you can just use the the funcitons
 * listed by TA to get the board information.(functions
 * 1. ~ 4. are listed in next block)
 *
 * The STL library functions is not allowed to use.
******************************************************/

/*************************************************************************
 * 1. int board.get_orbs_num(int row_index, int col_index)
 * 2. int board.get_capacity(int row_index, int col_index)
 * 3. char board.get_cell_color(int row_index, int col_index)
 * 4. void board.print_current_board(int row_index, int col_index, int round)
 *
 * 1. The function that return the number of orbs in cell(row, col)
 * 2. The function that return the orb capacity of the cell(row, col)
 * 3. The function that return the color fo the cell(row, col)
 * 4. The function that print out the current board statement
*************************************************************************/


Player playerR108061102('r'), playerB108061102('b');
class PT102{
    public:
        int orbs_num;   // The number of the orbs on the cell
        int capacity;   // The max number of the orbs of a cell
        char color;     // The color of the cell, 'w'(No one has placed the orbs), 'r'(Red Player) and 'b'(Blue player)
        bool explode;

        PT102();
        PT102(const PT102&);
        // The basic functions of the Cell
        int get_orbs_num();
        int get_capacity();
        char get_color();
        bool get_explode();

        void set_orbs_num(int orbs);
        void set_capacity(int cap);
        void set_color(char col);
        void set_explode(bool tof);

        PT102 operator=(const PT102&);
};

class BD108061102{
    public:
        PT102 cells[ROW][COL];                       // The 5*6 board whose index (0,0) is start from the upper left CN
        void cell_reaction_marker();                // After the explosion, mark all the cell that  will explode in next iteration
        bool cell_is_full(PT102* cell);              // Check wether the cell is full of orbs and set the explosion variable to be true
        void add_orb(int i, int j, char color);     // Add orb into the cell (i, j)
        void cell_reset(int i, int j);              // Reset the cell to the initial state (color = 'w', orb_num = 0)
        void cell_explode(int i, int j);            // The explosion of cell (i, j). It will add the orb into neighbor cells
        void cell_chain_reaction(Player player);    // If there is aa explosion, check wether the chain reaction is active

        BD108061102();
        BD108061102(const BD108061102 &T) {
            for (int i = 0; i < 5; i++) {
                for (int j = 0; j < 6; j++) {
                    cells[i][j] = T.cells[i][j];
                }
            }
        };

        // The basic functions of the Board
        int get_orbs_num(int i, int j);
        int get_capacity(int i, int j);
        char get_cell_color(int i, int j);

        bool place_orb(int i, int j, Player* player);      // Use this function to place a orb into a cell
        void print_current_board(int i, int j, int round); // Print out the current state of the hole board

        bool win_the_game(Player player);                  // The function that is used to check wether the player wins the game after his/her placemnet operation
        BD108061102 operator=(const BD108061102 &);
        //const PT102* operator[] (const int) const;
       /* PT102* operator[] (const int);
        const PT102 * operator[] (const int &x) const {
            return cells[x];
        }*/
};


bool placement_illegal_ST(Player player, PT102 cell){

    bool illegal = false;
    // Check wether the cell has be placed or not
    if(cell.get_color() == 'w') return illegal;

    //Check color
    if(cell.get_color() != player.get_color()) illegal = true;

    return illegal;

}
PT102::PT102(){
    orbs_num = 0, capacity = 8, color='w', explode = false;
}
PT102::PT102(const PT102& P){
    orbs_num = P.orbs_num, capacity = P.capacity, color=P.color, explode = P.explode;
}
int PT102::get_capacity(){
    return this->capacity;
}

int PT102::get_orbs_num(){
    return this->orbs_num;
}

char PT102::get_color(){
    return this->color;
}

bool PT102::get_explode(){
    return this->explode;
}

void PT102::set_orbs_num(int orbs){
    this->orbs_num = orbs;
}

void PT102::set_capacity(int cap){
    this->capacity = cap;
}

void PT102::set_color(char col){
    this->color = col;
}

void PT102::set_explode(bool tof){
    this->explode = tof;
}

PT102 PT102::operator=(const PT102 &P) {

    capacity = P.capacity;
    color = P.color;
    explode = P.explode;
    orbs_num = P.orbs_num;
    return *this;
}
BD108061102::BD108061102(){

    ////// Initialize the borad with correct capacity //////
    // The CNs of the board
    cells[0][0].set_capacity(3), cells[0][5].set_capacity(3),
    cells[4][0].set_capacity(3), cells[4][5].set_capacity(3);

    // The edges of the board
    cells[0][1].set_capacity(5), cells[0][2].set_capacity(5), cells[0][3].set_capacity(5), cells[0][4].set_capacity(5),
    cells[1][0].set_capacity(5), cells[2][0].set_capacity(5), cells[3][0].set_capacity(5),
    cells[1][5].set_capacity(5), cells[2][5].set_capacity(5), cells[3][5].set_capacity(5),
    cells[4][1].set_capacity(5), cells[4][2].set_capacity(5), cells[4][3].set_capacity(5), cells[4][4].set_capacity(5);

}

bool BD108061102::place_orb(int i, int j, Player * player){

    if(!placement_illegal_ST(*player, cells[i][j])){
        int temp = cells[i][j].get_orbs_num();
        temp += 1;
        cells[i][j].set_orbs_num(temp);
        cells[i][j].set_color(player->get_color());
    }
    else{
        //player->set_illegal();
        return false;
    }

    if(cell_is_full(&cells[i][j])){
        cell_explode(i, j);
        cell_reaction_marker();
        cell_chain_reaction(*player);
    }

    return true;
}

bool BD108061102::cell_is_full(PT102* cell){
    if(cell->get_orbs_num() >= cell->get_capacity()){
        cell->set_explode(true);
        return true;
    }
    else return false;
}

void BD108061102::add_orb(int i, int j, char color){
    int orb_num = cells[i][j].get_orbs_num();
    orb_num ++;
    cells[i][j].set_orbs_num(orb_num);
    cells[i][j].set_color(color);
}

void BD108061102::cell_reset(int i, int j){
    cells[i][j].set_orbs_num(0);
    cells[i][j].set_explode(false);
    cells[i][j].set_color('w');
}

void BD108061102::cell_explode(int i, int j){

    int orb_num;
    char color = cells[i][j].get_color();

    cell_reset(i, j);

    if( i + 1 < ROW){
        add_orb(i+1, j, color);
    }

    if( j + 1 < COL){
        add_orb(i, j+1, color);
    }

    if( i - 1 >= 0){
        add_orb(i-1, j, color);
    }

    if( j - 1 >= 0){
        add_orb(i, j-1, color);
    }
    if( i + 1 < ROW && j - 1 >= 0){
        add_orb(i+1, j-1, color);
    }
    if( i - 1 >= 0 && j - 1 >= 0){
        add_orb(i-1, j-1, color);
    }
    if( i + 1 < ROW && j + 1 < COL){
        add_orb(i+1, j+1, color);
    }
    if( i - 1 >= 0 && j + 1 < COL){
        add_orb(i-1, j+1, color);
    }
}

void BD108061102::cell_reaction_marker(){

    // Mark the next cell whose number of orbs is equal to the capacity
    for(int i = 0; i < ROW; i++){
            for(int j = 0; j < COL; j++){
                cell_is_full(&cells[i][j]);
            }
        }
}

void BD108061102::cell_chain_reaction(Player player){

    bool chain_reac = true;

    while(chain_reac){

        chain_reac = false;

        for(int i = 0; i < ROW; i++){
            for(int j = 0; j < COL; j++){
                if(cells[i][j].get_explode()){
                    cell_explode(i ,j);
                    chain_reac = true;
                }
            }
        }

        if(win_the_game(player)){
            return;
        }

        cell_reaction_marker();
    }
}

bool BD108061102::win_the_game(Player player){

    char player_color = player.get_color();
    bool win = true;

    for(int i = 0; i < ROW; i++){
        for(int j = 0; j < COL; j++){
            if(cells[i][j].get_color() == player_color || cells[i][j].get_color() == 'w') continue;
            else{
                win = false;
                break;
            }
        }
        if(!win) break;
    }
    return win;
}

int BD108061102::get_orbs_num(int i, int j){
    return cells[i][j].get_orbs_num();
}

int BD108061102::get_capacity(int i, int j){
    return cells[i][j].get_capacity();
}

char BD108061102::get_cell_color(int i, int j){
    return cells[i][j].get_color();
}

BD108061102 BD108061102::operator=(const BD108061102 &T) {
    for (int i = 0; i < ROW; i++) {
        for (int j = 0; j < COL; j++) {
            cells[i][j].set_color(T.cells[i][j].color);
            cells[i][j].set_orbs_num(T.cells[i][j].orbs_num);
            cells[i][j].set_capacity(T.cells[i][j].capacity);
            cells[i][j].set_explode(T.cells[i][j].explode);
        }
    }
    return *this;
}

void BD108061102::print_current_board(int i, int j, int round){

    int orb_num;
    char symbol;
    cout << "Round: " << round << endl;
    cout << "Place orb on (" << i << ", " << j << ")" << endl;
    cout << "=============================================================" << endl;
    for(int i = 0; i < ROW; i++){
        for(int j = 0; j < COL; j++){

            symbol = cells[i][j].get_color();
            switch(symbol){
                case 'r':
                    symbol = 'O';
                    break;
                case 'b':
                    symbol = 'X';
                    break;
                default:
                    break;
            }

            orb_num = cells[i][j].get_orbs_num();
            switch(orb_num){
                case 0:
                    cout << "|       | ";
                    break;
                case 1:
                    cout << "|" << symbol << "      | ";
                    break;
                case 2:
                    cout << "|" << symbol << symbol << "     | ";
                    break;
                case 3:
                    cout << "|" << symbol << symbol << symbol << "    | ";
                    break;
                case 4:
                    cout << "|" << symbol << symbol << symbol << symbol << "   | ";
                    break;
                case 5:
                    cout << "|" << symbol << symbol << symbol << symbol << symbol << "  | ";
                    break;
                case 6:
                    cout << "|" << symbol << symbol << symbol << symbol << symbol << symbol << " | ";
                    break;
                default:
                    cout << "|" << symbol << symbol << symbol << symbol << symbol << symbol << symbol << "| ";
                    break;

            }
        }
        cout << endl;
    }
    cout << "=============================================================" << endl << endl;
}

int x108061102, y108061102;
BD108061102 TMP108061102[5][30], CUR108061102, BestScore;

int Score (int num, int last_num, int depth, Player player, char mycolor) {
    char invert = player.get_color() == 'r' ? 'b' : 'r';
    int Tscore;
    int CNx[4] = {0, 0, 5, 5}, CNy[4] = {0, 4, 0, 4};
    int EG1x[] = {2, 4, 4, 2, 0, 0};
    int EG1y[] = {0, 2, 3, 5, 2, 3};
    int EG2x[] = {0, 0, 1, 1, 3, 3, 4, 4};
    int EG2y[] = {2, 3, 0, 5, 0, 5, 2, 3};

    if (depth == 0) {
        int sum = 0;
        TMP108061102[depth][num] = TMP108061102[depth + 1][last_num];
        for (int i = 0; i < 4; i++) {
            char Tcolor = CUR108061102.cells[CNy[i]][CNx[i]].get_color();
            char TMPcolor = TMP108061102[depth][num].cells[CNy[i]][CNx[i]].get_color();
            if (Tcolor == mycolor && TMPcolor != mycolor)
                sum -= 60;
            else if (Tcolor != mycolor && TMPcolor == mycolor) sum += 60;
        }
        for (int i = 0; i < 6; i++) {
            char Tcolor = CUR108061102.cells[EG1x[i]][EG1y[i]].get_color();
            char TMPcolor = TMP108061102[depth][num].cells[EG1x[i]][EG1y[i]].get_color();
            if (Tcolor == mycolor && TMPcolor != mycolor)
                sum -= 40;
            else if (Tcolor != mycolor && TMPcolor == mycolor) sum += 40;
        }

        for (int i = 0; i < 8; i++) {
            char Tcolor = CUR108061102.cells[EG2x[i]][EG2y[i]].get_color();
            char TMPcolor = TMP108061102[depth][num].cells[EG2x[i]][EG2y[i]].get_color();
            if (Tcolor == mycolor && TMPcolor != mycolor)
                sum -= 40;
            else if (Tcolor != mycolor && TMPcolor == mycolor) sum += 40;
        }

        for (int i = 0; i < ROW; i++) {
            for (int j = 0; j < COL; j++) {
                if (TMP108061102[depth][num].cells[i][j].get_color() == mycolor) {
                    sum+=10;
                }
            }
        }
        for (int i = 0; i < 4; i++) {
            char Tcolor = CUR108061102.cells[CNy[i]][CNx[i]].get_color();
            char TMPcolor = TMP108061102[depth][num].cells[CNy[i]][CNx[i]].get_color();
            if (Tcolor == invert && TMPcolor != invert)
                sum += 150;
            else if (Tcolor != invert && TMPcolor == invert) sum -= 150;
        }
        for (int i = 0; i < 6; i++) {
            char Tcolor = CUR108061102.cells[EG1x[i]][EG1y[i]].get_color();
            char TMPcolor = TMP108061102[depth][num].cells[EG1x[i]][EG1y[i]].get_color();
            if (Tcolor == invert && TMPcolor != invert)
                sum += 100;
            else if (Tcolor != invert && TMPcolor == invert) sum -= 100;
        }

        for (int i = 0; i < 8; i++) {
            char Tcolor = CUR108061102.cells[EG2x[i]][EG2y[i]].get_color();
            char TMPcolor = TMP108061102[depth][num].cells[EG2x[i]][EG2y[i]].get_color();
            if (Tcolor == invert && TMPcolor != invert)
                sum += 100;
            else if (Tcolor != invert && TMPcolor == invert) sum -= 100;
        }
        int TMPtt = 0;
        int WINc = 0;
        for (int i = 0; i < ROW; i++) {
            for (int j = 0; j < COL; j++) {
                if (TMP108061102[depth][num].cells[i][j].get_color() == invert) {
                    sum -= 2;
                }
                else if(TMP108061102[depth][num].cells[i][j].get_color() == mycolor) {
                    //TMPtt += TMP[depth][num].cells[i][j].get_orbs_num();
                    WINc++;
                } else WINc++;
            }
        }
        sum += TMPtt;
        if (WINc == 30) sum += 30000;
        if (sum >= Tscore) BestScore = TMP108061102[depth][num];
        return sum;
    }


    if (player.get_color() == mycolor) {
        int a = -INF;
        Player *P = mycolor == 'r' ? &playerB108061102 : &playerR108061102;
        for (int i = 0; i < ROW; i++) {
            for (int j = 0; j < COL; j++) {
                if (last_num == -1) {
                    TMP108061102[depth][num] = CUR108061102;
                }
                else {
                    TMP108061102[depth][num] = TMP108061102[depth + 1][last_num];
                }
                if (TMP108061102[depth][num].get_cell_color(i, j) != invert) {
                    TMP108061102[depth][num].place_orb(i, j, &player);
                    int score = Score(i * COL + j, num, depth - 1, *P, mycolor);
                    if (a <= score) {
                        a = score;
                        if (depth == D) {
                            y108061102 = i;
                            x108061102 = j;
                            Tscore = score;
                        }

                    }
                }
            }
        }

        return a;
    } else {
        int b = INF;
        Player *P = mycolor == 'r' ? &playerR108061102 : &playerB108061102;
        for (int i = 0; i < ROW; i++) {
            for (int j = 0; j < COL; j++) {
                if (last_num == -1) {
                    TMP108061102[depth][num] = CUR108061102;
                }
                else {
                    TMP108061102[depth][num] = TMP108061102[depth + 1][last_num];
                }
                 if (TMP108061102[depth][num].get_cell_color(i, j) != invert) {
                    TMP108061102[depth][num].place_orb(i, j, &player);
                    int score = Score(i * COL + j, num, depth - 1, *P, mycolor);
                    if (b > score) {
                        b = score;

                    }
                }
            }
        }
        return b;
    }
}
void algorithm_B(Board board, Player player, int index[]){

    char color = player.get_color();
    char invert = color == 'r' ? 'b' : 'r';

    x108061102 = y108061102 = 0;
    int bx, by;
    for (int i = 0; i < ROW; i++) {
        for (int j = 0; j < COL; j++) {
            CUR108061102.cells[i][j].orbs_num = board.get_orbs_num(i, j);
            CUR108061102.cells[i][j].color = board.get_cell_color(i, j);
            CUR108061102.cells[i][j].capacity = board.get_capacity(i, j);
            CUR108061102.cells[i][j].explode = false;
            if (board.get_cell_color(i, j) != invert) {
                y108061102 = i;
                x108061102 = j;
                by = i;
                bx = j;
            }
        }
    }

    //printf("start Game Tree\n");
    //CUR108061102.print_current_board(0, 0, 0);
    BestScore = CUR108061102;
    double START,END; START = clock();

    Player *start = color == 'r' ? &playerR108061102 : &playerB108061102;
    Score(0, -1 , D, *start, color);
    index[0] = y108061102;
    index[1] = x108061102;

}
