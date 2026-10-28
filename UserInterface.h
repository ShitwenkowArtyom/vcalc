#pragma once

#include <string>

#include "Params.h"

class UserInterface {
public:
    UserInterface();

    void run(int argc, const char** argv);

    std::string getDescription() const;

private:
    Params params;

    bool parse(int argc, const char** argv);
};