
#include <iostream>
#include <vector>

int main()
{
    // variable
    std::vector<int> dummyData;

    // compile time constant
    std::size_t constexpr numberOfDummyData = 100;

    // we can avoid unnecesary re-allocations and copies if we already know the desired/maximum size.
    //dummyData.reserve(numberOfDummyData);

    for (std::size_t index = 0; index < numberOfDummyData; ++index)
    {
        // runtime constant
        int const dummyNumber = ((static_cast<int>(index) * 5512) << 1) % 2021;

        // inserting numbers at the back of the array.
        // std::vector reallocates memory if needed.
        dummyData.push_back(dummyNumber);
    }

    std::cout << "We have inserted " << dummyData.size() << " dummy numbers into the vector." << std::endl;

    for (std::size_t index = 0; index < dummyData.size(); index += 20)
    {
        std::cout << index << " => " << dummyData.at(index) << std::endl;
    }

    for (auto it = dummyData.begin(); it != dummyData.end(); ++it)
    {
        auto& element = *it;
        ++element;
    }

    std::cout << "---" << std::endl;

    for (std::size_t index = 0; index < dummyData.size(); index += 20)
    {
        std::cout << index << " => " << dummyData.at(index) << std::endl;
    }

    for (auto& element : dummyData)
    {
        ++element;
    }

    std::cout << "---" << std::endl;

    for (std::size_t index = 0; index < dummyData.size(); index += 20)
    {
        std::cout << index << " => " << dummyData.at(index) << std::endl;
    }
}
