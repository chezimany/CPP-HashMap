#include <iostream>
#include <string>
#include <fstream>
#include <filesystem>
#include <sstream>
#include  <cassert>
#include "HashMap.hpp"

/** \brief The number of arguments this program expects to get. */
#define PROG_NUM_ARGS 2

/** \brief The number of argument, in argv, that contains the encoding file path. */
#define FILE_PATH_ARG_LOC 1

/** \brief The usage message. */
#define USAGE_MESSAGE "Usage: Encoder <encoding file>"

/** \brief A message that being displayed when there's no input to encode. */
#define NO_INPUT_TO_ENCODE_MESSAGE "You must enter an input to encode."

/** \brief A message that being displayed if the user used a character that's not appears in the hasmap
 * and therefore can not be encoded. */
#define INVALID_CHARACTER_MESSAGE "There's no mapping to encode the character: "

/**
 * \brief A message shown if there's a problem with the encoding creation.
 * A problem can be raised either because of std::bad_alloc, file format issues, error within HashMap::insert etc.
 */
#define READ_ENCODING_ERROR_MESSAGE "Could not create the encoding mapping."

HashMap<char, char>* readEncoding(const char* filePath)
{
    /* Open an input stream to the file */
    std::ifstream fp(filePath);
    if (!fp)
    {
        return nullptr;
    }

    /* Allocate a new hash-_map */
    auto hashMap = new HashMap<char, char>();

    /* Read line-by-line */
    std::string line;
    while (std::getline(fp, line))
    {
        /* Read the source and destination */
        std::istringstream iss(line);
        char src;
        char dst;
        iss >> src >> dst;
        if (!hashMap->insert(src, dst)) {
            delete hashMap; // Remember to free memory
        }
    }

    return hashMap;
}

/**
 * @brief The main function that runs the program.
 * @param argc Non-negative value representing the number of arguments passed
 * to the program from the environment in which the program is run.
 * @param argv Pointer to the first element of an array of pointers to null-terminated
 * multibyte strings that represent the arguments passed to the program from the execution
 * environment. The value of argv[argc] is guaranteed to be a null pointer.
 * @return 0, to tell the system the execution ended without errors, or 1, to
 * tell the system that the code has execution errors.
 */
int main(int argc, const char* argv[])
{

    /* Usage message */
    if (argc != PROG_NUM_ARGS)
    {
        std::cerr << USAGE_MESSAGE << std::endl;
        return EXIT_FAILURE;
    }

    auto hashMap = readEncoding(argv[FILE_PATH_ARG_LOC]);
    if (hashMap == nullptr)
    {
        std::cerr << READ_ENCODING_ERROR_MESSAGE << std::endl;
        return EXIT_FAILURE;
    }

    /* Get some input from the user */
    std::string input;
    std::cout << "Enter some message to decode: ";
    std::cin >> input;
    if (input.empty())
    {
        std::cerr << NO_INPUT_TO_ENCODE_MESSAGE << std::endl;
        return EXIT_FAILURE;
    }

    /* Iterate and encode */
    std::stringstream ss;
    for (auto i = 0; i < input.length(); ++i)
    {
        if (!hashMap->contains_key(input[i]))
        {
            std::cerr << INVALID_CHARACTER_MESSAGE << input[i] << std::endl;
            delete hashMap;
            return EXIT_FAILURE;
        }

        /* Decode */
        ss << hashMap->at(input[i]);
    }

    /* Print the encoded message and finish */
    std::cout << "The encoded message is: " << ss.str() << std::endl;
    delete hashMap;

    return EXIT_SUCCESS;
}