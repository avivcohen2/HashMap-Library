#include <iostream>
#include <string>
#include <fstream>
#include <filesystem>
#include <sstream>
#include <cassert>
#include <vector>
#include <map>
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

HashMap<char, int> *readEncoding(const char *filePath)
{
    /* Open an input stream to the file */
    std::ifstream fp(filePath);
    if (!fp)
    {
        return nullptr;
    }

    /* Allocate a new hash-map */
    auto hashMap = new HashMap<char, int>();

    /* Read line-by-line */
    std::string line;
    while (std::getline(fp, line))
    {
        /* Read the source and destination */
        std::istringstream iss(line);
        char src;
        int dst;
        iss >> src >> dst;
        if (!hashMap->insert(src, dst))
        {
            delete hashMap; // Remember to free memory
        }
    }

    return hashMap;
}

static const std::vector<int> _keys = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13};
static const std::vector<int> _values = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13};

void helper(std::vector<int> &keys, std::vector<char> &values)
{
    HashMap<int, char> m(keys.begin(), keys.end(), values.begin(), values.end());
}

int getRandomNumber(int max)
{
    return std::rand() % max;
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
int main()
{
    std::cout << "====================== default constructor ======================" << std::endl;
    const HashMap<int, int> map;
    assert(map.size() == 0);
    assert(map.capacity() == 16);
    assert(map.empty());
    assert(map.load_factor() == 0);
    try
    {
        map.at(5);
    }
    catch (std::exception &e)
    {
        std::cout << e.what() << std::endl;
    }
    std::cout << "====================== pass default constructor ======================" <<
              std::endl;

    std::cout << "====================== HashMap constructor1 ======================"
              << std::endl;
    {
        std::vector<int> wrongKeys{1, 2, 3};
        std::vector<char> wrongValues1{'a', 'b', 'c', 'd'};
        //check if the values > keys, then error expected
        try
        {
            helper(wrongKeys, wrongValues1);
        }
        catch (std::exception &e)
        {
            std::cout << e.what() << std::endl;
        }

        std::vector<char> wrongValues2{'a', 'b'};
        //check if the values < keys, then error expected
        try
        {
            helper(wrongKeys, wrongValues2);
        }
        catch (std::exception &e)
        {
            std::cout << e.what() << std::endl;
        }
        std::vector<int> keys1 = {1, 2, 3, 4, 5, 6, 7, 8, 9};
        std::vector<char> values1 = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i'};
        try
        {
            const HashMap<int, char> map(keys1.begin(), keys1.end(), values1.begin(),
                                         values1.end());
            assert(map.size() == 9);
            assert(map.capacity() == 16);
            assert(!map.empty());
            assert(map.load_factor() == 9 / 16.0);
            for (int i = 1; i < 9; i++)
            {
                assert(map.contains_key(i));
                auto val = map.at(i);
                assert(val == i - 1 + 'a');
            }
            assert(!map.contains_key(13));
        }
        catch (...)
        {
            //should not arrive here
            assert(false);
        }
        try
        {
            const HashMap<int, int> map(_keys.begin(), _keys.end(), _values.begin(), _values.end());
            assert(map.size() == 13);
            //should be bigger because of load factor
            assert(map.capacity() == 32);
            assert(!map.empty());
            assert(map.load_factor() == 13 / 32.0);
            map.printMap();
            for (int i = 1; i < 14; i++)
            {
                assert(map.contains_key(i));
                assert(map.at(i) == i);
            }
            assert(!map.contains_key(14));
        }
        catch (...)
        {
            //should not arrive here
            assert(false);
        }
    }

    std::cout << "====================== pass HashMap constructor1 ======================"
              << std::endl;
    std::cout << "====================== copy constructor ======================" << std::endl;
    try
    {
        HashMap<int, int> map(_keys.begin(), _keys.end(), _values.begin(), _values.end());
        HashMap<int, int> map1(map);
        assert(map1.size() == 13);
        //should be bigger because of load factor
        assert(map1.capacity() == 32);
        assert(!map1.empty());
        assert(map1.load_factor() == 13 / 32.0);

        assert(map1.insert(14, 14));
        //check that the map before didnt change
        assert(map.size() == 13);
        assert(map.capacity() == 32);
        assert(!map.empty());
        assert(map.load_factor() == 13 / 32.0);
        //check map1 changed
        assert(map1.size() == 14);
        //should be bigger because of load factor
        assert(map1.capacity() == 32);
        assert(!map1.empty());
        assert(map1.load_factor() == 14 / 32.0);
        assert(map.erase(1));
        //check that the map1 before didnt change
        assert(map.size() == 12);
        assert(map1.size() == 14);
    }
    catch (...)
    {
        //should not arrive here
        assert(false);
    }
    std::cout << "====================== pass copy constructor ======================" << std::endl;
    std::cout << "====================== bucket size ======================" << std::endl;

    try
    {
        HashMap<int, int> map(_keys.begin(), _keys.end(), _values.begin(), _values.end());
        for (int i = 1; i < 14; i++)
        {
            assert(map.bucket_size(i) == 1);
        }
        try
        {
            map.bucket_size(-1);
        }
        catch (std::exception &e)
        {
            std::cout << e.what() << std::endl;
        }
        std::vector<int> keys = _keys, values = _values;
        keys.push_back(33);
        values.push_back(19);
        HashMap<int, int> map1(keys.begin(), keys.end(), values.begin(), values.end());
        assert(map1.bucket_size(1) == 2);
        for (int i = 2; i < 14; i++)
        {
            assert(map1.bucket_size(i) == 1);
        }
        try
        {
            map.bucket_size(-1);
        }
        catch (std::exception &e)
        {
            std::cout << e.what() << std::endl;
        }
    }
    catch (...)
    {
        //should not arrive here
        assert(false);
    }
    std::cout << "====================== pass bucket size ======================" << std::endl;
    std::cout << "====================== bucket index ======================" << std::endl;

    try
    {
        const HashMap<int, int> map(_keys.begin(), _keys.end(), _values.begin(), _values.end());
        for (int i = 1; i < 14; i++)
        {
            assert(map.bucket_index(i) == i);
        }
        try
        {
            map.bucket_index(-1);
        }
        catch (std::exception &e)
        {
            std::cout << e.what() << std::endl;
        }
        std::vector<int> keys = _keys, values = _values;
        keys.push_back(33);
        values.push_back(19);
        HashMap<int, int> map1(keys.begin(), keys.end(), values.begin(), values.end());
        for (int i = 1; i < 14; i++)
        {
            assert(map1.bucket_index(i) == i);
        }
        assert(map1.bucket_index(33) == 1);
        try
        {
            map.bucket_index(-1);
        }
        catch (std::exception &e)
        {
            std::cout << e.what() << std::endl;
        }
    }
    catch (...)
    {
        //should not arrive here
        assert(false);
    }

    std::cout << "====================== pass bucket index ======================" << std::endl;
    std::cout << "====================== contains ======================" << std::endl;
    try
    {
        const HashMap<int, int> map(_keys.begin(), _keys.end(), _values.begin(), _values.end());
        for (int i = 0; i < 13; i++)
        {
            assert(map.contains_key(_keys[i]));
        }
        try
        {
            map.at(-1);
        }
        catch (std::exception &e)
        {
            std::cout << e.what() << std::endl;
        }
    }
    catch (...)
    {
        //should not arrive here
        assert(false);
    }
    std::cout << "====================== pass contains ======================" << std::endl;
    std::cout << "====================== at ======================" << std::endl;
    try
    {
        HashMap<int, int> map(_keys.begin(), _keys.end(), _values.begin(), _values.end());
        for (int i = 0; i < 13; i++)
        {
            map.at(_keys[i]);
            assert(map.at(_keys[i]) == _values[i]);
        }
        map.at(1) = 4;
        assert(map.at(1) == 4);
        try
        {
            map.at(-1);
        }
        catch (std::exception &e)
        {
            std::cout << e.what() << std::endl;
        }

        const HashMap<int, int> map1(_keys.begin(), _keys.end(), _values.begin(), _values.end());
        for (int i = 0; i < 13; i++)
        {
            map1.at(_keys[i]);
            assert(map1.at(_keys[i]) == _values[i]);
        }
        try
        {
            map1.at(-1);
        }
        catch (std::exception &e)
        {
            std::cout << e.what() << std::endl;
        }
    }
    catch (...)
    {
        //should not arrive here
        assert(false);
    }
    std::cout << "====================== pass at ======================" << std::endl;
    std::cout << "====================== clear ======================" << std::endl;

    try
    {
        HashMap<int, int> map(_keys.begin(), _keys.end(), _values.begin(), _values.end());
        map.clear();
        assert(map.size() == 0);
        assert(map.empty());
        assert(map.capacity() == 32);
        assert(map.load_factor() == 0);
        for (int i = 1; i < 14; i++)
        {
            assert(!map.contains_key(i));
        }
        HashMap<int, int> map1;
        //both maps don't have keys so they are equal
        assert(map.capacity() != map1.capacity());
    }
    catch (...)
    {
        //should not arrive here
        assert(false);
    }
    std::cout << "====================== pass clear ======================" << std::endl;
    std::cout << "====================== insert ======================" << std::endl;

    std::vector<int> keys = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13};
    std::vector<int> values = {13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1};
    int size = 0;
    try
    {
        HashMap<int, int> map;
        for (int i = 0; i < 12; i++)
        {
            size++;
            assert(map.insert(keys[i], values[i]));
            assert(map.size() == size);
            assert(map.at(keys[i]) == values[i]);
        }
        assert(map.insert(keys[12], values[12]));
        assert(!map.insert(keys[12], 1));
        assert(map.size() == 13);
        assert(map.capacity() == 32);
        assert(map.at(keys[12]) == values[12]);

        map.clear();
        int val;
        size = 0;
        for (int i = 0; i < 200; i++)
        {
            size++;
            val = getRandomNumber(500);
            assert(map.insert(i, val));
            assert(map.size() == size);
            assert(map.load_factor() == (double) map.size() / map.capacity());
            assert(map.at(i) == val);
        }
        assert(map.capacity() == 512);
        assert(!map.empty());
    } catch (...)
    {
        //should not arrive here
        assert(false);
    }

    std::cout << "====================== pass insert ======================" << std::endl;
    std::cout << "====================== erase ======================" << std::endl;

    //std::vector<int> keys = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13};
    //std::vector<int> values = {13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1};
    try
    {
        int size = 13;
        HashMap<int, int> map(keys.begin(), keys.end(), values.begin(), values.end());
        for (int i = 1; i < 6; i++)
        {
            assert(map.erase(i));
            assert(map.capacity() == 32);
            size--;
            assert(map.size() == size);
            assert(map.load_factor() == size / 32.0);
        }
        assert(map.erase(6));
        assert(!map.erase(-1));
        assert(map.capacity() == 16);
        assert(map.size() == 7);
        assert(map.load_factor() == 7 / 16.0);
        for (int i = 1; i < 7; i++)
        {
            assert(!map.contains_key(i));
            try
            {
                map.at(i);
            }
            catch (std::exception &e)
            {
                std::cout << e.what() << std::endl;
            }
            for (int i = 7; i < 13; i++)
            {
                assert(map.contains_key(i));
                try
                {
                    map.at(i);
                }
                catch (std::exception &e)
                {
                    std::cout << e.what() << std::endl;
                }
            }
        }
    }
    catch (...)
    {
        //should not arrive here
        assert(false);
    }

    std::cout << "====================== pass erase ======================" << std::endl;
    std::cout << "====================== operator ==, != ======================" << std::endl;

    try
    {
        HashMap<int, int> map(_keys.begin(), _keys.end(), _values.begin(), _values.end());
        HashMap<int, int> map1(_keys.begin(), _keys.end(), _values.begin(), _values.end());
        HashMap<int, int> map2;
        assert(map == map1);
        assert(map == map);
        assert(!(map != map1));
        assert(!(map != map));
        assert(map != map2);
        assert(map2 == map2);
        assert(map.insert(-2, -2));
        assert(map != map1);
        assert(!(map == map1));
    }
    catch (...)
    {
        //should not arrive here
        assert(false);
    }

    std::cout << "====================== pass operator ==, != ======================" << std::endl;
    std::cout << "====================== operator = ======================" << std::endl;

    try
    {
        const HashMap<int, int> map(_keys.begin(), _keys.end(), _values.begin(), _values.end());
        HashMap<int, int> map1;
        map1 = map;
        map.printMap();
        std::cout << " ******************** " << std::endl;
        map1.printMap();
        assert(map == map1);
        assert(map.size() == map1.size());
        assert(map.capacity() == map1.capacity());
        assert(map1.insert(14, 14));
        assert(map1.contains_key(14));
        assert(!map.contains_key(14));
    }
    catch (...)
    {
        //should not arrive here
        assert(false);
    }

    std::cout << "====================== pass operator = ======================" << std::endl;
    std::cout << "====================== operator= check self ======================" << std::endl;

    try
    {
        HashMap<int, int> map(_keys.begin(), _keys.end(), _values.begin(), _values.end());
        map = map;
        assert(map.insert(15, 15));
        assert(map == map);
        assert(map.size() == _keys.size() + 1);
        assert(map.capacity() == 32);
    }
    catch (...)
    {
        //should not arrive here
        assert(false);
    }
    std::cout << "====================== pass operator= check self ======================" <<
              std::endl;
    std::cout << "====================== operator[] ======================" << std::endl;
    try
    {
        HashMap<int, int> map;
        for (int i = 0; i < 15; i++)
        {
            map[i] = 2 * i;
            assert(map.contains_key(i));
            assert(map.at(i) == 2 * i);
            assert(map.size() == i + 1);
        }
        assert(map.capacity() == 32);
        map[1] = 0;
        assert(map.contains_key(1));
        assert(map.at(1) == 0);
        assert(map.size() == 15);
    }
    catch (...)
    {
        //should not arrive here
        assert(false);
    }
    std::cout << "====================== pass operator[] ======================" << std::endl;
    std::cout << "====================== iterator ======================" << std::endl;
    try
    {
        HashMap<int, int> map;
        for (unsigned int i = 0; i < 15; i++)
        {
            map[i] = 5 * i;
        }
        int counter = 0;
        auto it = map.cbegin();
        //check values in iterator
        map.printMap();
        for (; it != map.cend(); it++)
        {
            counter++;
            assert((*it).first * 5 == (*it).second);
        }
        //check the amount of values
        assert(counter == 15);
        //check that can run twice
        for (; it != map.cend(); ++it)
        {
            counter++;
            assert((*it).first * 5 == (*it).second);
        }
    }
    catch (...)
    {
        //should not arrive here
        assert(false);
    }
    std::cout << "====================== pass iterator ======================" << std::endl;
    std::cout << "====================== random insert and deletes ======================"
              << std::endl;
    try
    {
        HashMap<int, int> map;
        std::map<int, int> realMap;
        int num1, num2;
        for (unsigned int i = 0; i < 200; i++)
        {
            num1 = getRandomNumber(1000);
            num2 = getRandomNumber(500);
            map[num1] = num2;
            realMap[num1] = num2;
        }
        assert(map.size() == realMap.size());
        //check at & [], iterator
        for (auto it = map.cbegin(); it != map.cend(); ++it)
        {
            assert(realMap.count((*it).first) == 1);
            assert(realMap.at((*it).first) == (*it).second);
        }

        for (auto it : realMap)
        {
            assert(map.contains_key(it.first));
            assert(map.at(it.first) == it.second);
        }

        //check erase
        for (unsigned int i = 0; i < 200; i++)
        {
            num1 = getRandomNumber(1000);
            if (realMap.erase(num1))
            {
                assert(map.erase(num1));
            }
        }
        assert(map.size() == realMap.size());
        map.clear();
        realMap.clear();
        assert(map.size() == realMap.size());
        //check insert this time
        for (unsigned int i = 0; i < 200; i++)
        {
            num1 = getRandomNumber(1000);
            num2 = getRandomNumber(500);
            if (!map.contains_key(num1))
            {
                assert(map.insert(num1, num2));
                realMap[num1] = num2;
            }
        }
        assert(map.size() == realMap.size());
        //check at & [], iterator
        for (auto it = map.cbegin(); it != map.cend(); ++it)
        {
            assert(realMap.count((*it).first) == 1);
            assert(realMap.at((*it).first) == (*it).second);
        }

        for (auto it : realMap)
        {
            assert(map.contains_key(it.first));
            assert(map.at(it.first) == it.second);
        }
    }
    catch (...)
    {
        //should not arrive here
        assert(false);
    }

    std::cout << "====================== pass random insert and deletes ======================"
            << std::endl;

    return EXIT_SUCCESS;
}
