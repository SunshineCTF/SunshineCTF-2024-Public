#!/usr/local/bin/python3
# Web server
import base64
import binascii
import hashlib  # testing
import json
import os
import subprocess
import sys
import traceback
import uuid
from datetime import datetime
from urllib.parse import urlparse

import requests
from cryptography.exceptions import InvalidSignature
from cryptography.hazmat.backends import default_backend
from cryptography.hazmat.primitives import hashes, serialization
from cryptography.hazmat.primitives.asymmetric import padding
from flask import Flask, Response, abort, redirect, render_template, request

# Debug mode:
# - Enables federation with most clients (un-breaks the Actor to be a Person).
# - Enables the auto-solve endpoint.
debug = False
if os.environ.get("DEBUG", "") == "TRUE":
    debug = True

app = Flask(__name__)


def file_read(path):
    with open(path, "r") as file:
        return file.read().replace("\\n", "\n").strip()


domain = os.environ.get("DOMAIN", "localhost:8000")
users = {
    "miku": {
        "display": "Hatsune Miku",
        "username": "miku",
        "bio": "digital idol. hipster that hates john mastodon.",
        "img": f"https://{domain}/static/pfp/miku.png",
        "secure": True,
        "actor_type": "Person" if debug else "Vocaloid",
        "posts": [
            {
                "content": "<a href='/users/rin/'>@rin</a> is too trusting. She will leak her secrets but mine are safe and sound!"
            },
            {
                "content": "<a href='#'>#fediblock</a> every mastodon/akkoma/sharkey/pleroma/gotosocial/whatever server. except botnet ofc!!"
            },
            {"content": "i am in your wifi"},
        ],
    },
    "rin": {
        "display": "Kagamine Rin",
        "username": "rin",
        "bio": f"Sister of Kagamine Len and <a href='https://{domain}/users/miku/'>@miku</a> stan.",
        "img": f"https://{domain}/static/pfp/rin.png",
        "secure": False,
        "actor_type": "Person",
        "posts": [
            {
                "content": "Wait, didn't I have something to hide there too???<br><br>Eh, probably not. :3"
            },
            {
                "content": "I think Miku has her secrets. I don't know why though... I kept my Actor settings to Person so I can make friends!"
            },
            {
                "content": "Mastodon is super cool! Idk why <a href='/users/miku/'>@miku</a> limits her profile to other BotNet servers."
            },
        ],
    },
    "actor": {
        "display": "BotNet Service User",
        "username": "actor",
        "bio": "All requests from BotNet are signed by this service user.",
        "img": f"https://{domain}/static/pfp/miku.png",
        "secure": False,
        "actor_type": "Application",
        "posts": [],
    },
}

for user in users:
    users[user]["username_full"] = users[user].get("username") + "@" + domain

files_loaded = {
    "flag1": file_read("flag1.txt"),
    "flag2": file_read("flag2.txt"),
    "pubkey": file_read("pki/pub.pem"),
    "privkey": file_read("pki/key.pem"),
}


# Creates a Note object to send to a remote server.
def synthesize_post(
    message=f"@miku@{domain} test!", user=f"@miku@{domain}", sendas="miku"
):
    pubtime = datetime.utcnow().strftime("%Y-%m-%dT%H:%M:%SZ")

    username = user.split("@")[1]
    domain = user.split("@")[2]

    return {
        "@context": [
            "https://www.w3.org/ns/activitystreams",
            f"https://{domain}/schemas/litepub-0.1.jsonld",
            {"@language": "und"},
        ],
        "actor": f"https://{domain}/users/{sendas}/",
        "cc": [],
        # "context": f"https://{domain}/contexts/6d8e9ee3-199a-44c3-b466-8f2cfdffa674",
        "directMessage": 1,
        "id": f"https://{domain}/objects/{uuid.uuid4()}",
        "object": {
            "actor": f"https://{domain}/users/{sendas}/",
            "attachment": [],
            "attributedTo": f"https://{domain}/users/{sendas}/",
            "cc": [],
            "content": f'<p><span class="h-card"><a class="u-url mention" data-user="AZqCLkbWVvbS3rrXJQ" href="https://{domain}/users/{username}/" rel="ugc">@<span>{username}</span></a></span> nyaaa :3</p>',
            "contentMap": {
                "en": f'<p><span class="h-card"><a class="u-url mention" data-user="AZqCLkbWVvbS3rrXJQ" href="https://{domain}/users/{username}/" rel="ugc">@<span>{username}</span></a></span> nyaaa :3</p>'
            },
            # "context": f"https://{domain}/contexts/6d8e9ee3-199a-44c3-b466-8f2cfdffa674",
            # "conversation": f"https://{domain}/contexts/6d8e9ee3-199a-44c3-b466-8f2cfdffa674",
            "id": f"https://{domain}/objects/{uuid.uuid4()}",
            "published": pubtime,
            "sensitive": None,
            "url": f"https://{domain}/posts/{uuid.uuid4()}",
            "source": {
                "content": f"{user} nyaaa :3",
                "mediaType": "text/x.misskeymarkdown",
            },
            "summary": "",
            "tag": [
                {
                    "href": f"https://{domain}/users/{username}/",
                    "name": user,
                    "type": "Mention",
                }
            ],
            "to": [f"https://{domain}/users/{username}/"],
            "type": "Note",
        },
        "published": pubtime,
        "to": [f"https://{domain}/users/{username}/"],
        "type": "Create",
    }


