#ifndef SPACESECTORLLRBT_H
#define SPACESECTORLLRBT_H

#include "Sector.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

class SpaceSectorLLRBT {
public:
    Sector *root;

    SpaceSectorLLRBT();

    ~SpaceSectorLLRBT();

    void readSectorsFromFile(const std::string &filename);

    void insertSectorByCoordinates(int x, int y, int z);

    void displaySectorsInOrder();

    void displaySectorsPreOrder();

    void displaySectorsPostOrder();

    std::vector<Sector *> getStellarPath(const std::string &sector_code);

    void printStellarPath(const std::vector<Sector *> &path);

    void displaySectorsInOrderRecursive(Sector *node);

    void displaySectorsPreOrderRecursive(Sector *node);

    void displaySectorsPostOrderRecursive(Sector *node);

    void insertLLRBTNode(Sector *&root, int x, int y, int z);


    Sector *findSector(const Sector *root, const std::string &sector_code) const;

    std::vector<Sector *> findPathToSector(const std::string &sector_code);

    Sector *insertLLRBTNodeRecursive(Sector *current, Sector *parent, int x, int y, int z);

    void deleteSectorsPostOrder(Sector *node);
};

#endif // SPACESECTORLLRBT_H
