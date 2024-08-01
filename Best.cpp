#include <bits/stdc++.h>

#include <functional>
#include <omp.h>
using namespace std;
// Basic Stratergy
bool monotonic_inc(int i, int n, vector<pair<string, double>> &close) {
    for (int j = i - n + 1; j <= i; j++) {
        if (close[j - 1].second >= close[j].second) return false;
    };
    // cerr << "Increasing" << close[i].first << '\n';
    return true;
}

bool monotonic_dec(int i, int n, vector<pair<string, double>> &close) {
    for (int j = i - n + 1; j <= i; j++) {
        if (close[j - 1].second <= close[j].second) return false;
    };
    // cerr << "Decreasing" << close[i].first << '\n';
    return true;
}

pair<double, pair<string, string>> Basic(string sym, string start, string end) {
    int n = 7, x = 5;
    // Generate data
    string command = "python basic.py " + sym + " " + start + " " + end + " " + to_string(n);
    string command1 = "python3 basic.py " + sym + " " + start + " " + end + " " + to_string(n);
    system(command.c_str());
    system(command1.c_str());

    // Reading data
    std::ifstream data("basic.csv");
    string line;
    int count = 0;
    vector<pair<string, double>> close, prevClose;
    //
    getline(data, line);
    while (getline(data, line)) {
        string date, priceStr, op, prCl, cl, hi, lo, vwap, Trades;
        istringstream line1(line);
        getline(line1, date, ',');
        getline(line1, op, ',');
        getline(line1, hi, ',');
        getline(line1, lo, ',');
        getline(line1, prCl, ',');
        getline(line1, cl, ',');
        getline(line1, vwap, ',');
        getline(line1, Trades, ',');
        close.push_back({date, stod(cl)});
        prevClose.push_back({date, stod(prCl)});
        count++;
    }
    int position = 0;
    reverse(close.begin(), close.end());

    string stats = "Date,Order_dir,Quantity,Price\n";
    string Cashflow = "Date,Cashflow\n";
    double p = 0;
    for (int i = n; i < count; i++) {
        if (monotonic_inc(i, n, close) && (position < x)) {
            stats += (close[i].first + ",BUY,1," + to_string(close[i].second) + "\n");
            position++;
            p -= close[i].second;
        } else if (monotonic_dec(i, n, close) && (position > (-1 * x))) {
            stats += (close[i].first + ",SELL,1," + to_string(close[i].second) + "\n");
            position--;
            p += close[i].second;
        }
        Cashflow += close[i].first + "," + to_string(p) + '\n';
    };
    if (position > 0) {
        p += position * close[count - 1].second;
    } else {
        p -= position * close[count - 1].second;
    };
    return {p, {stats, Cashflow}};
};

// DMA Stratergy
pair<double, double> DMA(int i, int n, vector<pair<string, double>> close) {
    double sum = 0;
    for (int j = i - n + 1; j <= i; j++) {
        sum += close[j].second;
    }
    double avg = sum / n;
    double sd = 0;
    for (int j = i - n + 1; j <= i; j++) {
        sd += (close[j].second - avg) * (close[j].second - avg);
    }
    sd /= n;
    return {avg, sqrt(sd)};
};

