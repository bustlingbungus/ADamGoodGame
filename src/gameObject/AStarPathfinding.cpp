#include "AStarPathfinding.hpp"

//////////////////////////////////////////////////////////////////
//                                                              //
//                            NOTE                              //
//                                                              //
//     the   pathfinding   algorithm   in   this  file  has     //
//     been modified  to not  consider diagonal  movements.     //
//                                                              //
//     this was done to  prevent entities from attemting to     //
//     move diagonally on two  traversible cells when there     //
//     are adjacent  barrier  cells  that  would prevent or     //
//                     slow their movement.                     //  
//                                                              //
//     For example, when  rounding  a corner, or when there     //
//     are  barriers   placed  diagonally  (imagine  how  a     //
//     bishop  may  move  between   two  diagonally  placed     //
//     pawns).  Entities are  not able to  do this, as they     //
//                         get blocked.                         //
//                                                              //
//     Unfortunately, this leads to some  ineficcient paths     //
//     for  entities  in this  game.  When walking  through     //
//     unobstructed  areas,  they have  to move  diagonally     //
//     with  an  awkward  series  of  orthogonal  movements     //
//     (imagine  a rook  trying to  move  across  an  empty     //
//               chessboard, in a diagonal line).               //
//                                                              //
//////////////////////////////////////////////////////////////////

namespace AStar
{
    std::vector<std::vector<int>> *grid = nullptr;
    int *barrier = nullptr;



    // sets the grid to pathfind in, as well as the integer used to represent barriers
    void Open(std::vector<std::vector<int>> *Grid, int *Barrier) {
        grid = Grid; barrier = Barrier;
    }



    // constructor
    Node::Node(Vector2Int Cell, Vector2Int Destination, int Initial_gCost, Node *Parent) : 
    cell(Cell), B(Destination), gCost(Initial_gCost), parent(Parent) { 
        hCost = findCost(cell, B);
        fCost = gCost + hCost;

        int num = (*grid)[cell.x][cell.y];
        isTraversible = (num&(*barrier))? false : true;
    }

    Node::Node(Vector2Int Cell, Vector2Int Destination) : cell(Cell), B(Destination) {
        fCost = gCost = -1;
        hCost = findCost(cell, B);
        int num = (*grid)[cell.x][cell.y];
        isTraversible = (num&(*barrier))? false : true;    
    }


    // determines if a node is valid (is in open OR closed)
    bool Node::isValidNode() {
        return fCost<0 || hCost<0 || gCost<0;
    }

    // sets the gCost, and updates the fCost accordingly
    void Node::set_gCost(int new_gCost) {
        gCost = new_gCost;
        fCost = gCost + hCost;
    }



    // returns the cells of the neighbouring nodes
    std::vector<Vector2Int> Node::getNeighbours()
    {
        int x1 = cell.x-1, x2 = cell.x+1, y1 = cell.y-1, y2 = cell.y+1;
        return {{cell.x,y1}, {x1,cell.y}, {x2,cell.y}, {cell.x,y2},
                {x1,y1},{x2,y1},{x1,y2},{x2,y2}};
    }






    bool Node::operator==(const Node& other) {
        // nodes are considered equal if they have the same coordinates
        return cell == other.cell;
    }




    // finds the distance from the destination to the node's cell
    int findCost(Vector2Int origin, Vector2Int dest)
    {
        int res = 0;
        // method: increase the distance until the two points are equal
        while (dest != origin) {
            int dispX = clamp(-1, 1, origin.x-dest.x),
                dispY = clamp(-1, 1, origin.y-dest.y),
                distance = clamp(0, 14, (10*Abs(dispX))+(10*Abs(dispY)));

            dest.x += dispX; dest.y += dispY;
            res += distance;
        }
        return res;
    }





    // constructor, takes a start point, an endpoint, and the dimensions of a map
    Pathfinding::Pathfinding(Vector2Int Start, Vector2Int End, Vector2Int MapDimensions, unsigned int Depth) :
    start(Start), end(End), searchDepth(Depth) {
        // make the entire nodes vector empty node objects
        for (int x = 0; x < MapDimensions.x; x++) {
            for (int y = 0; y < MapDimensions.y; y++) {
                Vector2Int p(x, y);
                auto node = new Node(p, end);
                nodes.push_back(node);
            }
        }
    }



