from flask import Flask, render_template, request, redirect, url_for, flash, session,send_file,jsonify,Response
from flask_sqlalchemy import SQLAlchemy
from werkzeug.security import generate_password_hash, check_password_hash
import requests
import pandas as pd
from datetime import datetime
import json
import plotly.graph_objects as go
import pytz
import csv
from sklearn.metrics.pairwise import cosine_similarity
from sklearn.feature_extraction.text import TfidfVectorizer
import yfinance as yf
from jugaad_data.nse import NSELive

n = NSELive()
with open('final.csv', 'r') as csv_file:
    csv_reader = csv.DictReader(csv_file)
    for row in csv_reader:
        name_symbol = dict(row)

with open('final.txt', 'r') as file:
    company_names = [line.strip() for line in file]
symbols = []
for company in company_names:
    symbols.append(name_symbol[company])

s = ' '.join(symbols)
values = []
# check if global variable values is identified inside function
with open("final_list.json", 'r') as file:
    values = json.load(file)

def search_by_symbol(target_symbol):
    for company in values:
        if company['symbol'] == target_symbol:
            return company
    return None


def get_ticker_metadata(ticker):
    try:
        info = yf.Ticker(ticker).info
        return info
    except Exception as e:
        print(e)

def refresh_data():
    ticker= yf.Tickers(s)
    for symbol in symbols:
        try:
            info = ticker.tickers[symbol].info
            result = {"company_name": "not_found",
                "market_cap": "not_found",
                "PE": "not_found",
                'Close Price': 'not_found'}
            if info:
                if 'longName' in info:
                    result["company_name"] = info['longName']
                if "marketCap" in info:
                    result["market_cap"] = (round((info['marketCap']/10000000), 2))
                if 'trailingPE' in info:
                    result['PE'] = info['trailingPE']
                if 'previousClose' in info:
                    result['Close Price']= info['previousClose']

            if(result['market_cap'] != 'not_found' and result['company_name'] != 'not_found' and result['PE']!= 'not_found' and result['Close Price'] != 'not_found'):
                values.append(result)
        except Exception as e:
            print(e)


def get_filter(parameters):
    if(len(values) == 0):
        refresh_data()
    min_market_cap = parameters.get('min_market_cap', 0)
    max_market_cap = parameters.get('max_market_cap', 2000)

    min_pe_ratio = parameters.get('min_pe_ratio', 5)
    max_pe_ratio = parameters.get('max_pe_ratio', 3000)

    min_close_price = parameters.get('min_close_price', 0)
    max_close_price = parameters.get('max_close_price', 1000)

    filtered_companies = [
            company for company in values
        if 'market_cap' in company and 'bln$' in company['market_cap']
        and min_market_cap < float(company['market_cap'].split()[0]) < max_market_cap
        and 'PE' in company and min_pe_ratio < float(company['PE']) < max_pe_ratio
        and 'Close Price' in company and min_close_price < float(company['Close Price']) < max_close_price
    ]
    return filtered_companies


class QueryFinder:
    def __init__(self, base_list):
        self.base_list = base_list
        self.vectorizer = TfidfVectorizer()
        self.vectors = self._compute_tfidf_vectors()

    def _compute_tfidf_vectors(self):
        return self.vectorizer.fit_transform(self.base_list)

    def find_closest_queries(self, query, num_closest=5):
        query_vector = self.vectorizer.transform([query])

        similarity_scores = cosine_similarity(query_vector, self.vectors)

        closest_indices = similarity_scores.argsort()[0][-num_closest:][::-1]

        closest_queries = [self.base_list[idx] for idx in closest_indices]

        return closest_queries

query_finder = QueryFinder(company_names)
def parseTimestamp(inputdata):
  timestamplist = []
  timestamplist.extend(inputdata["chart"]["result"][0]["timestamp"])
  timestamplist.extend(inputdata["chart"]["result"][0]["timestamp"])

  calendertime = []
  utc_timezone = pytz.utc
  ist_timezone = pytz.timezone('Asia/Kolkata')

  for ts in timestamplist:
    dt = datetime.utcfromtimestamp(ts).replace(tzinfo=utc_timezone)
    dt = dt.astimezone(ist_timezone)
    calendertime.append(dt.strftime("%m-%d-%Y %H:%M:%S"))
  return calendertime

