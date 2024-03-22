#include <algorithm>
#include "SpaceSectorBST.h"

using namespace std;

SpaceSectorBST::SpaceSectorBST() : root(nullptr) {}

SpaceSectorBST::~SpaceSectorBST() {
    deleteSectorsPostOrder(root);
}

void SpaceSectorBST::deleteSectorsPostOrder(Sector *node) {
    if (node != nullptr) {
        // Traverse the left subtree
        deleteSectorsPostOrder(node->left);

        // Traverse the right subtree
        deleteSectorsPostOrder(node->right);

        // Delete the current node
        delete node;
    }
}

void SpaceSectorBST::readSectorsFromFile(const std::string &filename) {
    std::ifstream infile(filename);
    std::string line;

    while (std::getline(infile, line)) {
        std::istringstream iss(line);

        int x, y, z;
        char comma; // To handle the commas in the input file format

        if (iss >> x >> comma >> y >> comma >> z) {
            insertSectorByCoordinates(x, y, z);
        }
    }

    infile.close();
}

void SpaceSectorBST::insertSectorByCoordinates(int x, int y, int z) {
    root = insertSectorRecursive(root, x, y, z);
}

Sector *SpaceSectorBST::insertSectorRecursive(Sector *node, int x, int y, int z) {
    if (node == nullptr) {
        // Create a new node and return it
        return new Sector(x, y, z);
    }

    // Compare coordinates for insertion
    if (x < node->x || (x == node->x && y < node->y) || (x == node->x && y == node->y && z < node->z)) {
        // Insert to the left
        node->left = insertSectorRecursive(node->left, x, y, z);
        node->left->parent = node;
    } else {
        // Insert to the right
        node->right = insertSectorRecursive(node->right, x, y, z);
        node->right->parent = node;
    }

    return node;
}

Sector *SpaceSectorBST::findSector(const Sector *root, const std::string &sector_code) const {
    if (root == nullptr) {
        return nullptr; // Not found
    }

    Sector *found = nullptr;

    // In-order traversal
    if ((found = findSector(root->left, sector_code)) != nullptr) {
        return found;
    }

    if (root->sector_code == sector_code) {
        return const_cast<Sector *>(root); // Found
    }

    if ((found = findSector(root->right, sector_code)) != nullptr) {
        return found;
    }

    return nullptr; // Not found
}

void SpaceSectorBST::deleteSector(const std::string &sector_code) {
    Sector *nodeToDelete = findSector(root, sector_code);

    // If the sector is found, proceed with deletion
    if (nodeToDelete != nullptr) {
        root = deleteSectorRecursive(root, nodeToDelete);
    }
}

Sector *SpaceSectorBST::deleteSectorRecursive(Sector *current, Sector *nodeToDelete) {
    if (current == nullptr) {
        return nullptr; // Sector not found
    }

    // Extract coordinates for comparison
    int xToDelete = nodeToDelete->x;
    int yToDelete = nodeToDelete->y;
    int zToDelete = nodeToDelete->z;

    int xCurrent = current->x;
    int yCurrent = current->y;
    int zCurrent = current->z;

    // Compare coordinates for deletion
    if (xToDelete < xCurrent || (xToDelete == xCurrent && yToDelete < yCurrent) ||
        (xToDelete == xCurrent && yToDelete == yCurrent && zToDelete < zCurrent)) {
        // Delete in the left subtree
        current->left = deleteSectorRecursive(current->left, nodeToDelete);
    } else if (xToDelete > xCurrent || (xToDelete == xCurrent && yToDelete > yCurrent) ||
               (xToDelete == xCurrent && yToDelete == yCurrent && zToDelete > zCurrent)) {
        // Delete in the right subtree
        current->right = deleteSectorRecursive(current->right, nodeToDelete);
    } else {
        // Node to delete found, apply deletion logic
        current = deleteNodeAndHandleCases(current, nodeToDelete);
    }

    return current;
}

Sector *SpaceSectorBST::deleteNodeAndHandleCases(Sector *current, Sector *nodeToDelete) {
    if (current->left == nullptr && current->right == nullptr) {
        // Case 1: Node with no children
        handleCase1(current, nodeToDelete);
    } else if ((current->left == nullptr && current->right != nullptr) ||
               (current->right == nullptr && current->left != nullptr)) {
        // Case 2: Node with one child
        handleCase2(current, nodeToDelete);
    } else {
        // Case 3: Node with two children
        handleCase3(current, nodeToDelete);
    }
    return current;
}


