// https://leetcode.com/problems/decode-ways/discuss/1412599/c-constant-space-iterative-dp-solution-all-cases-listed
// constant space, Time: O(N)
class Solution 
{
    public:
        int numDecodings(string s) 
        {
            if(s[0] == '0') return 0;
            int n = s.length();
            if(n == 1) return 1;
            
            vector<int> ans(2);
            ans[0] = 1; // one way to decode first letter
            
            if(s[0] == '1' && s[1] != '0')  ans[1] = ans[0] + 1;  // 1 followed by non zero
            else if(s[0] == '2' && (s[1] <= '6' && s[1] > '0')) ans[1] = ans[0] + 1; // 2 followed by >0 <7
            else if((s[0] == '2' || s[0] == '1') && s[1] == '0') ans[1] = 1; // 2 and 1 followed by 0
            else if(s[1] == '0') ans[1] = 0; // can't interpret 0 if not following a 1 or 2
            else ans[1] = ans[0];  // all other cases
            
            int tmp;
            for(int i = 2; i < n; ++i)
            {
               if(s[i - 1] == '1' && s[i] != '0') tmp = ans[0] + ans[1];  // 1 followed by non zero  
               else if(s[i - 1] == '2' && (s[i] <= '6' && s[i] > '0')) tmp = ans[0] + ans[1]; // 2 followed by >0 <7
               else if((s[i - 1] == '2' || s[i - 1] == '1') && s[i] == '0')  tmp = ans[0]; // 2 and 1 followed by 0
               else if(s[i] == '0') tmp = 0; // can't interpret 0 if not following a 1 or 2
               else tmp = ans[1]; 
                
               ans[0] = ans[1];
               ans[1] = tmp;
            }
            
            return ans[1];
        }
};  
