#include "AVLTree.h"

#include <functional>
#include <string>

AVLTree::AVLTree() : root(nullptr) {} // Default constructor

// Destructor
AVLTree::~AVLTree() {
    searchAndDestroy(root);
    delete root;
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
    return 0;
}

bool AVLTree::AVLNode::isLeaf() const {
    return false;
}

size_t AVLTree::AVLNode::getHeight() const {
    return 0;
}

bool AVLTree::removeNode(AVLNode*& current){
    if (!current) {
        return false; // Nothing to delete
    }

    AVLNode* toDelete = current;

    auto nChildren = current->numChildren();

    // CASE ONE: NO CHILD
    if (current->isLeaf()) {
        // case 1 we can delete the node
        current = nullptr; // Parent pointer points to nullptr now
        delete toDelete;
        return true;

        // CASE 2: ONE CHILD
    }

    if (current->numChildren() == 1) {
        // case 2 - replace current with its only child

        AVLNode* child;

        // Pick the only child
        if (current->left != nullptr) {
            current = current->left;
        } else {
            current = current->right;
        }

        current = child; // Replace node with its child
        delete toDelete; // Delete original node
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
        size_t leftHeight, rightHeight = 0;
        if (current->left != nullptr) {
            leftHeight = current->left->getHeight();
        }

        if (current->right != nullptr) {
            rightHeight = current->right->getHeight();
        }

        size_t max = (leftHeight > rightHeight) ? leftHeight : rightHeight;
        current->height = max + 1;

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
        size_t leftHeight, rightHeight = 0;
        if (current->left != nullptr) {
            leftHeight = current->left->getHeight();
        }

        if (current->right != nullptr) {
            rightHeight = current->right->getHeight();
        }

        size_t max = (leftHeight > rightHeight) ? leftHeight : rightHeight;
        current->height = max + 1;

        balanceNode(current);
        return true;
    }
    else {
        // Found the node (assuming key == current->key)
        return removeNode(current);
    }

}

// public remove
bool AVLTree::remove(const KeyType& key) {
    return remove(root, key);
}

void AVLTree::balanceNode(AVLNode *&node) {
    // algorithm from notes
}

// node parameter is always the root node
bool AVLTree::insert(const KeyType& key, const ValueType& value) {
    return insertNode(root, key, value);
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

    // Update height on way back up
    current->height = 1 + std::max(
        current->left ? current->left->getHeight() : 0,
        current->right ? current->right->getHeight() : 0
        );

    // Rebalance
    balanceNode(current);
    return true; // After successful balance AND insertion

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
