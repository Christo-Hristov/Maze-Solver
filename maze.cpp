/* This file includes functions that are ultimately used to
 * produce a solution to a provided maze.  The maze is in the form
 * of a grid of booleans, true indicating a clear space, and false
 * indicating a wall.  The solution is represented as a stack of
 * grid locations where the top of the stack is the final move
 * through the maze.
 * This code was written solely by Christo Hristov
 */

#include <iostream>
#include <fstream>
#include "error.h"
#include "filelib.h"
#include "grid.h"
#include "maze.h"
#include "mazegraphics.h"
#include "queue.h"
#include "set.h"
#include "stack.h"
#include "vector.h"
#include "testing/SimpleTest.h"
using namespace std;


/* This function takes in a grid of booleans (the maze) and a current grid location
 * and returns a set of the valid moves from that location.  Valid moves are
 * moves that are one step away to the north, south, east, and west from the current position,
 * in the grid, and not a wall.
 */
Set<GridLocation> generateValidMoves(Grid<bool>& maze, GridLocation cur) {
    Set<GridLocation> neighbors;
    if (maze.inBounds(cur.row, cur.col - 1) && maze[cur.row][cur.col - 1]){ // checking if north location is open and in bounds
        GridLocation north = {cur.row, cur.col - 1};
        neighbors.add(north);
    }
    if (maze.inBounds(cur.row - 1, cur.col) && maze[cur.row - 1][ cur.col]) { // checking if west location is open and in bounds
        GridLocation west = {cur.row - 1, cur.col};
        neighbors.add(west);
    }
    if (maze.inBounds(cur.row, cur.col + 1) && maze[cur.row][ cur.col + 1]) { // checking if south location is open and in bounds
        GridLocation south = {cur.row, cur.col + 1};
        neighbors.add(south);
    }
    if (maze.inBounds(cur.row + 1, cur.col) && maze[cur.row + 1][ cur.col]) { // checking if east location is open and in boudns
        GridLocation east = {cur.row + 1, cur.col};
        neighbors.add(east);
    }
    return neighbors;
}

/* This function takes in a maze (grid of booleans) and a stack of grid locations
 * delineating a solution path to the maze.  It then checks if that path is valid
 * by making sure each move is valid, the path starts at the upper left of the grid and
 * ends at the lower right corner, and that the path does not loop, or go over the same
 * spot twice.
 */
void validatePath(Grid<bool>& maze, Stack<GridLocation> path) {
    GridLocation mazeExit = {maze.numRows()-1,  maze.numCols()-1}; // initializes a GridLocation object to the bottom right square of the grid
    if (path.isEmpty()) {
        error ("Path is empty");
    }

    if (path.peek() != mazeExit) {
        error("Path does not end at maze exit"); // checks if path ends at the maze exit and if not returns an error
    }
    GridLocation target = path.pop(); // creates a GridLocation variable target which will keep track of the last move of the path
    Set<GridLocation> repeats; // initializing a set which keeps track of which GridLocations have already been used to account for loops
    repeats.add(target);
    while (!path.isEmpty()) {
        GridLocation check = path.peek(); // create a variable check without changign the stack that keeps track of the move before the target move
        if (!generateValidMoves(maze, check).contains(target)) { // checks if target move is a valid move following the check move
            error("A move in the path is not a valid move");
        }
        target = path.pop(); // update target to the next move in the path and removes that move from the stack
        if (repeats.contains(target)) { // checks if the next move in the stack has already been done (the path has a loop)
            error("There is a loop in the path");
        } else {
            repeats.add(target); // if there is no loop, the move completed is added to the repeat set
        }

    }
    if (target.row != 0 || target.col != 0 || !maze[target]) { // checks to make sure the last position in the stack is the start of the maze
        error("Path does not start at maze start");
    }

}
/*
 * Given a stack of grid locations and a specific target grid location,
 * stackContains returns true if the target grid location is in the stack,
 * and false otherwise.
 */
bool stackContains(Stack<GridLocation> &stack, GridLocation target) {
    while (!stack.isEmpty()){
        GridLocation check = stack.pop(); // sets a check variable to the grid location at the top of the stack
        if (target == check) {
            return true;
        }
    }
    return false;
}


/* Given a grid of boleans (maze) as an input, return a stack of grid locations
 * that describe the shortest path from the top left entry of the maze to the
 * bottom right exit of the maze.  Within this stack, the top element is the last
 * move of the path and the bottom element is the first move.  Assume that the inputted
 * grid is at least a 2x2 grid with a solution and open exit and entry points.
 */
