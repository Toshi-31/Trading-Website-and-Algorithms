#include <bits/stdc++.h>
using namespace std;
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
int main(int argc, char **argv) {
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);
    cout.tie(NULL);
    // freopen("Error.txt", "w", stderr);

    // Read Command Line Arguements
    int x = stoi(argv[1]), position = 0, count = 0;
    string sym = argv[2], start = argv[3], end = argv[4];

    // Generate Data
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

    // Open Files in append mode
    std::ofstream A("order_statistics.csv", std::ios_base::app);
    std::ofstream B("daily_cashflow.csv", std::ios_base::app);

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
    A << stats;
    B << Cashflow;
    std::ofstream d("final_pnl.txt");
    d << prlo;
    return 0;
};