pair<double, pair<string, string>> DMA_Strat(string sym, string start, string end) {
    int x = 5, n = 50, p = 2;
    // Generate Data
    string command = "python DMA.py " + sym + " " + start + " " + end + " " + to_string(n);
    system(command.c_str());

    string command1 = "python3 DMA.py " + sym + " " + start + " " + end + " " + to_string(n);
    system(command1.c_str());

    // Read Generated Data
    std::ifstream data("DMA.csv");
    string line;
    int count = 0;
    vector<pair<string, double>> close, prevClose;
    getline(data, line);
    while (getline(data, line)) {
        string date, priceStr, op, prCl, cl, hi, lo, vwap, Trades;
        istringstream line1(line);
        getline(line1, date, ',');
        getline(line1, op, ',');
        getline(line1, hi, ',');
        getline(line1, lo, ',');
        getline(line1, prCl, ',');
        getline(line1, cl, ',');
        getline(line1, vwap, ',');
        getline(line1, Trades, ',');
        close.push_back({date, stod(cl)});
        prevClose.push_back({date, stod(prCl)});
        count++;
    }
    int position = 0;

    string stats = "Date,Order_dir,Quantity,Price\n";
    string Cashflow = "Date,Cashflow\n";
    double prlo = 0;
    reverse(close.begin(), close.end());

    for (int i = n; i < count; i++) {
        auto y = DMA(i, n, close);
        double sd = y.second, avg = y.first;
        if ((close[i].second > (avg + p * sd)) && (position < x)) {
            stats += (close[i].first + ",BUY,1," + to_string(close[i].second) + "\n");
            position++;
            prlo -= close[i].second;
        } else if ((close[i].second < (avg - p * sd)) && (position > (-1 * x))) {
            stats += (close[i].first + ",SELL,1," + to_string(close[i].second) + "\n");
            position--;
            prlo += close[i].second;
        };
        Cashflow += close[i].first + "," + to_string(prlo) + '\n';
    }
    if (position > 0) {
        prlo += position * close[count - 1].second;
        position = 0;
    } else {
        prlo -= position * close[count - 1].second;
        position = 0;
    };
    return {prlo, {stats, Cashflow}};
}

// DMA++
double AMA(int i, int n, double c1, double c2, const vector<pair<string, double>> &close) {
    double change = close[i].second - close[i - n].second;
    double absolute_change = 0;
    double SF = 0.5;

    for (int j = i - n + 1; j <= i; j++) {
        absolute_change += abs(close[j].second - close[j - 1].second);
    }
    double ER = change / absolute_change;
    double P = close[i - n + 1].second, AMA = close[i - n + 1].second;
    for (int j = i - n + 1; j <= i; j++) {
        SF += c1 * (((((2 * ER) / (1 + c2)) - 1) / (((2 * ER) / (1 + c2)) + 1)) - SF);
        AMA += SF * (P - AMA);
    };
    return AMA;
};
pair<double, pair<string, string>> DMAp(string sym, string start, string end) {
    // Read Command Line Arguements
    int x = 5, n = 14, p = 5;
    int max_hold_days = 28, position = 0, count = 0;
    double c1 = 2, c2 = 0.2;

    // Generate Data
    string command = "python DMAp.py " + sym + " " + start + " " + end + " " + to_string(n);
    system(command.c_str());
    string command1 = "python3 DMAp.py " + sym + " " + start + " " + end + " " + to_string(n);
    system(command1.c_str());

    //  Read Generated Data
    string line;
    vector<pair<string, double>> close, prevClose;
    std::ifstream data("DMAp.csv");
    getline(data, line);
    while (getline(data, line)) {
        string date, priceStr, op, prCl, cl, hi, lo, vwap, Trades;
        istringstream line1(line);
        getline(line1, date, ',');
        getline(line1, op, ',');
        getline(line1, hi, ',');
        getline(line1, lo, ',');
        getline(line1, prCl, ',');
        getline(line1, cl, ',');
        getline(line1, vwap, ',');
        getline(line1, Trades, ',');
        close.push_back({date, stod(cl)});
        prevClose.push_back({date, stod(prCl)});
        count++;
    }
    reverse(close.begin(), close.end());

    string stats = "Date,Order_dir,Quantity,Price\n";
    string Cashflow = "Date,Cashflow\n";

    queue<pair<double, int>> buy, sell;
    double prlo = 0.0;
    // Calculate Values
    for (int i = n; i < count; i++) {
        cerr << close[i].first << ' ' << close[i].second << endl;
        double y = AMA(i, n, c1, c2, close);
        // cerr << "AMA " << y << " AMA(1+p) " << y * (1 + (double)p / (double)100) << " AMA(1-p) " << y * (1 - (((double)p / (double)100))) << " Actual " << close[i].second << endl;
        if ((close[i].second > y * (1 + (double)p / (double)100)) && (position < x)) {
            if (!sell.empty()) {
                sell.pop();
            } else {
                buy.push({close[i].second, i + max_hold_days});
            }
            position++;
            stats += (close[i].first + ",BUY,1," + to_string(close[i].second) + "\n");
            prlo -= close[i].second;
        } else if ((close[i].second < (1 - (((double)p / (double)100)))) && (position > (-1 * x))) {
            if (!buy.empty()) {
                buy.pop();
            } else {
                sell.push({close[i].second, i + max_hold_days});
            }
            position--;
            stats += (close[i].first + ",SELL,1," + to_string(close[i].second) + "\n");
            prlo += (close[i].second);
        }
        if ((!buy.empty() && buy.front().second <= i)) {
            stats += (close[i].first + ",SELL,1," + to_string(close[i].second) + "\n");
            position--;
            prlo += (close[i].second);
            buy.pop();
        } else if (!sell.empty() && sell.front().second <= i) {
            stats += (close[i].first + ",BUY,1," + to_string(close[i].second) + "\n");
            position++;
            prlo -= (close[i].second);
            sell.pop();
        };

        Cashflow += close[i].first + "," + to_string(prlo) + '\n';
    };
    if (position > 0) {
        prlo += position * close[count - 1].second;
        position = 0;
    } else if (position < 0) {
        prlo -= position * close[count - 1].second;
        position = 0;
    };
    return {prlo, {stats, Cashflow}};
}
// MACD
double EWM(int start, int days, vector<pair<string, double>> close) {
    double a = ((2.0) / (days + 1));
    double ewm = close[0].second;
    for (int i = 0; i <= start; i++) {
        ewm = a * (close[i].second - ewm) + ewm;
    };
    return ewm;
};
double EWM(int start, int days, vector<double> M) {
    double ALPHA = ((2.0) / (days + 1));
    double result = M[0];
    for (int i = 0; i <= start; i++) {
        result = ALPHA * (M[i] - result) + result;
    };
    return result;
};

