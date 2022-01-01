// https://leetcode.com/problems/array-of-doubled-pairs/discuss/1397344/c-binary-search-with-explanation-beats-80599984-timespace
// binary search; Time: O(nlogn), Space: depends on sorting algorithm
class Solution 
{
    public:
        bool canReorderDoubled(vector<int>& arr) 
        {   
            if(arr.size() == 0) return true; // special case of empty array 
            
            sort(arr.begin(), arr.end()); // sort the array
            vector<int>::iterator index;
            
            for(int i = 0; i < arr.size(); ++i)
            {   
                if(arr[i] == INT_MIN) continue; // the element is used, skip it
                
                if(arr[i] >= 0)   // non-positive, find element 2x of it
                {   
                    if((index = lower_bound(arr.begin() + i + 1, arr.end(), arr[i] * 2)) != arr.end())
                    {   
                        int loc = index - arr.begin();
                        if(arr[loc] == 2 * arr[i])  // verify the value is indeed 2x since lower_bound() returns the first element >= what we look for
                        {   
                            arr[i] = INT_MIN;   // mark the pair
                            arr[loc] = INT_MIN;
                            continue;   
                        }
                    }
                }
                else// negative, need to find element 1/2 of it
                {   
                    if(arr[i] % 2) return false; // can't find interger 1/2 of odd element
                    if((index = lower_bound(arr.begin() + i + 1, arr.end(), arr[i] / 2)) != arr.end())
                    {   
                        int loc = index - arr.begin();
                        if(arr[loc] == arr[i] / 2)
                        {   
                            arr[i] = INT_MIN;
                            arr[loc] = INT_MIN;
                            continue;   
                        }
                    }
                }
                return false;
            }
            return true;
        }   
};