Stack<GridLocation> solveMaze(Grid<bool>& maze) {
    MazeGraphics::drawGrid(maze); // draws the maze in a graphics window
    Queue<Stack<GridLocation>> paths; // initializes a queue that keeps track of the current testable paths
    GridLocation end = {maze.numRows()-1,  maze.numCols()-1}; // defines the end location of the maze
    Stack<GridLocation> start = {{0, 0}};
    paths.enqueue(start);
    while (!paths.isEmpty()) {
        Stack<GridLocation> currentPath = paths.dequeue(); // initialize currentPath to the path at the front of the paths queue
        MazeGraphics::highlightPath(currentPath, "blue", 10); // draws the current path in a graphics window
        GridLocation currentMove = currentPath.peek(); // initializes a variable current move to the latest move in the path
        if (currentMove == end) { // checks to see if the last move brings the path to the end
            return currentPath;
        }
        Set<GridLocation> validMoves = generateValidMoves(maze, currentMove); // creates a set of valid moves following the last move of the current path being checked
        for (GridLocation move: validMoves) {
           Stack<GridLocation> copy = currentPath; // creates a copy of the current path so that the current path is not affected by the call to stackContains
           if (!stackContains(copy, move)) { // checks if the move is already in the stack, if not, the move is added to the stack, the stack added to the queue, and then the move removed from the stack
               currentPath.push(move);
               paths.enqueue(currentPath);
               currentPath.pop();
           }
        }

    }
    return start;
}

/*
 * The given readMazeFile function correctly reads a well-formed
 * maze from a file.
 *
 * This provided function is fully correct. You do not need to change
 * any of the code in this function.
 */
void readMazeFile(string filename, Grid<bool>& maze) {
    /* The following code reads data from the file into a Vector
     * of strings representing the lines of the file.
 0,0    */
    ifstream in;

    if (!openFile(in, filename))
        error("Cannot open file named " + filename);

    Vector<string> lines;
    readEntireFile(in, lines);

    /* Now that the file data has been read into the Vector, populate
     * the maze grid.
     */
    int numRows = lines.size();        // rows is count of lines
    int numCols = lines[0].length();   // cols is length of line
    maze.resize(numRows, numCols);     // resize grid dimensions

    for (int r = 0; r < numRows; r++) {
        if (lines[r].length() != numCols) {
            error("Maze row has inconsistent number of columns");
        }
        for (int c = 0; c < numCols; c++) {
            char ch = lines[r][c];
            if (ch == '@') {        // wall
                maze[r][c] = false;
            } else if (ch == '-') { // corridor
                maze[r][c] = true;
            } else {
                error("Maze location has invalid character: '" + charToString(ch) + "'");
            }
        }
    }
}

/* The given readSolutionFile correctly reads a path from a file.
 *
 * This provided function is fully correct. You do not need to change
 * any of the code in this function.
 */
void readSolutionFile(string filename, Stack<GridLocation>& soln) {
    ifstream in;

    if (!openFile(in, filename)) {
        error("Cannot open file named " + filename);
    }

    if (!(in >> soln)) {// if not successfully read
        error("Maze solution did not have the correct format.");
    }
}


/* * * * * * Test Cases * * * * * */

PROVIDED_TEST("generateValidMoves on location in the center of 3x3 grid with no walls") {
    Grid<bool> maze = {{true, true, true},
                       {true, true, true},
                       {true, true, true}};
    GridLocation center = {1, 1};
    Set<GridLocation> expected = {{0, 1}, {1, 0}, {1, 2}, {2, 1}};

    EXPECT_EQUAL(generateValidMoves(maze, center), expected);
}

PROVIDED_TEST("generateValidMoves on location on the side of 3x3 grid with no walls") {
    Grid<bool> maze = {{true, true, true},
                       {true, true, true},
                       {true, true, true}};
    GridLocation side = {0, 1};
    Set<GridLocation> expected = {{0,0}, {0,2}, {1, 1}};

    EXPECT_EQUAL(generateValidMoves(maze, side), expected);
}

PROVIDED_TEST("generateValidMoves on corner of 2x2 grid with walls") {
    Grid<bool> maze = {{true, false},
                       {true, true}};
    GridLocation corner = {0, 0};
    Set<GridLocation> expected = {{1, 0}};

    EXPECT_EQUAL(generateValidMoves(maze, corner), expected);
}

PROVIDED_TEST("validatePath on correct solution") {
    Grid<bool> maze = {{true, false},
                       {true, true}};
    Stack<GridLocation> soln = { {0 ,0}, {1, 0}, {1, 1} };

    EXPECT_NO_ERROR(validatePath(maze, soln));
}

PROVIDED_TEST("validatePath on correct solution loaded from file for medium maze") {
    Grid<bool> maze;
    Stack<GridLocation> soln;
    readMazeFile("res/5x7.maze", maze);
    readSolutionFile("res/5x7.soln", soln);

    EXPECT_NO_ERROR(validatePath(maze, soln));
}

PROVIDED_TEST("validatePath on correct solution loaded from file for large maze") {
    Grid<bool> maze;
    Stack<GridLocation> soln;
    readMazeFile("res/25x33.maze", maze);
    readSolutionFile("res/25x33.soln", soln);

    EXPECT_NO_ERROR(validatePath(maze, soln));
}

