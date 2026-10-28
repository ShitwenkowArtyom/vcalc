#include <exception>
#include <iostream>

#include "UserInterface.h"

int main(int argc, const char** argv) {
    try {
        UserInterface interface;

        interface.run(argc, argv);

        return 0;
    } catch (const std::exception& e) {
        std::cerr << "error: " << e.what() << std::endl;

        return 1;
    } catch (...) {
        std::cerr << "Exception of unknown type!" << std::endl;

        return 2;
    }
}