pair<double, pair<string, string>> MACD(string sym, string start, string end) {

    int count = 0, position = 0, x = 5;

    // Generate Data
    string command = "python MACD.py " + sym + " " + start + " " + end;
    system(command.c_str());
    string command = "python3 MACD.py " + sym + " " + start + " " + end;
    system(command.c_str());
    //  Read Generated Data
    string line;
    vector<pair<string, double>> close, prevClose;
    std::ifstream data("MACD.csv");
    getline(data, line);
    while (getline(data, line)) {
        string date, priceStr, op, prCl, cl, hi, lo, vwap, Trades;
        istringstream line1(line);
        getline(line1, date, ',');
        getline(line1, op, ',');
        getline(line1, hi, ',');
        getline(line1, lo, ',');
        getline(line1, prCl, ',');
        getline(line1, cl, ',');
        getline(line1, vwap, ',');
        getline(line1, Trades, ',');
        close.push_back({date, stod(cl)});
        prevClose.push_back({date, stod(prCl)});
        count++;
    }
    reverse(close.begin(), close.end());

    string stats = "Date,Order_dir,Quantity,Price\n";
    string Cashflow = "Date,Cashflow\n";
    double prlo = 0;

    vector<double> M;
    for (int i = 0; i < count; i++) {
        double MACD = EWM(i, 12, close) - EWM(i, 26, close);
        M.push_back(MACD);
        double Signal = EWM(M.size() - 1, 9, M);

        if ((MACD > Signal) && (position < x)) {
            stats += (close[i].first + ",BUY,1," + to_string(close[i].second) + "\n");
            position++;
            prlo -= close[i].second;
        } else if ((MACD < Signal) && (position > (-1 * x))) {
            stats += (close[i].first + ",SELL,1," + to_string(close[i].second) + "\n");
            position--;
            prlo += close[i].second;
        }
        Cashflow += close[i].first + "," + to_string(prlo) + '\n';
    }

    // Square Off last position
    if (position > 0) {
        prlo += position * close[count - 1].second;
        position = 0;
    } else {
        prlo -= position * close[count - 1].second;
        position = 0;
    };
    return {prlo, {stats, Cashflow}};
};
// RSI

