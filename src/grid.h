#ifndef GRID_H
#define GRID_H

#include <vector>

class Grid {
public:
    Grid(int width, int height);

    bool isValid(int x, int y) const;
    bool isObstacle(int x, int y) const;
    void setObstacle(int x, int y);
    void generateRandomObstacles(double density);
    int getWidth() const;
    int getHeight() const;

private:
    int width;
    int height;
    std::vector<int> cells;

    int index(int x, int y) const;
};

#endif
