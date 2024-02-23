#ifndef EX6_HASHMAP_HPP
#define EX6_HASHMAP_HPP

// ------------------------------ includes ------------------------------
#include <cstdlib>
#include <utility>
#include <list>
#include <stdexcept>
#include <iostream>

// -------------------------- const definitions -------------------------
#define DEFAULT_CAPACITY 16
#define MINIMAL_CAPACITY 1
#define LOW_LOAD_FACTOR 0.25
#define HIGH_LOAD_FACTOR 0.75

// -------------------------- namespaces definitions -------------------------
using std::list;
using std::pair;
template<typename KeyT, typename ValueT> using bucket = list<pair<KeyT, ValueT>>;

// ------------------------------ functions -----------------------------
/**
 * class of HashMap containing KeyT and ValueT
 */
template<typename KeyT, typename ValueT>
class HashMap
{
private:
    /**
     * capacity of HashMap
     */
    size_t _capacity;
    /**
     * size of HashMap
     */
    size_t _size;
    /**
    * pointer to lists of pairs containing the HashMap elements
    */
    bucket<KeyT, ValueT> *_hashTable;

    /**
     * @return true if we will pass the high load factor after adding an item to the hashMap
     */
    bool _upperLoadFactor() const noexcept
    {
        return load_factor() > HIGH_LOAD_FACTOR;
    }

    /**
     * @return true if we will pass the low load factor after removing an item to the hashMap
     */
    bool _lowerLoadFactor() const noexcept
    {
        return load_factor() < LOW_LOAD_FACTOR;
    }

    /**
     * creates a new hashTable and transfers all items from the old one, also deletes the old
     * hashTable
     * @param newCapacity - the capacity after resizing
     */
    void _rehash(size_t newCapacity) noexcept(false)
    {
        auto *newMap = new bucket<KeyT, ValueT>[newCapacity];
        for (size_t i = 0; i < capacity(); i++)
        {
            for (auto tuple: _hashTable[i])
            {
                size_t idx = std::hash<KeyT>{}(tuple.first) & (newCapacity - 1);
                newMap[idx].push_back(pair<KeyT, ValueT>{tuple.first, tuple.second});
            }
        }
        delete[] _hashTable;
        _hashTable = newMap;
        _capacity = newCapacity;
    }

    /**
     * @param key - the key we want to add
     * @return an index according to the wanted hash function
     */
    size_t _hash(const KeyT &key) const noexcept
    {
        return std::hash<KeyT>{}(key) & (capacity() - 1);
    }

    // -------------------------- exception classes -------------------------

    /**
     * exception thrown if a given key is not found in the HashMap
     */
    class KeyNotFound : public std::exception
    {
        virtual const char *what() const noexcept
        {
            return "key is not found";
        }
    };

    /**
     * exception thrown if given vectors do not have the same size
     */
    class VectorsLength : public std::exception
    {
        virtual const char *what() const noexcept
        {
            return "the vectors do not have same size";
        }
    };


public:
    /**
     * default constructor of HashMap
     */
    HashMap() : _capacity(DEFAULT_CAPACITY), _size(0)
    {
        _hashTable = new bucket<KeyT, ValueT>[DEFAULT_CAPACITY];
    }

    /**
     * a constructor that gets an iterator for keys and iterator for values and constructs a
     * HashMap with matching pairs
     * @tparam KeysInputIterator - type of keys iterator
     * @tparam ValuesInputIterator - type of values iterator
     * @param keysBegin - beginning of keys iterator
     * @param keysEnd - end of keys iterator
     * @param valuesBegin - beginning of values iterator
     * @param valuesEnd - end of values iterator
     */
    template<typename KeysInputIterator, typename ValuesInputIterator>
    HashMap(const KeysInputIterator keysBegin, const KeysInputIterator keysEnd,
            const ValuesInputIterator valuesBegin, const ValuesInputIterator valuesEnd) : HashMap()
    {
        auto it1 = keysBegin;
        auto it2 = valuesBegin;
        if (std::distance(keysBegin, keysEnd) - std::distance(valuesBegin, valuesEnd))
        {
            throw VectorsLength{};
        }
        while (it1 != keysEnd)
        {
            KeyT curKey = *it1;
            ValueT curVal = *it2;
            if (!insert(curKey, curVal)) // the HashMap already contains this key, we need to
                // override its value
            {
                at(curKey) = curVal;
            }
            it1++;
            it2++;
        }
    }

    /**
     * copy constructor
     * @param other - HashMap to copy from
     */
    HashMap(const HashMap &other) : HashMap()
    {
        *this = other;
    }

    /**
     * HashMap destructor
     */
    ~HashMap()
    {
        delete[] _hashTable;
    }

    /**
     * @return size of the hashMap
     */
    size_t size() const noexcept
    {
        return _size;
    }

