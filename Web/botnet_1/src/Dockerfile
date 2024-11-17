FROM python:3.11-bookworm
WORKDIR /opt/chal

# RUN apk add --update \
#     && apk add \
#         python3-dev \
#         gcc \
#         musl-dev \
#         g++ \
#         bash

COPY requirements.txt /opt/chal/

RUN pip install -r requirements.txt --no-cache-dir

COPY . /opt/chal

RUN useradd -m unprivileged -s /bin/bash -u 1000 -U && \
    chown -R unprivileged:unprivileged /opt/chal

RUN chmod +x /opt/chal/docker-entrypoint.sh

USER unprivileged
EXPOSE 8000
ENTRYPOINT ["/opt/chal/docker-entrypoint.sh"]
