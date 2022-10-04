#include <fstream>
#include <sstream>
#include <string>
#include <vector>

namespace mat
{
    template <typename T = double>
    class Matrix
    {
        std::vector<std::vector<T>> data;

    public:
        // constroi matriz vazia
        Matrix()
        {
        }

        // constroi matriz m*n
        Matrix(int m, int n)
        {
            set_size(m, n);
        }

        // acessa elemento
        T &at(int i, int j)
        {
            return data.at(i).at(j);
        }

        // pega numero de linhas
        int get_num_rows() const
        {
            return data.size();
        }

        // pega numero de colunas
        int get_num_cols()
        {
            return data.size() ? data.at(0).size() : 0;
        }

        // seta tamanho da matriz
        void set_size(int m, int n)
        {
            data.resize(m);
            for (auto &row : data)
            {
                row.resize(n);
            }
        }

        // serialize matriz para impressao
        std::string serialize(std::string const &separator = " ")
        {
            std::ostringstream output;
            for (auto &row : data)
            {
                for (auto &item : row)
                {
                    output << item << separator;
                }
                output << "\n";
            }
            return output.str();
        }

        // zera os elementos da matriz
        void zero()
        {
            for (auto &row : data)
            {
                for (auto &item : row)
                {
                    item = T();
                }
            }
        }

        // somar outra matrix
        Matrix<T> operator+(Matrix<T> &other)
        {
            auto const m = get_num_rows();
            auto const n = get_num_cols();
            if (m != other.get_num_rows() || n != other.get_num_cols())
            {
                throw std::runtime_error("cannot sum matrices with different sizes");
            }
            auto sum = Matrix<T>(m, n);
            for (int i = 0; i < m; i++)
            {
                for (int j = 0; j < n; j++)
                {
                    sum.at(i, j) = data.at(i).at(j) + other.at(i, j);
                }
            }
            return sum;
        }

        // subtrair outra matrix
        Matrix<T> operator-(Matrix<T> &other)
        {
            auto const m = get_num_rows();
            auto const n = get_num_cols();
            if (m != other.get_num_rows() || n != other.get_num_cols())
            {
                throw std::runtime_error("cannot subtract matrices with different sizes");
            }
            auto difference = Matrix<T>(m, n);
            for (int i = 0; i < m; i++)
            {
                for (int j = 0; j < n; j++)
                {
                    difference.at(i, j) = data.at(i).at(j) - other.at(i, j);
                }
            }
            return difference;
        }

        // operador "menos na frente" (retorna uma copia negada da matrix)
        Matrix<T> operator-()
        {
            auto const m = get_num_rows();
            auto const n = get_num_cols();
            auto negated = Matrix<T>(m, n);
            for (int i = 0; i < m; i++)
            {
                for (int j = 0; j < n; j++)
                {
                    negated.at(i).at(j) = -data.at(i).at(j);
                }
            }
            return negated;
        }

        // operador "mais na frente" (apenas retorna uma copia da matriz)
        Matrix<T> operator+()
        {
            return *this;
        }

        // multiplicar por escalar
        Matrix<T> operator*(T value)
        {
            auto const m = get_num_rows();
            auto const n = get_num_cols();
            auto product = Matrix<T>(m, n);
            for (int i = 0; i < m; i++)
            {
                for (int j = 0; j < n; j++)
                {
                    product.at(i, j) = data.at(i).at(j) * value;
                }
            }
            return product;
        }
    };
}