/*
 * File:   newmainXC16.c
 * Author: alg52
 *
 * Created on 16 March 2019, 18:04
 */
#include "allcode_api.h"
#include <stdio.h>
#include <stdbool.h>

struct Cell{            
    bool visited;               //is the cell was visited
    bool north;                 //is there a wall on the north site of the cell
    bool east;                  //is there a wall on the east site of the cell
    bool south;                 //is there a wall on the south site of the cell
    bool west;                  //is there a wall on the west site of the cell
    int distanceToNest;         //distance from the cell to the nest (in cells)
};


int rightAngle = 90;
int calibrationAngle = 3;      
int distance = 155;             //distance from the middle of cell to the middle of next cell (in millimeters)

enum NESW {North, East, South, West};

int main()
{
    FA_RobotInit();
    FA_SetDriveSpeed (50);
    FA_LCDBacklight(80);
    
    struct Cell maze[5][5];     //creates matrix of cells which represents all cells of the maze
    int i,j;
    
    int XcoordNest, YcoordNest; //coordinates of the nest
        
    for(i=0;i<25;i++){

        maze[i/5][i%5].distanceToNest = NULL;
    }

    
    bool nestFound = false;
    bool leftIR=false;
    bool frontIR=false;
    bool rightIR=false;
    
    int IRleft, IRright;        //not really important variables, I use them only one specific time described further
    
    
    int valWall = 20;           //minimal value which defines wall in front of IR sensor 
    int valTape = 200;          //minimal value which defines black tape under the robot
    int valNest = 400;          //minimal value which defines the nest in currently occupied cell
    
    int Xcoord = 1;             //coordinates of robot X-axis
    int Ycoord = 0;             //coordinates of robot Y-axis
    
              
                                
    enum NESW direction = North;// direction of front of the robot 

    void turnRight(){           //turning robot 90 degrees and changing its direction
        FA_Right(rightAngle);
        if(direction==West){       
            direction=North;
        }else{
        direction++;
        }
    }
    void turnLeft(){            //turning robot 90 degrees and changing its direction
        FA_Left(rightAngle);
        if(direction==North){       
            direction=West;
        }else{
        direction--;
        }
    }
    
    void calibration(){
        IRright = FA_ReadIR(4);                                     //FA_ReadIR(4) could be straight in while statement, but it was not working,
                                                                    //I tried many ways and didn't want to waste more time, so I handled this by
        while((FA_ReadIR(0)>600) || (IRright>20 && IRright<250)){   //assigning value from FA_ReadIR(4) to variable "IRright".
                                                    
                
            FA_DelayMillis(1000);                                   //this delay helped me during demonstration, when I saw the robot turned slightly wrong angle I had a time to correct this
            //FA_BTSendString ("\nRead", 10);                       //these values sent via bluetooth helped me to specify values to while loop above
            //FA_BTSendNumber (FA_ReadIR(0));
                
            FA_Backwards (distance);
            FA_Right(calibrationAngle);
            FA_SetMotors (30, 30);
            while(FA_ReadLine(0)>valTape){};
            FA_Forwards(105);
            IRright = FA_ReadIR(4);
        } 
            
        IRleft = FA_ReadIR(0);                                      //FA_ReadIR(0) could be straight in while statement, but it was not working,
                                                                    //I tried many ways and didn't want to waste more time, so I handled this by
        while((FA_ReadIR(4)>600) || (IRleft>20 && IRleft<250)){     //assigning value from FA_ReadIR(0) to variable "IRleft".

            FA_DelayMillis(1000);                                   //this delay helped me during demonstration, when I saw the robot turned slightly wrong angle I had a time to correct this
            //FA_BTSendString ("\nRead", 10);                       //these values sent via Bluetooth helped me to specify values to while loop above
            //FA_BTSendNumber (FA_ReadIR(4));
                
            FA_Backwards (distance);
            FA_Left(calibrationAngle);
            FA_SetMotors (30, 30);
            while(FA_ReadLine(0)>valTape){};
            FA_Forwards(105); 
            IRleft = FA_ReadIR(0);
        } 
    }
        
    void coordUpdate(){
        if(direction == North){
            Ycoord++;
        }else if(direction == East){
            Xcoord++;
        }else if(direction == South){
            Ycoord--;
        }else if(direction == West){
            Xcoord--;
        }
    }
    
    
    
    void checkWalls(){                      //check where around the robot are walls
        if(FA_ReadIR(0)>valWall){
            leftIR=true;
        }else{
            leftIR=false;
        }
        if(FA_ReadIR(2)>valWall){
            frontIR=true;
        }else{
            frontIR=false;
        }        
        if(FA_ReadIR(4)>valWall){
            rightIR=true;
        }else{
            rightIR=false;
        }
    }
    
    void scan(){
        if(!maze[Xcoord][Ycoord].visited){
            checkWalls();
            switch(direction){                              //saving layout of walls
                case North:
                    maze [Xcoord][Ycoord].visited = true;   
                    maze [Xcoord][Ycoord].north = frontIR;
                    maze [Xcoord][Ycoord].east = rightIR;
                    maze [Xcoord][Ycoord].south = false;    //robot just came from south, so there is no wall
                    maze [Xcoord][Ycoord].west = leftIR;
                    maze [1][0].south = true;               //start cell have wall on south, which is unusual
                    break;
                case East:
                    maze [Xcoord][Ycoord].visited = true;
                    maze [Xcoord][Ycoord].north = leftIR;
                    maze [Xcoord][Ycoord].east = frontIR;
                    maze [Xcoord][Ycoord].south = rightIR;
                    maze [Xcoord][Ycoord].west = false;
                    break;
                case South:
                    maze [Xcoord][Ycoord].visited = true;
                    maze [Xcoord][Ycoord].north = false;
                    maze [Xcoord][Ycoord].east = leftIR;
                    maze [Xcoord][Ycoord].south = frontIR;
                    maze [Xcoord][Ycoord].west = rightIR;
                    break;
                case West:
                    maze [Xcoord][Ycoord].visited = true;
                    maze [Xcoord][Ycoord].north = rightIR;
                    maze [Xcoord][Ycoord].east = false;
                    maze [Xcoord][Ycoord].south = leftIR;
                    maze [Xcoord][Ycoord].west = frontIR;
                break;
            }
        }      
    }
    

    void searchingNest(){
        if (FA_ReadLight()<valNest){
            nestFound = true;
            XcoordNest = Xcoord;                //saves current coordinates as coordinates of the nest
            YcoordNest = Ycoord;
            
            FA_PlayNote(1046.50,50);
            FA_PlayNote(2093.00,200);
            
        }
    }
    
    
    void BTsendCell(){                                              //prints on smartphone which walls robot just recognised for specific cell
        int N,E,S,W;
        if(maze [Xcoord][Ycoord].north){N = 1;}else{N = 0;};
        if(maze [Xcoord][Ycoord].east){E = 1;}else{E = 0;};
        if(maze [Xcoord][Ycoord].south){S = 1;}else{S = 0;};
        if(maze [Xcoord][Ycoord].west){W = 1;}else{W = 0;};
             
        FA_BTSendString ("\nCoordinates of cell: X-", 25);
        FA_BTSendNumber (Xcoord);
        FA_BTSendString ("  Y-",4);
        FA_BTSendNumber (Ycoord);
        FA_BTSendString ("\nN -", 5);
        FA_BTSendNumber (N);
        FA_BTSendString (" E -", 5);
        FA_BTSendNumber (E);
        FA_BTSendString (" S -", 5);
        FA_BTSendNumber (S);
        FA_BTSendString (" W -", 5);
        FA_BTSendNumber (W);
    }
    
    void wayHome(int X, int Y){                                         //recursive function which assign how far specific cell is from the nest, depends on walls around
        if((!maze[X][Y].north)&&(maze[X][Y+1].distanceToNest == NULL)){
            maze[X][Y+1].distanceToNest = maze[X][Y].distanceToNest+1;
            wayHome(X,Y+1);
        }
        if((!maze[X][Y].east)&&(maze[X+1][Y].distanceToNest == NULL)){
            maze[X+1][Y].distanceToNest = maze[X][Y].distanceToNest+1;
            wayHome(X+1,Y);
        }
        if((!maze[X][Y].south)&&(maze[X][Y-1].distanceToNest == NULL)){
            maze[X][Y-1].distanceToNest = maze[X][Y].distanceToNest+1;
            wayHome(X,Y-1);
        }
        if((!maze[X][Y].west)&&(maze[X-1][Y].distanceToNest == NULL)){
            maze[X-1][Y].distanceToNest = maze[X][Y].distanceToNest+1;
            wayHome(X-1,Y);
        }
    } 
    
    int nextStep;
    
    void backHome(){
        while(maze[Xcoord][Ycoord].distanceToNest != 0){
            FA_DelayMillis(1000);                   //this delay helped me during demonstration, when I saw the robot turned slightly wrong angle I had a time to correct this
            nextStep = maze[Xcoord][Ycoord].distanceToNest-1;   
            
            
            FA_BTSendString ("\nnextStep: ",30);    //prints on smartphone value of nextStep and distance to nest on cells around in case of debugging
            FA_BTSendNumber (nextStep);
            FA_BTSendString ("\ndistance north ",30);
            FA_BTSendNumber (maze[Xcoord][Ycoord+1].distanceToNest);
            FA_BTSendString ("\ndistance east ",30);
            FA_BTSendNumber (maze[Xcoord+1][Ycoord].distanceToNest);
            FA_BTSendString ("\ndistance south ",30);
            FA_BTSendNumber (maze[Xcoord][Ycoord-1].distanceToNest);
            FA_BTSendString ("\ndistance west ",30);
            FA_BTSendNumber (maze[Xcoord-1][Ycoord].distanceToNest);
            
            
            //check where should be next step and if there is a wall between
            if((maze[Xcoord][Ycoord+1].distanceToNest == nextStep) && (!maze[Xcoord][Ycoord].north)){
                i=North; j=West;

            }else if((maze[Xcoord+1][Ycoord].distanceToNest == nextStep) && (!maze[Xcoord][Ycoord].east)){
                i=East; j=North;

            }else if((maze[Xcoord][Ycoord-1].distanceToNest == nextStep) && (!maze[Xcoord][Ycoord].south)){
                i=South; j=East;

            }else if((maze[Xcoord-1][Ycoord].distanceToNest == nextStep) && (!maze[Xcoord][Ycoord].west)){
                i=West; j=South;
            }
            
            if(direction == j){                 //turning to right direction
                turnRight();
            }else{
                while(direction != i){
                    turnLeft();
                }
            }
            
            FA_SetMotors (30, 30);
            while(FA_ReadLine(0)>200){};
            FA_Forwards(105);
            calibration();
            coordUpdate();
        }   
        for(i=0;i<3;i++){                       //when robot returned to the nest play victory melody
            FA_PlayNote(1046.50,50);
            FA_PlayNote(1174.66,50);
            FA_PlayNote(1318.51,50);
            FA_PlayNote(1396.91,50);
            FA_PlayNote(1567.98,50);
            FA_PlayNote(1760.00,50);
            FA_PlayNote(1975.53,50);
            FA_PlayNote(2093.00,50);
        }
        FA_DelayMillis(10000);
    }
    
    void visitedEverything(){                   //check if every cell has been visited
        for(i=0;i<25;i++){
            if(!maze[i/5][i%5].visited){
                return;                         //if any cell are still unvisited, the function is break immediately, continuation of this is pointless and waste of resources 
            }
        }                                              
            FA_PlayNote(1046.50,50);            //if every cell was visited plays melody, then search for way to the nest
            FA_PlayNote(1174.66,50);
            FA_PlayNote(1318.51,50);
            FA_PlayNote(1396.91,50);
            FA_PlayNote(1567.98,50);
            FA_PlayNote(1760.00,50);
            FA_PlayNote(1975.53,50);
            FA_PlayNote(2093.00,50);
            wayHome(XcoordNest, YcoordNest);
            maze[XcoordNest][YcoordNest].distanceToNest = 0;
            
            FA_BTSendString ("\n",2);           //print on smartphone kind of map, which shows distance from every cell to the nest
            for(j=4; j>=0;j--){
                for(i=0; i<5;i++){
                    FA_BTSendNumber (maze[i][j].distanceToNest);
                    FA_BTSendString ("  ",2);
                }
                FA_BTSendString ("\n",2);
            }  
            
            backHome();                         //go back to the nest     
    }
    

    
    
    
    
    while(1){   
       FA_DelayMillis(1000);                    //this delay helped me during demonstration, when I saw the robot turned slightly wrong angle I had a time to correct this
        if(!nestFound){
            searchingNest();
        }
        scan();
        BTsendCell();                           //
        if(nestFound){
            visitedEverything();
        }
        
        if(FA_ReadIR(0)<20){                    //following left wall
            turnLeft();
        }else if(FA_ReadIR(2)<20){
        }else if(FA_ReadIR(4)<20){
            turnRight();
        }else{
            turnRight();
            turnRight();
            FA_Left(1);                         //correcting angle after turning 180 degrees
        }
        coordUpdate();
        
        FA_SetMotors (30, 30);
        while(FA_ReadLine(0)>200){};            //drives forward until black tape is found
        FA_Forwards(105);                       //drives into the middle of a cell
        
        calibration();                          
       
    }            
 	return 0;
}