# This creates an easily-parsable object that represents the signature from the remote server.
def sig_object_builder(input_str):
    signature_strs = input_str.strip().split(",")

    sig = {}
    counter = 0
    for entry in signature_strs:
        split_entry = entry.split("=")
        if split_entry[0].lower() == "signature":
            key = "signature"
            value = (
                signature_strs[counter]
                .replace("signature=", "")
                .replace('"', "")
                .replace("'", "")
            )
        else:
            key, value = split_entry
            key = key.lower()
            value = value.replace('"', "").replace("'", "")

        key = key.strip()
        value = value.strip()
        sig[key] = value

        counter += 1

    return sig


# This creates the string we are going to verify against (or sign ourselves)
def sig_string_builder(sig, request):
    string_to_compare = ""
    headers_to_compare = sig.get("headers").split(" ")
    for header in headers_to_compare:
        if header == "(request-target)":
            string_to_compare += f"(request-target): get {request.path}\n"
        else:
            string_to_compare += f"{header.lower()}: {request.headers.get(header)}\n"

    # Strip newline.
    string_to_compare = string_to_compare[:-1]

    return string_to_compare


# HTTP signature validator logic
def signature_validate(input_str, request):
    print(f"[signature_validate] Validation job: {input_str}")
    # Sig header -> object
    sig = sig_object_builder(input_str)

    href = sig.get("keyid")
    parsed_href = urlparse(href)
    date = datetime.utcnow().strftime("%a, %d %b %Y %H:%M:%S GMT")
    host = parsed_href.netloc
    path = parsed_href.path

    # get the public key from the remote ActivityPub server.
    # Note: the service accounts we request are usually fine to be requested without a signature.
    # This is at least true for Pleroma/Akkoma.
    req = requests.get(
        href,
        headers={
            "Accept": 'application/ld+json; profile="https://www.w3.org/ns/activitystreams"',
            "User-Agent": f"BotNet; {domain}",
        },
        verify=False,
    )
    print(req.text + "<<<")
    resp = req.json()
    foreign_pubkey = resp.get("publicKey", {}).get("publicKeyPem", "")

    # get the string we need to sign + the signature offered up by the requestor
    string_to_compare = sig_string_builder(sig, request)
    signature = sig.get("signature")

    print(f"signature_validate] signature: {signature}")

    # Signature validation logic.
    # Anything prefixed with "crypto_" is for interacting with the cryptography library.
    # reference: https://gist.github.com/sirosen/ec4196fee9779e5de865b0d03f12f0c8
    # Please do not take the reference at face value... lots of modifications needed to be made.
    crypto_pubkey = serialization.load_pem_public_key(
        foreign_pubkey.encode("utf8"), backend=default_backend()
    )
    decoded_sig = base64.b64decode(signature.encode("utf8"))
    print(f"[signature_validate] decoded_sig {decoded_sig}")

    print("vvv [signature_validate] compare string:")
    print(string_to_compare)
    print("^^^ ")

    try:
        crypto_pubkey.verify(
            decoded_sig,
            string_to_compare.encode("utf8"),
            padding.PKCS1v15(),
            hashes.SHA256(),
        )
        return True
    except InvalidSignature as e:
        print(f"InvalidSignature Error: {e}")
        return False


