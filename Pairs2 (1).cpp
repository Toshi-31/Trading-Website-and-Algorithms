
#include <bits/stdc++.h>
using namespace std;

double Mean(int i, int n, const vector<double> S) {
    double m = 0.0;
    for (int j = i - n + 1; j <= i; j++) {
        m += S[j];
    };
    return (m / n);
};

double SD(int i, int n, vector<double> S, double m) {
    double result = 0.0;
    for (int j = i - n + 1; j <= i; j++) {
        double spread =
            result += (S[j] - m) * (S[j] - m);
    };
    return sqrt((result) / m);
};

int main(int argc, char **argv) {
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);
    cout.tie(NULL);
    freopen("Error.txt", "w", stderr);
    // Read Command Line Arguements

    int x = stoi(argv[1]), n = stoi(argv[2]), count = 0, position = 0;
    double threshold = stod(argv[4]), stop_loss_threshold = stod(argv[5]);
    string sym1 = argv[6], sym2 = argv[7], start_date = argv[8], end_date = argv[9];
    vector<pair<string, double>> close1, high1, low1, prevClose1;

    std::ofstream A1("order_statistics1.csv", std::ios_base::app);
    std::ofstream A2("order_statistics2.csv", std::ios_base::app);
    std::ofstream B("daily_pnl.csv", std::ios_base::app);

    // Generate Data
    string command = "python Pair2.py " + sym1 + " " + sym2 + " " + start_date + " " + end_date + " " + to_string(n);
    system(command.c_str());

    std::ofstream A1("order_statistics1.csv", std::ios_base::app), A2("order_statistics2.csv", std::ios_base::app), B("daily_pnl.csv", std::ios_base::app);

    // Read Generated Data
    vector<pair<string, double>> close1, high1, low1, prevClose1;
    std::ifstream data1("data1.csv");
    string line;
    getline(data1, line);
    while (getline(data1, line)) {
        std::istringstream line1(line);
        std::string date, priceStr, prCl, cl, hi, lo;
        getline(line1, date, ',');
        getline(line1, hi, ',');
        getline(line1, hi, ',');
        getline(line1, hi, ',');
        getline(line1, lo, ',');
        getline(line1, prCl, ',');
        getline(line1, cl, ',');
        getline(line1, cl, ',');
        close1.push_back({date, stod(cl)});
        high1.push_back({date, stod(hi)});
        low1.push_back({date, stod(lo)});
        prevClose1.push_back({date, stod(prCl)});
        count++;
    }
    reverse(close1.begin(), close1.end());
    reverse(high1.begin(), high1.end());
    reverse(low1.begin(), low1.end());
    reverse(prevClose1.begin(), prevClose1.end());

    std::ifstream file("data2.csv");
    string line;
    vector<pair<string, double>> close2, high2, low2, prevClose2;
    getline(file, line);

    int count1 = 0;
    while (getline(file, line)) {
        std::istringstream line1(line);
        std::string date, priceStr, prCl, cl, hi, lo;
        getline(line1, date, ',');
        getline(line1, hi, ',');
        getline(line1, hi, ',');
        getline(line1, hi, ',');
        getline(line1, lo, ',');
        getline(line1, prCl, ',');
        getline(line1, cl, ',');
        getline(line1, cl, ',');
        close2.push_back({date, stod(cl)});
        high2.push_back({date, stod(hi)});
        low2.push_back({date, stod(lo)});
        prevClose2.push_back({date, stod(prCl)});
        count1++;
    }

    double pl = 0;
    vector<double> S;
    string stats1 = "Date,Order_dir,Quantity,Price\n", stats2 = "Date,Order_dir,Quantity,Price\n";
    string Cashflow;
    queue<vector<double>> spr;
    for (int i = n; i < count; i++) {
        double spread = close1[i].second - close2[i].second;
        S.push_back(spread);
        double m = Mean(i, n, S);
        double sd = SD(i, n, S, m);
        double z_score = (spread - m) / (sd);
        if ((position < x) && (z_score > threshold)) {
            stats1 += (close1[i].first + ",SELL,1," + to_string(close1[i].second) + "\n");
            stats2 += (close2[i].first + ",BUY,1," + to_string(close2[i].second) + "\n");
            position--;
            pl += spread;
            spr.push({pl, m, sd});
        } else if ((position > (-1 * x)) && (z_score < -1 * threshold)) {
            stats1 += (close1[i].first + ",BUY,1," + to_string(close1[i].second) + "\n");
            stats2 += (close2[i].first + ",SELL,1," + to_string(close2[i].second) + "\n");
            position++;
            pl -= spread;
            spr.push({pl, m, sd});
        }

        if (((spread - spr.front()[1]) / (spr.front()[2])) < stop_loss_threshold) {
            stats1 += (close1[i].first + ",BUY,1," + to_string(close1[i].second) + "\n");
            stats2 += (close2[i].first + ",SELL,1," + to_string(close2[i].second) + "\n");
            position++;
            pl -= spread;
            spr.pop();
        }
        Cashflow += close2[i].first + "," + to_string(pl) + '\n';
    }
    // Square Off last position
    double spread = close1[count - 1].second - close2[count - 1].second;
    if (position > 0) {
        pl += spread;
        position = 0;
    } else {
        pl -= spread;
        position = 0;
    };
    A1 << stats1;
    A2 << stats2;
    B << Cashflow;
    return 0;
};