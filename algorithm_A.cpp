#include <iostream>
#include <stdlib.h>
#include <time.h>
#include "../include/algorithm.h"
#include <string>
#include <stdlib.h> 
using namespace std;
void DFS(int row, int col, Board blueprint, int* chain, bool(*visited)[6]);
void reaction_from_blueprint(int row,int col,Board blueprint,int* chain,bool (*visited)[6],Player player);
bool can_save_corner_next_time(int* chain,Board enemy_blueprint,int color,bool* corner);
bool kill_my_corner(Board blueprint,Board pre_blueprint,int color,bool* corner);
bool next_round(Board enemy_blueprint_tmp,int color,int corner_i,int corner_j);
class point{
    private:
        int row;
        int col;
        int chain;
        int pt_color;
        bool danger;///for corner protect
    public:
        point(int i,int j, Board board, Player player){
            row = i;
            col = j;
            int pt_color = -1;
            int color = player.get_color();
            chain = 0;

            for(int i=0;i<5;i++){
                for(int j=0;j<6;j++){
                    if(board.get_cell_color(i,j-1)==color || board.get_cell_color(i,j-1)=='w')
                        pt_color = 1;///can put
                    else pt_color = -1;
                }
            }

            ///whether my corner is dangerous
            danger = false;
            for(int k=0;k<4;k++){
                if(board.get_cell_color(i,j)==color){
                    if(j-1<6 && board.get_cell_color(i,j-1)!=color && board.get_cell_color(i,j-1)!= 'w'){
                        if(board.get_orbs_num(i,j-1)==board.get_capacity(i,j-1)-1)danger = true;
                    }///left
                    if(j+1>-1 && board.get_cell_color(i,j+1)!=color && board.get_cell_color(i,j+1)!= 'w'){
                        if(board.get_orbs_num(i,j+1)==board.get_capacity(i,j+1)-1)danger = true;
                    }///right
                    if(i+1<5 && board.get_cell_color(i+1,j)!=color && board.get_cell_color(i+1,j)!= 'w'){
                        if(board.get_orbs_num(i+1,j)==board.get_capacity(i+1,j)-1)danger = true;
                    }///down
                    if(i-1>-1 && board.get_cell_color(i-1,j)!=color && board.get_cell_color(i-1,j)!= 'w'){
                        if(board.get_orbs_num(i-1,j)==board.get_capacity(i-1,j)-1)danger = true;
                    }///up
                }
            }

        }
        bool get_danger(){return danger;}
        int get_row(){return row;}
        int get_col(){return col;}
        int get_chain(){return chain;}
        void set_chain_numbers(Board board,Player player,string type){
            if(pt_color==-1)return;
            else {
                Board blueprint = board;
                bool visited[5][6];
                for(int i=0;i<5;i++)
                    for(int j=0;j<6;j++)visited[i][j]=false;
                visited[row][col] = true;

                chain = 0;
                ///***uising DFS to find the most profitable way to protect corner***///
                DFS(row,col,blueprint,&chain,visited);
                ///***find reaction***///
                reaction_from_blueprint(row,col,blueprint,&chain,visited,player);
                if(type == "protect_corner" && !danger) chain = 0;
                return;
            }
        }

};
void DFS(int row, int col, Board blueprint, int* chain, bool(*visited)[6]){
    if(row-1>-1){
        int n_row = row-1;
        int n_col = col;
        if(blueprint.get_orbs_num(n_row,n_col)+1 == blueprint.get_capacity(n_row,n_col) && !visited[n_row][n_col]){
            visited[n_row][n_col] = true;
            *chain++;
            DFS(n_row,n_col,blueprint,chain,visited);
        }
    }
    if(row+1<5){
        int n_row = row+1;
        int n_col = col;
        if(blueprint.get_orbs_num(n_row,n_col)+1 == blueprint.get_capacity(n_row,n_col) && !visited[n_row][n_col]){
            visited[n_row][n_col] = true;
            *chain++;
            DFS(n_row,n_col,blueprint,chain,visited);
        }
    }
    if(col-1>-1){
        int n_row = row;
        int n_col = col-1;
        if(blueprint.get_orbs_num(n_row,n_col)+1 == blueprint.get_capacity(n_row,n_col) && !visited[n_row][n_col]){
            visited[n_row][n_col] = true;
            *chain++;
            DFS(n_row,n_col,blueprint,chain,visited);
        }
    }
    if(col+1<6){
        int n_row = row;
        int n_col = col+1;
        if(blueprint.get_orbs_num(n_row,n_col)+1 == blueprint.get_capacity(n_row,n_col) && !visited[n_row][n_col]){
            visited[n_row][n_col] = true;
            *chain++;
            DFS(n_row,n_col,blueprint,chain,visited);
        }
    }
    return;
}
void reaction_from_blueprint(int row,int col,Board blueprint,int* chain,bool(*visited)[6],Player player){
    Board pre_blueprint = blueprint;
    blueprint.place_orb(row,col,&player);
    int color = player.get_color();
    ///get corner is more important
    for(int i=0;i<5;i++){
        for(int j=0;j<6;j++){
            if(blueprint.get_cell_color(i,j)!=pre_blueprint.get_cell_color(i,j)){
                visited[i][j]=true;
                int coef=1;
                if(blueprint.get_capacity(i,j)==4)coef=1;
                if(blueprint.get_capacity(i,j)==3)coef=2;
                if(blueprint.get_capacity(i,j)==2)coef=3;
                if(pre_blueprint.get_cell_color(i,j)!=color && pre_blueprint.get_cell_color(i,j)!='w')coef*=3;
                ///if let corner point = 'w' and we cant save it next time is not allow, make its chain to -1;
                /*bool corner[4] = {false,false,false,false};
                if(kill_my_corner(blueprint,pre_blueprint,color,corner)){
                    Board enemy_blueprint = blueprint;
                    if(!can_save_corner_next_time(chain,enemy_blueprint,color,corner)) *chain=-1;
                    if(*chain==-1)return;
                }*/
                *chain += coef; 
            }
            if(!visited[i][j] && blueprint.get_orbs_num(i,j)!=pre_blueprint.get_orbs_num(i,j)){
                visited[i][j]=true;
                int coef=1;
                if(blueprint.get_capacity(i,j)==4)coef=1;
                if(blueprint.get_capacity(i,j)==3)coef=2;
                if(blueprint.get_capacity(i,j)==2)coef=3;
                if(pre_blueprint.get_cell_color(i,j)!=color && pre_blueprint.get_cell_color(i,j)!='w')coef*=3;
                ///if let corner point = 'w' and we cant save it next time is not allow, make its chain to -1;
                /*bool corner[4] = {false,false,false,false};
                if(kill_my_corner(blueprint,pre_blueprint,color,corner)){
                    Board enemy_blueprint = blueprint;
                    if(!can_save_corner_next_time(chain,enemy_blueprint,color,corner)) *chain=-1;
                    if(*chain==-1)return;
                }*/
                *chain += coef; 
            }
        }
    }
    
    return;
}

