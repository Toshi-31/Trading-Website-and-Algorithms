import sys

from jugaad_data.nse import stock_df
from datetime import datetime,timedelta


train_start = datetime.strptime(sys.argv[2],"%d/%m/%Y").date()
train_end = datetime.strptime(sys.argv[3],"%d/%m/%Y").date()

start = datetime.strptime(sys.argv[4],"%d/%m/%Y").date()
end = datetime.strptime(sys.argv[5],"%d/%m/%Y").date()

sym = sys.argv[1]

df = stock_df(symbol=sym, from_date=train_start ,
            to_date=train_end, series="EQ")
size = len(df)
current_date = train_start
while(len(df)!= (size+1)):
    current_date = current_date-timedelta(days=1)
    df = stock_df(symbol=sym, from_date=current_date ,
            to_date=train_end, series="EQ")

df['DATE'] = df['DATE'].dt.strftime('%d/%m/%Y')
df= df.drop(columns=['SERIES','LTP','52W H','52W L','VOLUME','VALUE','SYMBOL'])
df.to_csv('data1.csv',index =False)

df = stock_df(symbol=sym, from_date=start,
            to_date=end, series="EQ")
size = len(df)
current_date = start
while(len(df)!= (size+1)):
    current_date = current_date-timedelta(days=1)
    df = stock_df(symbol=sym,from_date=current_date,to_date=end)

df['DATE'] = df['DATE'].dt.strftime('%d/%m/%Y')
df= df.drop(columns=['SERIES','LTP','52W H','52W L','VOLUME','VALUE','SYMBOL'])
df.to_csv('data2.csv',index =False)
