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
T fix_upper_pixel(Matrix<T> &matrix, int rowRef, int colRef, int numOfNeighbors)
{
    vector<T> neighbors;
    auto matrixCols = matrix.get_num_cols();
    auto matrixRows = matrix.get_num_rows();
    for (auto c = colRef - numOfNeighbors; c <= colRef + numOfNeighbors; c++)
        for (auto r = rowRef; r <= rowRef + numOfNeighbors; r++)
            neighbors.push_back(matrix.at(r, c));

    sort(neighbors.begin(), neighbors.end());
    auto size = neighbors.size();
    if (size % 2 == 0)
        return (neighbors[size / 2 - 1] + neighbors[size / 2]) / 2;
    else
        return neighbors[size / 2];
}

template <typename T = int>
T fix_lower_pixel(Matrix<T> &matrix, int rowRef, int colRef, int numOfNeighbors)
{
    vector<T> neighbors;
    auto matrixCols = matrix.get_num_cols();
    auto matrixRows = matrix.get_num_rows();
    for (auto c = colRef - numOfNeighbors; c <= colRef + numOfNeighbors; c++)
        for (auto r = rowRef - numOfNeighbors; r <= rowRef; r++)
            neighbors.push_back(matrix.at(r, c));

    sort(neighbors.begin(), neighbors.end());
    auto size = neighbors.size();
    if (size % 2 == 0)
        return (neighbors[size / 2 - 1] + neighbors[size / 2]) / 2;
    else
        return neighbors[size / 2];
}

template <typename T = int>
T fix_left_pixel(Matrix<T> &matrix, int rowRef, int colRef, int numOfNeighbors)
{
    vector<T> neighbors;
    auto matrixCols = matrix.get_num_cols();
    auto matrixRows = matrix.get_num_rows();
    for (auto c = colRef; c <= colRef + numOfNeighbors; c++)
        for (auto r = rowRef - numOfNeighbors; r <= rowRef + numOfNeighbors; r++)
            neighbors.push_back(matrix.at(r, c));

    sort(neighbors.begin(), neighbors.end());
    auto size = neighbors.size();
    if (size % 2 == 0)
        return (neighbors[size / 2 - 1] + neighbors[size / 2]) / 2;
    else
        return neighbors[size / 2];
}

template <typename T = int>
T fix_right_pixel(Matrix<T> &matrix, int rowRef, int colRef, int numOfNeighbors)
{
    vector<T> neighbors;
    auto matrixCols = matrix.get_num_cols();
    auto matrixRows = matrix.get_num_rows();
    for (auto c = colRef - numOfNeighbors; c <= colRef; c++)
        for (auto r = rowRef - numOfNeighbors; r <= rowRef + numOfNeighbors; r++)
            neighbors.push_back(matrix.at(r, c));

    sort(neighbors.begin(), neighbors.end());
    auto size = neighbors.size();
    if (size % 2 == 0)
        return (neighbors[size / 2 - 1] + neighbors[size / 2]) / 2;
    else
        return neighbors[size / 2];
}

template <typename T = int>
T fix_pixel(Matrix<T> &matrix, int rowRef, int colRef, int numOfNeighbors)
{
    vector<T> neighbors;
    auto matrixCols = matrix.get_num_cols();
    auto matrixRows = matrix.get_num_rows();
    for (auto c = colRef - numOfNeighbors; c < colRef + numOfNeighbors + 1; c++)
        for (auto r = rowRef - numOfNeighbors; r < rowRef + numOfNeighbors + 1; r++)
            neighbors.push_back(matrix.at(r, c));

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

    // TODO tratar os cantos
    
    // trata as bordas superior e inferior
    for (int col = numOfNeighbors; col < collumns - numOfNeighbors; col++)
        for (auto first_row = 0, last_row = rows - 1; first_row <= numOfNeighbors; first_row++, last_row--)
        {
            augumentedImage.at(first_row, col) = fix_upper_pixel(originalImage, first_row, col, numOfNeighbors);
            augumentedImage.at(last_row, col) = fix_lower_pixel(originalImage, last_row, col, numOfNeighbors);
        }

    // trata as boradas esqueda e direita
    for (int row = numOfNeighbors; row < rows - numOfNeighbors; row++)
        for (auto first_col = 0, last_col = rows - 1; first_col <= numOfNeighbors; first_col++, last_col--)
        {
            augumentedImage.at(row, first_col) = fix_left_pixel(originalImage, row, first_col, numOfNeighbors);
            augumentedImage.at(row, last_col) = fix_right_pixel(originalImage, row, last_col, numOfNeighbors);
        }

    for (int col = numOfNeighbors; col < collumns - numOfNeighbors; col++)
        for (int row = numOfNeighbors; row < rows - numOfNeighbors; row++)
        {
            augumentedImage.at(row, col) = fix_pixel(originalImage, row, col, numOfNeighbors);
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

    ofstream newImageFile(pathTofile.substr(0, pathTofile.find(".pgm")) + "_augumented.pgm");

    newImageFile << format << "\n";
    newImageFile << dimensions << "\n";
    newImageFile << maxPixValue << "\n";

    newImageFile << augumentedImage.serialize() << "\n";

    newImageFile.close();

    return 0;
}