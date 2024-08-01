
#include <bits/stdc++.h>
using namespace std;

long double Mean(int n, const vector<double> S) {
    long double m = 0.0;
    int i = S.size() - 1;
    for (int j = i - n + 1; j <= i; j++) {
        m += S[j];
    };
    return ((m) / ((long double)n));
};

long double SD(int n, vector<double> S, double m) {
    long double result = 0.0;
    int i = S.size() - 1;
    for (int j = i - n + 1; j <= i; j++) {
        result += (S[j] - m) * (S[j] - m);
    };
    return sqrt((result) / (long double)(n));
};

int main(int argc, char **argv) {
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);
    cout.tie(NULL);
    freopen("Error.txt", "w", stderr);

    // Read Command Line arguments;
    int x = stoi(argv[1]), n = stoi(argv[2]), count = 0, position = 0;
    double threshold = stod(argv[3]);
    int stop_loss_threshold = stoi(argv[4]);
    string sym1 = argv[5], sym2 = argv[6], start_date = argv[7], end_date = argv[8];
    // Generate Data
    // string command1 = "python Pair1.py " + sym1 + " " + sym2 + " " + start_date + " " + end_date + " " + to_string(n);
    string command = "python3 Pair1.py " + sym1 + " " + sym2 + " " + start_date + " " + end_date + " " + to_string(n);
    // system(command1.c_str());
    system(command.c_str());
    std::ofstream A1("order_statistics_1.csv", std::ios_base::app), A2("order_statistics_2.csv", std::ios_base::app);
    std::ofstream B("daily_cashflow.csv", std::ios_base::app);

    // Read Generated Data
    vector<pair<string, double>> close1, prevClose1;
    std::ifstream data1("data1.csv");
    string line;
    getline(data1, line);
    while (getline(data1, line)) {
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
        close1.push_back({date, stod(cl)});
        prevClose1.push_back({date, stod(prCl)});
        count++;
    }
    reverse(close1.begin(), close1.end());
    reverse(prevClose1.begin(), prevClose1.end());
    int count1 = 0;
    std::ifstream data2("data2.csv");
    vector<pair<string, double>> close2, prevClose2;

    getline(data2, line);
    while (getline(data2, line)) {
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
        close2.push_back({date, stod(cl)});
        prevClose2.push_back({date, stod(prCl)});
        count1++;
    }
    reverse(close2.begin(), close2.end());
    reverse(prevClose2.begin(), prevClose2.end());

    double pl = 0.0;
    vector<double> S;
    string stats1 = "Date,Order_dir,Quantity,Price\n", stats2 = "Date,Order_dir,Quantity,Price\n";
    string Cashflow = "Date,Cashflow\n";

    if (stop_loss_threshold == -1) {

        for (int i = 0; i < count; i++) {
            long double spread = close1[i].second - close2[i].second;
            S.push_back(spread);
            if (i >= n) {
                long double m = Mean(n, S);
                long double sd = SD(n, S, m);
                long double z_score = (spread - m) / (sd);
                if ((position > (-1 * x)) && (z_score > threshold)) {
                    stats1 += (close1[i].first + ",SELL,1," + to_string(close1[i].second) + "\n");
                    stats2 += (close2[i].first + ",BUY,1," + to_string(close2[i].second) + "\n");
                    position--;
                    pl += spread;
                } else if ((position < (x)) && (z_score < (-1 * threshold))) {
                    stats1 += (close1[i].first + ",BUY,1," + to_string(close1[i].second) + "\n");
                    stats2 += (close2[i].first + ",SELL,1," + to_string(close2[i].second) + "\n");
                    position++;
                    pl -= spread;
                }
                Cashflow += close2[i].first + "," + to_string(pl) + '\n';
            }
        }
        long double spread = close1[count - 1].second - close2[count - 1].second;
        if (position > 0) {
            pl += position * spread;
            position = 0;
        } else {
            pl -= position * spread;
            position = 0;
        };
    } else {
        queue<vector<double>> spr;
        for (int i = 0; i < count; i++) {
            double spread = close1[i].second - close2[i].second;
            S.push_back(spread);
            if (i >= n) {
                double m = Mean(n, S);
                double sd = SD(n, S, m);
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
    }
    A1 << stats1;
    A2 << stats2;
    B << Cashflow;

    std::ofstream data3("final_pnl.txt");
    data3 << pl << endl;
    return 0;
};