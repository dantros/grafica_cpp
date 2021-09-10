
#include <string>
#include <iostream>
#include <thread>
#include <mutex>
#include <queue>
#include <chrono>

using NumberType = unsigned int;

std::mutex mutex;
std::queue<NumberType> primeNumbers;

bool isPrime(NumberType number)
{
    // extracted from https://en.wikipedia.org/wiki/Primality_test
    //Primality test using 6k+-1 optimization.
    if (number <= 3)
    {
        return number > 1;
    }

    if ((number % 2 == 0) or (number % 3 == 0))
    {
        return false;
    }

    NumberType i = 5;

    while ((i<<2) <= number)
    {
        if ((number % i == 0) or (number % (i + 2) == 0))
        {
            return false;
        }
        i += 6;
    }
    return true;
}

void computeTask()
{
    // searching for prime number
    NumberType number = 1;
    while (true)
    {
        if (isPrime(number))
        {
            std::lock_guard<std::mutex> guard(mutex);
            primeNumbers.push(number);
        }
        ++number;
    }
}

void renderTask()
{
    NumberType newPrimeNumber;
    while (true)
    {
        {
            std::lock_guard<std::mutex> guard(mutex);
            while (not primeNumbers.empty())
            {
                newPrimeNumber = primeNumbers.front();
                primeNumbers.pop();
                std::cout << "New prime number found: " << newPrimeNumber << ". " << primeNumbers.size() << " more in the queue to print." << std::endl;
            }
        }
        // we can give room to compute some numbers...
        std::this_thread::sleep_for(std::chrono::microseconds(1));
    }
}

int main()
{
    std::thread computeThread(computeTask);
    std::thread renderThread(renderTask);

    // waiting for the threads to finish...
    computeThread.join();
    renderThread.join();
}