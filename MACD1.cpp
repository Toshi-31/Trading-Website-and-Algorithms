#include <bits/stdc++.h>
using namespace std;
double EWM(int start, int days, vector<pair<string, double>> close) {
    double a = 2.0 / (days + 1);
    double ewm = close[0].second;
    for (int i = 0; i <= start; i++) {
        ewm += a * (close[i].second - ewm);
    };
    return ewm;
}
double EWM(int start, int days, vector<double> M) {
    double ALPHA = 2.0 / (days + 1);
    double result = M[0];
    for (int i = 0; i <= start; i++) {
        result += ALPHA * (M[i] - result);
    };
    return result;
}
// int counter = 0;
// int a = static_cast<double>(2) / (days + 1);
// int ptr = 1;
// double ewm = M[0];
// while (counter <= 8) {
//     ewm = a * (M[ptr] - ewm) + ewm;
//     ptr++;
//     counter++;
//     start--;
// }
// return ewm;

// double EWM(int i, int n, vector<pair<string, double>> &close) {
//     double EWM = close[i - n + 1].second, a = 2 / (n + 1);
//     for (int j = i - n + 1; j <= i; j++) {
//         EWM = a * (close[j].second - EWM) + EWM;
//     }
//     return EWM;
// }
// double EWM(int i, int n, vector<double> &close) {
//     double EWM = close[i - n + 1], a = 2 / (n + 1);
//     for (int j = i - n + 1; j <= i; j++) {
//         EWM = a * (close[j] - EWM) + EWM;
//     }
//     return EWM;
// }

int main(int argc, char **argv) {
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);
    cout.tie(NULL);
    freopen("Error.txt", "w", stderr);

    // Read Command Line Arguements
    int x = stoi(argv[1]), position = 0, count = 0;
    string sym = argv[2], start = argv[3], end = argv[4];

    // Generate Data
    string command = "python MACD.py " + sym + " " + start + " " + end;
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
        double short_ewm = EWM(i, 12, close);
        double long_ewm = EWM(i, 26, close);
        double MACD = short_ewm - long_ewm;
        M.push_back(MACD);
        int signal = EWM(i, 9, M);
        if ((MACD > signal) && (position < x)) {
            stats += (close[i].first + ",BUY,1," + to_string(close[i].second) + "\n");
            position++;
            prlo -= close[i].second;
        } else if ((MACD < signal) && (position > (-1 * x))) {
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
    // Cashflow += close[count - 1].first + "," + to_string(prlo) + '\n';
    A << stats;
    B << Cashflow;

    std::ofstream d("final_pnl.txt");
    d << prlo;
    return 0;
};
