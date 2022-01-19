/**
 * Definition for singly-linked list.
 * struct ListNode {
 *     int val;
 *     ListNode *next;
 *     ListNode(int x) : val(x), next(NULL) {}
 * };
 */

// floyd's turtle and hare; Space O(1)
// https://leetcode.com/problems/linked-list-cycle-ii
class Solution 
{
    public:
        ListNode *detectCycle(ListNode *head) 
        {   
            // preparations and edge case
            ListNode* turtle = head;
            ListNode* hare = head;
            
            if(!head
               || !(head->next)
               || !(hare->next->next)) return NULL;
            
            turtle = head->next;
            hare = head->next->next;
            
            // find cycle
            while(turtle != hare)
            {
                if(!(hare->next) || !(hare->next->next)) return NULL;
                
                turtle = turtle->next;
                hare = hare->next->next;
            }
            
            
            // cycle found; determine where it begins
            turtle = head;
            while(turtle != hare)
            {
                turtle = turtle->next;
                hare = hare->next;
            }
            
            return turtle;
        }
};