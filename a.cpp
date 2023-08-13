#include <iostream>
#include <vector>
#include <map>
#include <memory>
#include <algorithm>

using namespace std;

class FPNode {
public:
    int item;
    int count;
    shared_ptr<FPNode> parent;
    vector<shared_ptr<FPNode>> children;

    FPNode(int _item, int _count, shared_ptr<FPNode> _parent = nullptr)
        : item(_item), count(_count), parent(_parent) {}
};

class FPTree {
public:
    shared_ptr<FPNode> root;
    map<int, vector<shared_ptr<FPNode>>> headerTable;

    FPTree() : root(make_shared<FPNode>(-1, 0)) {}

    void insert(const vector<int>& transaction, int count = 1) {
        shared_ptr<FPNode> currentNode = root;

        for (int item : transaction) {
            shared_ptr<FPNode> childNode = currentNode->children.empty() ? nullptr : findChild(currentNode, item);

            if (!childNode) {
                childNode = make_shared<FPNode>(item, count, currentNode);
                currentNode->children.push_back(childNode);

                // Update header table
                if (headerTable.find(item) == headerTable.end()) {
                    headerTable[item] = { childNode };
                } else {
                    headerTable[item].push_back(childNode);
                }
            } else {
                childNode->count += count;
            }

            currentNode = childNode;
        }
    }

    shared_ptr<FPNode> findChild(const shared_ptr<FPNode>& node, int item) {
        for (const auto& child : node->children) {
            if (child->item == item) {
                return child;
            }
        }
        return nullptr;
    }
};

void mineFrequentPatterns(const FPTree& fpTree, int minSupport, vector<int> prefix = {}) {
    for (const auto& itemPair : fpTree.headerTable) {
        int item = itemPair.first;
        const vector<shared_ptr<FPNode>>& itemNodes = itemPair.second;

        int support = 0;
        for (const auto& node : itemNodes) {
            support += node->count;
        }

        if (support >= minSupport) {
            vector<int> newPattern = prefix;
            newPattern.push_back(item);
            cout << "Frequent Pattern: ";
            for (int val : newPattern) {
                cout << val << " ";
            }
            cout << " (Support: " << support << ")" << endl;

            FPTree conditionalTree;
            map<int, int> itemSupports;

            for (const auto& node : itemNodes) {
                vector<int> path;
                shared_ptr<FPNode> currNode = node;
                while (currNode->parent != nullptr) {
                    path.push_back(currNode->item);
                    currNode = currNode->parent;
                }
                reverse(path.begin(), path.end());
                if (!path.empty()) {
                    path.pop_back(); // Exclude the current item
                    conditionalTree.insert(path, node->count);
                    itemSupports[node->item] += node->count;
                }
            }

            mineFrequentPatterns(conditionalTree, minSupport, newPattern);
        }
    }
}

int main() {
    FPTree fpTree;

    vector<vector<int>> transactions = {
        {1, 2, 3},
        {2, 3, 4},
        {1, 3, 4},
        {2, 3},
        {1, 2, 4},
        {1, 3}
    };

    int minSupport = 2;

    for (const auto& transaction : transactions) {
        fpTree.insert(transaction);
    }

    mineFrequentPatterns(fpTree, minSupport);

    return 0;
}
