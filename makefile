CXX= g++
CXXFLAGS= -std=c++11 -Wall
requirements:
	pip install -r requirements.txt

ifeq ($(strategy),BASIC)
	$(CXX) $(CXXFLAGS) Basic.cpp -o Basic.exe
	./Basic.exe $(x) $(n) $(symbol) $(start_date) $(end_date)
endif
ifeq ($(strategy),DMA)
	$(CXX) $(CXXFLAGS) DMA.cpp  -o DMA.exe
	./DMA.exe $(x) $(n) $(p) $(symbol) $(start_date) $(end_date)
endif
ifeq ($(strategy),DMA++)
	$(CXX) $(CXXFLAGS) DMA++.cpp -o DMA++.exe
	./DMA++.exe $(x) $(n) $(p) $(max_hold_days) $(c1) $(c2) $(symbol) $(start_date) $(end_date)
endif
ifeq ($(strategy),MACD)
	$(CXX) $(CXXFLAGS) MACD.cpp -o MACD.exe
	./MACD.exe $(x) $(symbol) $(start_date) $(end_date)
endif
ifeq ($(strategy),RSI)
	$(CXX) $(CXXFLAGS) RSI.cpp -o RSI.exe
	./RSI.exe $(x) $(n) $(oversold_threshold) $(overbought_threshold) $(symbol)  $(start_date) $(end_date)
endif
ifeq ($(strategy),ADX)
	$(CXX) $(CXXFLAGS) ADX.cpp -o ADX.exe
	./ADX.exe $(x) $(n) $(adx_threshold) $(symbol) $(start_date) $(end_date)
endif
ifeq ($(strategy),LINEAR_REGRESSION)
	$(CXX) $(CXXFLAGS) LR.cpp -o LR.exe
	./LR.exe $(x) $(p) $(symbol) $(train_start_date) $(train_end_date) $(start_date) $(end_date)
endif
ifeq ($(strategy),BEST_OF_ALL)
	$(CXX) $(CXXFLAGS) Best.cpp -o Best.exe
	./Best.exe $(symbol) $(start_date) $(end_date)
endif
ifeq ($(strategy),PAIRS)
ifdef stop_loss_threshold
	$(CXX) $(CXXFLAGS) Pairs.cpp -o Pairs.exe
	./Pairs.exe $(x) $(n) $(threshold) $(stop_loss_threshold) $(symbol1) $(symbol2) $(start_date) $(end_date)
else
	$(CXX) $(CXXFLAGS) Pairs.cpp -o Pairs.exe
	./Pairs.exe $(x) $(n) $(threshold) -1 $(symbol1) $(symbol2) $(start_date) $(end_date)
endif
endif