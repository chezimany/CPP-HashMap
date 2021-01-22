//
// Created by ciman on 9/9/2020.
//

#ifndef SUMMEREX6_HASHMAP_HPP
#define SUMMEREX6_HASHMAP_HPP

#include <iostream>
#include <list>
#include <vector>
#include <utility>
#include <algorithm>
#include <exception>
using std::list;
using std::vector;
using std::pair;
using std::exception;

/**
 * an associative container that contains key-value pairs with unique keys.
 * Search, insertion, and removal of elements have average constant-time complexity.
 * @tparam KeyT - key of each pair.
 * @tparam ValueT - value of each pair.
 */
template<typename KeyT, typename ValueT>
class HashMap
{
private:
    /**
     * threshold to determine when to increase the table currNumOfElements.
     */
    double upperThreshold{};

    /**
     * threshold to determine when to decrease the table currNumOfElements.
     */
    double lowerThreshold{};

    /**
     * maximum number of elements the hash _map can currently contain.
     */
    int maxNumOfElements{};

    /**
     * number of elements the has _map currently holds.
     */
    int currNumOfElements{};

    /**
     * an array of struct bucket to hold elements.
     */
    vector<pair<KeyT, ValueT>> *_map = nullptr;

    /**
     * compare lengthe of 2 vectors represented by iterators.
     * @tparam Iterator1 - type of 1st vector.
     * @tparam Iterator2 - type of 2st vector.
     * @param iterator1Begin - beginning of 1st vector.
     * @param iterator1End - ending of 1nd vector.
     * @param iterator2Begin - beginning of 2nd vector.
     * @param iterator2End - ending of 2nd vector.
     * @return - -1 if of different sizes, length if are the same.
     */
    template<typename Iterator1, typename Iterator2>
    int _areSizesEqual(Iterator1 iterator1Begin, Iterator1 iterator1End, Iterator2 iterator2Begin, Iterator2 iterator2End)
    {
        int lenOfFirst = 0;
        int lenOfSecond = 0;
        for (auto i = iterator1Begin; i != iterator1End ; ++i)
        {
            lenOfFirst++;
        }
        for (auto i = iterator2Begin; i != iterator2End ; ++i)
        {
            lenOfSecond++;
        }
        if (lenOfFirst != lenOfSecond)
        {
            return -1;
        }
        return lenOfFirst;
    }

    /**
     * get the hash code of a given value.
     * @param v - value to generate hash code on.
     * @return - hash code.
     */
    int _clamp(size_t v, size_t capacity) const
    {
        return v & (capacity - 1);
    }

    /**
     * increase the size of the _map.
     */
    void _increaseMapSize()
    {
        auto* toReplace = new vector<pair<KeyT, ValueT>>[capacity() * 2]();
        for (size_t i = 0; i < capacity(); i++)
        {
            vector<pair<KeyT, ValueT>>& list = _map[i];
            if (!list.empty())
            {
                for (pair<KeyT, ValueT> pair : list)
                {
                    int index = _clamp(std::hash<KeyT>{}(pair.first), capacity() * 2);
                    toReplace[index].push_back(pair);
                }
            }
        }
        maxNumOfElements *= 2;
        delete [] _map;
        _map = nullptr;
        _map = toReplace;
    }

    /**
     * decrease the size of the _map.
     */
    void _decreaseMapSize()
    {
        size_t updatedCap = capacity() / 2;
        auto* toReplace = new vector<pair<KeyT, ValueT>>[updatedCap]();
        for (int i = 0; i < capacity(); ++i)
        {
            vector<pair<KeyT, ValueT>>& list = _map[i];
            if (!list.empty())
            {
                for (pair<KeyT, ValueT> pair : list)
                {
                    int index = _clamp(std::hash<KeyT>{}(pair.first), capacity() / 2);
                    toReplace[index].push_back(pair);
                }
            }
        }
        this->maxNumOfElements = updatedCap;
        delete [] _map;
        _map = nullptr;
        _map = toReplace;
    }

public:

    /**
     * an iterator for the map.
     */
    class const_iterator: public std::iterator<std::forward_iterator_tag, pair<KeyT, ValueT>>
    {
    private:
        const HashMap<KeyT, ValueT>* _map;
        typename vector<pair<KeyT, ValueT>>::const_iterator _ptr;
        int _bucket;
        int _elements_counter;


    public:
        typedef const_iterator self_type;
        typedef pair<KeyT, ValueT> value_type;
        typedef pair<KeyT, ValueT>& reference;
        typedef typename vector<pair<KeyT, ValueT>>::const_iterator pointer;
        typedef std::forward_iterator_tag iterator_category;
        typedef int difference_type;


        /**
         * default constructor.
         */
        const_iterator():_map(nullptr), _ptr(nullptr), _bucket(0), _elements_counter(0)
        {

        }

        /**
         * a constructor to construct from an hash map.
         * @param hashMap - hash map to construct from.
         */
        explicit const_iterator(const HashMap<KeyT, ValueT>* hashMap, bool isEnd):_map(hashMap), _ptr(nullptr), _bucket(0), _elements_counter(0)
        {
            if (isEnd)
            {
                _ptr = _map->_map[_map->capacity() - 1].end();
                _elements_counter = _map->size() - 1;
            }
            else
            {
                _ptr = _map->_map[0].begin();
                for (size_t i = 0; i < hashMap->capacity(); ++i)
                {
                    if (!_map->_map[i].empty())
                    {
                        _ptr = _map->_map[i].begin();
                        _bucket = i;
                        break;
                    }
                }
            }
        }

        /**
         * copy constructor.
         * @param other - other iterator to copy from.
         */
        const_iterator(const const_iterator& other):const_iterator()
        {
            _map = other._map;
            _ptr = other._ptr;
            _bucket = other._bucket;
            _elements_counter = other._elements_counter;
        }

        /**
         * dereference operator.
         * @return - pair<KeyT, ValueT>.
         */
        value_type operator *()
        {
            return *_ptr;
        }

        /**
         * pointer operator.
         * @return - the iterator currently held.
         */
        const pair<KeyT, ValueT>* operator ->()
        {
            return _ptr.operator->();
        }

        const_iterator& operator =(const self_type& other)
        {
            this->_map = other._map;
            _ptr = other._ptr;
            _bucket = other._bucket;
            _elements_counter = other._elements_counter;
        }

        /**
         * forwarding operator.
         * @return - self.
         */
        self_type operator ++(int)
        {
            ++_ptr;
            if (_elements_counter == _map->size() - 1) /////TODO:check counter
            {
                _ptr = _map->_map[_map->capacity() - 1].end();
                return *this;
            }
            if (_ptr == _map->_map[_bucket].end() && _bucket < _map->capacity() - 1)
            {
                _bucket++;
                while (_map->_map[_bucket].empty() && _bucket < _map->capacity() - 1)
                {
                    _bucket++;
                }
                _ptr = _map->_map[_bucket].begin();
                _elements_counter++;
                return *this;
            }
            _elements_counter++;
            return *this;
        }

        /**
         * forwarding operator.
         * @return - self.
         */
        self_type operator ++()
        {
            self_type toReturn = *this;
            ++_ptr;
            if (_elements_counter == _map->size() - 1) /////TODO:check counter
            {
                _ptr = _map->_map[_map->capacity() - 1].end();
                return toReturn;
            }
            if (_ptr == _map->_map[_bucket].end() && _bucket < _map->capacity() - 1)
            {
                _bucket++;
                while (_map->_map[_bucket].empty() && _bucket < _map->capacity() - 1)
                {
                    _bucket++;
                }
                _ptr = _map->_map[_bucket].begin();
                _elements_counter++;
                return toReturn;
            }
            _elements_counter++;
            return toReturn;
        }

