#ifndef KNN_DAT_PARSER_H
#define KNN_DAT_PARSER_H

#include "kNN_Data.h"

#include <fstream>
#include <sstream>
#include <string>

class kNN_Dat_Parser {
public:
    // Constructor
    kNN_Dat_Parser() {}

    // Function to parse the .dat file
    Dataset parse(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Unable to open file: " << filename << std::endl;
            return Dataset();
        }

        Dataset dataset;
        std::string line;

        bool isHeader = true;
        bool foundThreshold = false;
        while (getline(file, line)) {
            // Skip empty lines
            if (line.empty()) {
                continue;
            }

            // Process header lines
            if (line[0] == '#') {
                if (isHeader) {
                    parseHeader(line, dataset);
                }
                if (line.find("Threshold") != std::string::npos) {
                    foundThreshold = true;
                }
                continue;
            }

            // Process the threshold value after the header
            if (foundThreshold) {
                parseThreshold(line, dataset);
                break;
            }

            // Once we're past the header lines, start processing data lines
            isHeader = false;

            Point point = parseLine(line);
            dataset.points.push_back(point);
        }



        return dataset;
    }

private:
    // Function to parse a single line of data
    Point parseLine(const std::string& line) {
        std::stringstream ss(line);
        std::string token;
        std::vector<double> features;
        std::string label;

        while (getline(ss, token, ',')) {
            try {
                if (std::isdigit(token[0]) || token[0] == '-') {
                    features.push_back(stod(token));
                } else {
                    label = token;
                }
            } catch (const std::exception& e) {
                std::cerr << "Error parsing token: " << token << std::endl;
            }
        }

        return Point(features, label);
    }

    // Function to parse header information
    void parseHeader(const std::string& line, Dataset& dataset) {
        if (line.find("Features:") != std::string::npos) {
            std::string header = line.substr(line.find(":") + 1);
            std::stringstream ss(header);
            std::string feature;

            while (getline(ss, feature, ',')) {
                // Trim leading and trailing spaces
                feature.erase(feature.find_last_not_of(" \n\r\t") + 1);
                feature.erase(0, feature.find_first_not_of(" \n\r\t"));
                dataset.header.push_back(feature);
            }
        }
    }

    // Function to parse the threshold
    void parseThreshold(const std::string& line, Dataset& dataset) {
        std::string thresholdStr = line.substr(line.find_last_of(" ") + 1);
        try {
            dataset.threshold = stod(thresholdStr);
        } catch (const std::exception& e) {
            std::cerr << "Error parsing threshold: " << thresholdStr << std::endl;
        }
    }
};

#endif // KNN_DAT_PARSER_H
