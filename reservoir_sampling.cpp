// https://leetcode.com/problems/linked-list-random-node/

class Solution 
{   
    // reservoir sampling; time O(1)/O(n), space O(1)
    private:
        ListNode* _head;

    public:
        Solution(ListNode* head) 
                    : _head(head)
        {}
        
        int getRandom() 
        {   
            ListNode* temp = _head;
            int ret_val = temp->val, counter = 1;
            temp = temp->next;
            while(temp)
            {
                int rand_num = rand() % (counter + 1); // generate between 0, counter
                if(rand_num <= 0)  ret_val = temp->val;
                
                ++counter;
                temp = temp->next;
            }
            
            return ret_val;
        }
    

    // more straightforward solution: store in a vector; time O(n)/O(1), space O(n)
    /*
    private:
        vector<int> nums;

    public:
        Solution(ListNode* head) 
        {
            while(head)
            {
                nums.push_back(head->val);
                head = head->next;
            }
        }
        
        int getRandom() 
        {
            return nums[rand() % nums.size()];
        }
    */
    
};