#include <iostream>
#include <vector>
#include <string>
#include <assert.h>
#include "vector.hpp"

int main()
{

    // Test with int
    asd::vector<int> myVec1;
    std::vector<int> stdVec1;

    for (int i = 0; i < 100; ++i)
    {
        myVec1.push_back(i);
        stdVec1.push_back(i);

        assert(myVec1.size() == stdVec1.size());
        assert(myVec1.capacity() >= myVec1.size());
        for (std::size_t j = 0; j < myVec1.size(); ++j)
        {
            assert(myVec1[j] == stdVec1[j]);
        }
    }

    // Test with std::string (non-trivial type)
    asd::vector<std::string> myVec2;
    std::vector<std::string> stdVec2;

    for (int i = 0; i < 100; ++i)
    {
        myVec2.push_back("test" + std::to_string(i));
        stdVec2.push_back("test" + std::to_string(i));

        assert(myVec2.size() == stdVec2.size());
        assert(myVec2.capacity() >= myVec2.size());
        for (std::size_t j = 0; j < myVec2.size(); ++j)
        {
            assert(myVec2[j] == stdVec2[j]);
        }
    }

    // Test copy constructor
    asd::vector<std::string> myVec3(myVec2);
    std::vector<std::string> stdVec3(stdVec2);
    assert(myVec3.size() == stdVec3.size());
    for (std::size_t i = 0; i < myVec3.size(); ++i)
    {
        assert(myVec3[i] == stdVec3[i]);
    }

    // Test move constructor
    asd::vector<std::string> myVec4(std::move(myVec2));
    std::vector<std::string> stdVec4(std::move(stdVec2));
    assert(myVec4.size() == stdVec4.size());
    for (std::size_t i = 0; i < myVec4.size(); ++i)
    {
        assert(myVec4[i] == stdVec4[i]);
    }
    assert(myVec2.size() == 0);
    assert(stdVec2.size() == 0);

    myVec2.emplace_back("asd");
    assert(myVec2[0] == "asd");

    std::cout << "examples done" << std::endl;

    return 0;
}