double RSI(int i, int n, const vector<pair<string, double>> &close) {
    double gain = 0, loss = 0;
    for (int j = i - n + 1; j <= i; j++) {
        double x = close[j].second - close[j - 1].second;
        if (x > 0) {
            gain += x;
        } else {
            loss -= x;
        }
    }
    double RS = gain / loss;
    return (100.0 - (100.0 / (1.0 + RS)));
};

pair<double, pair<string, string>> RSI_Strat(string sym, string start, string end) {

    // Read Command Line Arguemnets
    int x = 5, n = 14, count = 0, position = 0;
    double oversold_theshold = 30, overbought_threshold = 70;

    // Generate Data
    string command = "python RSI.py " + sym + " " + start + " " + end + " " + to_string(n);
    system(command.c_str());
    string command1 = "python3 RSI.py " + sym + " " + start + " " + end + " " + to_string(n);
    system(command1.c_str());

    // Read Generated Data
    vector<pair<string, double>> close, prevClose;
    std::ifstream data("RSI.csv");
    string line;
    getline(data, line);
    while (getline(data, line)) {
        string date, priceStr, op, prCl, cl, hi, lo, vwap, Trades;
        istringstream line1(line);
        getline(line1, date, ',');
        getline(line1, op, ',');
        getline(line1, hi, ',');
        getline(line1, lo, ',');
        getline(line1, prCl, ',');
        getline(line1, cl, ',');
        getline(line1, vwap, ',');
        getline(line1, Trades, ',');
        close.push_back({date, stod(cl)});
        prevClose.push_back({date, stod(prCl)});
        count++;
    }

    string stats = "Date,Order_dir,Quantity,Price\n";
    string Cashflow = "Date,Cashflow\n";

    double prlo = 0.0;
    for (int i = n; i < count; i++) {
        double R = RSI(i, n, close);
        cerr << " R " << R << '\n';
        if ((position < x) && (R < oversold_theshold)) {
            stats += (close[i].first + ",BUY,1," + to_string(close[i].second) + "\n");
            position++;
            prlo -= close[i].second;
        } else if ((position > (-1 * x)) && (R > overbought_threshold)) {
            stats += (close[i].first + ",SELL,1," + to_string(close[i].second) + "\n");
            position--;
            prlo += close[i].second;
        }
        Cashflow += close[i].first + "," + to_string(prlo) + '\n';
    };
    if (position > 0) {
        prlo -= position * close[count - 1].second;
        position = 0;
    } else {
        prlo += position * close[count - 1].second;
        position = 0;
    }; // cout << prlo << endl;
    return {prlo, {stats, Cashflow}};
};
// ADX

double max(double a, double b) {
    if (a > b)
        return a;
    return b;
};
double EWM1(int start, int days, const vector<double> &v) {
    double ALPHA = (2 / (days + 1));
    double res = v[0];
    for (int i = 0; i <= start; i++) {
        res = ALPHA * (v[i] - res) + res;
    };
    return res;
};