def parseValues(inputdata):

  valueList = {'OPEN': [],'CLOSE':[] ,'HIGH':[] ,'LOW':[]}
  valueList['OPEN'].extend(inputdata["chart"]["result"][0]["indicators"]["quote"][0]["open"])
  valueList['CLOSE'].extend(inputdata["chart"]["result"][0]["indicators"]["quote"][0]["close"])
  valueList['HIGH'].extend(inputdata["chart"]["result"][0]["indicators"]["quote"][0]["high"])
  valueList['LOW'].extend(inputdata["chart"]["result"][0]["indicators"]["quote"][0]["low"])

  return valueList
def get_interval(range):
    if(range == "1d"):
        return "5m"
    elif(range == "1wk"):
        return "15m"
    elif(range =="1mo"):
        return "60m"
    elif(range == "1y"):
        return "1wk"
    elif(range =="5y"):
        return "1mo"


def generate_plot(symbol,range):
    interval = get_interval(range)
    url = "https://apidojo-yahoo-finance-v1.p.rapidapi.com/stock/v3/get-chart"
    querystring = {"interval":interval,"symbol":symbol,"range":range,"region":"IN","includePrePost":"false","useYfid":"true","includeAdjustedClose":"true","events":"capitalGain,div,split"}

    headers = {
        "X-RapidAPI-Key": "5091a3807emshd9177749716494fp10be60jsn19c60ba96d0d",
        "X-RapidAPI-Host": "apidojo-yahoo-finance-v1.p.rapidapi.com"
    }

    response = requests.get(url, headers=headers, params=querystring)
    inputdata ={}
    inputdata["Timestamp"] = parseTimestamp(response.json())
    inputdata["Values"] = parseValues(response.json()) 
    timestamp_df = pd.DataFrame(inputdata['Timestamp'])

    values_df = pd.DataFrame(inputdata['Values'])
    df = pd.concat([timestamp_df, values_df], axis=1)
    df.dropna(inplace=True)
    inputdata = df.to_dict(orient= 'list')
    inputdata['Timestamp']= inputdata[0]
    inputdata.pop(0)

    df = inputdata
    df['Timestamp'] = pd.to_datetime(df['Timestamp'])

    if range == '1d':
        fig = go.Figure(data=[go.Candlestick(x=df['Timestamp'],
                        open=df['OPEN'],
                        high=df['HIGH'],
                        low=df['LOW'],
                        close=df['CLOSE'],
                        increasing_fillcolor='#0abb92',
                        increasing_line_width=1,
                        decreasing_fillcolor='#d55438',
                        decreasing_line_width=1,
                        increasing_line_color='#0abb92',
                        decreasing_line_color='#d55438',
                        hoverinfo='text',  
                        hovertext=[f"{timestamp.strftime('%H:%M')}- {(timestamp+pd.Timedelta(minutes=15)).strftime('%H:%M')} <br>{round(close,1)}" 
                        for timestamp, close in zip(df['Timestamp'], df['CLOSE'])])])
    elif range == '1wk':
        fig = go.Figure(data=[go.Candlestick(x=df['Timestamp'],
                open=df['OPEN'],
                high=df['HIGH'],
                low=df['LOW'],
                close=df['CLOSE'],
                increasing_fillcolor='#0abb92',
                increasing_line_width=1,
                decreasing_fillcolor='#d55438',
                decreasing_line_width=1,
                increasing_line_color='#0abb92',
                decreasing_line_color='#d55438',
                hoverinfo='text',  
                hovertext=[f"{timestamp.strftime('%d %b %H:%M')} - {(timestamp+pd.Timedelta(minutes=60)).strftime('%d %b %H:%M')} <br>{round(close,1)}" 
                  for timestamp, close in zip(df['Timestamp'], df['CLOSE'])])])

    elif range == '1mo':
        fig = go.Figure(data=[go.Candlestick(x=df['Timestamp'],
                open=df['OPEN'],
                high=df['HIGH'],
                low=df['LOW'],
                close=df['CLOSE'],
                increasing_fillcolor='#0abb92',
                increasing_line_width=1,
                decreasing_fillcolor='#d55438',
                decreasing_line_width=1,
                increasing_line_color='#0abb92',
                decreasing_line_color='#d55438',
                hoverinfo='text',  
                hovertext=[f"{timestamp.strftime('%d %b %H:%M')} - {(timestamp+pd.Timedelta(minutes=60)).strftime('%d %b %H:%M')} <br>{round(close,1)}" 
                  for timestamp, close in zip(df['Timestamp'], df['CLOSE'])])])

    elif range == '1y':
        fig = go.Figure(data=[go.Candlestick(x=df['Timestamp'],
                open=df['OPEN'],
                high=df['HIGH'],
                low=df['LOW'],
                close=df['CLOSE'],
                increasing_fillcolor='#0abb92',
                increasing_line_width=1,
                decreasing_fillcolor='#d55438',
                decreasing_line_width=1,
                increasing_line_color='#0abb92',
                decreasing_line_color='#d55438',
                hoverinfo='text',
                hovertext=[f"{timestamp.strftime('%d %b %Y')} - {(timestamp+pd.Timedelta(days=7)).strftime('%d %b %Y')} <br>{round(close,1)}" 
                  for timestamp, close in zip(df['Timestamp'], df['CLOSE'])])])

    elif range == '5y':    
        
        df['EndOfMonth'] = df['Timestamp'] + pd.offsets.MonthEnd(0)
        fig = go.Figure(data=[go.Candlestick(x=df['Timestamp'],
                        open=df['OPEN'],
                        high=df['HIGH'],
                        low=df['LOW'],
                        close=df['CLOSE'],
                        increasing_fillcolor='#0abb92',
                        increasing_line_width=1,
                        decreasing_fillcolor='#d55438',
                        decreasing_line_width=1,
                        increasing_line_color='#0abb92',
                        decreasing_line_color='#d55438',
                        hoverinfo='text',
                        hovertext=[
                            f"{timestamp.strftime('%d %b %Y')} - {end_of_month.strftime('%d %b %Y')}<br>{round(close, 1)}"
                            for timestamp, end_of_month, close in zip(df['Timestamp'], df['EndOfMonth'], df['CLOSE'])])])
        
    fig.update_layout(title='',
                    paper_bgcolor='#fff',
                    plot_bgcolor='#fff',
                    xaxis=dict(
                        type='category',
                        tickvals=df['Timestamp'],
                        # ticktext=formatted_times,
                        tickfont=dict(color='white'),  
                        showline=True,
                        tickangle=0,
                        showgrid=False
                    ),
                    yaxis=dict(
                        showgrid=False,
                        tickmode='linear',
                        tick0=min(df['LOW']),
                        tickfont=dict(color='white'),  
                        dtick=(max(df['HIGH']) - min(df['LOW'])) / 10,
                    ),
                    hoverlabel=dict(
                        font=dict(color='white'),
                        bgcolor='black',
                        bordercolor='rgba(0, 0, 0, 0)',
                    ),hovermode='x'  
    )
                    

    fig.update_yaxes(showticklabels=False)
    fig.update_xaxes(showticklabels=False)
    # fig.show()
    plot_html =fig.to_html(full_html=False)
    json_data = json.dumps({"html_fig":plot_html,"status":"success"})
    return jsonify(json_data)

