#include <iostream>
#include <fstream>
#include <string>
#include <cstdio>

#include <gmpxx.h>

static const mpz_class mpz0(0);
static const mpz_class mpz2(2);
static const mpz_class mpz3(3);
static const mpz_class mpz5(5);

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
    int r = random_init(state);
    if (r != 0)
        return r;

    // etc
}
