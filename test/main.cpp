#include <iostream>
#include <algorithm>
#include <map>
#include <set>
#include <vector>
#include <unordered_map>
#include <unordered_set>

using namespace std;

struct ListNode {
    int val;
    ListNode *next;

    ListNode() : val(0), next(nullptr) {}

    ListNode(int x) : val(x), next(nullptr) {}

    ListNode(int x, ListNode *next) : val(x), next(next) {}
};


class Solution {
public:
    ListNode *detectCycle(ListNode *head) {
        if (head == nullptr) {
            return nullptr;
        }

        auto slow = head, fast = head;
        while (fast != nullptr) {
            slow = slow->next;
            if (fast->next == nullptr) {
                return nullptr;
            }
            fast = fast->next->next;
            if (fast == slow) {
                auto p = head;
                while (p!= fast) {
                    p = p->next;
                    fast = fast->next;
                }
                return p;
            }
        }
        return nullptr;
    }
};


int main() {
    int value = 1;
    auto copy_val = [value]() {
        return value;
    };

    value = 100;
    cout << copy_val() << endl;

    return 0;
}