bool kill_my_corner(Board blueprint,Board pre_blueprint,int color,bool* corner){
    bool valid;
    if(blueprint.get_cell_color(0,0)!=color && pre_blueprint.get_cell_color(0,0)==color) corner[0] = true;
    if(blueprint.get_cell_color(0,5)!=color && pre_blueprint.get_cell_color(0,5)==color) corner[1] = true;
    if(blueprint.get_cell_color(4,5)!=color && pre_blueprint.get_cell_color(4,5)==color) corner[2] = true;
    if(blueprint.get_cell_color(4,0)!=color && pre_blueprint.get_cell_color(4,0)==color) corner[3] = true;
    valid = (corner[0]|corner[1]|corner[2]|corner[3]);
    return valid;
}
bool can_save_corner_next_time(int* chain,Board enemy_blueprint,int color,bool* corner){
    Player red_player(RED);
    Player blue_player(BLUE);
    for(int i=0;i<4;i++){
        if(corner[i]){
            int corner_i,corner_j;
            if(i=0) {
                corner_i = 0;
                corner_j = 0;
            }
            if(i=1) {
                corner_i = 0;
                corner_j = 5;
            }
            if(i=2) {
                corner_i = 4;
                corner_j = 5;
            }
            if(i=3) {
                corner_i = 4;
                corner_j = 0;
            }
            corner[i] = false;
            Board enemy_blueprint_tmp = enemy_blueprint;
            for(int j=0;j<5;j++){
                for(int k=0;k<6;k++){
                    if(enemy_blueprint.get_cell_color(i,j)!=color){
                        enemy_blueprint_tmp.place_orb(i,j,&blue_player);
                        if(!next_round(enemy_blueprint_tmp,color,corner_i,corner_j))return true;///can't save
                    }
                }
            }
        }
    }
    if(corner[0]|corner[1]|corner[2]|corner[3])return false;
    return true;
}
bool next_round(Board enemy_blueprint_tmp,int color,int corner_i,int corner_j){
    Player red_player(RED);
    Player blue_player(BLUE);
    for(int i=0;i<5;i++){
        for(int j=0;j<6;j++){
            Board my_next_round_blueprint;
            if(enemy_blueprint_tmp.get_cell_color(i,j)==color || enemy_blueprint_tmp.get_cell_color(i,j)=='w'){
                my_next_round_blueprint = enemy_blueprint_tmp;
                my_next_round_blueprint.place_orb(i,j,&red_player);
                if(my_next_round_blueprint.get_cell_color(corner_i,corner_j)==color) return true;
            }
        }
    }
    return false;///can't save
}

