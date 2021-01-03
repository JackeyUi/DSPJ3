#include <iostream>
#include <stdlib.h>
#include <algorithm>
#include <time.h>
#include "../include/algorithm.h"
#define INF 1000000
using namespace std;


#define DEPTH 2

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


Player playerR('r'), playerB('b');
class Point{
    public:
        int orbs_num;   // The number of the orbs on the cell
        int capacity;   // The max number of the orbs of a cell
        char color;     // The color of the cell, 'w'(No one has placed the orbs), 'r'(Red Player) and 'b'(Blue player)
        bool explode;

        Point();
        Point(const Point&);
        // The basic functions of the Cell
        int get_orbs_num();
        int get_capacity();
        char get_color();
        bool get_explode();

        void set_orbs_num(int orbs);
        void set_capacity(int cap);
        void set_color(char col);
        void set_explode(bool tof);

        Point operator=(const Point&);
};

class Table{
    public:
        Point cells[ROW][COL];                       // The 5*6 board whose index (0,0) is start from the upper left corner
        void cell_reaction_marker();                // After the explosion, mark all the cell that  will explode in next iteration
        bool cell_is_full(Point* cell);              // Check wether the cell is full of orbs and set the explosion variable to be true
        void add_orb(int i, int j, char color);     // Add orb into the cell (i, j)
        void cell_reset(int i, int j);              // Reset the cell to the initial state (color = 'w', orb_num = 0)
        void cell_explode(int i, int j);            // The explosion of cell (i, j). It will add the orb into neighbor cells
        void cell_chain_reaction(Player player);    // If there is aa explosion, check wether the chain reaction is active