def parseValues2(chart_data):
    valueList = {'OPEN': [], 'CLOSE': [], 'HIGH': [], 'LOW': [], 'VOLUME': []}
    valueList['OPEN'].extend(chart_data["chart"]["result"][0]["indicators"]["quote"][0]["open"])
    valueList['CLOSE'].extend(chart_data["chart"]["result"][0]["indicators"]["quote"][0]["close"])
    valueList['HIGH'].extend(chart_data["chart"]["result"][0]["indicators"]["quote"][0]["high"])
    valueList['LOW'].extend(chart_data["chart"]["result"][0]["indicators"]["quote"][0]["low"])
    valueList['VOLUME'].extend(chart_data["chart"]["result"][0]["indicators"]["quote"][0]["volume"])

    return valueList
def generate_plot2(symbols, range):
    interval = get_interval(range)
    fig = go.Figure()

    for symbol in symbols:
        url = "https://apidojo-yahoo-finance-v1.p.rapidapi.com/stock/v3/get-chart"
        querystring = {
            "interval": interval,
            "symbol": symbol,
            "range": range,
            "region": "IN",
            "includePrePost": "false",
            "useYfid": "true",
            "includeAdjustedClose": "true",
            "events": "capitalGain,div,split"
        }

        headers = {
            "X-RapidAPI-Key": "5091a3807emshd9177749716494fp10be60jsn19c60ba96d0d",
            "X-RapidAPI-Host": "apidojo-yahoo-finance-v1.p.rapidapi.com"
        }

        response = requests.get(url, headers=headers, params=querystring)
        data = response.json()

        timestamps = parseTimestamp(data)
        values = parseValues2(data)
        # this remove the Nan/None values from the datset.
        timestamp_df = pd.DataFrame(timestamps)
        values_df = pd.DataFrame(values)
        df = pd.concat([timestamp_df, values_df], axis=1)
        df.dropna(inplace=True)
        inputdata = df.to_dict(orient= 'list')
        timestamps = inputdata[0]
        values = inputdata
        values.pop(0)
        fig.add_trace(go.Scatter(x=timestamps, y=values['CLOSE'], mode='lines', name=f'{symbol}_CLOSE'))

    fig.update_layout(title='',
                      paper_bgcolor='#fff',
                      plot_bgcolor='#fff',
                      xaxis=dict(
                          type='category',
                          tickfont=dict(color='white'),
                          showline=True,
                          tickangle=0,
                          showgrid=False
                      ),
                      yaxis=dict(
                          showgrid=False,
                          tickmode='linear',
                          tickfont=dict(color='white'),
                      ),
                      hoverlabel=dict(
                          font=dict(color='white'),
                          bgcolor='black',
                          bordercolor='rgba(0, 0, 0, 0)',
                      ),
                      hovermode='x'
                      )

    fig.update_yaxes(showticklabels=False)
    fig.update_xaxes(showticklabels=False)
    # fig.show()

    plot_html2 = fig.to_html(full_html=False)
    json_data = json.dumps({"html_fig": plot_html2, "status": "success"})
    return jsonify(json_data)

