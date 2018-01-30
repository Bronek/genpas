#include <iostream>
#include <fstream>
#include <string>
#include <set>
#include <cstdio>

#include <gmpxx.h>

int random_init(gmp_randstate_t& state)
{
    gmp_randinit_default(state);
    FILE* random = fopen("/dev/random", "r");
    if (random == nullptr) {
        puts("ERROR: unable to access /dev/random");
        return 3;
    }

    unsigned long int seed;
    if (fread(&seed, sizeof(seed), 1, random) != 1) {
        puts("ERROR: unable to read /dev/random");
        fclose(random);
        return 4;
    }
    gmp_randseed_ui(state, seed);
    fclose(random);

    return 0;
}

bool special_char(const char &a)
{
    return (a <= ' ' || (int)a >= 127 || (int)a < 0);
}

int main(int argc, char** argv)
{
    const bool help = (argc == 2
        && (std::strcmp(argv[1], "--help") == 0
            || std::strcmp(argv[1], "-h") == 0
            || std::strcmp(argv[1], "-?") == 0));
    const bool stdin = (argc >= 2
        && std::strcmp(argv[1], "-") == 0);

    if (help || argc < 2 || (argc > 2 && stdin)) {
        std::printf("%sne or more file names expected, or use '-' to read from input\n", (help ? "O" : "ERROR: o"));
        return (help ? 0 : 1);
    }

    std::set<std::string> words;
    size_t max = 0;
    for (int i = 1; i < argc; ++i) {
        // open file, or read from standard input
        std::ifstream file;
        std::istream* const in = (stdin ? &std::cin : &file);
        if (not stdin) {
            file.open(argv[i]);
            if (not file.is_open()) {
                std::printf("ERROR: unable to read input: %s\n", argv[i]);
                return 2;
            }
        }

        for (;;) {
            std::string tmp;
            if (!std::getline(*in, tmp))
                break;

            // erase special characters
            tmp.erase(std::remove_if(tmp.begin(), tmp.end(), &special_char), tmp.end());
            if (tmp.empty())
                continue;

            // capitalise first letter, if it's a letter
            if (tmp[0] >= 'a' && tmp[0] <= 'z')
                tmp[0] -= ('a' - 'A');

            max = std::max(max, tmp.size());
            words.insert(tmp);
        }
    }

    if (words.empty()) {
        std::printf("ERROR: empty wordlist\n");
        return 3;
    }

    gmp_randstate_t state;
    const auto r = random_init(state);
    if (r != 0)
        return r;

    std::string result;
    // ensure no re-allocations with partial result
    result.reserve(10 * max);
    for (int i = 0; i < 10; ++i) {
        auto it = words.begin();
        auto rnd = gmp_urandomm_ui(state, words.size());
        std::advance(it, rnd);
        result += *it;
    }

    std::puts(result.c_str());

    // overwrite result in memory before releasing it to OS
    for (int i = 0; i < (int)result.size(); ) {
        const unsigned char rnd = (unsigned char)gmp_urandomb_ui(state, 8);
        if ((result[i] ^= rnd) & (1 << 7))
            i++;
    }
}
