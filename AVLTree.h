/**
 * AVLTree.h
 */

#ifndef AVLTREE_H
#define AVLTREE_H
#include <optional>
#include <string>
#include <vector>

using namespace std;

class AVLTree {
public:
    using KeyType = std::string;
    using ValueType = size_t;

    bool insert(const KeyType& key, const ValueType&); // insert method
    bool remove(const KeyType& key); // remove method
    bool contains(const KeyType& key) const;

    optional<ValueType> get(const KeyType& key) const;
    size_t& operator[](const KeyType& key);

    vector<size_t> findRange(const KeyType& lowKey, const KeyType& highKey) const;
    vector<string> keys() const;
    size_t size() const; // O(1)
    ValueType getHeight() const; // Height of entire tree

    AVLTree(); // Default constructor
    AVLTree(const AVLTree& other); // Copy constructor
    AVLTree& operator=(const AVLTree& other); // Assignment operator
    ~AVLTree(); // deconstructor

    size_t getTreeHeight() const;

    friend ostream& operator<<(ostream& os, const AVLTree& avlTree);


protected:
    class AVLNode {
    public:
        KeyType key;
        ValueType value;
        size_t height;

        AVLNode* left;
        AVLNode* right;

        // 0, 1 or 2
        size_t numChildren() const;
        // true or false
        bool isLeaf() const;
        // number of hops to deepest leaf node
        size_t getHeight() const;

        // Constructors:
        AVLNode() : key(), value(0), height(1), left(nullptr), right(nullptr) {}
        AVLNode(const KeyType& k, const ValueType& v) :
        key(k), value(v), height(1), left(nullptr), right(nullptr) {}

        AVLNode(const KeyType& k, const ValueType& v, size_t h, AVLNode* l, AVLNode* r) :
        key(k), value(v), height(1), left(l), right(r) {}

    };

private:
    AVLNode* root;
    size_t treeSize; // private member variable for O(1) size

    // helpers for insert and remove
    bool insertNode(AVLNode*& current, const KeyType& key, const ValueType& value);
    // this overloaded remove will do the recursion to remove the node
    bool remove(AVLNode*& current, KeyType key);
    // removeNode contains the logic for actually removing a node based on the numebr of children
    bool removeNode(AVLNode*& current);

    // balance and rotation helpers
    // You will implement this, but it is needed for removeNode()
    void balanceNode(AVLNode*& node);
    void rotateToRight(AVLNode*& node);
    void rotateToLeft(AVLNode*& node);

    // helper for heights
    int getNodeHeight(AVLNode* node) const;

    // node finding helpers
    bool containsNode(AVLNode* node, const KeyType& key) const;
    optional<ValueType> getNode(AVLNode* node, const KeyType& key) const;
    AVLNode* nodeOperator(AVLNode*& node, const KeyType& key);

    // range and keys helpers
    void findRangeHelper(AVLNode* node, const KeyType& lowKey, const KeyType& highKey, vector<size_t>& res) const;
    void getKeys(AVLNode* node, vector<KeyType>& vec) const;

    // copy and destroy helpers
    void searchAndDestroy(AVLNode* node); // (get it? Like Metallica >.<)  helper: finds node and deletes it
    AVLNode* deepCopy(AVLNode* node);
};

#endif //AVLTREE_H
