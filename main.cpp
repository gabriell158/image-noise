#include "matrix.hpp"
#include "args.hpp"

#include <iostream>
#include <algorithm>
#include <thread>

using namespace mat;
using namespace std;

size_t split(const string &txt, vector<string> &strs, char ch)
{
    size_t pos = txt.find(ch);
    size_t initialPos = 0;
    strs.clear();

    // Decompose statement
    while (pos != string::npos)
    {
        strs.push_back(txt.substr(initialPos, pos - initialPos));
        initialPos = pos + 1;

        pos = txt.find(ch, initialPos);
    }

    // Add the last one
    strs.push_back(txt.substr(initialPos, min(pos, txt.size()) - initialPos + 1));

    return strs.size();
}

template <typename T = int>
T fix_pixel(Matrix<T> &matrix, int rowRef, int colRef, int numOfNeighbors)
{
    vector<T> neighbors;
    auto matrixCols = matrix.get_num_cols();
    auto matrixRows = matrix.get_num_rows();
    for (auto c = colRef - numOfNeighbors; c < colRef + numOfNeighbors + 1; c++)
    {
        for (auto r = rowRef - numOfNeighbors; r < rowRef + numOfNeighbors + 1; r++)
        {
            if ((c > 0 && r > 0) && (c != colRef || r != rowRef) && (c < matrixCols && r < matrixRows))
            {
                try
                {
                    neighbors.push_back(matrix.at(r, c));
                }
                catch (const exception &e)
                {
                    cerr << e.what() << '\n';
                }
            }
        }
    }
    sort(neighbors.begin(), neighbors.end());
    auto size = neighbors.size();
    if (size % 2 == 0)
        return (neighbors[size / 2 - 1] + neighbors[size / 2]) / 2;
    else
        return neighbors[size / 2];
}

int main(int argc, char **argv)
{
    string pathTofile;
    int numOfNeighbors = 1;
    auto err = get_args(argc, argv, pathTofile, numOfNeighbors);
    if (err)
        return err;

    ifstream imageFile(pathTofile);

    if (!imageFile.good())
    {
        cerr << "File does not exist\n";
        return -1;
    }

    string format; // formato da imagem
    // TODO nao usar getLine; a imagem nao necessariamente esta quebrada por linhas
    getline(imageFile, format);

    string dimensions;
    getline(imageFile, dimensions);

    vector<string> v;
    split(dimensions, v, ' ');
    // TODO validar conversao

    // numero de colunas
    auto collumns = stoi(v[0]);

    // numero de linhas
    auto rows = stoi(v[1]);

    string maxPixValue;
    if (format != "P1")
        getline(imageFile, maxPixValue);

    cout << "Formato : " << format + ".\n";
    cout << "Dimensao : " << collumns << "x" << rows << +".\n";
    cout << maxPixValue + ".\n";

    auto originalImage = Matrix<int>(rows, collumns);
    auto augumentedImage = Matrix<int>(rows, collumns);

    string myText;

    for (int r = 0; r < rows; r++)
    {
        getline(imageFile, myText);
        vector<string> rowVector;
        split(myText, rowVector, ' ');
        for (auto c = 0; c < collumns; c++)
            originalImage.at(r, c) = stoi(rowVector[c]);
    }

    imageFile.close();

    // auto numOfThreads = thread::hardware_concurrency();
    // vector<thread> threads(numOfThreads);
    // auto threadCount = 0;

    for (int col = 0; col < collumns; col++)
        for (int row = 0; row < rows; row++)
        {
            augumentedImage.at(row, col) = fix_pixel(originalImage, row, col, numOfNeighbors);
            // TODO tratar as boradas da imagem primeiro
            // if (threadCount == numOfThreads)
            // {
            //     for (auto &t : threads)
            //     {
            //         t.join();
            //     }
            //     threadCount = 0;
            // }
            // threads[threadCount] = thread(fix_pixel<int>, ref(augumentedImage), ref(originalImage), ref(row), ref(col), 1);
            // threadCount++;
        }
    // for (auto &t : threads)
    // {
    //     if (t.joinable())
    //         t.join();
    // }

    ofstream newImageFile(pathTofile.substr(0, pathTofile.find(".pgm")) + "_augumented.pgm");

    newImageFile << format << "\n";
    newImageFile << dimensions << "\n";
    newImageFile << maxPixValue << "\n";

    newImageFile << augumentedImage.serialize() << "\n";

    newImageFile.close();

    return 0;
}