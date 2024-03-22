#ifndef SPACESECTORBST_H
#define SPACESECTORBST_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

#include "Sector.h"

class SpaceSectorBST {

public:
    Sector *root;

    SpaceSectorBST();

    ~SpaceSectorBST();

    void readSectorsFromFile(const std::string &filename);

    void insertSectorByCoordinates(int x, int y, int z);

    void deleteSector(const std::string &sector_code);

    void displaySectorsInOrder();

    void displaySectorsPreOrder();

    void displaySectorsPostOrder();

    std::vector<Sector *> getStellarPath(const std::string &sector_code);

    void printStellarPath(const std::vector<Sector *> &path);

    Sector *insertSectorRecursive(Sector *node, int x, int y, int z);

    void displaySectorsInOrderRecursive(Sector *node);

    void displaySectorsPreOrderRecursive(Sector *node);

    void displaySectorsPostOrderRecursive(Sector *node);


    Sector *findMin(Sector *node) const;


    Sector *findSector(const Sector *root, const std::string &sector_code) const;

    Sector *deleteSectorRecursive(Sector *current, Sector *nodeToDelete);


    Sector *deleteNodeAndHandleCases(Sector *current, Sector *nodeToDelete);

    void handleCase1(Sector *&current, Sector *nodeToDelete);

    void handleCase2(Sector *&current, Sector *nodeToDelete);

    void handleCase3(Sector *&current, Sector *nodeToDelete);

    void deleteSectorsPostOrder(Sector *node);
};

#endif // SPACESECTORBST_H
