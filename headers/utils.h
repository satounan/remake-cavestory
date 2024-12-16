#pragma once
#include <string>
#include <vector>
class Utils
{
    public:
        static unsigned int split(const std::string &txt, std::vector<std::string> &strs, char ch)
        {
            if (txt.empty()) {
                strs.clear();
                return 0;
            }

            int pos = txt.find(ch);
            int initialPos = 0;
            strs.clear();

            while (pos != std::string::npos)
            {
                strs.push_back(txt.substr(initialPos, pos - initialPos + 1));
                initialPos = pos + 1;
                pos = txt.find(ch, initialPos);
            }

            // 处理最后一个子字符串
            // may wrong
            strs.push_back(txt.substr(initialPos));

            return strs.size();
        }
};