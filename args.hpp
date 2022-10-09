#include <iostream>

using namespace std;

int get_args(int &argc, char** &argv, string &fileName, int &neighbors)
{
    if (argc < 2)
    {
        cerr << "Usage: " << argv[0] << " -p PATH/TO/FILE -n NUMBER_OF_NEIGHBORS" << endl;
        return -1;
    }
    for (int i = 1; i < argc; i++)
    {
        if (string(argv[i]) == "-p")
        {
            if (i + 1 < argc)
            {
                i++;
                fileName = argv[i];
            }
            else
            {
                cerr << "-p option requires one argument." << endl;
                return -1;
            }
        }
        else if (string(argv[i]) == "-n")
        {
            if (i + 1 < argc)
            {
                try
                {
                    i++;
                    neighbors = stoi(argv[i]);
                }
                catch (const exception &e)
                {
                    cerr << "invalid argument for -n" << '\n';
                    return -1;
                }
            }
            else
            {
                cerr << "-n option requires one argument." << endl;
                return -1;
            }
        }
    }
    return 0;
}