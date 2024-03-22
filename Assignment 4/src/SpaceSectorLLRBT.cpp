#include <algorithm>
#include "SpaceSectorLLRBT.h"
#include <unordered_set>
#include <set>

using namespace std;

SpaceSectorLLRBT::SpaceSectorLLRBT() : root(nullptr) {}

void SpaceSectorLLRBT::readSectorsFromFile(const std::string &filename) {
    ifstream infile(filename);
    string line;
    while (getline(infile, line)) {
        istringstream iss(line);
        int x, y, z;
        char comma; // To handle the commas in the input file format
        if (iss >> x >> comma >> y >> comma >> z) {
            insertSectorByCoordinates(x, y, z);
        }
    }
    infile.close();
}

SpaceSectorLLRBT::~SpaceSectorLLRBT() {
    deleteSectorsPostOrder(root);
}

void SpaceSectorLLRBT::deleteSectorsPostOrder(Sector *node) {
    if (node != nullptr) {
        deleteSectorsPostOrder(node->left);
        deleteSectorsPostOrder(node->right);
        delete node;
    }
}

std::string colorToString(bool isRed) {
    return (isRed) ? "RED" : "BLACK";
}

void rotate(Sector *&node, bool isLeft) {
    Sector *temp = (isLeft) ? node->right : node->left;

    if (isLeft) {
        node->right = temp->left;
        if (temp->left != nullptr) {
            temp->left->parent = node;
        }
        temp->left = node;
    } else {
        node->left = temp->right;
        if (temp->right != nullptr) {
            temp->right->parent = node;
        }
        temp->right = node;
    }

    temp->color = node->color;
    node->color = RED;
    temp->parent = node->parent;

    if (node->parent != nullptr) {
        if (node == node->parent->left) {
            node->parent->left = temp;
        } else {
            node->parent->right = temp;
        }
    }

    node->parent = temp;
    node = temp;
}

void leftRotate(Sector *&node) {
    rotate(node, true);
}

void rightRotate(Sector *&node) {
    rotate(node, false);
}


void flipColors(Sector *node) {
        node->color = RED;
        node->left->color = BLACK;
        node->right->color = BLACK;
}

Sector *fixViolations(Sector *root) {
    if (root == nullptr) {
        return root;
    }
    if (root != nullptr) {
        // Enforce lef-leaning red nodes
        if ((root->left == nullptr || (root->left != nullptr && root->left->color == BLACK)) &&
            root->right != nullptr && root->right->color == RED) {
            leftRotate(root);
        }
        // Balance consecutive red links
        if (root->left != nullptr && root->left->color == RED) {
            if (root->left->left != nullptr && root->left->left->color == RED) {
                rightRotate(root);
                flipColors(root);
            }
        }
        // Two red child
        if (root->left != nullptr && root->left->color == RED && root->right != nullptr && root->right->color == RED) {
            flipColors(root);
        }
    }
    return root;
}

void SpaceSectorLLRBT::insertSectorByCoordinates(int x, int y, int z) {
    insertLLRBTNode(root, x, y, z);
    if (root != nullptr) {
        root->color = BLACK;
    }
}

Sector *SpaceSectorLLRBT::insertLLRBTNodeRecursive(Sector *current, Sector *parent, int x, int y, int z) {

    if (current == nullptr) {
        Sector *newNode = new Sector(x, y, z);
        newNode->color = RED;
        newNode->parent = parent;
        return newNode;
    }
    int xCurrent = current->x;
    int yCurrent = current->y;
    int zCurrent = current->z;

    if (x < current->x || (x == xCurrent && y < yCurrent) ||
        (x == xCurrent && y == yCurrent && z < zCurrent)) {
        current->left = insertLLRBTNodeRecursive(current->left, current, x, y, z);
    } else {
        current->right = insertLLRBTNodeRecursive(current->right, current, x, y, z);
    }
    return fixViolations(current);
}

void SpaceSectorLLRBT::insertLLRBTNode(Sector *&root, int x, int y, int z) {
    root = insertLLRBTNodeRecursive(root, nullptr, x, y, z);
    root->color = BLACK;
}