/***********************************point*************************************/

bool finish(int row, int col, Board board, Player player);
void protect_corner(int* row, int* col,Board board, int color);
void put_on_corner(int* row, int* col, Board board, int color);
void general_cal(int* row,int* col,Board board,Player player);
void get_final_pos(int* row,int* col, point* corner_point);
bool max(int a,int b);
void algorithm_A(Board board, Player player, int index[]){
    // cout << board.get_capacity(0, 0) << endl;
    // cout << board.get_orbs_num(0, 0) << endl;
    // cout << board.get_cell_color(0, 0) << endl;
    // board.print_current_board(0, 0, 0);

    //////////// Random Algorithm ////////////
    // Here is the random algorithm for your reference, you can delete or comment it.
    srand(time(NULL));
    int row, col;
    row = col = -1;
    int color = player.get_color();
    while(1){
        /***protect my corner first***/
        protect_corner(&row, &col, board, color);
        if(finish(row,col,board,player))break;
        /***put orb on corner (0,0)->(5,4)->(0,4)->(5,0)***/      
        put_on_corner(&row, &col, board, color);
        if(finish(row,col,board,player))break;
        /***using DFS to find profitable way cap 2>3>4***/
        general_cal(&row,&col,board,player);
        if(finish(row,col,board,player))break;
        while(1){
            row = rand() % 5;
            col = rand() % 6;
            if(finish(row,col,board,player))break;
        }
        
        if(finish(row,col,board,player))break;
    }
    index[0] = row;
    index[1] = col;
}
void get_final_pos(int* row,int* col,point* corner_point){
    point pt = corner_point[0];
    pt = (max(pt.get_chain(),corner_point[1].get_chain()))? pt:corner_point[1];
    pt = (max(pt.get_chain(),corner_point[2].get_chain()))? pt:corner_point[2];
    pt = (max(pt.get_chain(),corner_point[3].get_chain()))? pt:corner_point[3];
    if(pt.get_chain()>0){
        *row = pt.get_row();
        *col = pt.get_col();
    }
    return;
}
bool max(int a,int b){
    if(a>=b) return true;
    return false;
}
void protect_corner(int* row, int* col, Board board, int color){
    point corner_point[4] = { point(0,0,board,color),
                              point(4,5,board,color),
                              point(4,0,board,color),
                              point(0,5,board,color) };
    for(int i=0;i<4;i++) corner_point[i].set_chain_numbers(board,color,"protect_corner");
    ///***compare the chains***///
    ///***case of same chain i ignore first***///
    get_final_pos(row,col,corner_point);
    return;
}
void put_on_corner(int* row, int* col, Board board, int my_color){
    int corner_color[4] = { board.get_cell_color(0,0), 
                            board.get_cell_color(4,5), 
                            board.get_cell_color(4,0), 
                            board.get_cell_color(0,5) };
    if(corner_color[0]=='w'){
        *row = 0;
        *col = 0;
    }
    else if(corner_color[1]=='w'){
        *row = 4;
        *col = 5;
    }
    else if(corner_color[2]=='w'){
        *row = 4;
        *col = 0;
    }
    else if(corner_color[3]=='w'){
        *row = 0;
        *col = 5;
    }
    return;
}
void general_cal(int* row,int* col,Board board,Player player){
    int color = player.get_color();
    point pts[5][6]={ point(0,0,board,color),point(0,1,board,color),point(0,2,board,color),point(0,3,board,color),point(0,4,board,color),point(0,5,board,color),
                      point(1,0,board,color),point(1,1,board,color),point(1,2,board,color),point(1,3,board,color),point(1,4,board,color),point(1,5,board,color),
                      point(2,0,board,color),point(2,1,board,color),point(2,2,board,color),point(2,3,board,color),point(2,4,board,color),point(2,5,board,color),
                      point(3,0,board,color),point(3,1,board,color),point(3,2,board,color),point(3,3,board,color),point(3,4,board,color),point(3,5,board,color),
                      point(4,0,board,color),point(4,1,board,color),point(4,2,board,color),point(4,3,board,color),point(4,4,board,color),point(4,5,board,color),                    
                    };

    for(int i=0;i<5;i++)
        for(int j=0;j<6;j++)pts[i][j].set_chain_numbers(board,color,"general_cal");

    int f_row = -1;
    int f_col = -1;
    int chain = -1;
    for(int i=0;i<5;i++){
        for(int j=0;j<6;j++){
            ///need to cnsider same chain nums///
            Board outcome = board;
            outcome.place_orb(i,j,&player);
            bool cant=0;
            if(board.get_cell_color(0,0)==color && outcome.get_cell_color(0,0)!=color){
                if((outcome.get_orbs_num(0,1)!=outcome.get_capacity(0,1)-1) && (outcome.get_orbs_num(1,0)!=outcome.get_capacity(1,0)-1))
                    cant = 1;
            }
            if(board.get_cell_color(0,5)==color && outcome.get_cell_color(0,5)!=color){
                if((outcome.get_orbs_num(0,4)!=outcome.get_capacity(0,4)-1) && (outcome.get_orbs_num(1,5)!=outcome.get_capacity(1,5)-1))
                    cant = 1;
            }
            if(board.get_cell_color(4,5)==color && outcome.get_cell_color(4,5)!=color){
                if((outcome.get_orbs_num(4,4)!=outcome.get_capacity(4,4)-1) && (outcome.get_orbs_num(3,5)!=outcome.get_capacity(3,5)-1))
                    cant = 1;
            }
            if(board.get_cell_color(4,0)==color && outcome.get_cell_color(4,0)!=color){
                if((outcome.get_orbs_num(4,1)!=outcome.get_capacity(4,1)-1) && (outcome.get_orbs_num(3,0)!=outcome.get_capacity(3,0)-1))
                    cant = 1;
            }
            if(!cant && chain<pts[i][j].get_chain()){       
                f_row = i;
                f_col = j;
                chain = pts[i][j].get_chain();
            }
        }
    }
    *row = f_row;
    *col = f_col;
    /*if(pts[0][0].get_chain()==-1){
        *row = -1;
        *col = -1;
    }*/
    return;
}
bool finish(int row,int col,Board board,Player player){
    int color = player.get_color();
    if(row!=-1 && col!=-1 && (board.get_cell_color(row, col) == color || board.get_cell_color(row, col) == 'w')){
        return true;
    }
    return false;
}