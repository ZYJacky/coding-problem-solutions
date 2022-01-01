// https://leetcode.com/problems/number-of-matching-subsequences/discuss/1290068/c-24-line-perplexingly-good-brute-forece-solution-without-using-hashmap-beat-10096
class Solution 
{   
    public:
        int numMatchingSubseq(string s, vector<string>& words)
        {   
            int count = 0;
            for(string word : words)
            {   
                size_t pos = 0; 
                ++count;  // assume is a subsequence
                for(int i = 0; i < word.size(); ++i)
                {   
                    if(i == 0) pos = s.find(word[i], pos);  // search all s for the first char
                    else pos = s.find(word[i], pos + 1);  // else only search the remaining part
                    if(pos == -1) 
                    {   
                        --count;  // revert if not a subsequence
                        break;
                    }
                }
            }
            return count;
        }
};