    /**
     * @return current capacity of the hashMap
     */
    size_t capacity() const noexcept
    {
        return _capacity;
    }

    /**
     * @return true if hashMap is empty
     */
    bool empty() const noexcept
    {
        return _size == 0;
    }

    /**
     * the function gets a key and a value, and inserts them to the hashMap
     * @param key - the key
     * @param val - the value
     * @return - true if the insertion ended successfully
     */
    bool insert(const KeyT &key, const ValueT &val) noexcept
    {
        if (contains_key(key))
        {
            return false;
        }
        size_t index = _hash(key);
        _hashTable[index].push_back(pair<KeyT, ValueT>{key, val});
        _size++;
        if (_upperLoadFactor())
        {
            _rehash(capacity() * 2);
        }
        return true;
    }

    /**
     * the function checks if a certain key is already in the map
     * @param key - the key we are looking for
     * @return - true if it does
     */
    bool contains_key(const KeyT &key) const noexcept
    {
        int index = _hash(key);
        if (empty() || _hashTable[index].empty())
        {
            return false;
        }
        for (auto &tuple: _hashTable[index])
        {
            if (tuple.first == key)
            {
                return true;
            }
        }
        return false;
    }

    /**
     * const version of the function - the function gets a key and returns its value. in case the
     * key is not in the hashMap an exception is thrown.
     * @param key - the key
     * @return - key's value
     */
    const ValueT &at(const KeyT &key) const noexcept(false)
    {
        size_t index = _hash(key);
        for (auto &tuple: _hashTable[index])
        {
            if (tuple.first == key)
            {
                return tuple.second;
            }
        }
        throw KeyNotFound{};
    }

    /**
     * the function gets a key and returns its value. in case the key is not in the hashMap an
     * exception is thrown
     * @param key - the key
     * @return - key's value
     */
    ValueT &at(const KeyT &key) noexcept(false)
    {
        size_t index = _hash(key);
        for (auto &tuple: _hashTable[index])
        {
            if (tuple.first == key)
            {
                return tuple.second;
            }
        }
        throw KeyNotFound{};
    }

    /**
     * the function gets a key and erases its value
     * @param key - the key
     * @return - true if the erase was done successfully
     */
    bool erase(const KeyT &key) noexcept
    {
        if (!contains_key(key))
        {
            return false;
        }
        size_t index = _hash(key);
        for (auto tuple: _hashTable[index])
        {
            if (tuple.first == key)
            {
                _hashTable[index].remove(tuple);
                break;
            }
        }
        _size--;
        if (_lowerLoadFactor() && capacity() > MINIMAL_CAPACITY)
        {
            _rehash(capacity() / 2);
        }
        return true;
    }

    /**
     * @return current load factor
     */
    double load_factor() const noexcept
    {
        return (double) size() / capacity();
    }

    /**
     * the function gets a key and returns it's bucket size. the function throws an exception if
     * the key was not found
     * @param key - the key
     * @return - size of bucket
     */
    size_t bucket_size(const KeyT &key) const noexcept(false)
    {
        if (!contains_key(key))
        {
            throw KeyNotFound{};
        }
        size_t index = _hash(key);
        return _hashTable[index].size();
    }

    /**
     * the function gets a key and returns the bucket's index if the map contains the key, or
     * throws an exception if not
     * @param key - the key
     * @return - bucket index
     */
    size_t bucket_index(const KeyT &key) const noexcept(false)
    {
        if (!contains_key(key))
        {
            throw KeyNotFound{};
        }
        return _hash(key);
    }

    /**
     * the function clears the map from all elements
     */
    void clear() noexcept
    {
        for (size_t i = 0; i < _capacity; i++)
        {
            _hashTable[i].clear();
        }
        _size = 0;
    }

    /**
     * assignment operator
     * @param other - hashMap to copy elements from
     * @return reference to HashMap
     */
    HashMap &operator=(const HashMap &other) noexcept
    {
        if (this != &other)
        {
            this->clear();
            delete[] this->_hashTable;
            this->_capacity = other.capacity();
            this->_size = other.size();
            this->_hashTable = new bucket<KeyT, ValueT>[_capacity];
            for (size_t i = 0; i < other.capacity(); i++)
            {
                _hashTable[i] = other._hashTable[i];
            }
        }
        return *this;
    }

    /**
     * subscript operator
     * @param key
     * @return
     */
    ValueT &operator[](const KeyT &key) noexcept
    {
        int index = _hash(key);
        if (empty() || _hashTable[index].empty() || !contains_key(key))
        {
            insert(key, ValueT());
        }
        for (auto &tuple: _hashTable[index])
        {
            if (tuple.first == key)
            {
                return tuple.second;
            }
        }
        return at(key);
    }