app = Flask(__name__)
app.secret_key = 'your_secret_key'  # Replace with your actual secret key

# Database Configuration
app.config['SQLALCHEMY_DATABASE_URI'] = 'sqlite:///users.db'
app.config['SQLALCHEMY_TRACK_MODIFICATIONS'] = False
db = SQLAlchemy(app)

# User Model
class User(db.Model):
    id = db.Column(db.Integer, primary_key=True)
    username = db.Column(db.String(100), unique=True, nullable=False)
    password_hash = db.Column(db.String(200), nullable=False)

# Initialize Database within Application Context
with app.app_context():
    db.create_all()



@app.route('/api/refresh',methods =['GET'])
def fun():
    refresh_data()
    response = json.dumps({"status":"success"})
    return Response(response=response,content_type='application/json')

@app.route('/')
def index():
    return render_template('login.html')

@app.route('/register', methods=['GET', 'POST'])
def register():
    if request.method == 'POST':
        username = request.form['username']
        password = request.form['password']
        hashed_password = generate_password_hash(password, method='pbkdf2:sha256')

        new_user = User(username=username, password_hash=hashed_password)
        db.session.add(new_user)
        db.session.commit()

        flash('Registration successful! Please login.')
        return redirect(url_for('index'))

    return render_template('register.html')

@app.route('/login', methods=['POST'])
def login():
    username = request.form['username']
    password = request.form['password']
    user = User.query.filter_by(username=username).first()

    if user and check_password_hash(user.password_hash, password):
        session['user_id'] = user.id
        session['username'] = user.username
        return redirect(url_for('dashboard'))
    else:
        flash('Invalid username or password')
        return redirect(url_for('index'))

@app.route('/dashboard')
def dashboard():
    if 'user_id' in session:
        return render_template('welcome.html', username=session['username'])
    else:
        return redirect(url_for('index'))

@app.route('/logout')
def logout():
    session.pop('user_id', None)
    session.pop('username', None)
    return redirect(url_for('index'))

@app.route('/api/get_data2',methods= ['POST'])
def get_info2():
    data = json.loads(request.get_data(as_text=True))
    symbol = data['symbol']
    s1 = symbol[:-3]
    print(s1)
    q = n.stock_quote(s1)
    response = json.dumps(q)
    return jsonify(response)

@app.route('/api/get_data3',methods= ['POST'])
def get_info3():
    data = json.loads(request.get_data(as_text=True))
    # symbol = data['symbol']
    # s1 = symbol[:-3]
    # print(symbol)
    nifty = n.live_index("NIFTY 50")
    # q = n.stock_quote(symbol)
    response = json.dumps(nifty)
    return jsonify(response)

