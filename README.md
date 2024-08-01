COP290
ASSIGNMENT 1 - SUBPART 3

For each strategy, the int main() includes the following in general:

It starts by parsing command-line arguments which specify the parameters for the analysis, such as the number of previous data points to consider (n), the stock symbol (sym), start and end dates (start and end), and a parameter x (maximum/minimum position we can hold), overbought/oversold threshold, stoploss threshold,etc.
Then it constructs a command string to run a Python script ({strategy}.py) to generate or fetch stock market data within the specified date range and for the specified symbol. The output is stored in a CSV file named {strategy}.csv.
After generating the data, it reads the CSV file line by line, extracting relevant information.
Date: The date of the trading day.
Opening Price (op): The price of the stock at the opening of the trading day.
High Price (hi): The highest price of the stock during the trading day.
Low Price (lo): The lowest price of the stock during the trading day.
Previous Closing Price (prCl): The closing price of the stock on the previous trading day.
Closing Price (cl): The closing price of the stock on the current trading day.
Volume Weighted Average Price (vwap): The volume weighted average price of the stock during the trading day.
Number of Trades (Trades): The number of trades that occurred during the trading day.
It stores relevant information like date and closing price in vectors. The closing price vector (close) is reversed to process the data in chronological order.
It opens two output CSV files, order_statistics.csv and daily_cashflow.csv, in append mode for writing.

Summarization and Implementation of each strategy by us:

1. Basic
   Function Definitions:
   monotonic_inc and monotonic_dec: These functions check if a sequence of closing prices is monotonically increasing or decreasing over a given window of time. They take the current index i, the window size n, and a vector of pairs containing date and closing price as input.
   Main function:
   It iterates over the data, checking for increasing or decreasing trends in closing prices over the specified window size (n). Based on these trends, it makes trading decisions (buy or sell) and updates the trading position and cash flow accordingly.
   Finally, it writes the trading statistics (stats) and daily cash flow (Cashflow) to the respective CSV files.

2. n-Day Moving Average (DMA)
   Function Definition:
   This function calculates the moving average and standard deviation of closing prices over a given window of n days. It takes the current index i, the window size n, and a vector of pairs containing date and closing price as input. It returns a pair containing the calculated average and standard deviation.
   Main function:
   It iterates over the data, calculating the moving average and standard deviation using the DMA function. Based on the DMA strategy, it makes trading decisions (buy or sell) and updates the trading position and cash flow accordingly.
   Finally, it writes the trading statistics (stats) and daily cash flow (Cashflow) to the respective CSV files.

3. Improving DMA
   Function Definition (AMA):
   This function calculates the Adaptive Moving Average (AMA) based on the current index i, the window size n, constants c1 and c2, and a vector of pairs containing date and closing price (close).
   It calculates the change in closing price (change) over the given window.
   It computes the absolute change in closing prices over the window to determine the Efficiency Ratio (ER).
   It iteratively calculates the AMA using the formula AMA += SF \* (P - AMA), where SF is the Smoothing Factor and P is the closing price till starting from the DAY1 to the current day.
   The AMA is returned as the result.
   Main function:
   It implements the Adaptive Moving Average (AMA) trading strategy by iterating over the data.
   It calculates the AMA for each data point and makes buy or sell decisions based on whether the closing price is above or below a certain percentage (p) of the AMA.
   Trading decisions are made based on a queue data structure (buy and sell) to hold potential future buy or sell orders.
   The trading statistics (stats) and daily cash flow (Cashflow) are updated accordingly.
   Finally, it writes the trading statistics and cash flow data to the respective CSV files.

4. MACD Indicator
   EWM Function:
   The code defines two overloaded EWM functions to calculate the Exponential Weighted Mean:
   double EWM(int start, int days, vector<pair<string, double>> prices): This function calculates the EWM for a given number of days (days) with start as the ending index, in the vector prices, which contains pairs of dates and closing prices.
   double EWM(int start, int days, vector<double> MACD_vect): This function calculates the EWM for a given number of days (days) with start as the ending index, in the vector MACD_vect, which contains MACD values.
   Main function:
   It calculates the MACD indicator for each data point using the difference between the 12-day and 26-day EWMs of closing prices.
   It calculates the Signal Line, which is the 9-day EWM of the MACD.
   Based on the MACD and Signal Line, it makes buy or sell decisions. If the MACD is greater than the Signal Line and the position is less than x, it buys one share. If the MACD is less than the Signal Line and the position is greater than -x, it sells one share.
   It updates the trading statistics (stats) and daily cash flow (Cashflow) accordingly.
   Finally, it writes the trading statistics and cash flow data to the respective CSV files.

5. Relative Strength Index (RSI)
   RSI Function:
   The code defines a function RSI to calculate the Relative Strength Index (RSI) for a given index i, window size n, and a vector of pairs containing date and closing price (close).
   Inside the function, it calculates the average gain (gain) and average loss (loss) over the last n days.
   It iterates through the past n days, calculating the difference between consecutive closing prices (x), and adds positive differences to gain and negative differences (adjusted to be positive) to loss.
   After calculating gain and loss, it computes the Relative Strength (RS) as the ratio of average gain to average loss.
   Finally, it calculates the RSI using the formula provided in the algorithm and returns the result.
   Main function:
   It calculates the RSI for each data point using the RSI function.
   Based on the RSI values and the specified thresholds for oversold and overbought conditions, it generates buy or sell signals.
   It updates the trading statistics (stats) and daily cash flow (Cashflow) accordingly.
   Finally, it writes the trading statistics and cash flow data to the respective CSV files.

