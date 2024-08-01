#include <bits/stdc++.h>
using namespace std;

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
int main(int argc, char **argv) {
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);
    cout.tie(NULL);
    int x = stoi(argv[1]), n = stoi(argv[2]);
    string sym = argv[3], start = argv[4], end = argv[5];
    // Generate data
    string command1 = "python3 basic.py " + sym + " " + start + " " + end + " " + to_string(n);
    system(command1.c_str());
    // Reading data
    std::ifstream data("basic.csv");
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
    std::ofstream A("order_statistics.csv", std::ios_base::app);
    std::ofstream B("daily_cashflow.csv", std::ios_base::app);
    int position = 0;
    reverse(close.begin(), close.end());

    string stats = "Date,Order_dir,Quantity,Price\n";
    string Cashflow = "Date,Cashflow\n";
    double p = 0;
    for (int i = n; i < count; i++) {
        if (monotonic_inc(i, n, close) && (position < x)) {
            // stats << close[i].first << ",BUY,1," << close[i].second << "\n";
            stats += (close[i].first + ",BUY,1," + to_string(close[i].second) + "\n");
            position++;
            p -= close[i].second;
        } else if (monotonic_dec(i, n, close) && (position > (-1 * x))) {
            // stats << close[i].first << ",SELL,1," << close[i].second << "\n";
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

    A << stats << endl;
    B << Cashflow << endl;
    std::ofstream data1("final_pnl.txt");
    data1 << p << endl;
    return 0;
};