pair<double, pair<string, string>> ADX(string sym, string start, string end) {
    int x = 5, n = 14, count = 0, position = 0;
    double adx_threshold = 25;

    // Generate data
    string command = "python ADX.py " + sym + " " + start + " " + end + " " + to_string(n);
    system(command.c_str());
    string command1 = "python3 ADX.py " + sym + " " + start + " " + end + " " + to_string(n);
    system(command1.c_str());

    // Reading data
    std::ifstream data("ADX.csv");
    vector<pair<string, double>> close, high, low, prevClose;

    string line;
    getline(data, line);
    while (getline(data, line)) {
        string date, priceStr, op, prCl, cl, hi, lo, vwap, Trades;
        istringstream line1(line);
        getline(line1, date, ',');
        getline(line1, op, ',');
        getline(line1, hi, ',');
        getline(line1, lo, ',');
        getline(line1, prCl, ',');
        getline(line1, cl, ',');
        getline(line1, vwap, ',');
        getline(line1, Trades, ',');
        close.push_back({date, stod(cl)});
        prevClose.push_back({date, stod(prCl)});
        high.push_back({date, stod(hi)});
        low.push_back({date, stod(lo)});
        count++;
    };
    reverse(close.begin(), close.end());
    reverse(high.begin(), high.end());
    reverse(low.begin(), low.end());
    reverse(prevClose.begin(), prevClose.end());

    // Calculate Value
    string stats = "Date,Order_dir,Quantity,Price\n";
    string Cashflow = "Date,Cashflow\n";
    double prlo = 0.0;
    vector<double> TRs, DM_plus, DM_minus, DXs;
    for (int i = 0; i < count; i++) {
        // Calculate TR,DM+,DM-
        double TR = max(high[i].second - low[i].second, max(high[i].second - prevClose[i].second, low[i].second - prevClose[i].second));
        double DMP = max(0, high[i].second - high[i - 1].second);
        double DMM = max(0, low[i].second - low[i - 1].second);
        DM_plus.push_back(DMP);
        DM_minus.push_back(DMM);
        TRs.push_back(TR);

        // calculate DI+ ,DI-
        double DIP = EWM(i, n, DM_plus);
        double DIM = EWM(i, n, DM_minus);
        // Calculate DX and ADX as EWM of ADX
        double DX = ((DIP - DIM) / (DIP + DIM)) * 100;
        DXs.push_back(DX);
        // cout << 1 << endl;
        double ADX = EWM(i, n, DXs);
        // Buy sell based on ADX ;
        if ((ADX > adx_threshold) && (position < x)) {
            // cerr << close[i].first << ",BUY,1," << close[i].second << "\n";
            stats += (close[i].first + ",BUY,1," + to_string(close[i].second) + "\n");
            position++;
            prlo -= close[i].second;
        } else if ((ADX < adx_threshold) && (position > (-1 * x))) {
            // cerr << close[i].first << ",SELL,1," << close[i].second << "\n";
            stats += (close[i].first + ",SELL,1," + to_string(close[i].second) + "\n");
            position--;
            prlo += close[i].second;
        };
        Cashflow += close[i].first + "," + to_string(prlo) + '\n';
    }

    // Square Off last position
    if (position > 0) {
        prlo -= position * close[count - 1].second;
        position = 0;
    } else {
        prlo += position * close[count - 1].second;
        position = 0;
    };
    return {prlo, {stats, Cashflow}};
};
// LR

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

pair<double, pair<string, string>> LR(string sym, string start, string end) {
    int x = 5, p = 2, count = 0, position = 0;

    // Generate data
    string command = "python LR2.py " + sym + " " + start + " " + end;
    system(command.c_str());

    string command1 = "python3 LR2.py " + sym + " " + start + " " + end;
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
    // Square Off last position
    if (position > 0) {
        prlo -= position * close[count1 - 1].second;
        position = 0;
    } else {
        prlo += position * close[count1 - 1].second;
        position = 0;
    };
    return {prlo, {stats, Cashflow}};
};
//
int main(int argc, char **argv) {
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);
    cout.tie(NULL);
    freopen("Error.txt", "w", stderr);
    string sym = argv[1], start = argv[2], end = argv[3];
    std::ofstream A("order_statistics.csv", std::ios_base::app);
    std::ofstream B("daily_cashflow.csv", std::ios_base::app);
    pair<double, pair<string, string>> results[7];

#pragma omp parallel for
    for (int i = 0; i < 7; ++i) {
        if (i == 0) results[i] = Basic(sym, start, end);
        if (i == 1) results[i] = DMA_Strat(sym, start, end);
        if (i == 2) results[i] = DMAp(sym, start, end);
        if (i == 3) results[i] = MACD(sym, start, end);
        if (i == 4) results[i] = RSI_Strat(sym, start, end);
        if (i == 5) results[i] = ADX(sym, start, end);
        if (i == 6) results[i] = LR(sym, start, end);
    }
    double m = -1 * DBL_MAX;
    int index = -1;
    for (int i = 0; i < 7; ++i) {
        auto &result = results[i].first;
        cout << result << endl;
        if (result > m) {
            index = i;
            m = result;
        }
    };
    string stats = results[index].second.first;
    string Cashflow = results[index].second.second;
    A << stats << endl;
    B << Cashflow << endl;
    std::ofstream data1("final_pnl.txt");
    data1 << m << endl;
    return 0;
};
