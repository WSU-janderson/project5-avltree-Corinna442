#include "AVLTree.h"

#include <functional>
#include <string>

// Copy constructor
AVLTree::AVLTree(const AVLTree& other) {
    root = deepCopy(other.root);
}

// Private helper for copy constructor
AVLTree::AVLNode* AVLTree::deepCopy(AVLNode* node) {
// This recursively copies nodes
    if (node == nullptr) {
        return nullptr;
    }

    // Create node with the same parameters
    AVLNode* newNode = new AVLNode(node->key, node->value, node->height, nullptr, nullptr);

    // Copy left subtree
    newNode->left = deepCopy(node->left);

    // Copy right subtree
    newNode->right = deepCopy(node->right);

    return newNode;
}

// node parameter is always the root node
bool AVLTree::insert(const KeyType& key, const ValueType& value) {
    return insertNode(root, key, value);
}

// public remove
bool AVLTree::remove(const KeyType& key) {
    return remove(root, key);
}

bool AVLTree::insertNode(AVLNode*& current, const KeyType& key, const ValueType& value) {

    // Base case: recurse and find null spot.
    if (current == nullptr) {
        current = new AVLNode{key, value, 1, nullptr, nullptr};
        return true; // go back to parent now
    }

    // Check for duplicate key
    if (key == current->key) {
        return false;
    }

    bool success;

    if (key < current->key) {
        success = insertNode(current->left, key, value);
    } else {
        success = insertNode(current->right, key, value);
    }

    // If insertion failed because of a duplicate then stop
    if (!success) {
        return false;
    }

    size_t leftHeight = 0, rightHeight = 0;
    if (current->left != nullptr) {
        leftHeight = current->left->getHeight();
    }

    if (current->right != nullptr) {
        rightHeight = current->right->getHeight();
    }

    current->height = 1 + std::max(leftHeight, rightHeight);

    // Rebalance
    balanceNode(current);
    return true; // After successful balance AND insertion

}

bool AVLTree::removeNode(AVLNode*& current){
    if (!current) {
        return false; // Nothing to delete
    }

    //AVLNode* toDelete = current;

    auto nChildren = current->numChildren();

    // CASE ONE: NO CHILD
    if (current->isLeaf()) {
        // case 1 we can delete the node
        delete current;
        current = nullptr; // Parent pointer points to nullptr now
        return true;
    }

    // CASE 2: ONE CHILD
    if (current->numChildren() == 1) {
        // case 2 - replace current with its only child

        // pick one child
        AVLNode* child = nullptr;
        if (current->left != nullptr) {
            child = current->left;
        } else {
            child = current->right;
        }

        delete current; // Delete original node
        current = child; // Replace node with its child
        return true;
    }
    // CASE 3: TWO CHILDREN
        // case 3 - we have two children,
        // get smallest key in right subtree by
        // getting right child and go left until left is null
        AVLNode* smallestInRight = current->right;
        // I could check if smallestInRight is null,
        // but it shouldn't be since the node has two children
        while (smallestInRight->left != nullptr) {
            smallestInRight = smallestInRight->left;
        }

        // Copy successor pair into this current node
        std::string newKey = smallestInRight->key;
        int newValue = smallestInRight->value;
        current->key = newKey;
        current->value = newValue;

        // DON'T NEED THIS: my remove function will utilize balance and height
        // current->height = current->getHeight();
        // balanceNode(current);

    // Remove successor node from the right subtree
    bool removedNode = remove(current->right, smallestInRight->key);

    return removedNode;

    }

// private remove
bool AVLTree::remove(AVLNode*& current, KeyType key) {

    // Base case
    // Tree is empty or we reached a dead end
    if (!current) {
        return false;
    }

    // Search left subtree for the node to remove (key-matching)
    if (key < current->key) {
        bool found = remove(current->left, key);
        if (!found) {
            return false;
        }

        // ---POST NODE DELETION--- //
        // Node was removed->update height and rebalance
        size_t leftHeight = 0, rightHeight = 0;
        if (current->left != nullptr) {
            leftHeight = current->left->getHeight();
        }

        if (current->right != nullptr) {
            rightHeight = current->right->getHeight();
        }

        current->height = 1 + std::max(leftHeight, rightHeight);

        balanceNode(current);

        // Continue up the tree
        return true;
    }
    // Search right subtree
    else if (key > current->key) {
        bool found = remove(current->right, key);
        if (!found) {
            return false;
        }

        // Update height and rebalance
        size_t leftHeight = 0, rightHeight = 0;
        if (current->left != nullptr) {
            leftHeight = current->left->getHeight();
        }

        if (current->right != nullptr) {
            rightHeight = current->right->getHeight();
        }

        current->height = 1 + std::max(leftHeight, rightHeight);

        balanceNode(current);
        return true;
    }
    else {
        // Found the node (assuming key == current->key)
        return removeNode(current);
    }

}

