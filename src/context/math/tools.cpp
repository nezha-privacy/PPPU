#include "tools.h"

#include <vector>

/// @brief Get combinatorial number of n and m.
/// @param n From n different elements
/// @param m Extract m elements
/// @return The result, C(n, m)
double combination(int n, int m)
{
    static std::vector<std::vector<double>> combination;

    while (combination.size() <= n) {
        int nn = combination.size();
        combination.emplace_back(std::vector<double>(nn + 1));

        combination[nn][0] = 1;
        combination[nn][nn] = 1;
        for (int mm = 1; mm < nn; ++mm) {
            combination[nn][mm] += combination[nn - 1][mm - 1] + combination[nn - 1][mm];
        }
    }

    return combination[n][m];
}

/// @brief Get bernoulli number of n.
/// @param n The input number to be calculated
/// @return The result, B(n)
double bernoulli(int n)
{
    static std::vector<double> bernoulli{1.0, -1.0 / 2, 1.0 / 6};

    while (bernoulli.size() <= n) {
        int nn = bernoulli.size();
        bernoulli.push_back(0);
        if (nn % 2 == 1)
            continue;

        for (int i = 0; i < nn; ++i) {
            bernoulli[nn] += combination(nn + 1, i) * bernoulli[i];
        }
        bernoulli[nn] *= (-1.0) / (nn + 1);
    }

    return bernoulli[n];
}
