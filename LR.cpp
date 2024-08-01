#include <bits/stdc++.h>
using namespace std;

vector<vector<long double>> mul(const vector<vector<long double>> &A, const vector<vector<long double>> &B) {
    int n = A.size(), m = A[0].size(), p = B[0].size();
    vector<vector<long double>> C(n, vector<long double>(p));
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < p; j++) {
            for (int k = 0; k < m; k++) {
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    };
    return C;
}
// Transpose
vector<vector<long double>> transpose(const vector<vector<long double>> &A) {
    int n = A.size(), m = A[0].size();
    vector<vector<long double>> T(m, vector<long double>(n, 0.0));
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            T[j][i] = A[i][j];
        }
    };
    return T;
}
long double coFactor(int r, int c, const vector<vector<long double>> &A);
// Compute Determinant
long double det(const vector<vector<long double>> &A) {
    int n = A.size();
    if (n == 1) {
        return A[0][0];
    }
    long double result = 0;
    for (int i = 0; i < n; i++) {
        result += A[0][i] * coFactor(0, i, A);
    }
    return result;
};

// Compute coFactor

long double coFactor(int r, int c, const vector<vector<long double>> &A) {
    int n = A.size();
    vector<vector<long double>> X(n - 1, vector<long double>(n - 1));
    for (int i = 0, ix = 0; i < n; i++) {
        if (i == r) continue;
        for (int j = 0, iy = 0; j < n; j++) {
            if (j == c) continue;
            X[ix][iy] = A[i][j];
            iy++;
        };
        ix++;
    };
    return ((r + c) % 2 == 0) ? det(X) : -det(X);
};

// Adjoint Matrix for square matrices(to compute inverse);
vector<vector<long double>> adj(const vector<vector<long double>> &A) {
    int n = A.size();
    vector<vector<long double>> Adj(n, vector<long double>(n));
    for (int i = 0; i < A.size(); i++) {
        for (int j = 0; j < A[0].size(); j++) {
            Adj[i][j] = coFactor(i, j, A);
        };
    };
    Adj = transpose(Adj);
    return Adj;
};

// Inverse
vector<vector<long double>> inv(const vector<vector<long double>> &A) {
    vector<vector<long double>> X = adj(A);
    long double mag = det(A);
    for (int i = 0; i < A.size(); i++) {
        for (int j = 0; j < A[0].size(); j++) {
            X[i][j] = X[i][j] / mag;
        }
    };
    return X;
};

