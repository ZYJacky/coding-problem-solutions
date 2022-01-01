// https://leetcode.com/problems/generate-parentheses/discuss/1277093/c-easy-to-understand-recursive-solution-with-detailed-explanation-beats-100
class Solution 
{
    public:
        vector<string> generateParenthesis(int n) 
        {   
            vector<string> well_formed;
            tryParenthesis("(", 1, n, well_formed);  // must start with "("
            
            return well_formed;
        }
        
        void tryParenthesis(string curr, int curr_sum, int n, vector<string>& well_formed)
        {   
            if(curr.length() == 2*n - 1)
            {   
                if(curr_sum == 1)  well_formed.push_back(curr + ")");  // must end with ")"
                return;
            }
            
            if(curr_sum > n || curr_sum < 0) return;  // cannot be valid, terminate early
            
            tryParenthesis(curr + "(", curr_sum + 1, n, well_formed);
            
            tryParenthesis(curr + ")", curr_sum - 1, n, well_formed);
        }
        
};