        /**
         * compare to other iterator.
         * @param other - other iterator for comparison.
         * @return - true if equal, false else.
         */
        bool operator ==(const self_type& other)
        {
            return _ptr == other._ptr && _map == other._map;
        }

        /**
         * compare to other iterator.
         * @param other - other iterator for comparison.
         * @return - true if different, false else.
         */
        bool operator !=(const self_type& other)
        {
            return !(this->operator==(other));
        }

    };

    typedef const_iterator iterator;

    /**
     * a default constructor.
     */
    HashMap(): upperThreshold(0.75), lowerThreshold(0.25), maxNumOfElements(16), currNumOfElements(0)
    {
        _map = new vector<pair<KeyT, ValueT>>[16]();
    }

    /**
     * a copy constructor.
     * @param other - other hashmap to copy from.
     */
    HashMap(const HashMap<KeyT, ValueT>& other):HashMap()
    {
        maxNumOfElements = other.maxNumOfElements;
        currNumOfElements = other.currNumOfElements;
        delete [] _map;
        _map = nullptr;
        _map = new vector<pair<KeyT, ValueT>>[capacity()]();
        for (size_t i = 0; i < other.capacity(); ++i)
        {
            vector<pair<KeyT, ValueT>> toCopy = other._map[i];
            for (pair<KeyT, ValueT> pair: toCopy)
            {
                _map[i].push_back(pair);
            }
        }
    }

    /**
     * gets 2 iterators for keys and values and stores them in the _map/
     * @tparam KeysInputIterator - iterator to keys.
     * @tparam ValuesInputIterator - iterator to values.
     * @param keysBegin - begining of keys vector.
     * @param keysEnd - end of keys vector.
     * @param valuesBegin - beginning of values vector.
     * @param valuesEnd - end of values vector.
     */
    template<typename KeysInputIterator, typename ValuesInputIterator> ///// TODO:continue func.
    HashMap(const KeysInputIterator keysBegin, const KeysInputIterator keysEnd, const ValuesInputIterator valuesBegin, const ValuesInputIterator valuesEnd): HashMap()
    {
        int sizesEqual = _areSizesEqual(keysBegin, keysEnd, valuesBegin, valuesEnd);
        if (sizesEqual < 0)
        {
            delete [] _map;
            _map = nullptr;
            throw std::length_error("given vectors are of different size.");
        }
        auto key = keysBegin;
        auto value = valuesBegin;
        while (key != keysEnd && value != valuesEnd)
        {
            KeyT keyT = *key;
            if (contains_key(keyT))
            {
                this->operator[](keyT) = *value;
            }
            else
            {
                insert(keyT, *value);
            }
            key++;
            value++;
        }

    }

    /**
     * delete sand frees the _map.
     */
    ~HashMap()
    {
        delete [] _map;
        _map = nullptr;
    }

    /**
     * get the number of elements the _map currently contains.
     * @return - the number of elements the _map currently contains.
     */
    size_t size() const
    {
        return currNumOfElements;
    }

    /**
     * get the capacity of the _map.
     * @return - the capacity.
     */
    size_t capacity() const
    {
        return maxNumOfElements;
    }

    /**
     * check if the _map is empty.
     * @return - true or false.
     */
    bool empty() const
    {
        return size() == 0;
    }

    /**
     * Inserts element into the container, if the container doesn't already contain an element with an equivalent key.
     * @param key - key to insert.
     * @param value - value to insert.
     * @return - a bool denoting whether the insertion took place.
     */
    bool insert(const KeyT& key, const ValueT& value)
    {
        if (contains_key(key))
        {
            return false;
        }
        pair<KeyT, ValueT> pair(key, value);
        int index = _clamp(std::hash<KeyT>{}(pair.first), capacity());
        _map[index].push_back(pair);
        currNumOfElements++;
        if (load_factor() > this->upperThreshold)
        {
            _increaseMapSize();
        }
        return true;
    }

