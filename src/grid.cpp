#include "grid.h"
#include <random>

Grid::Grid(int w, int h): width(w), height(h), cells(w*h, 0){}


int Grid::index(int x, int y) const{
    return y * width + x;
}

bool Grid::isValid(int x, int y)const{
    return x>=0 && x<width && y>=0 && y<height;


}


bool Grid::isObstacle(int x, int y)const{
    return cells[index(x, y)] == 1;
}

void Grid::setObstacle(int x, int y){
    if(isValid(x, y)){
        cells[index(x, y)] = 1;
    }
}

void Grid::generateRandomObstacles(double density){
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<>dist(0.0, 1.0);

    for(int y=0;y<height;y++){
        for(int x=0;x<width;x++){
            if(dist(gen)< density){
                setObstacle(x, y);
            }
        }
    }
}


int Grid::getWidth()const{
    return width;
}

int Grid::getHeight()const{
    return height;
}