    /**
     * subscript operator
     * @param key
     * @return
     */
    ValueT operator[](const KeyT &key) const noexcept
    {
        int index = _hash(key);
        if (empty() || _hashTable[index].empty() || !contains_key(key))
        {
            return ValueT();
        }
        for (auto &tuple: _hashTable[index])
        {
            if (tuple.first == key)
            {
                return tuple.second;
            }
        }
        return at(key);
    }

    /**
     * checks if two hashMaps are identical
     * @param other - another hashMap
     * @return true if they are
     */
    bool operator==(const HashMap &other) const noexcept
    {
        if (this->capacity() != other.capacity() || this->size() != other.size())
        {
            return false;
        }
        for (size_t i = 0; i < this->capacity(); i++)
        {
            for (const auto &tuple: _hashTable[i])
            {
                if (!other.contains_key(tuple.first) || other[tuple.first] != tuple.second)
                {
                    return false;
                }
            }
        }
        return true;
    }

    /**
     * checks if two hashMaps are not identical
     * @param other - another hashMap
     * @return true if they are different
     */
    bool operator!=(const HashMap &other) const noexcept
    {
        return !(*this == other);
    }


// -------------------------- iterator class -------------------------

    /**
     * class of a const iterator for HashMap
     */
    class ConstIterator
    {
        const HashMap *_map;
        size_t _curIndex;
        typename list<pair<KeyT, ValueT>>::const_iterator _cur;
        int _counter;

    public:
        /**
         * iterator traits:
         */
        typedef pair<KeyT, ValueT> value_type;
        typedef const pair<KeyT, ValueT> *pointer;
        typedef const pair<KeyT, ValueT> &reference;
        typedef int difference_type;
        typedef std::forward_iterator_tag iterator_category;


        /**
         * @return the data of the element pointed to by the iterator
         */
        value_type operator*() const
        {
            return *_cur;
        }

        /**
         * assignment operator for iterator - points to the same HashMap object, same bucket
         * index and same pair of KeyT, ValueT
         * @param other
         * @return
         */
        ConstIterator &operator=(const ConstIterator &other)
        {
            if (this != &other)
            {
                this->_map = other._map;
                this->_curIndex = other._curIndex;
                this->_cur = other._cur;
                this->_counter = other._counter;
            }
            return *this;
        }

        /**
         * prefix increment operator
         * @return
         */
        ConstIterator &operator++()
        {
            _cur++;
            _counter++;
            while (_cur == _map->_hashTable[_curIndex].end() && ++_curIndex < _map->capacity())
            {
                _cur = _map->_hashTable[_curIndex].begin();
            }
            return *this;
        }

        /**
         * postfix increment operator
         * @return
         */
        ConstIterator operator++(int)
        {
            ConstIterator tmp(*this);
            ++(*this);
            return tmp;
        }

        /**
         * equal operator
         * @param other - other iterator
         * @return true if iterators are the same
         */
        bool operator==(const ConstIterator &other) const
        {
            return this->_map == other._map && this->_counter == other._counter;
        }

        /**
         * not equal operator
         * @param other - other iterator
         * @return true if iterators are not the same
         */
        bool operator!=(const ConstIterator &other) const
        {
            return !(*this == other);
        }

        /**
         * @return pointer to the element pointed to by the iterator
         */
        pointer operator->() const
        {
            return &(*_cur);
        }

        ConstIterator() : _map(nullptr), _curIndex(0),
                          _cur(nullptr), _counter(0)
        {}

        /**
         * const iterator constructor
         * @param node
         */
        ConstIterator(const HashMap *hashMap, bool end) : _map(hashMap),
                                                          _curIndex(0),
                                                          _cur(nullptr), _counter(0)
        {
            if (end)
            {
                _curIndex = _map->capacity();
                _counter = _map->size();
            }
            else
            {
                _curIndex = 0;
                if (!_map->_hashTable[_curIndex].empty())
                {
                    _cur = _map->_hashTable[_curIndex].begin();
                }
                while (_map->_hashTable[_curIndex].empty() && ++_curIndex < _map->capacity())
                {
                    _cur = _map->_hashTable[_curIndex].begin();
                }
            }
        }

        /**
         * const iterator copy constructor
         * @param other
         */
        ConstIterator(const ConstIterator &other) : _map(other._map), _curIndex(other._curIndex),
                                                    _cur(other._cur), _counter(other._counter)
        {}
    };

    typedef ConstIterator const_iterator;
    typedef ConstIterator iterator;

    const_iterator begin() const
    {
        return ConstIterator(this, false);
    }

    const_iterator end() const
    {
        return ConstIterator(this, true);
    }

    const_iterator cbegin() const
    {
        return begin();
    }

    const_iterator cend() const
    {
        return end();
    }
};


#endif //EX6_HASHMAP_HPP