@app.route('/api/get_data',methods=['POST'])
def get_info():
    data =json.loads(request.get_data(as_text=True))
    # print(data)
    symbol = data['symbol']
    # print(symbol)
    response = json.dumps(get_ticker_metadata(symbol))
    return jsonify(response)

@app.route('/stocks')
def stocks():
    return redirect(url_for('show_user_profile',symbol='SBIN.NS'))

@app.route('/stocks/<symbol>')
def show_user_profile(symbol):
    company = search_by_symbol(symbol)
    name = company['company_name']
    if(name!=None):
        print(name)
        return render_template('stocks.html',symbol=symbol,name= name)
    else:
        print("Not found")
        return render_template('stocks.html')


@app.route('/compare')
def compare():
    return render_template('compare.html')
@app.route('/api/graph',methods=['POST','GET'])     
def graph(): 
    data = request.get_json()
    symbol = data['symbol']
    range = data['range']
    s1 = symbol[1:-1]
    print(s1,range)
    return generate_plot(s1,range)

@app.route('/api/compare',methods = ['POST','GET'])
def graph2():
    data = request.get_json()
    print(data)
    symbols = data['symbols']  # symbols- list of symbols of companies that we want to compare
    print(symbols)
    range = data['range'] 
    # response = json.dumps([1])
    # return jsonify(response)
    return generate_plot2(symbols=symbols, range=range)


def get_filter(parameters=None):
    min_market_cap = parameters.get('min_market_cap', 0)
    max_market_cap = parameters.get('max_market_cap', 2000000)

    min_pe_ratio = parameters.get('min_pe_ratio', -9000)
    max_pe_ratio = parameters.get('max_pe_ratio', 300000)

    min_close_price = parameters.get('min_close_price', 0)
    max_close_price = parameters.get('max_close_price', 100000)

    filtered_companies = [
            company for company in values
        if 'market_cap' in company
        and (min_market_cap) < ((company['market_cap'])) < (max_market_cap)
        and 'PE' in company and min_pe_ratio < float(company['PE']) < max_pe_ratio
        and 'Close Price' in company and min_close_price < float(company['Close Price']) < max_close_price
    ]
    filtered_companies = sorted(filtered_companies, key=lambda x: x['market_cap'], reverse=True)
    return filtered_companies


@app.route('/screener',methods = ['GET'])
def screener():
    return render_template('filter.html')

@app.route('/api/filter',methods =['POST'])
def filter():
    data =json.loads(request.get_data(as_text=True))
    try:
        min_market_cap = data['min_market_cap']
        max_market_cap = data['max_market_cap']
        min_pe_ratio = data['min_pe_ratio']
        max_pe_ratio = data['max_pe_ratio']
        min_close_price = data['min_close_price']
        max_close_price = data['max_close_price']
        parameters = {
        'min_market_cap': float(min_market_cap),
        'max_market_cap': float(max_market_cap),
        'min_pe_ratio': float(min_pe_ratio),
        'max_pe_ratio': float(max_pe_ratio),
        'min_close_price': float(min_close_price),
        'max_close_price': float(max_close_price)
        }
        print(parameters)
        filtered = get_filter(parameters=parameters)
        # print(len(filtered))
        response = json.dumps({'status':'success','companies':filtered})
        return jsonify(response)
        # response = jsonify({'status':'success'})
        # response.headers['Content-Type'] = 'application/json'
        # return response
    except Exception as e:
        # Handle exceptions, log the error, or return an appropriate response
        return jsonify({'error': str(e)}), 500
    # response = json.dumps({'result':'success'})
    # return jsonify({'result': "success"})
    # return response

@app.route('/api/search',methods = ['GET'])
def search_results():
    if(request.method == 'GET'):
        # print(request)
        query = request.args.get('query')
        # print(query)
        closest_queries = query_finder.find_closest_queries(query)
        # print(closest_queries)
        l = []
        for query in closest_queries:
            d ={}
            d['Name']= query
            d['Symbol'] = name_symbol[query]
            l.append(d)
        # print(l)
        response = json.dumps(l)
        # print(closest_queries)
        # response.headers.add('Content-Type', 'application/json')
        return jsonify(response)

if __name__ == '__main__':
    app.run(debug=True)
