#include "main.h"

int main()
{
    {
        CNibbles nibbles;

        while (!nibbles.GetQuit())
        {
            nibbles.Update();
        }
    }

    std::cout << std::endl << "Leak Counter: " << g_LeakCounter << std::endl;
    std::cout << "Press any Key to quit" << std::endl;
    
    while (true)
    {
        if (_kbhit())
        {
            break;
        }
    }

    return 0;
}