int main(int argc, char **argv) {
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);
    cout.tie(NULL);
    freopen("Error.txt", "w", stderr);

    // Reading Command Line Arguements
    int x = stoi(argv[1]), p = stoi(argv[2]), count = 0, position = 0;
    string sym = argv[3], train_start = argv[4], train_end = argv[5], start = argv[6], end = argv[7];

    // Generate data
    string command = "python LR.py " + sym + " " + train_start + " " + train_end + " " + start + " " + end;
    system(command.c_str());

    string command1 = "python3 LR.py " + sym + " " + train_start + " " + train_end + " " + start + " " + end;
    system(command1.c_str());

    // Read train data from data1
    vector<pair<string, long double>> train_close, train_high, train_low, train_prevClose, train_VWAP, train_NoofTrades, train_open;
    string line;
    std::ifstream train_data("data1.csv");
    getline(train_data, line);
    while (getline(train_data, line)) {
        string date, open, prCl, cl, hi, lo, vwap, Trades;
        istringstream line1(line);
        getline(line1, date, ',');
        getline(line1, open, ',');
        getline(line1, hi, ',');
        getline(line1, lo, ',');
        getline(line1, prCl, ',');
        getline(line1, cl, ',');
        getline(line1, vwap, ',');
        getline(line1, Trades, ',');
        train_open.push_back({date, stold(open)});
        train_high.push_back({date, stold(hi)});
        train_low.push_back({date, stold(lo)});
        train_prevClose.push_back({date, stold(prCl)});
        train_close.push_back({date, stold(cl)});
        train_VWAP.push_back({date, stold(vwap)});
        train_NoofTrades.push_back({date, stold(Trades)});
        count++;
    }

    reverse(train_close.begin(), train_close.end());
    reverse(train_high.begin(), train_high.end());
    reverse(train_low.begin(), train_low.end());
    reverse(train_prevClose.begin(), train_prevClose.end());
    reverse(train_VWAP.begin(), train_VWAP.end());
    reverse(train_NoofTrades.begin(), train_NoofTrades.end());
    reverse(train_open.begin(), train_open.end());

    // Read data from data2
    vector<pair<string, long double>>
        close, high, low, prevClose, VWAP, NoofTrades, open;
    int count1 = 0;
    std::ifstream data("data2.csv");
    getline(data, line);
    while (getline(data, line)) {
        string date, op, prCl, cl, hi, lo, vwap, Trades;
        istringstream line1(line);
        getline(line1, date, ',');
        getline(line1, op, ',');
        getline(line1, hi, ',');
        getline(line1, lo, ',');
        getline(line1, prCl, ',');
        getline(line1, cl, ',');
        getline(line1, vwap, ',');
        getline(line1, Trades, ',');
        open.push_back({date, stold(op)});
        high.push_back({date, stold(hi)});
        low.push_back({date, stold(lo)});
        prevClose.push_back({date, stold(prCl)});
        close.push_back({date, stold(cl)});
        VWAP.push_back({date, stold(vwap)});
        NoofTrades.push_back({date, stold(Trades)});
        count1++;
    }

    reverse(close.begin(), close.end());
    reverse(high.begin(), high.end());
    reverse(low.begin(), low.end());
    reverse(prevClose.begin(), prevClose.end());
    reverse(VWAP.begin(), VWAP.end());
    reverse(NoofTrades.begin(), NoofTrades.end());
    reverse(open.begin(), open.end());

    std::ofstream A("order_statistics.csv", std::ios_base::app);
    std::ofstream B("daily_cashflow.csv", std::ios_base::app);

    // Training
    vector<vector<long double>> X(count - 1, vector<long double>(8, 1.0));
    for (int i = 1; i < count; i++) {
        X[i - 1][0] = 1.0;
        X[i - 1][1] = train_close[i - 1].second;
        X[i - 1][2] = train_open[i - 1].second;
        X[i - 1][3] = train_VWAP[i - 1].second;
        X[i - 1][4] = train_low[i - 1].second;
        X[i - 1][5] = train_high[i - 1].second;
        X[i - 1][6] = train_NoofTrades[i - 1].second;
        X[i - 1][7] = train_open[i].second;
    };

    vector<vector<long double>> y(count - 1, vector<long double>(1, 0));
    for (int i = 1; i < count; i++) {
        y[i - 1][0] = train_close[i].second;
    };
    auto X_trans = transpose(X);
    auto Y = mul(X_trans, X);
    auto Inv = inv(Y);
    auto z = mul(X_trans, y);
    auto coefficents = mul(Inv, z);
    // cout << "X: " << endl;
    // for (int i = 0; i < X.size(); i++) {
    //     for (int j = 0; j < X[0].size(); j++) {
    //         cout << X[i][j] << ' ';
    //     }
    //     cout << endl;
    // }
    // cout << "y: " << endl;
    // for (int i = 0; i < y.size(); i++) {
    //     for (int j = 0; j < y[0].size(); j++) {
    //         cout << y[i][j] << ' ';
    //     }
    //     cout << endl;
    // }

    // cout << "X_trans: " << endl;
    // for (int i = 0; i < X_trans.size(); i++) {
    //     for (int j = 0; j < X_trans[0].size(); j++) {
    //         cout << X_trans[i][j] << ' ';
    //     }
    //     cout << endl;
    // }

    // cout << "Y: " << endl;
    // for (int i = 0; i < Y.size(); i++) {
    //     for (int j = 0; j < Y[0].size(); j++) {
    //         cout << Y[i][j] << ' ';
    //     }
    //     cout << endl;
    // }

    // cout << "Inv: " << endl;
    // for (int i = 0; i < Inv.size(); i++) {
    //     for (int j = 0; j < Inv[0].size(); j++) {
    //         cout << Inv[i][j] << ' ';
    //     }
    //     cout << endl;
    // }

    // cout << "Z " << endl;
    // for (int i = 0; i < z.size(); i++) {
    //     for (int j = 0; j < z[0].size(); j++) {
    //         cout << z[i][j] << ' ';
    //     }
    //     cout << endl;
    // };

    // cout << "Coefficients: " << endl;
    // for (int i = 0; i < coefficents.size(); i++) {
    //     for (int j = 0; j < coefficents[0].size(); j++) {
    //         cout << coefficents[i][j] << ' ';
    //     };
    //     cout << endl;
    // };
    // for (int i = 1; i < count; i++) {
    //     double result = 0.0;
    //     result += coefficents[0][0];
    //     result += coefficents[1][0] * train_close[i - 1].second;
    //     result += coefficents[2][0] * train_open[i - 1].second;
    //     result += coefficents[3][0] * train_VWAP[i - 1].second;
    //     result += coefficents[4][0] * train_low[i - 1].second;
    //     result += coefficents[5][0] * train_high[i - 1].second;
    //     result += coefficents[6][0] * train_NoofTrades[i - 1].second;
    //     result += coefficents[7][0] * train_open[i].second;
    //     cerr << "Prediction " << result << " Close " << train_close[i].second << endl;
    // }
    double prlo = 0.0;
    string stats = "Date,Order_dir,Quantity,Price\n";
    string Cashflow = "Date,Cashflow\n";
    for (int i = 1; i < count1; i++) {
        double result = 0.0;
        result += coefficents[0][0];
        result += coefficents[1][0] * close[i - 1].second;
        result += coefficents[2][0] * open[i - 1].second;
        result += coefficents[3][0] * VWAP[i - 1].second;
        result += coefficents[4][0] * low[i - 1].second;
        result += coefficents[5][0] * high[i - 1].second;
        result += coefficents[6][0] * NoofTrades[i - 1].second;
        result += coefficents[7][0] * open[i].second;
        if ((result > (close[i].second * (1 + (((double)p) / (100))))) && (position < x)) {
            stats += (close[i].first + ",BUY,1," + to_string(close[i].second) + "\n");
            position++;
            prlo -= close[i].second;
        } else if ((result < (close[i].second * (1 - (((double)p) / (100))))) && (position > (-1 * x))) {
            stats += (close[i].first + ",SELL,1," + to_string(close[i].second) + "\n");
            position--;
            prlo += close[i].second;
        }
        Cashflow += close[i].first + "," + to_string(prlo) + '\n';
    }
    if (position > 0) {
        prlo -= position * close[count1 - 1].second;
        position = 0;
    } else {
        prlo += position * close[count1 - 1].second;
        position = 0;
    };
    A << stats;
    B << Cashflow;
    std::ofstream d("final_pnl.txt");
    d << prlo;

    return 0;
}
