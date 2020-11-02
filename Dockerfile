FROM ubuntu:latest

COPY install.sh .
RUN sh install.sh

CMD ["sh", "run.sh"]
