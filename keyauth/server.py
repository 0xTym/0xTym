from flask import Flask, request, redirect, render_template, session, url_for
import sqlite3
from werkzeug.security import generate_password_hash, check_password_hash
from functools import wraps

app = Flask(__name__)
app.secret_key = 'change_me'
DATABASE = 'keyauth.db'


def get_db():
    conn = sqlite3.connect(DATABASE)
    conn.row_factory = sqlite3.Row
    return conn


def init_db():
    conn = get_db()
    c = conn.cursor()
    c.execute(
        """
        CREATE TABLE IF NOT EXISTS users (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            username TEXT UNIQUE NOT NULL,
            password TEXT NOT NULL
        )
        """
    )
    c.execute(
        """
        CREATE TABLE IF NOT EXISTS keys (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            key TEXT UNIQUE NOT NULL,
            active INTEGER NOT NULL DEFAULT 1,
            created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
        )
        """
    )
    conn.commit()
    conn.close()


@app.before_first_request
def setup():
    init_db()
    # Create default admin if not exists
    conn = get_db()
    c = conn.cursor()
    c.execute("SELECT * FROM users WHERE username = ?", ("admin",))
    if c.fetchone() is None:
        c.execute(
            "INSERT INTO users (username, password) VALUES (?, ?)",
            ("admin", generate_password_hash("admin")),
        )
        conn.commit()
    conn.close()


def login_required(f):
    @wraps(f)
    def decorated_function(*args, **kwargs):
        if 'user_id' not in session:
            return redirect(url_for('login'))
        return f(*args, **kwargs)
    return decorated_function


@app.route('/login', methods=['GET', 'POST'])
def login():
    if request.method == 'POST':
        username = request.form['username']
        password = request.form['password']
        conn = get_db()
        c = conn.cursor()
        c.execute("SELECT * FROM users WHERE username = ?", (username,))
        user = c.fetchone()
        conn.close()
        if user and check_password_hash(user['password'], password):
            session['user_id'] = user['id']
            return redirect(url_for('admin'))
        return render_template('login.html', error='Invalid credentials')
    return render_template('login.html')


@app.route('/logout')
def logout():
    session.clear()
    return redirect(url_for('login'))


@app.route('/admin', methods=['GET', 'POST'])
@login_required
def admin():
    conn = get_db()
    c = conn.cursor()
    if request.method == 'POST':
        new_key = request.form.get('new_key')
        if new_key:
            try:
                c.execute("INSERT INTO keys (key) VALUES (?)", (new_key,))
                conn.commit()
            except sqlite3.IntegrityError:
                pass
        for key_id in request.form.getlist('deactivate'):
            c.execute("UPDATE keys SET active=0 WHERE id=?", (key_id,))
            conn.commit()
    c.execute("SELECT * FROM keys")
    keys = c.fetchall()
    conn.close()
    return render_template('admin.html', keys=keys)


@app.route('/api/verify', methods=['POST'])
def verify():
    data = request.get_json() or {}
    key = data.get('key')
    if not key:
        return {"success": False, "message": "No key provided"}, 400
    conn = get_db()
    c = conn.cursor()
    c.execute("SELECT * FROM keys WHERE key=? AND active=1", (key,))
    row = c.fetchone()
    conn.close()
    if row:
        return {"success": True}
    return {"success": False, "message": "Invalid key"}, 403


if __name__ == '__main__':
    app.run(debug=True)