    // searches for the best path between start and end
    LinkedCell * Pathfinding::findPath()
    {
        // add the start node to open
        open.push_back(new Node(start, end, 0, nullptr));

        // also keep track of the closest node; if a path is not found, return the 
        // noce that was closest
        Node *currNode = nullptr, *closest = nullptr;
        unsigned int depth = 0;
        // loop until the path has been found
        while (true)
        {
            // find the bext node in open to explore
            currNode = findBestNode();
            // remove it from open, and add it to closed
            removeNodeFromOpen(currNode);
            closed.push_back(currNode);
            

            // if the current node is the end node, a path has been found
            if (currNode->cell == end) break;
            else if (depth >= searchDepth) {
                currNode = closest;
                break;
            }
            depth++;


            // neighbours of the current node (there will be 4)
            std::vector<Vector2Int> neighbours = currNode->getNeighbours();
            for (int i = 0; i < 4; i++) 
            {
                Vector2Int cell = neighbours[i];
                int idx = findCellInVector(cell, nodes);
                if (idx < 0) continue; // when the cell is out of bounds (happens when currNode is on a map edge)
                Node* neighbour = nodes[idx];

                // skip if the neighbour is not traversible, or is in closed
                if (!neighbour->isTraversible || findNodeInVector(neighbour, closed)>=0) continue;

                // if the new path to neighbour is shorter OR neighbour is not in open
                // set the fCost of neighbour, and make neighbour's parent currNode
                int new_gCost = findCost(currNode->cell, neighbour->cell) + currNode->gCost;
                bool notInOpen = findNodeInVector(neighbour, open) == -1;

                if (new_gCost<neighbour->gCost || notInOpen) {
                    // set the fCost and parent
                    neighbour->set_gCost(new_gCost);
                    neighbour->parent = currNode;
                    // add the neighbouring cell to open, if it wasnt there already
                    if (notInOpen) open.push_back(neighbour);

                }
            }
            // if this is the new closest cell (lowest fCost), save it
            if (closest == nullptr) closest = currNode;
            else if (currNode->fCost<closest->fCost || (currNode->fCost==closest->fCost&&currNode->hCost < closest->hCost)) 
                closest = currNode;
        }
        // convert the path found into a linked list of cells, then return it
        LinkedCell *res = convertPathToLinkedCells(currNode);
        // PrintLinkedList(currNode);
        return res;
    }



    // finds the best node in open to evaluate (lowest fCost, if equal then lowest hCost)
    Node * Pathfinding::findBestNode()
    {
        Node *res = open[0];
        // keep track of the lowest fCost and hCosts
        int lowest_f = res->fCost, lowest_h = res->hCost;

        int n = open.size();
        for (int i = 1; i < n; i++) {
            Node* curr = open[i];

            // a node is the best if it has the lowest fCost, OR if it has an equally low
            // fCost AND a lower hCost 
            if (curr->fCost<lowest_f || (curr->fCost==lowest_f && curr->hCost<lowest_h)) {
                lowest_f = curr->fCost; lowest_h = curr->hCost;
                res = curr;
            }
        }
        return res;
    }



    // removes a node from open. does nothing if the node is not in open
    void Pathfinding::removeNodeFromOpen(Node *node)
    {
        // find the index of node in the open vector
        int idx = findNodeInVector(node, open);
        // if idx >= 0, erase it from open, otherwise do nothing
        if (idx >= 0) open.erase(open.begin() + idx);
    }

    // returns true if the specified node is in the specified vector
    int Pathfinding::findNodeInVector(Node *node, std::vector<Node*> vec) {
        return findCellInVector(node->cell, vec);
    }

    int Pathfinding::findCellInVector(Vector2Int cell, std::vector<Node*> vec) {
        int n = vec.size();
        for (int i = 0; i < n; i++) {
            if (cell == vec[i]->cell) {
                return i;
            }
        }
        return -1;
    }


    // cleans up the pointers for all cells, except those in the returned path
    void Pathfinding::close()
    {
        int n = nodes.size();
        for (int i = 0; i < n; i++) {
            Node *curr = nodes[i];
            if (curr) delete curr;
        }
    }



    // constructor
    LinkedCell::LinkedCell(Vector2Int Cell, LinkedCell *Next) : cell(Cell), next(Next) {}



    // simply converts all the nodes in the finished path to LinkedCell objects
    LinkedCell * Pathfinding::convertPathToLinkedCells(Node *end)
    {
        if (end == nullptr) return nullptr;
        LinkedCell *currCell = new LinkedCell(end->cell, nullptr), *res = currCell;
        Node *currNode = end->parent;

        while (currNode != nullptr) {
            LinkedCell *prev = currCell;
            currCell = new LinkedCell(currNode->cell, nullptr);
            prev->next = currCell;
            currNode = currNode->parent;
        }
        return res;
    }





    

    // reconnects the linked list of nodes in reverse order
    Node * ReverseLinkedList(Node *end)
    {
        if (end == nullptr) return end;
        Node *prev = nullptr, *next = end->parent;
        while (next != nullptr) {
            end->parent = prev;
            prev = end;
            end = next;
            next = next->parent;
        }
        end->parent = prev;
        return end;
    }

    LinkedCell * ReverseLinkedList(LinkedCell *end)
    {
        if (end == nullptr) return end;
        LinkedCell *prev = nullptr, *next = end->next;
        while (next != nullptr) {
            end->next = prev;
            prev = end;
            end = next;
            next = next->next;
        }
        end->next = prev;
        return end;
    }


    void PrintLinkedList(AStar::Node* head)
    {
        if (head == nullptr) return;
        AStar::Node *curr;
        for (curr = head; curr->parent != nullptr; curr = curr->parent) {
            Vector2Int p = curr->cell;
            std::cout <<'('<< p.x <<", "<< p.y <<") -> ";
        }
        Vector2Int p = curr->cell;
        std::cout <<'('<< p.x <<", "<< p.y <<")\n\n";
    }


    void PrintLinkedList(LinkedCell *head)
    {
        if (head == nullptr) return;
        LinkedCell *curr;
        for (curr = head; curr->next != nullptr; curr = curr->next) {
            Vector2Int p = curr->cell;
            std::cout <<'('<< p.x <<", "<< p.y <<") -> ";
        }
        Vector2Int p = curr->cell;
        std::cout <<'('<< p.x <<", "<< p.y <<")\n\n";
    }
}