#include "matrix.h"

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
vector<T> getNeighbors(Matrix<T> &matrix, int rowRef, int colRef, int numOfNeighbors)
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
    return neighbors;
}

template <typename T = int>
T calculateMedian(vector<T> values)
{
    auto size = values.size();
    if (size % 2 == 0)
        return (values[size / 2 - 1] + values[size / 2]) / 2;
    else
        return values[size / 2];
}

template <typename T = int>
void fixPixel(Matrix<T> &newImage, Matrix<T> &originalImage, int &row, int &col, int neighbors)
{
    try
    {
        newImage.at(row, col) = calculateMedian(getNeighbors(originalImage, row, col, neighbors));
    }
    catch (const exception &e)
    {
        cerr << e.what() << '\n';
    }
}

int main()
{
    string nameOfFile = "dataset_hard/001.pgm";
    // Create a text string, which is used to output the text file
    string myText;

    // Read from the text file
    ifstream imageFile(nameOfFile);

    string format; // formato da imagem
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
    auto xImage = Matrix<int>(rows, collumns);

    auto numOfThreads = thread::hardware_concurrency();
    vector<thread> threads(numOfThreads);
    auto threadCount = 0;

    for (size_t r = 0; r < rows; r++)
    {
        getline(imageFile, myText);
        vector<string> rowVector;
        split(myText, rowVector, ' ');
        for (auto c = 0; c < collumns; c++)
            originalImage.at(r, c) = stoi(rowVector[c]);
    }

    imageFile.close();

    for (int col = 0; col < collumns; col++)
        for (int row = 0; row < rows; row++)
        {
            // auto pixel = originalImage.at(row, col);
            // if (0 == pixel || pixel == 255)
            // {
                xImage.at(row, col) = calculateMedian(getNeighbors(originalImage, row, col, 1));
                if (threadCount == numOfThreads)
                {
                    for (auto &t : threads)
                    {
                        t.join();
                    }
                    threadCount = 0;
                }
                threads[threadCount] = thread(fixPixel<int>, ref(xImage), ref(originalImage), ref(row), ref(col), 1);
                threadCount++;
            // }
            // else
            // {
            //     xImage.at(row, col) = pixel;
            // }
        }
    for (auto &t : threads)
    {
        if (t.joinable())
            t.join();
    }
    threadCount = 0;
    ofstream newImageFile(nameOfFile + "_augumented.pgm");

    newImageFile << format << "\n";
    newImageFile << dimensions << "\n";
    newImageFile << maxPixValue << "\n";

    newImageFile << xImage.serialize() << "\n";

    newImageFile.close();

    return 0;
}