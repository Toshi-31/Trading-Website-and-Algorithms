#include <bits/stdc++.h>
using namespace std;

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

int main(int argc, char **argv) {
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);
    cout.tie(NULL);
    freopen("Error.txt", "w", stderr);

    // Read Command Line Arguemnets
    int x = stoi(argv[1]), n = stoi(argv[2]), count = 0, position = 0;
    double oversold_theshold = stod(argv[3]), overbought_threshold = stod(argv[4]);
    string sym = argv[5], start = argv[6], end = argv[7];

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

    // Open Files in append Mode
    std::ofstream A("order_statistics.csv", std::ios_base::app);
    std::ofstream B("daily_cashflow.csv", std::ios_base::app);

    string stats = "Date,Order_dir,Quantity,Price\n";
    string Cashflow = "Date,Cashflow\n";
    double prlo = 0;
    reverse(close.begin(), close.end());

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
    };
    A << stats;
    B << Cashflow;
    std::ofstream d("final_pnl.txt");
    d << prlo;
    return 0;
};
