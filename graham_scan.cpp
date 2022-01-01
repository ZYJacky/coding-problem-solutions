// https://leetcode.com/problems/erect-the-fence/
// implementation of graham scan algorithm for finding convex hull
class Solution 
{
    public:
        vector<vector<int>> outerTrees(vector<vector<int>>& trees) 
        {   
            if(trees.size() <= 3) return trees;
            sort(trees.begin(), trees.end());
        
            vector<vector<int>> ans;
            
            stack<vector<int>> s;   // tracks points on convex hull
            s.push(trees[0]);       // left-most point must be on convex hull
            s.push(trees[1]);
            
            // build lower hull
            for(int i = 2; i < trees.size(); ++i)
            {   
                vector<int> p = first(s);
                while(s.size() > 1 && orient(p, s.top(), trees[i]) < 0)  // keeps popping if turning right
                {   
                    s.pop();
                    if(s.size() > 1) p = first(s);
                }
                s.push(trees[i]);
            }
            
            // store lower hull
            while(s.size() != 0) 
            {
                ans.push_back(s.top());
                cout << s.top()[0] << " " << s.top()[1] << endl;
                s.pop();
            }
        
            s.push(trees[0]);
            s.push(trees[1]);
            
            // build upper hull
            for(int i = 2; i < trees.size(); ++i)
            {   
                vector<int> p = first(s);
                while(s.size() > 1 && orient(p, s.top(), trees[i]) > 0) // keeps popping if turning left
                {   
                    s.pop();
                    if(s.size() > 1) p = first(s);
                }
                s.push(trees[i]);
            }
        
            while(s.size() != 0) 
            {   
                if(find(ans.begin(), ans.end(), s.top()) == ans.end()) ans.push_back(s.top()); // avoid duplicate at the end
                s.pop();
            }
        
            return ans;
        }
    
        // helper functions
  
        vector<int> first(stack<vector<int>> s)
        {
            s.pop();
            return s.top();
        }
    
        int orient(vector<int>& p, vector<int>& q, vector<int>& r)
        {
            return (q[0] * r[1] - r[0] * q[1]) - (p[0] * r[1] - r[0] * p[1]) + (p[0] * q[1] - q[0] * p[1]);
        }
};