    /**
     * checks if the container contains element with specific key
     * @param key - key value of the element to search for.
     * @return - true if there is such an element, otherwise false.
     */
    bool contains_key(const KeyT& key) const
    {
        int index = _clamp(std::hash<KeyT>{}(key), capacity());
        vector<pair<KeyT, ValueT>>& bucket = _map[index];
        for (pair<KeyT, ValueT> pair: bucket)
        {
            if (pair.first == key)
            {
                return true;
            }
        }
        return false;
    }

    /**
     * Returns a reference to the mapped value of the element with key equivalent to key. If no such element exists,
     * an exception of type std::out_of_range is thrown.
     * @param key - the key of the element to find.
     * @return - Reference to the mapped value of the requested element.
     */
    ValueT& at(const KeyT& key)
    {
        if (!contains_key(key))
        {
            throw std::out_of_range("Hash _map does not contain given key.");
        }
        int index = _clamp(std::hash<KeyT>{}(key), capacity());
        vector<pair<KeyT, ValueT>>& bucket = _map[index];
        auto it = bucket.begin();
        while (it->first != key)
        {
            it++;
        }
        return it->second;
    }

    /**
     * Returns a const reference to the mapped value of the element with key equivalent to key. If no such element exists,
     * an exception of type std::out_of_range is thrown.
     * @param key - the key of the element to find.
     * @return - Reference to the mapped value of the requested element.
     */
    const ValueT& at(const KeyT& key) const
    {
        if (!contains_key(key))
        {
            throw std::out_of_range("Hash _map does not contain the given key.");
        }
        int index = _clamp(std::hash<KeyT>{}(key), capacity());
        vector<pair<KeyT, ValueT>>& bucket = _map[index];
        auto it = bucket.begin();
        while (it->first != key)
        {
            it ++;
        }
        return it->second;
    }

    /**
     * Removes the element (if one exists) with the key equivalent to key.
     * @param key - key value of the elements to remove
     * @return - true if removed successfully, false otherwise.
     */
    bool erase(const KeyT& key)
    {
        int index = _clamp(std::hash<KeyT>{}(key), capacity());
        vector<pair<KeyT, ValueT>>& bucket = _map[index];
        if (!contains_key(key))
        {
            return false;
        }
        auto it = bucket.begin();
        while (it->first != key)
        {
            it ++;
        }
        bucket.erase(it);
        this->currNumOfElements--;
        if (capacity() > 1 && load_factor() < this->lowerThreshold)
        {
            _decreaseMapSize();
        }
        return true;
    }

    /**
     * Returns the average number of elements per bucket.
     * @return - Average number of elements per bucket.
     */
    double load_factor() const
    {
        return (double) size() / (double) capacity();
    }

    /**
     * Returns the number of elements in the bucket containing the given key.
     * @param key - the key which the bucket containg it we want.
     * @return - The number of elements in the bucket.
     */
    size_t bucket_size(const KeyT& key) const
    {

        if (!contains_key(key))
        {
            throw std::out_of_range("Hash map does not contain the given key.");
        }
        int index = _clamp(std::hash<KeyT>{}(key), capacity());
        vector<pair<KeyT, ValueT>>& bucket = _map[index];
        return bucket.size();
    }

    /**
     * get the index of a bucket that contains a given key.
     * @param key - a key to find the bucket.
     * @return - size of the bucket containing the key.
     */
    size_t bucket_index(const KeyT& key) const
    {
        if (!contains_key(key))
        {
            throw std::out_of_range("Hash _map does not contain the given key.");
        }
        int index = _clamp(std::hash<KeyT>{}(key), capacity());
        return index;
    }

