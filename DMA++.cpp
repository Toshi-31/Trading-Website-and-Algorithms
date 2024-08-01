#include <bits/stdc++.h>
using namespace std;
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

int main(int argc, char **argv) {
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);
    cout.tie(NULL);
    // freopen("Error.txt", "w", stderr);

    // Read Command Line Arguements
    int x = stoi(argv[1]), n = stoi(argv[2]), p = stoi(argv[3]);
    int max_hold_days = stoi(argv[4]), position = 0, count = 0;
    double c1 = stod(argv[5]), c2 = stod(argv[6]);
    string sym = argv[7], start = argv[8], end = argv[9];

    // cout << 1 << endl;
    // Generate Data
    string command = "python DMAp.py " + sym + " " + start + " " + end + " " + to_string(n);
    system(command.c_str());

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

    // Open Files in append mode
    std::ofstream A("order_statistics.csv", std::ios_base::app);
    std::ofstream B("daily_pnl.csv", std::ios_base::app);

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
    A << stats;
    B << Cashflow;
    std::ofstream d("final_pnl.txt");
    d << prlo;
    return 0;
};
