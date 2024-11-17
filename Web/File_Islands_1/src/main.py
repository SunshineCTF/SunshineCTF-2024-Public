import os
from flask import (
    redirect,
    url_for,
    Flask,
    render_template,
    request,
    send_from_directory,
)

app = Flask(__name__)

FLAG = os.getenv("FLAG", "flag{this_is_a_fake_flag}")


@app.errorhandler(404)
def page_not_found(e):
    # You can return a custom message or render a template
    return redirect(url_for("not_found", cave="adventure/secret_map"))


@app.route("/", methods=["GET"])
def index():
    return render_template("home.html", class_name="home-bg")


@app.route("/about", methods=["GET"])
def about():
    return render_template("about.html")


@app.route("/robots.txt", methods=["GET"])
def robots():
    return send_from_directory("static", "robots.txt")


@app.route("/404", methods=["GET"])
def not_found():
    message_file = request.args.get("cave", "NONE")
    try:
        # if we used send_file or send_from_directory we would've been OK
        # important to use secure framework functions for simple tasks like this
        message = open(message_file, "r").read()
    except:  # noqa: E722
        message = f"File {message_file} was not found"

    return render_template("404.html", message=message, class_name="not-found-bg"), 404


def create_app():
    return app


if __name__ == "__main__":
    app.run(host="0.0.0.0", port=8000, debug=False)