void SpaceSectorLLRBT::displaySectorsInOrder() {
    cout << "Space sectors inorder traversal:" << endl;
    displaySectorsInOrderRecursive(root);
}

void SpaceSectorLLRBT::displaySectorsInOrderRecursive(Sector *node) {
    if (node != nullptr) {
        displaySectorsInOrderRecursive(node->left);
        cout << colorToString(node->color) << " sector: " << node->sector_code << endl;
        displaySectorsInOrderRecursive(node->right);
    }
}

void SpaceSectorLLRBT::displaySectorsPreOrder() {
    cout << "Space sectors preorder traversal:" << endl;
    displaySectorsPreOrderRecursive(root);
}

void SpaceSectorLLRBT::displaySectorsPreOrderRecursive(Sector *node) {
    if (node != nullptr) {
        cout << colorToString(node->color) << " sector: " << node->sector_code << endl;
        displaySectorsPreOrderRecursive(node->left);
        displaySectorsPreOrderRecursive(node->right);
    }
}

void SpaceSectorLLRBT::displaySectorsPostOrder() {
    cout << "Space sectors postorder traversal:" << endl;
    displaySectorsPostOrderRecursive(root);
}

void SpaceSectorLLRBT::displaySectorsPostOrderRecursive(Sector *node) {
    if (node != nullptr) {
        displaySectorsPostOrderRecursive(node->left);
        displaySectorsPostOrderRecursive(node->right);
        cout << colorToString(node->color) << " sector: " << node->sector_code << endl;
    }
}

std::vector<Sector *> mergeVectors(const std::vector<Sector *> &vec1, vector<Sector *> vec2, Sector *destination) {
    std::vector<Sector *> result;
    for (const auto &sector1: vec1) {
        result.push_back(sector1);
        auto it = std::find(vec2.begin(), vec2.end(),
                            sector1);

        if (it != vec2.end()) {
            size_t index = std::distance(vec2.begin(), it);
            for (size_t i = index + 1; i < vec2.size(); ++i) {
                result.push_back(vec2[i]);
            }
            vec2.clear();
            auto ite = std::find(result.begin(), result.end(),
                                 destination);
            if (ite != vec2.end()) {
                return result;
            }
        }
    }
    return result;
}

std::vector<Sector *> SpaceSectorLLRBT::getStellarPath(const std::string &sector_code) {
    Sector *x = findSector(root, sector_code);
    if (x == nullptr) {
        return {};
    }
    std::vector<Sector *> path = findPathToSector(sector_code);
    return path;
}

Sector *SpaceSectorLLRBT::findSector(const Sector *root, const std::string &sector_code) const {
    if (root == nullptr) {
        return nullptr;
    }
    Sector *found = nullptr;
    if ((found = findSector(root->left, sector_code)) != nullptr) {
        return found;
    }
    if (root->sector_code == sector_code) {
        return const_cast<Sector *>(root); // Found
    }
    if ((found = findSector(root->right, sector_code)) != nullptr) {
        return found;
    }
    return nullptr;
}

std::vector<Sector *> SpaceSectorLLRBT::findPathToSector(const std::string &sector_code) {
    std::vector<Sector *> visitedEarthToRoot;
    std::vector<Sector *> visitedDestinationToRoot;
    std::vector<Sector *> path;
    Sector *destinationSector = findSector(root, sector_code);
    Sector *earth = findSector(root, "0SSS");
    while (earth != nullptr) {
        visitedEarthToRoot.push_back(earth);
        earth = earth->parent;
    }
    while (destinationSector != nullptr) {
        visitedDestinationToRoot.push_back(destinationSector);
        destinationSector = destinationSector->parent;
    }
    std::reverse(visitedDestinationToRoot.begin(), visitedDestinationToRoot.end());
    path = mergeVectors(visitedEarthToRoot, visitedDestinationToRoot, destinationSector);
    return path;
}

void SpaceSectorLLRBT::printStellarPath(const std::vector<Sector *> &path) {
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