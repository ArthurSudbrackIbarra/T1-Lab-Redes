FROM gcc:4.9
WORKDIR /T1_Lab_Redes
COPY main.c ./
RUN gcc -o main main.c
EXPOSE 1-65000
CMD ["./main"]