AVLTree::AVLTree() : root(nullptr) {} // Default constructor

// Destructor
AVLTree::~AVLTree() {
    searchAndDestroy(root);
    root = nullptr;
}

// Destructor helper
void AVLTree::searchAndDestroy(AVLNode* node) {
    // Base case if there is no more nodes (except root)
    if (node == nullptr) {
        return;
    }

    searchAndDestroy(node->left);
    searchAndDestroy(node->right);
    delete node;
}

size_t AVLTree::AVLNode::numChildren() const {
    // Count left and right children (increment) and return the num
    size_t numChildren = 0;
    if (left) {
        numChildren++;
    }
    if (right) {
        numChildren++;
    }
    return numChildren;
}

bool AVLTree::AVLNode::isLeaf() const {
    return (left == nullptr && right == nullptr);
}

size_t AVLTree::AVLNode::getHeight() const {
    return height;
}

int AVLTree::getNodeHeight(AVLNode* node) const {
    return node ? node->getHeight() : 0;
}

size_t AVLTree::getTreeHeight() const {
    return root ? root->getHeight() : 0;
}



void AVLTree::balanceNode(AVLNode *&node) {

    // No using recursion**

    if (node == nullptr) {
        return;
    }

    int balance = getNodeHeight(node->left) - getNodeHeight(node->right);

    // CASE 1: LEFT HEAVY
    if (balance > 1) {
        int llHeight = getNodeHeight(node->left->left);
        int lrHeight = getNodeHeight(node->left->right);

        // Left-left subcase
        if (llHeight >= lrHeight) {
            // Rotate node to the right
            rotateToRight(node);
        }
        // left-right
        else {
            // Double rotation: node->left
            // Single rotation: node
            rotateToLeft(node->left);
            rotateToRight(node);
        }
        return;
    }

    // CASE 2: RIGHT HEAVY
    if (balance < -1) {

        int rrHeight = getNodeHeight(node->right->right);
        int rlHeight = getNodeHeight(node->right->left);
        // right-right subcase
        if (rrHeight >= rlHeight) {
            // Rotate node to the left
            rotateToLeft(node);
        }
        // right-left
        else {
            // Double rotation: node->right
            // Single rotation: node
            rotateToRight(node->right);
            rotateToLeft(node);
        }
        return;
    }
}

// Rotate right helper
void AVLTree::rotateToRight(AVLNode*& node) {

    // Store values
    AVLNode* hook = node->left; // new root ( B is left of A )
    AVLNode *hookRight = hook->right;;

    // Rotate
    hook->right = node; //hook becomes the root: right of B is now A
    node->left = hookRight; // Right node from hook rotates left of prev root node (left of A is D)

    node->height = 1 + std::max(getNodeHeight(node->left), getNodeHeight(node->right));
    hook->height = 1 + std::max(getNodeHeight(hook->left), getNodeHeight(hook->right));

    node = hook; // hook becomes new root

}

// Rotate left helper
void AVLTree::rotateToLeft(AVLNode*& node) {

    // Store values
    AVLNode* hook = node->right; // new root ( B is right of A )
    AVLNode* hookLeft = hook->left; // To the left of the hook (D)

    // Rotate
    hook->left = node; //hook becomes the root: left of B is now A
    node->right = hookLeft; // Left node from hook rotates right of prev root node (right of A is D)

    node->height = 1 + std::max(getNodeHeight(node->left), getNodeHeight(node->right));
    hook->height = 1 + std::max(getNodeHeight(hook->left), getNodeHeight(hook->right));

    node = hook; // hook becomes new root

}

ostream& operator<<(ostream& os, const AVLTree& avlTree) {
    function<void(AVLTree::AVLNode*)> print;
    print = [&](AVLTree::AVLNode* node) {
        if (node == nullptr) {
            return;
        }
        print(node->left);
        os << node->key;
        print(node->right);
    };

    print(avlTree.root);

    return os;
}