def signature_sign(target, host, date, post=None):
    """
    Sign a signature as @actor (this instance's service account).
    """
    # If POST request, add a Digest.
    digest = None
    if post:
        digest_raw = hashlib.sha256(post.encode("utf8")).digest()
        digest = "sha-256=" + base64.b64encode(digest_raw).decode("utf8")

    keyid = f"https://{domain}/users/actor/#main-key"
    headers = "(request-target) host date"
    signature_text = f"""(request-target): {target}
host: {host}
date: {date}"""

    if digest:
        headers += " digest"
        signature_text += f"\ndigest: {digest}"

    print("[signature_sign] Issued signature text:")
    print(signature_text)
    print(f"[signature_sign] md5: {hashlib.md5(signature_text.encode('utf8')).hexdigest()}")
    print()

    local_privkey = files_loaded.get("privkey", "")
    crypto_privkey = serialization.load_pem_private_key(
        local_privkey.encode("utf8"), backend=default_backend(), password=None
    )

    encoded_data = signature_text.encode("utf8")
    sig_raw = crypto_privkey.sign(encoded_data, padding.PKCS1v15(), hashes.SHA256())

    signature = base64.b64encode(sig_raw).decode("utf8")

    print(f"[signature_sign] encoded_sig {sig_raw}")

    print(f"[signature_sign] zexpecting: {signature}")

    final = f'keyId="{keyid}",algorithm="rsa-sha256",headers="{headers}",signature="{signature}"'

    if digest:
        return final, digest
    else:
        return final


def remote_fetch_profile(handle):
    """
    Given a WebFinger handle, return an Actor.
    """
    try:
        if handle.find("@") == 0:
            handle = handle[1:]

        username, remote_domain = handle.split("@")

        # Process WebFinger
        print(f"WebFinger -> {username}@{remote_domain}")
        wf_req = requests.get(
            f"https://{remote_domain}/.well-known/webfinger?resource=acct:{username}@{remote_domain}",
        verify=False
        )

        # If no handle,
        if wf_req.status_code >= 400:
            print("Remote user not found")
            return None

        wf_body = wf_req.json()

        href = None
        for link in wf_body.get("links", []):
            if link.get("type") == "application/activity+json":
                href = link.get("href")

        actor_body = signed_get(href)

        return actor_body
    except Exception as e:
        print("remote_fetch_profile failed!")
        print(e)
        return None


def signed_get(url):
    """
    Given a URL, return a given resource. Signs things too!
    """
    try:
        parsed_href = urlparse(url)
        # Get initial post set
        date = datetime.utcnow().strftime("%a, %d %b %Y %H:%M:%S GMT")
        host = parsed_href.netloc
        path = parsed_href.path

        sig = signature_sign(f"get {path}", host, date)
        actor_req = requests.get(
            url,
            headers={
                "Accept": 'application/ld+json; profile="https://www.w3.org/ns/activitystreams"',
                "Content-Type": 'application/ld+json; profile="https://www.w3.org/ns/activitystreams"',
                "Date": date,
                "Host": host,
                "Signature": sig,
                "User-Agent": f"BotNet; {domain}",
            },
            verify=False
        )

        if actor_req.status_code >= 400:
            print(f"Could not retrieve URL: {url}")
            print(actor_req.text)
            print(f"\tSignature: {sig}")
            return None

        actor_body = actor_req.json()

        return actor_body
    except Exception as e:
        print(f"Error in signed_get('{url}'):")
        print(e)
        return None


def signed_post(url, body):
    """
    Given a URL, POSTs to a given resource. Signs things too!
    """
    try:
        parsed_href = urlparse(url)
        # Get initial post set
        date = datetime.utcnow().strftime("%a, %d %b %Y %H:%M:%S GMT")
        host = parsed_href.netloc
        path = parsed_href.path

        str_body = json.dumps(body)
        print(str_body)

        # Signatures are seemingly confirmed to be functional.
        sig, digest = signature_sign(f"post {path}", host, date, post=str_body)
        actor_req = requests.post(
            url,
            headers={
                "Accept": 'application/ld+json; profile="https://www.w3.org/ns/activitystreams"',
                "Content-Type": "application/activity+json",
                "Date": date,
                "Host": host,
                "Digest": digest,
                "Signature": sig,
                "User-Agent": f"BotNet; {domain}",
            },
            data=str_body,
            verify=False
        )

        if actor_req.status_code >= 400:
            print(f"Could not retrieve URL: {url}")
            print(actor_req.text)
            print(f"\tSignature: {sig}")
            print(f"\tDigest: {digest}")
            return None

        if actor_req.text != "":
            return actor_req.json()
        else:
            return {}

    except Exception as e:
        print(e)
        return None


