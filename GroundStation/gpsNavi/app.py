from flask import Flask, render_template

app = Flask(__name__)


def fetch_new_data():
    with open('datalink/shared_array.json', 'r') as file:
        json_data = file.read()

    return json_data


@app.route('/')
def home():
    return render_template('index.html')


@app.route('/get_data')
def get_data():
    # Fetch and prepare the updated data
    json_data = fetch_new_data()
    # Return the JSON response
    return json_data


if __name__ == '__main__':
    app.run(host="0.0.0.0", port=4000, ssl_context=('cert/192.168.1.3.pem', 'cert/192.168.1.3-key.pem'))
