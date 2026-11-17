#ifndef BST_H
#define BST_H

// Binary Search Tree. Key = int (tracking ID), value = pointer.
// NOTE: the tree does NOT delete the records it points to.
template <typename V>
class BST {
private:
    struct TreeNode {
        int key;
        V* value;
        TreeNode* left;
        TreeNode* right;
        TreeNode(int k, V* v) {
            key = k;
            value = v;
            left = nullptr;
            right = nullptr;
        }
    };

    TreeNode* root;
    int count;
    int height;        // number of levels (a BST never loses nodes, so we just track the maximum)
    int lastSteps;     // how many nodes the last search visited

    void inorderRec(TreeNode* node, void (*visit)(int, V*)) const {
        if (node == nullptr) return;
        inorderRec(node->left, visit);
        visit(node->key, node->value);
        inorderRec(node->right, visit);
    }

    void preorderRec(TreeNode* node, void (*visit)(int, V*)) const {
        if (node == nullptr) return;
        visit(node->key, node->value);
        preorderRec(node->left, visit);
        preorderRec(node->right, visit);
    }

    void postorderRec(TreeNode* node, void (*visit)(int, V*)) const {
        if (node == nullptr) return;
        postorderRec(node->left, visit);
        postorderRec(node->right, visit);
        visit(node->key, node->value);
    }

    // Delete every tree node WITHOUT recursion. A plain BST can become a very
    // long chain, and recursing down a long chain could overflow the stack.
    // Idea: if a node has a left child, rotate that child above it;
    // otherwise delete the node and move to its right child.
    void destroyAll() {
        TreeNode* node = root;
        while (node != nullptr) {
            if (node->left != nullptr) {
                TreeNode* l = node->left;
                node->left = l->right;
                l->right = node;
                node = l;
            }
            else {
                TreeNode* r = node->right;
                delete node;
                node = r;
            }
        }
        root = nullptr;
    }

public:
    BST() {
        root = nullptr;
        count = 0;
        height = 0;
        lastSteps = 0;
    }

    ~BST() {
        destroyAll();
    }

    BST(const BST&) = delete;
    BST& operator=(const BST&) = delete;

    int size() const { return count; }
    int getHeight() const { return height; }
    int getLastSteps() const { return lastSteps; }

    // Returns false if the key already exists
    bool insert(int key, V* value) {
        TreeNode* parent = nullptr;
        TreeNode* cur = root;
        int depth = 0;

        while (cur != nullptr) {          // walk down, left or right
            parent = cur;
            depth++;
            if (key < cur->key) cur = cur->left;
            else if (key > cur->key) cur = cur->right;
            else return false;            // duplicate
        }

        TreeNode* node = new TreeNode(key, value);
        if (parent == nullptr) root = node;
        else if (key < parent->key) parent->left = node;
        else parent->right = node;

        count++;
        if (depth + 1 > height) height = depth + 1;
        return true;
    }

    // Returns nullptr if not found
    V* search(int key) {
        lastSteps = 0;
        TreeNode* cur = root;
        while (cur != nullptr) {
            lastSteps++;
            if (key == cur->key) return cur->value;
            if (key < cur->key) cur = cur->left;
            else cur = cur->right;
        }
        return nullptr;
    }

    // Traversals: 'visit' is called for every node
    void inorder(void (*visit)(int, V*)) const { inorderRec(root, visit); }
    void preorder(void (*visit)(int, V*)) const { preorderRec(root, visit); }
    void postorder(void (*visit)(int, V*)) const { postorderRec(root, visit); }
};

#endif