6. ADX
   Function Definitions:
   The max function returns the maximum of two given doubles.
   The EWM function calculates the Exponential Weighted Moving Average (EWM) for a given vector of doubles over a specified number of days. This is done similar to how it was done in the MACD Indicator.
   Main function:
   Calculates the True Range (TR) and Directional Movement (DM) values for each period based on the provided formulas. It defines ATR as EWM of TR over past n days.
   Computes the Directional Index (DI) values and then the ADX value based on the EWM of DX over the past n days.
   Generates buy or sell signals based on whether the ADX value crosses the specified threshold. If the ADX is above the threshold, a buy signal is generated, and if it's below, a sell signal is generated.
   Writes the generated statistics and cash flow to respective CSV files.

7. Linear Regression
   Matrix Operations Functions:
   mul: Performs matrix multiplication between two matrices.
   transpose: Computes the transpose of a matrix.
   det: Computes the determinant of a square matrix.
   coFactor: Computes the cofactor of a specific element in a matrix.
   adj: Computes the adjoint matrix of a square matrix.
   inv: Computes the inverse of a square matrix.
   dot: Computes element-wise multiplication between two matrices.
   Main function:
   Reads training data from data1.csv and test data from data2.csv, parsing them into vectors for various features like opening price, high price, low price, etc.
   Performs matrix operations to train a linear regression model using the training data.
   Uses the trained coefficients to predict the closing price for each day in the test data.
   Generates buy or sell signals based on the predicted price compared to the actual price, following the specified percentage difference (p). If the predicted price is greater than the actual price by a percentage greater than or equal to p, it generates a buy signal, and if it's less, it generates a sell signal.
   Writes the trading statistics and cash flow to order_statistics.csv and daily_pnl.csv, respectively.
   Training Process:
   The training process involves creating a feature matrix X and a target matrix y from the training data.
   The feature matrix X includes various features like the previous close price, opening price, VWAP (Volume Weighted Average Price), low price, high price, number of trades, and opening price of the next day.
   The target matrix y contains the closing prices.
   The code then computes the coefficients of the linear regression model using matrix operations.

8. Best of all
   The code implements several trading strategies using technical analysis indicators- Exponential Moving Average (EMA), Dual Moving Average (DMA), Adaptive Moving Average (AMA), Moving Average Convergence Divergence (MACD), Relative Strength Index (RSI), Average Directional Index (ADX), and Linear Regression (LR). Each strategy is implemented as a function that takes input data (stock prices), analyzes it based on the respective indicator, and generates trading signals (buy/sell orders).Each strategy function returns a pair containing the net profit/loss and a string representing the trading statistics and cash flow. The strategies are then evaluated based on their performance in terms of profitability and the maximum profit is printed to the console.
   OpenMP is used for parallel execution.
9. Mean-Reverting Pairs Trading Strategy
   Mean Function:
   This function calculates the rolling mean of the spread of the pair of stocks over a given lookback period (n). It takes the current index i, the lookback period n, and the vector S containing the spread values.
   SD Function:
   This function calculates the rolling standard deviation of the spread of the pair of stocks over a given lookback period (n). It takes the current index i, the lookback period n, the vector S containing the spread values, and the mean m calculated by the Mean function.
   Main Function:
   It reads the generated data from two CSV files (data1.csv and data2.csv) for each stock.
   It iterates through the data, calculating the spread, mean, and standard deviation for each day.
   Based on the z-score calculated using the spread, mean, and standard deviation, it generates buy or sell signals. If the z-score exceeds a threshold, it generates a buy or sell signal for the spread.
   It records the trading statistics and cash flow for each trade and writes them to CSV files.
   Finally, it squares off the last position and writes the final cash flow to a CSV file.
10. Mean-Reverting Pairs Trading Strategy with stop_loss_threshold
    This extends the previous pairs trading strategy by incorporating a stop-loss mechanism. Here's a breakdown of the changes and additions:
    Stop-loss Mechanism:
    After taking a position based on the z-score crossing the threshold, the code calculates the z-score again at each iteration.
    If the z-score moves in the unexpected direction beyond the stop_loss_threshold, the code closes the position to limit losses.
    The stop-loss condition is checked after each position is opened, and if met, the position is closed immediately.
    Code Modifications:
    The main logic for generating buy and sell signals based on the z-score remains the same as before.
    After generating a buy or sell signal, the code checks if the stop-loss condition is met. If it is, the position is closed.
    The spr queue is used to store the spread, mean, and standard deviation of each position for stop-loss calculation.
    Output:
    The trading statistics, including order details and daily P&L, are written to CSV files (order_statistics1.csv, order_statistics2.csv, daily_cashflow.csv).
