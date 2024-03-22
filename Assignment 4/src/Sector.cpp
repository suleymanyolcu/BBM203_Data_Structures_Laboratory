#include <valarray>
#include "Sector.h"

// Constructor implementation

Sector::Sector(int x, int y, int z) : x(x), y(y), z(z), left(nullptr), right(nullptr), parent(nullptr), color(RED) {
    // TODO: Calculate the distance to the Earth, and generate the sector code
    distance_from_earth = sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2));
    generateSectorCode();
}

Sector::~Sector() {

}

Sector &Sector::operator=(const Sector &other) {
    if (this != &other) {
        // Copy member variables
        x = other.x;
        y = other.y;
        z = other.z;
        distance_from_earth = other.distance_from_earth;
        sector_code = other.sector_code;
        color = other.color;
    }
    return *this;
}

bool Sector::operator==(const Sector &other) const {
    return (x == other.x && y == other.y && z == other.z);
}

bool Sector::operator!=(const Sector &other) const {
    return !(*this == other);
}


void Sector::generateSectorCode() {
    int distance_component = static_cast<int>(distance_from_earth);
    std::string x_component = getCoordinateComponent(x, 'R', 'L', 'S');
    std::string y_component = getCoordinateComponent(y, 'U', 'D', 'S');
    std::string z_component = getCoordinateComponent(z, 'F', 'B', 'S');
    sector_code = std::to_string(distance_component) + x_component + y_component + z_component;
}

std::string Sector::getCoordinateComponent(int value, char positive_dir, char negative_dir, char zero_dir) const {
    if (value == 0) {
        return std::string(1, zero_dir);
    } else if (value > 0) {
        return std::string(1, positive_dir);
    } else {
        return std::string(1, negative_dir);
    }
}
