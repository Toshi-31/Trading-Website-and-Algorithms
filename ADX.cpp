#include <bits/stdc++.h>
using namespace std;

double max(double a, double b) {
    if (a > b)
        return a;
    return b;
};
double EWM(int start, int days, const vector<double> &v) {
    double ALPHA = (2 / (days + 1));
    double res = v[0];
    for (int i = 0; i <= start; i++) {
        res = ALPHA * (v[i] - res) + res;
    };
    return res;
};
int main(int argc, char **argv) {
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);
    cout.tie(NULL);
    freopen("Error.txt", "w", stderr);

    // Read Command Line Arguements
    int x = stoi(argv[1]), n = stoi(argv[2]), count = 0, position = 0;
    double adx_threshold = stod(argv[3]);
    string sym = argv[4], start = argv[5], end = argv[6];

    // Generate data
    string command = "python ADX.py " + sym + " " + start + " " + end + " " + to_string(n);
    system(command.c_str());

    // Reading data
    std::ifstream data("ADX.csv");
    vector<pair<string, double>> close, high, low, prevClose;

    // Open Files in append Mode
    std::ofstream A("order_statistics.csv", std::ios_base::app), B("daily_pnl.csv", std::ios_base::app);
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
    A << stats;
    B << Cashflow;
    return 0;
};