@app.route("/")
def index():
    return render_template("index.html", debug=debug)


@app.route("/.well-known/webfinger")
def webfinger():
    """
    Functional implementation of WebFinger.
    """

    if not request.args.get("resource"):
        return "No resource provided."

    user_requested = request.args.get("resource", f"acct:dummy@{domain}")

    user_req_filtered = user_requested.replace("acct:", "")
    if user_req_filtered.find("@") == 0:
        user_req_filtered = user_req_filtered[1:]

    print("@" + domain)
    user_handle_filtered = user_req_filtered.replace("@" + domain, "")
    print(user_handle_filtered)
    user = users[user_handle_filtered]

    if not user or (user_req_filtered.lower() != user.get("username_full").lower()):
        return abort(404)

    data = {
        "subject": "acct:" + user.get("username_full"),
        "aliases": [f"https://{domain}/users/{user.get('username')}/"],
        "links": [
            {
                "rel": "http://webfinger.net/rel/profile-page",
                "type": "text/html",
                "href": f"https://{domain}/users/{user.get('username')}/",
            },
            {
                "rel": "self",
                "type": "application/activity+json",
                "href": f"https://{domain}/users/{user.get('username')}/",
            },
        ],
    }

    resp = Response(json.dumps(data))
    resp.headers["Access-Control-Allow-Origin"] = "*"
    resp.headers["Content-Type"] = "application/jrd+json"

    return resp


# I was gonna do something with this but nahh... it's not fun imho.
@app.route("/users/<username>/inbox/", methods=["POST"])
@app.route("/inbox/", methods=["POST"])
def inbox_user(username="miku"):
    print("you've got mail!")
    print(request.headers)
    print(request.json)

    # Send a reply, as a test.
    data = synthesize_post()
    raw_data = json.dumps(data)

    return {
        "@context": [
            "https://www.w3.org/ns/activitystreams",
            f"https://{domain}/schemas/litepub-0.1.jsonld",
            {"@language": "und"},
        ],
        "first": f"https://{domain}/users/{username}/inbox/?page=true",
        "id": f"https://{domain}/users/{username}/inbox/",
        "type": "OrderedCollection",
    }

@app.route("/users/<username>")
def profile_redir(username):
    return f"Error: /users/{username} (what you did) != /users/{username}/ (what you want)."


