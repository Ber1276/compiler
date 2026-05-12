#include <iostream>
#include <fstream>
#include <string>
using namespace std;

string PreprocessFile(const string &filename)
{
    string processed;            // save resule
    bool inBlockComment = false; // block comment
    bool lastWasSpace = false;   // is white flag

    ifstream file(filename);
    if (!file.is_open())
    {
        cerr << "can't open file , what a pity" << filename << endl;
        return "";
    }

    string line;
    while (getline(file, line)) // read line by line
    {
        int i = 0;
        int len = line.length();

        while (i < len)
        {
            if (!inBlockComment && i < len - 1 && line[i] == '/' && line[i + 1] == '/')
            {
                break; // ignore
            } // meet //

            if (!inBlockComment && i < len - 1 && line[i] == '/' && line[i + 1] == '*')
            {
                inBlockComment = true;
                i += 2;
                continue;
            } // meet /* (block comment start)

            if (inBlockComment && i < len - 1 && line[i] == '*' && line[i + 1] == '/')
            {
                inBlockComment = false;
                i += 2;
                continue;
            } // meet */ (block comment end)

            if (inBlockComment)
            {
                i++;
                continue;
            }

            if (isspace(line[i]))
            {
                if (!lastWasSpace)
                {
                    processed += ' ';
                    lastWasSpace = true;
                }
            }
            else
            {
                processed += line[i];
                lastWasSpace = false;
            }
            i++;
        }
    }

    file.close();
    return processed;
}

int main()
{
    // test1--- pre process
    cout << "pre process result" << PreprocessFile("test.c") << endl;

    return 0;
}