    HashMap<KeyT, ValueT>& operator =(const HashMap<KeyT, ValueT> & other)
    {
        if (this == &other)
        {
            return *this;
        }
        this->maxNumOfElements = other.capacity();
        this->currNumOfElements = other.size();
        delete [] this->_map;
        this->_map = nullptr;
        this->_map = new vector<pair<KeyT, ValueT>>[other.capacity()]();
        for (size_t i = 0; i < other.capacity(); ++i)
        {
            vector<pair<KeyT, ValueT>> toCopy = other._map[i];
            for (pair<KeyT, ValueT> pair: toCopy)
            {
                _map[i].push_back(pair);
            }
        }
        return *this;
    }

    /**
     * Erases all elements from the container. After this call, size() returns zero.
     */
    void clear()
    {
        this->currNumOfElements = 0;
        for (size_t i = 0; i < capacity(); ++i)
        {
            _map[i].clear();
        }
    }

    /**
     * Returns a reference to the value that is mapped to a key equivalent to key.
     * @param key - the key of the element to find.
     * @return - reference to the mapped value of the existing element whose key is equivalent to key.
     */
    ValueT& operator [](const KeyT& key)
    {
        if (!contains_key(key))
        {
            ValueT value;
            insert(key, value);
        }
        int index = bucket_index(key);
        vector<pair<KeyT, ValueT>>& bucket = _map[index];
        auto it = bucket.begin();
        int j = 0;
        while (it->first != key && it != bucket.end())
        {
            it ++;
            j++;
        }
        return std::get<1>(bucket[j]);
    }

    /**
     * Returns a const reference to the value that is mapped to a key equivalent to key.
     * @param key - the key of the element to find.
     * @return - the mapped value of the existing element whose key is equivalent to key.
     */
    ValueT operator[](const KeyT& key) const
    {
        if (!contains_key(key))
        {
            return ValueT();
        }
        int index = bucket_index(key);
        vector<pair<KeyT, ValueT>>& bucket = _map[index];
        auto it = bucket.begin();
        int j = 0;
        while (it->first != key && it != bucket.end())
        {
            it++;
            j++;
        }
        return std::get<1>(bucket[j]);
    }

    /**
     * return a const iterator to the beginning of the hashmap.
     */
    iterator begin() const
    {
        return const_iterator(this, false);
    }

    /**
     *
     * @return - a const iterator to beginning of the hashmap.
     */
    const_iterator cbegin() const
    {
        return const_iterator(this, false);
    }

    /**
     * return a const iterator to the end of the map.
     */
    iterator end() const
    {
        return const_iterator(this, true);
    };

    /**
     * return a const iterator to the end of the map.
     */
    const_iterator cend() const
    {
        return const_iterator(this, true);
    }

    template<typename Key, typename Value>
    /**
     * Compares the contents of two unordered containers.
     * @param lhs - unordered container to compare.
     * @param rhs - unordered container to compare.
     * @return - true if the contents of the containers are equal, false otherwise.
     */
    friend bool operator ==(const HashMap<Key, Value>& lhs, const HashMap<Key, Value>& rhs);

    template<typename Key, typename Value>
    /**
     * Compares the contents of two unordered containers.
     * @param lhs - unordered container to compare.
     * @param rhs - unordered container to compare.
     * @return - false if the contents of the containers are equal, true otherwise.
     */
    friend bool operator !=(const HashMap<Key, Value>& lhs, const HashMap<Key, Value>& rhs);


};

template<typename Key, typename Value>
bool operator==(const HashMap<Key, Value> &lhs, const HashMap<Key, Value> &rhs)
{
    if (lhs.size() != rhs.size() || lhs.capacity() != rhs.capacity())
    {
        return false;
    }
    for (size_t i = 0; i < lhs.capacity(); ++i)
    {
        if (!std::is_permutation(lhs._map[i].begin(), lhs._map[i].end(), rhs._map[i].begin(), rhs._map[i].end()))
        {
            return false;
        }
    }
    return true;
}

template<typename Key, typename Value>
bool operator!=(const HashMap<Key, Value> &lhs, const HashMap<Key, Value> &rhs)
{
    return !(lhs == rhs);
}


#endif //SUMMEREX6_HASHMAP_HPP