@app.route("/users/<username>/")
def profile(username):
    """
    Gets a given user.
    """
    print(f"<-- User Request: {username}")
    print(request.headers.get("Signature", f"Error in /users/{username}/ Request Not Signed"))

    # Username checker.
    username = username.lower()
    user = users.get(username, None)

    is_remote = False

    if "@" in username:
        actor_data = remote_fetch_profile(username)
        if actor_data:
            handle = username
            # Handle parser
            if handle.find("@") == 0:
                handle = handle[1:]
            remote_domain = handle.split("@")[1]

            user = {
                "display": actor_data.get("name"),
                "username": actor_data.get("preferredUsername", "")
                + "@"
                + remote_domain,
                "username_full": actor_data.get("preferredUsername", "")
                + "@"
                + remote_domain,
                "bio": actor_data.get("summary"),
                "img": actor_data.get("icon", {}).get("url"),
                "secure": False,
                "actor_type": actor_data.get("type", "Person"),
                "posts": [],
            }
            is_remote = True

            outbox = signed_get(actor_data.get("outbox"))
            if outbox:
                outbox_url = outbox.get("first", "")

                # Implementation note: we are NOT getting all of a user's posts.
                # That's too much for a CTF challenge lmao.
                # (Yes, that's where I am drawing the line.)
                posts = signed_get(outbox_url)
                for post in posts.get("orderedItems", []):
                    user["posts"].append(post.get("object", None))

    if not user:
        return abort(404)

    mime = request.headers.get("Accept").lower().split(";")[0]
    # https://www.w3.org/TR/activitypub/#retrieving-objects
    # Technically, the Accept header should include
    # `profile="https://www.w3.org/ns/activitystreams"`
    # but in practice, clients are pretty lax. So I will be too.
    print(mime)
    if (
        "application/ld+json" not in mime and "application/activity+json" not in mime
    ) and username != "actor":
        print("\tReturning HTTP...")
        return render_template("profile.html", user=user, external=is_remote)

    else:
        print("\tReturning LD+JSON")
        if is_remote:
            return "This is a remote user.", 400
        if user.get("secure", True) == False:
            is_valid = True
        else:
            # sig parsing
            try:
                is_valid = signature_validate(
                    request.headers.get("Signature", ""), request
                )
            except Exception as e:
                print("INVALID SIGNATURE!")
                print(e)
                is_valid = False

            print(f"is_valid {is_valid}")

            if not is_valid:
                return abort(401)

        # if pass, send ActivityPub Actor
        data = {
            "@context": [
                "https://www.w3.org/ns/activitystreams",
                f"https://{domain}/schemas/litepub-0.1.jsonld",
                {"@language": "und"},
            ],
            "alsoKnownAs": [],
            "attachment": [],
            "capabilities": {},
            "endpoints": {
                "oauthAuthorizationEndpoint": f"https://{domain}/oauth/authorize",
                "oauthRegistrationEndpoint": f"https://{domain}/api/v1/apps",
                "oauthTokenEndpoint": f"https://{domain}/oauth/token",
                "sharedInbox": f"https://{domain}/inbox/",
                "uploadMedia": f"https://{domain}/api/ap/upload_media/",
            },
            "featured": f"https://{domain}/users/{user.get('username')}/collections/featured/",
            "followers": f"https://{domain}/users/{user.get('username')}/followers/",
            "following": f"https://{domain}/users/{user.get('username')}/following/",
            "icon": {"type": "Image", "url": user.get("img")},
            "id": f"https://{domain}/users/{user.get('username')}/",
            "inbox": f"https://{domain}/users/{user.get('username')}/inbox/",
            "name": user.get("display"),
            "preferredUsername": user.get("username"),
            "outbox": f"https://{domain}/users/{user.get('username')}/outbox/",
            "publicKey": {
                "id": f"https://{domain}/users/{user.get('username')}/#main-key",
                "owner": f"https://{domain}/users/{user.get('username')}/",
                "publicKeyPem": files_loaded.get(
                    "pubkey", "(no PKI imported; report to @shuga__@discord.com)"
                ),
            },
            "summary": user.get("bio"),
            # change Actor type to something else to break federation on 99% of clients.
            "type": user.get("actor_type"),
            "url": f"https://{domain}/users/{user.get('username')}/",
        }

        # first flag
        if username == "rin":
            data["attachment"].append(
                {
                    "name": "flag_1",
                    "type": "CtfFlagValue",
                    "value": files_loaded.get("flag1", "sun{ERROR}"),
                }
            )
        else:
            data["attachment"].append(
                {"name": "flag_1", "type": "CtfFlagValue", "value": "Rin has the flag."}
            )

        # if secure is enabled, insert flag. else, give hint.
        if user.get("secure", True) and username == "miku":
            data["attachment"].append(
                {
                    "name": "flag_2",
                    "type": "CtfFlagValue",
                    "value": files_loaded.get("flag2", "sun{ERROR}"),
                }
            )
        else:
            data["attachment"].append(
                {
                    "name": "flag_2",
                    "type": "CtfFlagValue",
                    "value": "Miku has the flag.",
                }
            )

        resp = Response(json.dumps(data))
        resp.headers["Access-Control-Allow-Origin"] = "*"
        resp.headers["Content-Type"] = "application/activity+json; charset=utf-8"

        return resp


