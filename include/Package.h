#pragma once

#include <iostream>

struct Package {
    int id;
    int arrivalTime;
    int destination;

    bool operator==(const Package& other) const {
        return id == other.id;
    }

    bool operator<(const Package& other) const {
        return id < other.id;
    }
};
