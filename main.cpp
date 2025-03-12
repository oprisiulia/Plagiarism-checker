#include <iostream>
#include <iomanip>
#include <stdexcept>

#include "plagiarism.h"

std::string rgb_color(int r, int g, int b) {
    return "\033[38;2;" + std::to_string(r) + ";" + std::to_string(g) + ";" + std::to_string(b) + "m";
}

std::string reset_color() {
    return "\033[0m";
}

void rate_to_color(float rate, int& r, int& g, int& b) {
    if (rate < 0.0f) rate = 0.0f;
    if (rate > 100.0f) rate = 100.0f;
    if (rate <= 50.0f) {
        r = static_cast<int>((rate / 50.0f) * 255);
        g = 255;
    }
    else {
        r = 255;
        g = static_cast<int>((1.0f - ((rate - 50.0f) / 50.0f)) * 255);
    }
    b = 0;
}

int main() {
    std::string file1, file2;
    std::cout << "Enter the name of file 1: ";
    std::getline(std::cin, file1);
    std::cout << "Enter the name of file 2: ";
    std::getline(std::cin, file2);

    try {
        Plagiarism checker(file1, file2, 5);
        float rate = checker.get_rate();
        int red, green, blue;
        rate_to_color(rate, red, green, blue);
        std::string color_code = rgb_color(red, green, blue);
        std::string reset = reset_color();

        std::cout << "------------------------------\n";
        std::cout << "Plagiarism Comparison Result\n";
        std::cout << "------------------------------\n";
        std::cout << "Plagiarism Rate: "
            << color_code << std::fixed << std::setprecision(2) << rate << "%" << reset << "\n";
        std::cout << "------------------------------\n";
    }
    catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << "\n";
        return 1;
    }

    return 0;
}