        Table();
        Table(const Table &T) {
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
        Table operator=(const Table &);
        //const Point* operator[] (const int) const;
       /* Point* operator[] (const int);
        const Point * operator[] (const int &x) const {
            return cells[x];
        }*/
};


bool placement_illegal_ST(Player player, Point cell){

    bool illegal = false;
    // Check wether the cell has be placed or not
    if(cell.get_color() == 'w') return illegal;

    //Check color
    if(cell.get_color() != player.get_color()) illegal = true;

    return illegal;

}
Point::Point(){
    orbs_num = 0, capacity = 8, color='w', explode = false;
}
Point::Point(const Point& P){
    orbs_num = P.orbs_num, capacity = P.capacity, color=P.color, explode = P.explode;
}
int Point::get_capacity(){
    return this->capacity;
}

int Point::get_orbs_num(){
    return this->orbs_num;
}

char Point::get_color(){
    return this->color;
}

bool Point::get_explode(){
    return this->explode;
}

void Point::set_orbs_num(int orbs){
    this->orbs_num = orbs;
}

void Point::set_capacity(int cap){
    this->capacity = cap;
}

void Point::set_color(char col){
    this->color = col;
}

void Point::set_explode(bool tof){
    this->explode = tof;
}

Point Point::operator=(const Point &P) {

    capacity = P.capacity;
    color = P.color;
    explode = P.explode;
    orbs_num = P.orbs_num;
    return *this;
}
Table::Table(){

    ////// Initialize the borad with correct capacity //////
    // The corners of the board
    cells[0][0].set_capacity(3), cells[0][5].set_capacity(3),
    cells[4][0].set_capacity(3), cells[4][5].set_capacity(3);

    // The edges of the board
    cells[0][1].set_capacity(5), cells[0][2].set_capacity(5), cells[0][3].set_capacity(5), cells[0][4].set_capacity(5),
    cells[1][0].set_capacity(5), cells[2][0].set_capacity(5), cells[3][0].set_capacity(5),
    cells[1][5].set_capacity(5), cells[2][5].set_capacity(5), cells[3][5].set_capacity(5),
    cells[4][1].set_capacity(5), cells[4][2].set_capacity(5), cells[4][3].set_capacity(5), cells[4][4].set_capacity(5);

}

bool Table::place_orb(int i, int j, Player * player){

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

bool Table::cell_is_full(Point* cell){
    if(cell->get_orbs_num() >= cell->get_capacity()){
        cell->set_explode(true);
        return true;
    }
    else return false;
}

void Table::add_orb(int i, int j, char color){
    int orb_num = cells[i][j].get_orbs_num();
    orb_num ++;
    cells[i][j].set_orbs_num(orb_num);
    cells[i][j].set_color(color);
}

void Table::cell_reset(int i, int j){
    cells[i][j].set_orbs_num(0);
    cells[i][j].set_explode(false);
    cells[i][j].set_color('w');
}

void Table::cell_explode(int i, int j){

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

void Table::cell_reaction_marker(){

    // Mark the next cell whose number of orbs is equal to the capacity
    for(int i = 0; i < ROW; i++){
            for(int j = 0; j < COL; j++){
                cell_is_full(&cells[i][j]);
            }
        }
}

void Table::cell_chain_reaction(Player player){

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

bool Table::win_the_game(Player player){

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

int Table::get_orbs_num(int i, int j){
    return cells[i][j].get_orbs_num();
}

int Table::get_capacity(int i, int j){
    return cells[i][j].get_capacity();
}

char Table::get_cell_color(int i, int j){
    return cells[i][j].get_color();
}

Table Table::operator=(const Table &T) {
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

/*Point* Table::operator[](int c) {
    return cells[c];
}*/
void Table::print_current_board(int i, int j, int round){

    int orb_num;
    char symbol;

    ////// Print out the current state of the board //////
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
int x, y;
Table Arr[5][50], T, best;
int eval (int num, int pa_num, int depth, Player player, char mycolor) {
    char invert = player.get_color() == 'r' ? 'b' : 'r';
    if (depth == 0) {
       // int sum = 0;
        Arr[depth][num] = Arr[depth + 1][pa_num];
        int winc = 0;
        int sum = 0;
        for(int i; i < 5; i++) {
            for(int j; j < 6; j++) {

                if(Arr[depth][num].cells[i][j].get_color() == mycolor || Arr[depth][num].cells[i][j].get_color() == 'w') winc += 1;
            }
        }
        if (winc == 30) sum += 1000000;

        {   // 角 8000
          if(Arr[depth][num].cells[0][0].get_color() == mycolor) sum += 30000;
          if(Arr[depth][num].cells[4][5].get_color() == mycolor) sum += 30000;
          if(Arr[depth][num].cells[0][5].get_color() == mycolor) sum += 30000;
          if(Arr[depth][num].cells[4][0].get_color() == mycolor) sum += 30000;
         // if(Arr[depth][num].cells[2][2].get_color() == mycolor) sum += 2000;
         // if(Arr[depth][num].cells[2][3].get_color() == mycolor) sum += 2000;
        }
        if ((Arr[depth][num].cells[0][0].get_color() == mycolor)){   //左上角
            if(Arr[depth][num].cells[0][0].get_orbs_num() == 2) sum += 1000;
            if(Arr[depth][num].cells[0][1].get_color() == mycolor) {  //右邊
                sum += 1500;
                if(Arr[depth][num].cells[0][1].get_orbs_num() > Arr[depth][num].cells[0][2].get_orbs_num() && Arr[depth][num].cells[0][2].get_color() != mycolor
                    || Arr[depth][num].cells[0][1].get_orbs_num() > (Arr[depth][num].cells[1][1].get_orbs_num() - 3) && Arr[depth][num].cells[1][1].get_color() != mycolor) sum+= 1500;
                /*if(Arr[depth][num].cells[0][0].get_orbs_num() == 2 && Arr[depth][num].cells[0][1].get_orbs_num() == 4
                    && Arr[depth][num].cells[0][2].get_orbs_num() == 3 && Arr[depth][num].cells[0][3].get_orbs_num() == 4 && Arr[depth][num].cells[0][3].get_color() != mycolor)
                        sum -= 1000;*/
            } else {
                if(Arr[depth][num].cells[0][0].get_orbs_num() > Arr[depth][num].cells[0][1].get_orbs_num() - 2) sum += 4000;
            }
            if(Arr[depth][num].cells[1][0].get_color() == mycolor) {  //下邊
                sum += 1500;
                if(Arr[depth][num].cells[1][0].get_orbs_num() > Arr[depth][num].cells[2][0].get_orbs_num() && Arr[depth][num].cells[2][0].get_color() != mycolor
                    || Arr[depth][num].cells[1][0].get_orbs_num() > (Arr[depth][num].cells[1][1].get_orbs_num() - 3) && Arr[depth][num].cells[1][1].get_color() != mycolor) sum+= 1500;
            }else {
                if(Arr[depth][num].cells[0][0].get_orbs_num() > Arr[depth][num].cells[1][0].get_orbs_num() - 2) sum += 4000;
            }
        }

        if ((Arr[depth][num].cells[0][5].get_color() == mycolor)){   //右上角
            if(Arr[depth][num].cells[0][5].get_orbs_num() == 2) sum += 1000;
            if(Arr[depth][num].cells[0][4].get_color() == mycolor) {  //左邊
                sum += 1500;
                if(Arr[depth][num].cells[0][4].get_orbs_num() > Arr[depth][num].cells[0][3].get_orbs_num() && Arr[depth][num].cells[0][3].get_color() != mycolor
                    || Arr[depth][num].cells[0][4].get_orbs_num() > (Arr[depth][num].cells[1][4].get_orbs_num() - 3) && Arr[depth][num].cells[1][4].get_color() != mycolor) sum+= 1500;
            }else {
                if(Arr[depth][num].cells[0][5].get_orbs_num() > Arr[depth][num].cells[0][4].get_orbs_num() - 2) sum += 4000;
            }
            if(Arr[depth][num].cells[1][5].get_color() == mycolor) {  //下邊
                sum += 1500;
                if(Arr[depth][num].cells[1][5].get_orbs_num() > Arr[depth][num].cells[2][5].get_orbs_num() && Arr[depth][num].cells[2][5].get_color() != mycolor
                    || Arr[depth][num].cells[1][5].get_orbs_num() > (Arr[depth][num].cells[1][4].get_orbs_num() - 3) && Arr[depth][num].cells[1][4].get_color() != mycolor) sum+= 1500;
            }else {
                if(Arr[depth][num].cells[0][5].get_orbs_num() > Arr[depth][num].cells[1][5].get_orbs_num() - 2) sum += 4000;
            }
        }
        if ((Arr[depth][num].cells[4][0].get_color() == mycolor)){   //左下角
            if(Arr[depth][num].cells[4][0].get_orbs_num() == 2) sum += 1000;
            if(Arr[depth][num].cells[4][1].get_color() == mycolor) {  //右邊
                sum += 1500;
                if(Arr[depth][num].cells[4][1].get_orbs_num() > Arr[depth][num].cells[4][2].get_orbs_num() && Arr[depth][num].cells[4][2].get_color() != mycolor
                    || Arr[depth][num].cells[4][1].get_orbs_num() > (Arr[depth][num].cells[3][1].get_orbs_num() - 3) && Arr[depth][num].cells[3][1].get_color() != mycolor) sum+= 1500;
                /*if(Arr[depth][num].cells[4][0].get_orbs_num() == 2 && Arr[depth][num].cells[4][1].get_orbs_num() == 4
                    && Arr[depth][num].cells[4][2].get_orbs_num() == 3 && Arr[depth][num].cells[4][3].get_orbs_num() == 4 && Arr[depth][num].cells[4][3].get_color() != mycolor)
                sum -= 1000;*/
            } else {
                if(Arr[depth][num].cells[4][0].get_orbs_num() > Arr[depth][num].cells[4][1].get_orbs_num() - 2) sum += 4000;
            }
            if(Arr[depth][num].cells[3][0].get_color() == mycolor) {  //上邊
                sum += 1500;
                if(Arr[depth][num].cells[3][0].get_orbs_num() > Arr[depth][num].cells[2][0].get_orbs_num() && Arr[depth][num].cells[2][0].get_color() != mycolor
                    || Arr[depth][num].cells[3][0].get_orbs_num() > (Arr[depth][num].cells[3][1].get_orbs_num() - 3) && Arr[depth][num].cells[3][1].get_color() != mycolor) sum+= 1500;
            } else {
                if(Arr[depth][num].cells[4][0].get_orbs_num() > Arr[depth][num].cells[3][0].get_orbs_num() - 2) sum += 4000;
            }
        } /*else if (Arr[depth][num].cells[4][0].get_color() != 'w') { //左下-敵
            sum -= 1000;
                if(Arr[depth][num].cells[3][0].get_color() == mycolor) {
                    sum -= 1000;
                    if(Arr[depth][num].cells[3][0].get_orbs_num() > Arr[depth][num].cells[0][4].get_orbs_num() + 2) sum += 1000;
                }
                if(Arr[depth][num].cells[4][1].get_color() == mycolor) {
                    sum -= 1000;
                    if(Arr[depth][num].cells[4][1].get_orbs_num() > Arr[depth][num].cells[4][0].get_orbs_num() + 2) sum += 1000;
                }

        }*/

        if ((Arr[depth][num].cells[4][5].get_color() == mycolor)){   //右下角
         if(Arr[depth][num].cells[4][5].get_orbs_num() == 2) sum += 1000;
            if(Arr[depth][num].cells[4][4].get_color() == mycolor) {  //左邊
                sum += 1500;
                if(Arr[depth][num].cells[4][4].get_orbs_num() > Arr[depth][num].cells[4][3].get_orbs_num()
                    || Arr[depth][num].cells[4][4].get_orbs_num() > (Arr[depth][num].cells[3][4].get_orbs_num() - 3)) sum+= 1500;
            }else {
                if(Arr[depth][num].cells[4][5].get_orbs_num() > Arr[depth][num].cells[4][4].get_orbs_num() - 2) sum += 4000;
            }
            if(Arr[depth][num].cells[3][5].get_color() == mycolor) {  //上邊
                sum += 1500;
                if(Arr[depth][num].cells[3][5].get_orbs_num() > Arr[depth][num].cells[2][5].get_orbs_num() || Arr[depth][num].cells[3][5].get_orbs_num() > (Arr[depth][num].cells[3][4].get_orbs_num() - 3)) sum+= 1500;
            }else {
                if(Arr[depth][num].cells[4][5].get_orbs_num() > Arr[depth][num].cells[3][5].get_orbs_num() - 2) sum += 4000;
            }
        }

        int EdgeScore = 8000;
        if ((Arr[depth][num].cells[0][2].get_color() == mycolor)){ //(0 2)
            if(Arr[depth][num].cells[0][2].get_orbs_num() > Arr[depth][num].cells[0][3].get_orbs_num() && Arr[depth][num].cells[0][3].get_color() != mycolor && Arr[depth][num].cells[0][3].get_orbs_num() != 0) sum += EdgeScore;

            if(Arr[depth][num].cells[0][2].get_orbs_num() > Arr[depth][num].cells[0][1].get_orbs_num() && Arr[depth][num].cells[0][1].get_color() != mycolor && Arr[depth][num].cells[0][1].get_orbs_num() != 0) sum += EdgeScore;

            if((Arr[depth][num].cells[0][2].get_orbs_num() > (Arr[depth][num].cells[1][2].get_orbs_num() - 3)) && Arr[depth][num].cells[1][2].get_color() != mycolor && Arr[depth][num].cells[1][2].get_orbs_num() != 0
                /*|| (Arr[depth][num].cells[0][2].get_orbs_num() > (Arr[depth][num].cells[1][2].get_orbs_num() - 3)) && Arr[depth][num].cells[1][3].get_color() != mycolor*/) sum += EdgeScore;
        }
        if ((Arr[depth][num].cells[0][3].get_color() == mycolor)){ //(0 3)
            if(Arr[depth][num].cells[0][3].get_orbs_num() > Arr[depth][num].cells[0][2].get_orbs_num() && Arr[depth][num].cells[0][2].get_color() != mycolor && Arr[depth][num].cells[0][2].get_orbs_num() != 0) sum += EdgeScore;
            if(Arr[depth][num].cells[0][3].get_orbs_num() > Arr[depth][num].cells[0][4].get_orbs_num() && Arr[depth][num].cells[0][4].get_color() != mycolor && Arr[depth][num].cells[0][4].get_orbs_num() != 0) sum += EdgeScore;
            if( ((Arr[depth][num].cells[0][3].get_orbs_num() > (Arr[depth][num].cells[1][3].get_orbs_num() - 3)) && Arr[depth][num].cells[1][3].get_color() != mycolor && Arr[depth][num].cells[1][3].get_orbs_num() != 0)
                /*|| ((Arr[depth][num].cells[0][3].get_orbs_num() > (Arr[depth][num].cells[1][2].get_orbs_num() - 3)) && Arr[depth][num].cells[1][2].get_color() != mycolor)*/ ) sum += 3000;
        }

        if ((Arr[depth][num].cells[4][2].get_color() == mycolor)){ // (4 2)
            if(Arr[depth][num].cells[4][2].get_orbs_num() > Arr[depth][num].cells[4][3].get_orbs_num() && Arr[depth][num].cells[4][3].get_color() != mycolor && Arr[depth][num].cells[4][3].get_orbs_num() != 0) sum += EdgeScore;
            if(Arr[depth][num].cells[4][2].get_orbs_num() > Arr[depth][num].cells[4][1].get_orbs_num() && Arr[depth][num].cells[4][1].get_color() != mycolor && Arr[depth][num].cells[4][1].get_orbs_num() != 0) sum += EdgeScore;
            if( (Arr[depth][num].cells[4][2].get_orbs_num() > (Arr[depth][num].cells[3][2].get_orbs_num() - 3) && Arr[depth][num].cells[3][2].get_color() != mycolor && Arr[depth][num].cells[3][2].get_orbs_num() != 0)
                /*|| (Arr[depth][num].cells[4][2].get_orbs_num() > (Arr[depth][num].cells[3][3].get_orbs_num() - 3) && Arr[depth][num].cells[3][3].get_color() != mycolor)*/) sum += 3000;
        }
        if ((Arr[depth][num].cells[4][3].get_color() == mycolor)){ // (4 3)
            if(Arr[depth][num].cells[4][3].get_orbs_num() > Arr[depth][num].cells[4][2].get_orbs_num() && Arr[depth][num].cells[4][2].get_color() != mycolor && Arr[depth][num].cells[4][2].get_orbs_num() != 0) sum += EdgeScore;
            if(Arr[depth][num].cells[4][3].get_orbs_num() > Arr[depth][num].cells[4][4].get_orbs_num() && Arr[depth][num].cells[4][4].get_color() != mycolor && Arr[depth][num].cells[4][4].get_orbs_num() != 0) sum += EdgeScore;
            if( (Arr[depth][num].cells[4][3].get_orbs_num() > (Arr[depth][num].cells[3][3].get_orbs_num() - 3) && Arr[depth][num].cells[3][3].get_color() != mycolor && Arr[depth][num].cells[3][3].get_orbs_num() != 0)
                /*|| (Arr[depth][num].cells[4][3].get_orbs_num() > (Arr[depth][num].cells[3][2].get_orbs_num() - 3) && Arr[depth][num].cells[3][2].get_color() != mycolor)*/) sum += 3000;
        }

        if ((Arr[depth][num].cells[2][0].get_color() == mycolor)){ //(2 0)
            if(Arr[depth][num].cells[2][0].get_orbs_num() > Arr[depth][num].cells[1][0].get_orbs_num() && Arr[depth][num].cells[1][0].get_color() != mycolor && Arr[depth][num].cells[1][0].get_orbs_num() != 0) sum += EdgeScore;
            if(Arr[depth][num].cells[2][0].get_orbs_num() > Arr[depth][num].cells[3][0].get_orbs_num() && Arr[depth][num].cells[3][0].get_color() != mycolor && Arr[depth][num].cells[3][0].get_orbs_num() != 0) sum += EdgeScore;
            if( ((Arr[depth][num].cells[2][0].get_orbs_num() > (Arr[depth][num].cells[2][1].get_orbs_num() - 3)) && Arr[depth][num].cells[2][1].get_color() != mycolor && Arr[depth][num].cells[2][1].get_orbs_num() != 0)
                /*|| ((Arr[depth][num].cells[0][3].get_orbs_num() > (Arr[depth][num].cells[1][2].get_orbs_num() - 3)) && Arr[depth][num].cells[1][2].get_color() != mycolor)*/ ) sum += EdgeScore;
        }

        if ((Arr[depth][num].cells[2][5].get_color() == mycolor)){ //(2 5)
            if(Arr[depth][num].cells[2][5].get_orbs_num() + 1 > Arr[depth][num].cells[1][5].get_orbs_num() && Arr[depth][num].cells[1][5].get_color() != mycolor && Arr[depth][num].cells[1][5].get_orbs_num() != 0) sum += EdgeScore;

            if(Arr[depth][num].cells[2][5].get_orbs_num() + 1 > Arr[depth][num].cells[3][5].get_orbs_num() && Arr[depth][num].cells[3][5].get_color() != mycolor && Arr[depth][num].cells[3][5].get_orbs_num() != 0) sum += EdgeScore;

            if( ((Arr[depth][num].cells[2][5].get_orbs_num() > (Arr[depth][num].cells[2][4].get_orbs_num() - 3)) && Arr[depth][num].cells[2][4].get_color() != mycolor && Arr[depth][num].cells[2][4].get_orbs_num() != 0)
                /*|| ((Arr[depth][num].cells[0][3].get_orbs_num() > (Arr[depth][num].cells[1][2].get_orbs_num() - 3)) && Arr[depth][num].cells[1][2].get_color() != mycolor)*/ ) sum += EdgeScore;
        }

        {   // 上邊 1500 1000
          if(Arr[depth][num].cells[0][1].get_color() == mycolor) sum += 1500;
          if(Arr[depth][num].cells[0][2].get_color() == mycolor) sum += 3000;
          if(Arr[depth][num].cells[0][3].get_color() == mycolor) sum += 3000;
          if(Arr[depth][num].cells[0][4].get_color() == mycolor) sum += 1500;
        }
        {   // 下邊 1500 1000
          if(Arr[depth][num].cells[4][1].get_color() == mycolor) sum += 1500;
          if(Arr[depth][num].cells[4][2].get_color() == mycolor) sum += 3000;
          if(Arr[depth][num].cells[4][3].get_color() == mycolor) sum += 3000;
          if(Arr[depth][num].cells[4][4].get_color() == mycolor) sum += 1500;
        }
        {   // 左右邊 1500 1000

          if(Arr[depth][num].cells[1][0].get_color() == mycolor) sum += 1500;
          if(Arr[depth][num].cells[2][0].get_color() == mycolor) sum += 4000;
          if(Arr[depth][num].cells[3][0].get_color() == mycolor) sum += 1500;
          if(Arr[depth][num].cells[1][5].get_color() == mycolor) sum += 1500;
          if(Arr[depth][num].cells[2][5].get_color() == mycolor) sum += 3000;
          if(Arr[depth][num].cells[3][5].get_color() == mycolor) sum += 1500;
        }
        {
            if(Arr[depth][num].cells[2][2].get_color() == mycolor) {
                sum += 1000;
                if(Arr[depth][num].cells[2][3].get_color() != mycolor && Arr[depth][num].cells[2][3].get_orbs_num() < Arr[depth][num].cells[2][2].get_orbs_num() && Arr[depth][num].cells[2][3].get_orbs_num() != 0) sum += 2000;
            }
            if(Arr[depth][num].cells[2][3].get_color() == mycolor) {
                sum += 1000;
                if(Arr[depth][num].cells[2][2].get_color() != mycolor && Arr[depth][num].cells[2][2].get_orbs_num() < Arr[depth][num].cells[2][3].get_orbs_num() && Arr[depth][num].cells[2][2].get_orbs_num() != 0) sum += 2000;
            }
        }

        {
          //  if(Arr[depth][num].cells[1][2].get_color() == mycolor && Arr[depth][num].cells[1][2].get_orbs_num() == 7) sum -= 100000;
        }

        /*if(B.cells[0][0].get_color() == invert) sum += 10000;
          if(B.cells[4][5].get_color() == invert) sum -= 10000;
          if(B.cells[0][5].get_color() == invert) sum -= 10000;
          if(B.cells[4][0].get_color() == invert) sum -= 10000;
          if(B.cells[0][1].get_color() == invert) sum -= 1500;
          if(B.cells[0][2].get_color() == invert) sum -= 1000;
          if(B.cells[0][3].get_color() == invert) sum -= 1000;
          if(B.cells[0][4].get_color() == invert) sum -= 1500;
          if(B.cells[4][1].get_color() == invert) sum -= 1500;
          if(B.cells[4][2].get_color() == invert) sum -= 1000;
          if(B.cells[4][3].get_color() == invert) sum -= 1000;
          if(B.cells[4][4].get_color() == invert) sum -= 1500;
          if(B.cells[1][0].get_color() == invert) sum -= 1500;
          if(B.cells[2][0].get_color() == invert) sum -= 1000;
          if(B.cells[3][0].get_color() == invert) sum -= 1500;
          if(B.cells[1][5].get_color() == invert) sum -= 1500;
          if(B.cells[2][5].get_color() == invert) sum -= 1000;
          if(B.cells[3][5].get_color() == invert) sum -= 1500;*/
        /*for (int i = 0; i < ROW; i++) {
            for (int j = 0; j < COL; j++) {
                if (Arr[depth][num].cells[i][j].get_color() == invert) {
                    sum--;
                    sum -= Arr[depth][num].cells[i][j].get_orbs_num();
                }
            }
        }*/
        //printf("reach depth 0, sum = %d\n", sum);
        //Arr[depth][num].print_current_board(0, 0, num);
        int sumMY = 0;
        int sumA = 0;
        int sumE = 0;
        for (int i = 0; i < 5; i++) {
            for (int j = 0; j < 6; j++) {
                sumA += Arr[depth][num].cells[i][j].get_orbs_num();
                if (Arr[depth][num].cells[i][j].get_color() == mycolor) {
                    sumMY += Arr[depth][num].cells[i][j].get_orbs_num();
                    sum += 6000;
                    sum += Arr[depth][num].cells[i][j].get_orbs_num() * 2000;
                    //sum += Arr[depth][num].cells[i][j].get_orbs_num();
                }
                if (Arr[depth][num].cells[i][j].get_color() != mycolor && Arr[depth][num].cells[i][j].get_color() != 'w') sum -= 100;
            }
        }
        if (sumMY == sumA) sum += 10000000;

        return sum;
    }
    //B.print_current_board(0, 0, 100);


    if (player.get_color() == mycolor) {
        int a = -INF;
        Player *P = mycolor == 'r' ? &playerR : &playerB;
        for (int i = 0; i < ROW; i++) {
            for (int j = 0; j < COL; j++) {
                if (pa_num == -1) {
                    Arr[depth][num] = T;
                }
                else {
                    Arr[depth][num] = Arr[depth + 1][pa_num];
                }
                if (Arr[depth][num].get_cell_color(i, j) != invert) {
                    Arr[depth][num].place_orb(i, j, &player);
                    int EVAL = eval(i * COL + j, num, depth - 1, *P, mycolor);
                    if (a < EVAL) {
                        a = EVAL;
                        if (depth == DEPTH) {
                            y = i;
                            x = j;
                        }
                        best = Arr[depth][num];
                    }
                }
            }
        }

        return a;
    } else {
        int b = INF;
        Player *P = mycolor == 'b' ? &playerR : &playerB;
        for (int i = 0; i < ROW; i++) {
            for (int j = 0; j < COL; j++) {
                if (pa_num == -1) {
                    Arr[depth][num] = T;
                }
                else {
                    Arr[depth][num] = Arr[depth + 1][pa_num];
                }
                 if (Arr[depth][num].get_cell_color(i, j) != invert) {
                    Arr[depth][num].place_orb(i, j, &player);
                    int EVAL = eval(i * COL + j, num, depth - 1, *P, mycolor);
                    if (b > EVAL) {
                        b = EVAL;
                        //y = i;
                        //x = j;
                    }
                }
            }
        }
        return b;
    }
}
void algorithm_A(Board board, Player player, int index[]){
    //static int cri[5][6] = {{3, 5, 5, 5, 5, 3}, {5, 8, 8, 8, 8, 5},
      //      {5, 8, 8, 8, 8, 5}, {5, 8, 8, 8, 8, 5}, {3, 5, 5, 5, 5, 3}}, dir[] = {0, 1, -1};
    //////your algorithm design///////////
    int B[5][6], C[5][6];
    char color = player.get_color();
    char invert = color == 'r' ? 'b' : 'r';

    x = y = 0;
    int bx, by;
    for (int i = 0; i < ROW; i++) {
        for (int j = 0; j < COL; j++) {
            T.cells[i][j].orbs_num = board.get_orbs_num(i, j);
            T.cells[i][j].color = board.get_cell_color(i, j);
            T.cells[i][j].capacity = board.get_capacity(i, j);
            T.cells[i][j].explode = false;
            if (board.get_cell_color(i, j) != invert) {
                y = i;
                x = j;
                by = i;
                bx = j;
            }
        }
    }

    //printf("start Game Tree\n");
    //T.print_current_board(0, 0, 0);
    best = T;
    double START,END; START = clock();


    eval(0, -1 , DEPTH, color == 'r' ? playerR : playerB, color);
    END = clock();
    cout << (END - START) / CLOCKS_PER_SEC << endl;
    //Arr[0][0] = T;
    //Arr[0][0].print_current_board(0, 0, 0);
    //cout << x << " " << y << endl;
    /*if (board.get_cell_color(y, x) == invert) {
        y = by;
        x = bx;
    }*/
    //printf("end Game Tree\n");
    //best.print_current_board(0, 0, 0);
    index[0] = y;
    index[1] = x;

    //cin >> index[0] >> index[1];
}