@app.route("/schemas/litepub-0.1.jsonld")
def schema_lite():
    # adapted from Akkoma
    data = {
        "@context": [
            "https://www.w3.org/ns/activitystreams",
            "https://w3id.org/security/v1",
            {
                "Emoji": "toot:Emoji",
                "Hashtag": "as:Hashtag",
                "PropertyValue": "schema:PropertyValue",
                "atomUri": "ostatus:atomUri",
                "conversation": {"@id": "ostatus:conversation", "@type": "@id"},
                "discoverable": "toot:discoverable",
                "capabilities": "litepub:capabilities",
                "ostatus": "http://ostatus.org#",
                "schema": "http://schema.org#",
                "toot": "http://joinmastodon.org/ns#",
                "value": "schema:value",
                "sensitive": "as:sensitive",
                "litepub": "http://litepub.social/ns#",
                "invisible": "litepub:invisible",
                "listMessage": {"@id": "litepub:listMessage", "@type": "@id"},
                "oauthRegistrationEndpoint": {
                    "@id": "litepub:oauthRegistrationEndpoint",
                    "@type": "@id",
                },
                "ChatMessage": "litepub:ChatMessage",
                "alsoKnownAs": {"@id": "as:alsoKnownAs", "@type": "@id"},
                "vcard": "http://www.w3.org/2006/vcard/ns#",
                "formerRepresentations": "litepub:formerRepresentations",
                "contentMap": {"@id": "as:content", "@container": "@language"},
            },
        ]
    }
    resp = Response(json.dumps(data))
    resp.headers["Access-Control-Allow-Origin"] = "*"
    resp.headers["Content-Type"] = "application/ld+json"
    return resp


@app.route("/autosolver")
def autosolver():
    """
    Out of scope.
    """
    if not debug:
        return abort(404)
    else:
        actor = remote_fetch_profile(users["miku"].get("username_full"))
        return {
            "success": True if actor else False,
            "actor": actor,
            "instance_domain": domain,
        }


@app.route("/sunshinectf_health")
@app.route("/sunshinectf_health/")
@app.route("/sunshinectf_health/<domain_override>/")
def health(domain_override=domain):
    """
    Out of scope. Health checker.
    """
    print(domain_override)

    flag1 = files_loaded.get("flag1", "sun{ERROR}")
    flag2 = files_loaded.get("flag2", "sun{ERROR}")

    headers = {
        "Accept": 'application/ld+json; profile="https://www.w3.org/ns/activitystreams"',
        "Content-Type": 'application/ld+json; profile="https://www.w3.org/ns/activitystreams"',
    }

    try:
        print("Starting flag #1 test...")
        flag1_response = requests.get(f"https://{domain_override}/users/rin/", headers=headers, verify=False).json()
        flag1_actual = flag1_response.get("attachment")[0].get("value")
        part1_healthy = flag1 == flag1_actual
    except Exception as e:
        print("vvv   Exception in Flag #1 (UNsigned requests)")
        tbe = traceback.TracebackException.from_exception(e)
        f = sys.exc_info()[2].tb_frame
        f = f.f_back
        while f is not None:
            tbe.stack.append(traceback.FrameSummary(
                f.f_code.co_filename, f.f_lineno, f.f_code.co_name))
            f = f.f_back
        print(''.join(tbe.format()))
        print("^^^")
        part1_healthy = False

    try:
        print("Starting flag #2 test...")
        remote_username_to_test = users["miku"].get("username_full").replace(domain, domain_override)
        print(f"Flag #2 test: full loop with {remote_username_to_test}")
        actor = remote_fetch_profile(remote_username_to_test)
        flag2_actual = actor.get("attachment")[1].get("value")
        part2_healthy = flag2 == flag2_actual
    except Exception as e:
        print("vvv   Exception in Flag #2 (signed requests)")
        tbe = traceback.TracebackException.from_exception(e)
        f = sys.exc_info()[2].tb_frame
        f = f.f_back
        while f is not None:
            tbe.stack.append(traceback.FrameSummary(
                f.f_code.co_filename, f.f_lineno, f.f_code.co_name))
            f = f.f_back
        print(''.join(tbe.format()))
        print("^^^")
        part2_healthy = False

    return {
        "part1": part1_healthy,
        "part2": part2_healthy,
        "debug": debug,
        "is_challenge_healthy": (part1_healthy and part2_healthy and (not debug)),
        "NOTE": "[sunshinectf_meta] This page is not in scope.",
        "domain": domain,
        "domain_override": domain_override
    }


@app.errorhandler(404)
def not_found(e):
    return "404 not found :(", 404


@app.errorhandler(401)
def unsigned_err(e):
    return "Request not signed", 401


# # Static files
# @app.route("/static/<path:path>")
# def static_file(path):
#     filepath = os.path.join("static", path)
#     return app.send_static_file(filepath)


def create_app():
    return app


if __name__ == "__main__":
    app.run(host="0.0.0.0", port=8000)
