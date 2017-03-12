/*
 * Snake in OpenCV: small, playable version of snake to test the
 * ability to create simple games with OpenCV. Total dev time ~1h
 *
 * Author: Matthias Kardel <mkardel@gmail.com> 2016
 */

#include <iostream>
#include <stdio.h>
#include <stdlib.h>

#include <deque>

#include <sstream>

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/opencv.hpp>

#include <ctime>    // For time()
#include <cstdlib>  // For srand() and rand()

using namespace std;
using namespace cv;

int gameboard_width = 20;
int gameboard_height = 20;

int game_speed = 25;
int step_size = 2; // step every nth frame
int scale = 10;
deque<Point> snake_pixel;

int move_direction = -1;
int move_direction_old = -1;

bool game_over = false;
bool took_dot = false;
int frame = 0;
Point dot(-1,-1);

bool paused = false;

bool testing = false;

Scalar foreground(0,128,0);
Scalar background(0,0,0);

bool self_collision_detection(){
    for(int i = 1 ; i < snake_pixel.size(); ++i){
        if( (snake_pixel[0].x == snake_pixel[i].x) &&
            (snake_pixel[0].y == snake_pixel[i].y)){
            return true;
        }
    }
    return false;
}

int main(){


    Mat empty_gameboard(gameboard_height, gameboard_width, CV_8UC3, background);
    Mat gameboard;
    // initialize snake pixel:
    snake_pixel.push_front(Point(int(gameboard_width/2), int(gameboard_height/2)));

    // test: put longer snake
    if(testing){
        for(int i = 1; i < 20; ++i)
        snake_pixel.push_back(Point(int(gameboard_width/2)+i, int(gameboard_height/2)));
    }
    srand(time(0));

    for(;;){

        // draw the gameboard
        gameboard = empty_gameboard.clone();
        for( int i = 0; i < snake_pixel.size(); ++i ) {
            line(gameboard, snake_pixel[i], snake_pixel[i], foreground);
        }

        // test if dot was collected

        // generate new dot
        if( took_dot || (frame == 0) ){
            int x = (rand() % (gameboard_width - 1))  + 1;
            int y = (rand() % (gameboard_height - 1)) + 1;
            dot.x = x;
            dot.y = y;
            took_dot = false;
        }
        line(gameboard, dot, dot, foreground); // draw dot

        resize(gameboard, gameboard, cvSize( int(gameboard.cols * scale), int(gameboard.rows * scale)), 0, 0, INTER_NEAREST);

        // detect collision with wall
        if( (snake_pixel[0].x == -1 && move_direction == 1 ) ||
            (snake_pixel[0].y == -1 && move_direction == 0 ) ||
            (snake_pixel[0].x == gameboard_width && move_direction == 3) ||
            (snake_pixel[0].y == gameboard_height  && move_direction == 2)) {
                // collision with wall
                game_over = true;                
        }

        // detect collision with self
        if( self_collision_detection() )
            game_over = true;


        char key = (char)waitKey(game_speed);

        //draw score
        if ( true ) {
            ostringstream score_stream;
            score_stream << "SCORE: " << snake_pixel.size() - 1;
            string score = score_stream.str();
            putText(gameboard, score, cvPoint(gameboard.cols/2 - gameboard.cols/5 , gameboard.rows-gameboard.rows/10),
                        FONT_HERSHEY_COMPLEX_SMALL, 0.8, foreground, 1, CV_AA);
        }

        if( game_over ){
            move_direction = -1;
            ostringstream text_stream;
            text_stream << "GAME OVER";
            string text = text_stream.str();

            putText(gameboard, text, cvPoint(gameboard.cols / 2 - gameboard.cols/4 , gameboard.rows / 2),
                    FONT_HERSHEY_COMPLEX_SMALL, 0.8, foreground, 1, CV_AA);
            if(key == 'r' || key == 'R'){
                deque<Point> new_s;
                new_s.push_front(Point(int(gameboard_width/2), int(gameboard_height/2)));
                snake_pixel=new_s;
                game_over = false;
                frame = 0;
            }

        } else {
            move_direction_old = move_direction;
            if(key == 'W' || key == 'w')
                move_direction = 0; //up
            if(key == 'A' || key == 'a')
                move_direction = 1; //left
            if(key == 'S' || key == 's')
                move_direction = 2; //down
            if(key == 'D' || key == 'd')
                move_direction = 3; //down
        }


        if( paused ) {
            ostringstream pause_stream;
            pause_stream << "PAUSED";
            string pause = pause_stream.str();

            putText(gameboard, pause, cvPoint(gameboard.cols / 2 - 30 , gameboard.rows / 2),
                    FONT_HERSHEY_COMPLEX_SMALL, 0.8, foreground, 1, CV_AA);

        } else if(frame % step_size == 0){

            if( snake_pixel[0] == dot ){

                took_dot = true;
            }

            switch(move_direction){
                case 0:
                    snake_pixel.push_front( Point(snake_pixel[0].x, snake_pixel[0].y - 1  ));
                    break;
                case 1:
                    snake_pixel.push_front( Point(snake_pixel[0].x - 1, snake_pixel[0].y ));
                    break;

                case 2:
                    snake_pixel.push_front( Point(snake_pixel[0].x, snake_pixel[0].y + 1  ));
                    break;

                case 3:
                    snake_pixel.push_front( Point(snake_pixel[0].x + 1, snake_pixel[0].y ));
                    break;

            }

            // FIXME: this disables every collision with itself
            if(self_collision_detection()){ // reverse into self not possible
                snake_pixel.pop_front();
                move_direction = move_direction_old;
            } else if( (move_direction > -1) && !took_dot){ // out of initial state and dot wasnt taken
                snake_pixel.pop_back();
            }

        }

        namedWindow("SNAKE!", WINDOW_AUTOSIZE);
        imshow("SNAKE!", gameboard );
        frame++;

        if(key == 'q' || key == 'Q' || key == 27)
            break;
        if(key == 'p' || key == 'P'){
                paused = !paused;
        }



    }
    return 0;
}