void SpaceSectorBST::handleCase1(Sector *&current, Sector *nodeToDelete) {
    // Case 1: Node with no children
    if (current->parent != nullptr) {
        // Update parent's child pointer
        if (current == current->parent->left) {
            current->parent->left = nullptr;
        } else {
            current->parent->right = nullptr;
        }
    }
    delete nodeToDelete;
    current = nullptr;
}

void SpaceSectorBST::handleCase2(Sector *&current, Sector *nodeToDelete) {
    // Case 2: Node with one child
    Sector *child = (current->left != nullptr) ? current->left : current->right;
    if (current->parent != nullptr) {
        // Update parent's child pointer
        if (current == current->parent->left) {
            current->parent->left = child;
        } else {
            current->parent->right = child;
        }
    }
    child->parent = current->parent;
    delete nodeToDelete;
    current = child;
}

void SpaceSectorBST::handleCase3(Sector *&current, Sector *nodeToDelete) {
    // Case 3: Node with two children
    Sector *successor = findMin(nodeToDelete->right);
    // Copy successor's data to the current node
    current->x = successor->x;
    current->y = successor->y;
    current->z = successor->z;
    current->sector_code = successor->sector_code;
    current->distance_from_earth = successor->distance_from_earth;
    // Recursively delete the successor
    current->right = deleteSectorRecursive(current->right, successor);
}

Sector *SpaceSectorBST::findMin(Sector *node) const {
    // Helper function to find the node with the minimum sector code in the subtree
    while (node->left != nullptr) {
        node = node->left;
    }
    return node;
}

std::vector<Sector *> SpaceSectorBST::getStellarPath(const std::string &sector_code) {
    std::vector<Sector *> path;
    Sector *destination = findSector(root, sector_code);

    // Traverse up the tree from the destination to the root to obtain the path
    while (destination != nullptr) {
        path.push_back(destination);
        destination = destination->parent;
    }

    // Reverse the path to start from the root
    std::reverse(path.begin(), path.end());
    return path;
}

void SpaceSectorBST::printStellarPath(const std::vector<Sector *> &path) {
    // TODO: Print the stellar path obtained from the getStellarPath() function 
    // to STDOUT in the given format.
    if (path.empty()) {
        std::cout << "A path to Dr. Elara could not be found." << std::endl;
        return;
    }

    std::cout << "The stellar path to Dr. Elara: ";
    for (size_t i = 0; i < path.size(); ++i) {
        std::cout << path[i]->sector_code;
        if (i < path.size() - 1) {
            std::cout << "->";
        }
    }
    std::cout << std::endl;
}

void SpaceSectorBST::displaySectorsInOrder() {
    std::cout << "Space sectors inorder traversal:" << std::endl;
    displaySectorsInOrderRecursive(root);
}

void SpaceSectorBST::displaySectorsInOrderRecursive(Sector *node) {
    if (node != nullptr) {
        // Traverse the left subtree
        displaySectorsInOrderRecursive(node->left);

        // Print sector information
        std::cout << node->sector_code << std::endl;

        // Traverse the right subtree
        displaySectorsInOrderRecursive(node->right);
    }
}

void SpaceSectorBST::displaySectorsPreOrder() {
    std::cout << "Space sectors preorder traversal:" << std::endl;
    displaySectorsPreOrderRecursive(root);
}

void SpaceSectorBST::displaySectorsPreOrderRecursive(Sector *node) {
    if (node != nullptr) {
        // Print sector information
        std::cout << node->sector_code << std::endl;

        // Traverse the left subtree
        displaySectorsPreOrderRecursive(node->left);

        // Traverse the right subtree
        displaySectorsPreOrderRecursive(node->right);
    }
}

void SpaceSectorBST::displaySectorsPostOrder() {
    std::cout << "Space sectors postorder traversal:" << std::endl;
    displaySectorsPostOrderRecursive(root);
}

void SpaceSectorBST::displaySectorsPostOrderRecursive(Sector *node) {
    if (node != nullptr) {
        // Traverse the left subtree
        displaySectorsPostOrderRecursive(node->left);

        // Traverse the right subtree
        displaySectorsPostOrderRecursive(node->right);

        // Print sector information
        std::cout << node->sector_code << std::endl;
    }
}