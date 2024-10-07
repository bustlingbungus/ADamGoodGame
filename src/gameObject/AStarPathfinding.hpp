#ifndef A_STAR_HPP
#define A_STAR_HPP

#include <vector>
#include <iostream>
#include <memory>

#include "../engine/GameMath.hpp"

using namespace Math;

namespace AStar
{
    // the grid that the nodes will use to determine where non-traversible cells are
    extern std::vector<std::vector<int>> *grid;
    // the value in grid that represents a non-traversible cell
    extern int *barrier; 


    // sets the above pointers
    // sets the grid to pathfind in, as well as the integer used to represent barriers
    void Open(std::vector<std::vector<int>> *Grid, int *Barrier);



    // a class to represent a node in an A* grid
    class Node {
    public:
        Vector2Int cell; // the cell the node inhabits

        int gCost, // distance away from the starting node, point A 
            hCost, // distance away from the destination node, point B
            fCost; // sum of g and h cost

        bool isTraversible = false; // whether or not the cell may be traversed
        Node * parent = nullptr;

        // constructor
        Node(Vector2Int Cell, Vector2Int Destination, int Initial_gCost, Node *Parent);
        Node(Vector2Int Cell, Vector2Int Destination);

        // determines if a node is valid (is in open OR closed)
        bool isValidNode();

        // sets the gCost, and updates the fCost accordingly
        void set_gCost(int new_gCost);


        // returns the cells of the neighbouring nodes
        std::vector<Vector2Int> getNeighbours();

        bool operator==(const Node& other);

    private:
        // B the destination cell
        Vector2Int B;

    };




    // finds the distance from the destination to the node's cell
    int findCost(Vector2Int origin, Vector2Int dest);




    // much simpler structure than Node, just a cell and a pointer to the next cell
    struct LinkedCell {
        Vector2Int cell;
        LinkedCell *next;
        // constructor
        LinkedCell(Vector2Int Cell, LinkedCell *Next);
    };



    class Pathfinding {
    public:
        // searches for the best path between start and end
        // returns a pointer to the end node, which by following parent nodes will reveal a path
        LinkedCell * findPath();

        // constructor, takes a start point, an endpoint, and the dimensions of a map
        Pathfinding(Vector2Int Start, Vector2Int End, Vector2Int MapDimensions, unsigned int Depth);

        // cleans up the pointers for all cells
        void close();

    private:
        // how many times the pathfinding loop will iterate before giving up
        unsigned int searchDepth = INT_MAX;

        // list of all nodes
        std::vector<Node*> nodes;
        // list of open and closed nodes
        std::vector<Node*> open, closed;

        // beginning and end of the path
        Vector2Int start, end;


        // finds the best node in open to evaluate (lowest fCost, if equal then lowest hCost)
        Node * findBestNode();

        // removes a node from open. does nothing if the node is not in open
        void removeNodeFromOpen(Node *node);


        // returns true if the specified node is in the specified vector
        int findNodeInVector(Node *node, std::vector<Node*> vec);
        int findCellInVector(Vector2Int cell, std::vector<Node*> vec);

        // simply converts all the nodes in the finished path to LinkedCell objects
        LinkedCell * convertPathToLinkedCells(Node *end);
    };
    


    // reconnects the linked list of nodes in reverse order
    Node * ReverseLinkedList(Node *end);
    LinkedCell * ReverseLinkedList(LinkedCell *end);


    void PrintLinkedList(AStar::Node* head);
    void PrintLinkedList(LinkedCell *head);
}

#endif