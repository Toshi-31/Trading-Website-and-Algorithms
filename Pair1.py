import sys

from jugaad_data.nse import stock_df
from datetime import datetime,timedelta

sym1 = sys.argv[1]
sym2 = sys.argv[2]
start = datetime.strptime(sys.argv[3],"%d/%m/%Y").date()
end = datetime.strptime(sys.argv[4],"%d/%m/%Y").date()
n =int(sys.argv[5])


df = stock_df(symbol=sym1, from_date=start ,
            to_date=end, series="EQ")
size = len(df)
current_date = start


while(len(df)!= (size+n)):
    current_date = current_date-timedelta(days=1)
    df = stock_df(symbol=sym1, from_date=current_date ,
            to_date=end, series="EQ")

df['DATE'] = df['DATE'].dt.strftime('%d/%m/%Y')
df= df.drop(columns=['SERIES','LTP','52W H','52W L','VOLUME','VALUE','SYMBOL'])
df.to_csv('data1.csv',index =False)



df = stock_df(symbol=sym2, from_date=start,
            to_date=end, series="EQ")
size = len(df)
current_date = start

while(len(df)!= (size+n)):
    current_date = current_date-timedelta(days=1)
    df = stock_df(symbol=sym2,from_date=current_date,to_date=end)

df['DATE'] = df['DATE'].dt.strftime('%d/%m/%Y')
df= df.drop(columns=['SERIES','LTP','52W H','52W L','VOLUME','VALUE','SYMBOL'])
df.to_csv('data2.csv',index =False)