PROVIDED_TEST("validatePath on invalid path should raise error") {
    Grid<bool> maze = {{true, false},
                       {true, true}};
    Stack<GridLocation> not_end_at_exit = { {1, 0}, {0, 0} };
    Stack<GridLocation> not_begin_at_entry = { {1, 0}, {1, 1} };
    Stack<GridLocation> go_through_wall = { {0 ,0}, {0, 1}, {1, 1} };
    Stack<GridLocation> teleport = { {0 ,0}, {1, 1} };
    Stack<GridLocation> revisit = { {0 ,0}, {1, 0}, {0, 0}, {1, 0}, {1, 1} };

    EXPECT_ERROR(validatePath(maze, not_end_at_exit));
    EXPECT_ERROR(validatePath(maze, not_begin_at_entry));
    EXPECT_ERROR(validatePath(maze, go_through_wall));
    EXPECT_ERROR(validatePath(maze, teleport));
    EXPECT_ERROR(validatePath(maze, revisit));
}

PROVIDED_TEST("solveMaze on file 5x7") {
    Grid<bool> maze;
    readMazeFile("res/5x7.maze", maze);
    Stack<GridLocation> soln = solveMaze(maze);

    EXPECT_NO_ERROR(validatePath(maze, soln));
}

PROVIDED_TEST("solveMaze on file 21x23") {
    Grid<bool> maze;
    readMazeFile("res/21x23.maze", maze);
    Stack<GridLocation> soln = solveMaze(maze);

    EXPECT_NO_ERROR(validatePath(maze, soln));
}

STUDENT_TEST("generateValidMoves on empty grid") {
    Grid<bool> maze = {};
    GridLocation center = {1, 1};
    Set<GridLocation> expected = {};

    EXPECT_EQUAL(generateValidMoves(maze, center), expected);
}

STUDENT_TEST("generateValidMoves on location at center of 1x1 grid") {
    Grid<bool> maze = {{true}};
    GridLocation center = {0,0};
    Set<GridLocation> expected = {};

    EXPECT_EQUAL(generateValidMoves(maze, center), expected);
}

STUDENT_TEST("generateValidMoves on location at center of 3x3 grid with only walls") {
    Grid<bool> maze = {{false, false, false},
                       {false, false, false},
                       {false, false, false}};
    GridLocation center = {1,1};
    Set<GridLocation> expected = {};

    EXPECT_EQUAL(generateValidMoves(maze, center), expected);
}

STUDENT_TEST("validatePath on path that goes outside of maze") {
    Grid<bool> maze = {{true, true}};
    Stack<GridLocation> outsideMaze = { {0, 0}, {1, 0}, {1, 1}, {0, 1} };
    EXPECT_ERROR(validatePath(maze, outsideMaze));
}

STUDENT_TEST("validatePath on empty path") {
    Grid<bool> maze = {{true, true}};
    Stack<GridLocation> empty = {};
    EXPECT_ERROR(validatePath(maze, empty));
}

STUDENT_TEST("validatePath on path with one input") {
    Grid<bool> correctMaze = {{true}};
    Stack<GridLocation> one = {{0, 0}};
    EXPECT_NO_ERROR(validatePath(correctMaze, one));

    Grid<bool> incorrectMaze = {{true, false}};
    EXPECT_ERROR(validatePath(incorrectMaze, one));
}

STUDENT_TEST("validatePath on path that ends on a wall") {
    Grid<bool> maze = {{true, false},
                       {true, false}};
    Stack<GridLocation> soln = { {0 ,0}, {1, 0}, {1, 1} };
    EXPECT_ERROR(validatePath(maze, soln));
}

STUDENT_TEST("validatePath on path that starts on a wall") {
    Grid<bool> maze = {{false, false},
                       {true, true}};
    Stack<GridLocation> soln = { {0 ,0}, {1, 0}, {1, 1} };
    EXPECT_ERROR(validatePath(maze, soln));
}

STUDENT_TEST("stackContains on an empty stack") {
    Stack<GridLocation> empty = {};
    GridLocation test = {1, 0};
    EXPECT(!stackContains(empty, test));
}

STUDENT_TEST("stackContains with a stack that contains the location") {
    Stack<GridLocation> empty = {{1, 0}};
    GridLocation test = {1, 0};
    EXPECT(stackContains(empty, test));
}

STUDENT_TEST("stackContains on a stack that does not contain the location") {
    Stack<GridLocation> empty = {{0, 0}};
    GridLocation test = {1, 0};
    EXPECT(!stackContains(empty, test));
}

STUDENT_TEST("solveMaze on file 33x41") {
    Grid<bool> maze;
    readMazeFile("res/33x41.maze", maze);
    Stack<GridLocation> soln = solveMaze(maze);

    EXPECT_NO_ERROR(validatePath(maze, soln));
}

STUDENT_TEST("solveMaze on file 25x33") {
    Grid<bool> maze;
    readMazeFile("res/25x33.maze", maze);
    Stack<GridLocation> soln = solveMaze(maze);

    EXPECT_NO_ERROR(validatePath(maze, soln));
}



