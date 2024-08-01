#include <bits/stdc++.h>
using namespace std;
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

int main(int argc, char **argv) {
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);
    cout.tie(NULL);
    // freopen("Error.txt", "w", stderr);

    // Command Line Arguements;
    int x = stoi(argv[1]), n = stoi(argv[2]), p = stoi(argv[3]);
    string sym = argv[4], start = argv[5], end = argv[6];

    // Generate Data
    string command = "python DMA.py " + sym + " " + start + " " + end + " " + to_string(n);
    system(command.c_str());

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
    // Open Files in append mode
    std::ofstream A("order_statistics.csv", std::ios_base::app);
    std::ofstream B("daily_cashflow.csv", std::ios_base::app);
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
    A << stats;
    B << Cashflow;
    std::ofstream d1("final_pnl.txt");
    d1 << prlo;
    return 0;
};
