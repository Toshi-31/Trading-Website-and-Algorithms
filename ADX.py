import sys

from jugaad_data.nse import stock_df
from datetime import datetime,timedelta

start = datetime.strptime(sys.argv[2],"%d/%m/%Y").date()
end = datetime.strptime(sys.argv[3],"%d/%m/%Y").date()
n = int(sys.argv[4])
sym = sys.argv[1]

df = stock_df(symbol=sym, from_date=start,to_date=end, series="EQ")

df['DATE'] = df['DATE'].dt.strftime('%d/%m/%Y')
df= df.drop(columns=['SERIES','LTP','52W H','52W L','VOLUME','VALUE','SYMBOL'])
df.to_csv('ADX.csv',index =False)