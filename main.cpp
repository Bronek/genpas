#include <iostream>
#include <fstream>
#include <string>
#include <cstdio>

#include <gmpxx.h>

int random_init(gmp_randstate_t& state)
{
    gmp_randinit_default(state);
    FILE* random = fopen("/dev/random", "r");
    if (random == NULL)
    {
        puts("ERROR: unable to access /dev/random");
        return 3;
    }
    unsigned long int seed;
    if (fread(&seed, sizeof(seed), 1, random) != 1)
    {
        puts("ERROR: unable to read /dev/random");
        return 4;
    }
    gmp_randseed_ui(state, seed);
    fclose(random);

    return 0;
}

bool special_char(const char &a)
{
    return a <= ' ' || (int)a >= 127 || (int)a < 0;
}

int main(int argc, char** argv)
{
    if (argc != 2)
    {
        puts("ERROR: file name expected");
        return 1;
    }

    std::ifstream in(argv[1]);
    if (!in.is_open())
    {
        printf("ERROR: unable to open: %s\n", argv[1]);
        return 2;
    }

    gmp_randstate_t state;
    const auto r = random_init(state);
    if (r != 0)
        return r;

    std::vector<std::string> words;
    for (;;)
    {
        std::string tmp;
        if (!std::getline(in, tmp))
            break;

        // erase special characters
        tmp.erase(std::remove_if(tmp.begin(), tmp.end(), &special_char), tmp.end());
        if (tmp.empty())
            continue;

        // capitalise first letter, if it's a letter
        if (tmp[0] >= 'a' && tmp[0] <= 'z')
            tmp[0] -= ('a' - 'A');
        words.push_back(tmp);
    }

    if (words.empty())
    {
        printf("ERROR: empty wordlist: %s\n", argv[1]);
        return 3;
    }

    for (int i = 0; i < 10; ++i)
    {
        const auto l = gmp_urandomm_ui(state, words.size());
        printf("%s", words[l].c_str());
    }
    printf("\n");
}
